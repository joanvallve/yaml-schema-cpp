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

void flattenNode(YAML::Node& node, std::vector<std::string> folders, bool is_schema)
{
    switch (node.Type())
    {
        case YAML::NodeType::Map:
            flattenMap(node, folders, is_schema);
            break;
        case YAML::NodeType::Sequence:
            flattenSequence(node, folders, is_schema);
            break;
        case YAML::NodeType::Scalar:
        default:
            break;
    }
}

void flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema)
{
    for (auto node_i : node)
    {
        flattenNode(node_i, folders, is_schema);
    }
}

void flattenMap(YAML::Node& node, std::vector<std::string> folders, bool is_schema)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (auto n : node)
    {
        if (n.first.as<std::string>() == "follow")
        {
            // Try to find the file
            filesystem::path path_follow = findFile(n.second.as<std::string>(), folders);

            // Case input yaml, folders has to be modified to keep relative paths
            if (not is_schema)
            {
                folders.front() = path_follow.parent_path().string();
            }
            
            // load "follow" file
            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            // Recursively flatten the "follow" file
            flattenNode(node_child, folders, is_schema);

            for (auto nc : node_child)
            {
                // Case schema
                if (is_schema)
                {
                    addNodeSchema(node_aux,nc.first.as<std::string>(),nc.second);
                }
                // Case input yaml
                else
                {
                    addNodeYaml(node_aux,nc.first.as<std::string>(),nc.second);
                }
            }
        }
        else
        {
            // Recursively flatten the "follow" file
            flattenNode(n.second, folders, is_schema);

            // Case schema
            if (is_schema)
            {
                addNodeSchema(node_aux,n.first.as<std::string>(),n.second);
            }
            // Case input yaml
            else
            {
                addNodeYaml(node_aux,n.first.as<std::string>(),n.second);
            }
        }
    }

    node = node_aux;
}

void addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value)
{
    if (node[key])
    {
        switch (value.Type()) 
        {
            case YAML::NodeType::Scalar:
            {
                throw std::runtime_error("Trying to add an already existing scalar node.");
            }
            case YAML::NodeType::Sequence:
            {
                for (auto value_seq_node : value)
                {
                    node[key].push_back(value_seq_node);
                }
                break;
            }
            case YAML::NodeType::Map:
            {
                for (auto value_map_node : value)
                {
                    YAML::Node node_key = node[key];
                    addNodeYaml(node_key, value_map_node.first.as<std::string>(), value_map_node.second);
                }
                break;
            }
            default:
            {
                throw std::runtime_error("Trying to add a node of type not known.");
            }
        }
    }
    else
    {
        node[key] = value;
    }
}

filesystem::path findFile(const std::string& name, const std::vector<std::string>& folders)
{
    for (auto folder : folders)
    {
        filesystem::path file_path = filesystem::path(folder) / filesystem::path(name);
        if (filesystem::exists(file_path))
        {
            return file_path;
        }
    }

    throw std::runtime_error("File '" + name + "' not found in provided folters");
}

void writeToLog(std::stringstream& log, const std::string& message)
{
    log << message << std::endl;
}

}  // namespace yaml_schema_cpp
