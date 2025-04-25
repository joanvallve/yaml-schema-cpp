#pragma once

#include <list>
#include <filesystem>
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
namespace filesystem = std::filesystem;
// 'follow' behavior:
// -> schema node (is_schema = true): only allowed single file names. It will be searched inside folders specified by
// 'folders'
// -> input node (is_schema = false): path to file allowed (either relative or absolute). Relative paths will be done
// with respect to the first item specified in folders. No search will be performed.
void flattenNode(YAML::Node&              node,
                 std::string              current_folder,
                 std::vector<std::string> schema_folders,
                 bool                     is_schema,
                 bool                     override);
void flattenMap(YAML::Node&              node,
                std::string              current_folder,
                std::vector<std::string> schema_folders,
                bool                     is_schema,
                bool                     override);
void flattenSequence(YAML::Node&              node,
                     std::string              current_folder,
                     std::vector<std::string> schema_folders,
                     bool                     is_schema,
                     bool                     override);

void writeErrorToLog(std::stringstream& log,
                     const std::string& _acc_field,
                     const YAML::Node   _node_schema,
                     const std::string& _error_message);
void writeNodeSchemaToLog(std::stringstream& log,
                          const std::string& _acc_field,
                          const YAML::Node   _node_schema,
                          std::string        _tabs = "");

void addNodeYaml(YAML::Node&        node,
                 const std::string& key,
                 const YAML::Node&  value,
                 bool               override,
                 std::string        parent_path = "");

filesystem::path findFileRecursive(const std::string& name_with_extension, const std::vector<std::string>& folders);

std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key);

std::string sequenceToString(const YAML::Node& node);

}  // namespace yaml_schema_cpp
