// Copyright (C) 2022,2023,2024 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
// Authors: Josep Martí Saumell (jmarti@iri.upc.edu) and Joan Vallvé Navarro (jvallve@iri.upc.edu)
// All rights reserved.
//
// YAML-SCHEMA-CPP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "yaml-schema-cpp/yaml_utils.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <memory>
#include <Eigen/Dense>

#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"

namespace yaml_schema_cpp
{
namespace filesystem = boost::filesystem;

void flattenNode(YAML::Node& node, std::vector<std::string> folders, bool is_schema, bool override)
{
    switch (node.Type())
    {
        case YAML::NodeType::Map:
            flattenMap(node, folders, is_schema, override);
            break;
        case YAML::NodeType::Sequence:
            flattenSequence(node, folders, is_schema, override);
            break;
        case YAML::NodeType::Scalar:
        default:
            break;
    }
}

void flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema, bool override)
{
    for (auto node_i : node)
    {
        flattenNode(node_i, folders, is_schema, override);
    }
}

void flattenMap(YAML::Node& node, std::vector<std::string> folders, bool is_schema, bool override)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (auto n : node)
    {
        if (n.first.as<std::string>() == "follow")
        {
            filesystem::path path_follow;

            if (is_schema)
            {
                path_follow = findFileRecursive(n.second.as<std::string>(), folders);
            }
            else
            {
                const filesystem::path follow_value(n.second.as<std::string>());
                path_follow = folders.front() / follow_value;
            }

            if (not filesystem::exists(path_follow))
            {
                throw std::runtime_error("In flattenNode: the file '" + path_follow.string() + "' does not exists");
            }

            // load "follow" file
            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            // Recursively flatten the "follow" file
            if (is_schema)
            {
                flattenNode(node_child, folders, is_schema, override);
            }
            else
            {
                flattenNode(node_child, {path_follow.parent_path().string()}, is_schema, override);
            }

            for (auto nc : node_child)
            {
                // Case schema
                if (is_schema)
                {
                    addNodeSchema(node_aux, nc.first.as<std::string>(), nc.second, override);
                }
                // Case input yaml
                else
                {
                    addNodeYaml(node_aux, nc.first.as<std::string>(), nc.second, override);
                }
            }
        }
        else
        {
            // Recursively flatten the "follow" file
            flattenNode(n.second, folders, is_schema, override);

            // Case schema
            if (is_schema)
            {
                addNodeSchema(node_aux, n.first.as<std::string>(), n.second, override);
            }
            // Case input yaml
            else
            {
                addNodeYaml(node_aux, n.first.as<std::string>(), n.second, override);
            }
        }
    }

    node = node_aux;
}

void addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value, bool override)
{
    if (node[key])
    {
        switch (value.Type())
        {
            case YAML::NodeType::Scalar: {
                if (override)
                {
                    node[key] = value;
                }
                else
                {
                    throw std::runtime_error("Trying to add an already existing scalar node.");
                }
            }
            case YAML::NodeType::Sequence: {
                for (auto value_seq_node : value)
                {
                    node[key].push_back(value_seq_node);
                }
                break;
            }
            case YAML::NodeType::Map: {
                for (auto value_map_node : value)
                {
                    YAML::Node node_key = node[key];
                    addNodeYaml(node_key, value_map_node.first.as<std::string>(), value_map_node.second, override);
                }
                break;
            }
            default: {
                throw std::runtime_error("Trying to add a node of type not known.");
            }
        }
    }
    else
    {
        node[key] = value;
    }
}

filesystem::path findFileRecursive(const std::string& name_with_extension, const std::vector<std::string>& folders)
{
    for (auto folder : folders)
    {
        if (filesystem::exists(folder) and filesystem::is_directory(folder))
        {
            for (auto const& entry : filesystem::recursive_directory_iterator(folder))
            {
                if (filesystem::is_regular_file(entry) and entry.path().filename().string() == name_with_extension)
                {
                    return entry;
                }
            }
        }
    }

    throw std::runtime_error("File '" + name_with_extension + "' not found in provided folders.");
}

void writeErrorToLog(std::stringstream& log,
                     const std::string& _acc_field,
                     const YAML::Node   _node_schema,
                     const std::string& _error_message)
{
    log << "ERROR in '" << _acc_field << "': " << _error_message << "\n";
    if (_node_schema.IsDefined())
    {
        log << "Schema specification:\n";
        writeNodeSchemaToLog(log, _acc_field, _node_schema, "  ");
    }
    log << "\n";
}

void writeNodeSchemaToLog(std::stringstream& log,
                          const std::string& _acc_field,
                          const YAML::Node   _node_schema,
                          std::string        _tabs)
{
    if (isSpecification(_node_schema))
    {
        log << _tabs << "DOC: " << _node_schema[DOC] << "\n";
        log << _tabs << "MANDATORY: " << _node_schema[MANDATORY] << "\n";
        if (_node_schema[VALUE]) log << _tabs << "VALUE: " << _node_schema[VALUE] << "\n";
        if (_node_schema[DEFAULT]) log << _tabs << "DEFAULT: " << _node_schema[DEFAULT] << "\n";
        log << _tabs << "TYPE: " << _node_schema[TYPE] << "\n";
        if (_node_schema[OPTIONS])
        {
            log << _tabs << "OPTIONS:\n";
            _tabs += " ";
            for (auto opt : _node_schema[OPTIONS]) log << _tabs << "- " << opt << "\n";
        }
    }
    else if (_node_schema.IsMap())
    {
        for (auto node_schema_child : _node_schema)
        {
            auto acc_field_child =
                (_acc_field.empty() ? "" : _acc_field + "/") + node_schema_child.first.as<std::string>();
            log << _tabs << acc_field_child << ":\n";
            writeNodeSchemaToLog(log, acc_field_child, node_schema_child.second, _tabs + "  ");
        }
    }
}

std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key)
{
    std::list<YAML::Node> nodes_with_key;

    if (root_node.IsSequence())
    {
        for (auto i = 0; i < root_node.size(); i++)
        {
            auto nodes_with_key_i = findNodesWithKey(root_node[i], key);
            nodes_with_key.insert(nodes_with_key.end(), nodes_with_key_i.begin(), nodes_with_key_i.end());
        }
    }
    if (root_node.IsMap())
    {
        if (root_node[key])
        {
            nodes_with_key.push_back(root_node[key]);
        }

        for (auto node_child : root_node)
        {
            auto node_child_with_key = findNodesWithKey(node_child.second, key);
            nodes_with_key.insert(nodes_with_key.end(), node_child_with_key.begin(), node_child_with_key.end());
        }
    }
    return nodes_with_key;
}

std::string sequenceToString(const YAML::Node& node)
{
    if (not node.IsSequence()) throw std::runtime_error("sequenceToString is only for sequence nodes");

    std::string ret("[");
    for (auto i = 0; i < node.size(); i++)
    {
        if (i > 0) ret += ", ";

        if (not node[i].IsSequence())
            ret += node[i].as<std::string>();
        else
            ret += sequenceToString(node[i]);
    }
    ret += "]";
    return ret;
}

}  // namespace yaml_schema_cpp
