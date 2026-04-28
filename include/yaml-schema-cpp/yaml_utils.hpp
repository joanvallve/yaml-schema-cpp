#pragma once

#include <list>
#include <iostream>
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
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

std::string findFileRecursive(const std::string& name_with_extension, const std::vector<std::string>& folders);

std::string findSchema(std::string                     name_schema,
                       const std::vector<std::string>& folders,
                       std::ostream&                   log = std::cout);

std::vector<std::string> getAllSchemas(const std::vector<std::string>& root_folders);

std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key);

std::string sequenceToString(const YAML::Node& node);

/**
 * @brief evaluates string to check if it is an array type (contains [ ])
 *        Searches for the first [ and ].
 * @param type_str INPUT string containing type
 * @param size OUTPUT (if sequence type): the size of the array
 *                    (example: for double[3][5] returns 3)
 * @returns either type_str is a sequence type ('[' and ']' found)
 */
bool isArrayType(const std::string& type_str, size_t& size);

/**
 * @brief get one lower level element type (example: for double[3][5] returns double[5])
 * @param type_str INPUT string containing type
 * @returns string removing first [...] of type_str
 */
std::string getLowerElementType(const std::string type_str);

/**
 * @brief get the lowest elements type (example: for double[3][5] returns double)
 * @param type_str INPUT string containing type
 * @returns string removing all [...] of type_str
 */
std::string getLowestElementType(const std::string type_str);

/**
 * @brief Get the type to be used to check from a schema node. 
 * @returns If TYPE does not start by "derived", returns TYPE.
 *          If TYPE start by "derived", substitutes "derived" by BASE
 */
std::string getCheckType(const YAML::Node& node);


}  // namespace yaml_schema_cpp
