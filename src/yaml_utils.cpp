// Copyright (C) 2022 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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
            const filesystem::path follow_value(n.second.as<std::string>());
            filesystem::path       path_follow;

            if (*follow_value.begin() == "..")
            {
                // TODO: the following might be a little inconsistent as it takes the first value in folders to append
                // the relative path (it might be a specific argument of the function instead)
                path_follow = folders.front() / follow_value;

                if (not filesystem::exists(path_follow))
                {
                    throw std::runtime_error("In flattenNode: the file '" + path_follow.string() +
                                             "' does not exists");
                }
            }
            else
            {
                // Try to find the file
                path_follow = findFileRecursive(n.second.as<std::string>(), folders);

                // Case input yaml, folders has to be modified to keep relative paths
                if (not is_schema)
                {
                    folders.front() = path_follow.parent_path().string();
                }
            }

            // load "follow" file
            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            // Recursively flatten the "follow" file
            flattenNode(node_child, folders, is_schema, override);

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

    throw std::runtime_error("File '" + name_with_extension + "' not found in provided folders");
}

void writeToLog(std::stringstream& log, const std::string& message)
{
    log << message;
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

}  // namespace yaml_schema_cpp
