/**
 * @file yaml_utils.hpp
 * @brief Utilities: flattening of YAML nodes ('follow'), file search, type strings, logging and node comparison.
 */
#pragma once

#include <list>
#include <iostream>
#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/type_check.hpp"

namespace yaml_schema_cpp
{
/**
 * @brief Flatten a node: resolve recursively all 'follow' keys, replacing them by the contents of the followed files.
 *
 * The value of 'follow' can be a file or a sequence of files:
 *  - a schema file (extension SCHEMA_EXTENSION, or no extension): only the file name, it is searched recursively in
 *    schema_folders (see findFileRecursive()).
 *  - an input YAML file (extension YAML_EXTENSION): absolute path or relative to current_folder. No search is done.
 *
 * The contents of the followed file are merged with the rest of the node (see addNodeSchema() and addNodeYaml()).
 *
 * In input YAML nodes (is_schema false), relative paths (scalar values starting with "./" or "../") are prepended
 * with the folder of the file where they are defined (current_folder for this node).
 *
 * @param node INPUT/OUTPUT node to be flattened
 * @param current_folder folder of the file containing node
 * @param schema_folders folders where the followed schema files are searched (recursively)
 * @param is_schema true if node is a schema node, false if it is an input YAML node
 * @param override if true, repeated keys are overridden, otherwise it is an error
 * @throws std::runtime_error if a followed file is not found, has a wrong extension or cannot be merged, or if there
 *         is a 'follow' cycle (a file that follows, directly or indirectly, itself)
 */
void flattenNode(YAML::Node&              node,
                 std::string              current_folder,
                 std::vector<std::string> schema_folders,
                 bool                     is_schema,
                 bool                     override);

/**
 * @brief Flatten a map node (see flattenNode()).
 * @param node INPUT/OUTPUT map node to be flattened
 * @param current_folder folder of the file containing node
 * @param schema_folders folders where the followed schema files are searched (recursively)
 * @param is_schema true if node is a schema node, false if it is an input YAML node
 * @param override if true, repeated keys are overridden, otherwise it is an error
 * @throws std::runtime_error (see flattenNode())
 */
void flattenMap(YAML::Node&              node,
                std::string              current_folder,
                std::vector<std::string> schema_folders,
                bool                     is_schema,
                bool                     override);

/**
 * @brief Flatten all elements of a sequence node (see flattenNode()).
 * @param node INPUT/OUTPUT sequence node to be flattened
 * @param current_folder folder of the file containing node
 * @param schema_folders folders where the followed schema files are searched (recursively)
 * @param is_schema true if node is a schema node, false if it is an input YAML node
 * @param override if true, repeated keys are overridden, otherwise it is an error
 * @throws std::runtime_error (see flattenNode())
 */
void flattenSequence(YAML::Node&              node,
                     std::string              current_folder,
                     std::vector<std::string> schema_folders,
                     bool                     is_schema,
                     bool                     override);

/**
 * @brief Load the file(s) of a 'follow' value, flatten them and add their contents to a node (see flattenNode()).
 * @param node node where the contents are added
 * @param node_follow value of 'follow': a file name/path or a sequence of them
 * @param current_folder folder of the file containing the 'follow' key
 * @param schema_folders folders where the followed schema files are searched (recursively)
 * @param override if true, repeated keys are overridden, otherwise it is an error
 * @throws std::runtime_error (see flattenNode())
 */
void insertNodes(YAML::Node&              node,
                 const YAML::Node&        node_follow,
                 std::string              current_folder,
                 std::vector<std::string> schema_folders,
                 bool                     override);

/**
 * @brief Write an error in the log: "ERROR in '<field>': <message>", followed by the schema specification (if any).
 * @param log OUTPUT stream where the error is written
 * @param _acc_field accumulated path of the field (e.g. "map1/param2[0]")
 * @param _node_schema schema node of the field (undefined to not write it)
 * @param _error_message error message
 */
void writeErrorToLog(std::stringstream& log,
                     const std::string& _acc_field,
                     const YAML::Node   _node_schema,
                     const std::string& _error_message);

/**
 * @brief Write (recursively) a schema node in the log in a readable way.
 * @param log OUTPUT stream where the schema node is written
 * @param _acc_field accumulated path of the field (e.g. "map1/param2")
 * @param _node_schema schema node to be written
 * @param _tabs indentation prepended to each line
 */
void writeNodeSchemaToLog(std::stringstream& log,
                          const std::string& _acc_field,
                          const YAML::Node   _node_schema,
                          std::string        _tabs = "");

/**
 * @brief Add an input YAML node to node[key], merging it with the existing one (used to flatten input YAML nodes).
 *
 *  - if node[key] does not exist, it is set to value.
 *  - if value is a scalar, node[key] is replaced if override, otherwise an exception is thrown.
 *  - if value is a sequence, its elements are appended to node[key].
 *  - if value is a map, all its children are added recursively.
 *
 * If parent_path is not empty and the resulting node[key] is a relative path (see isRelativePath()), parent_path is
 * prepended.
 *
 * @param node node where value is added
 * @param key key in node where value is added
 * @param value node to be added
 * @param override if true, existing scalars are overridden, otherwise it is an error
 * @param parent_path path prepended to relative path values ("" to leave them unchanged)
 * @throws std::runtime_error if a scalar exists and not override, or value is of unknown type
 */
void addNodeYaml(YAML::Node&        node,
                 const std::string& key,
                 const YAML::Node&  value,
                 bool               override,
                 std::string        parent_path = "");

/**
 * @brief Check if a string is a relative path.
 * @param value string
 * @return true if value starts with "./" or "../"
 */
bool isRelativePath(const std::string& value);

/**
 * @brief Search recursively a file in folders (in order).
 * @param name_with_extension name of the file (with extension, without path)
 * @param folders folders where the file is searched (recursively). Folders that do not exist are skipped.
 * @return the path of the first regular file found with the given name
 * @throws std::runtime_error if the file is not found (the message lists the folders) or a folder cannot be iterated
 */
std::string findFileRecursive(const std::string& name_with_extension, const std::vector<std::string>& folders);

/**
 * @brief Search recursively a schema file in folders (see findFileRecursive()).
 * @param name_schema name of the schema file, with or without extension SCHEMA_EXTENSION (without path)
 * @param folders folders where the schema file is searched (recursively)
 * @param log OUTPUT stream where the error is written (if any)
 * @return the path of the schema file, or an empty string if not found or wrong extension
 */
std::string findSchema(std::string                     name_schema,
                       const std::vector<std::string>& folders,
                       std::ostream&                   log = std::cout);

/**
 * @brief Get the names of all schema files found recursively in folders.
 * @param root_folders folders where the schema files are searched (recursively). Folders that do not exist are
 *        skipped.
 * @return the file names (without path) of all files with extension SCHEMA_EXTENSION
 */
std::vector<std::string> getAllSchemas(const std::vector<std::string>& root_folders);

/**
 * @brief Find (recursively, in maps and sequences) all the nodes with a given key.
 * @param root_node node where the keys are searched
 * @param key key to be found
 * @return the values of all the occurrences of key
 */
std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key);

/**
 * @brief Convert a sequence node (of scalars or sequences) into a flow-style string (e.g. "[1, 2, [a, b]]").
 * @param node sequence node
 * @return the string
 * @throws std::runtime_error if node is not a sequence (YAML::Exception if any element is a map)
 */
std::string sequenceToString(const YAML::Node& node);

/**
 * @brief evaluates string to check if it is an array type (contains [ ])
 *        Searches for the first [ and ].
 * @param type_str INPUT string containing type
 * @returns either type_str is a sequence type ('[' and ']' found)
 * @throws std::runtime_error if the brackets are malformed or the size is not a positive integer
 */
bool isArrayType(const std::string& type_str);

/**
 * @brief evaluates string to check if it is an array type (contains [ ])
 *        Searches for the first [ and ].
 * @param type_str INPUT string containing type
 * @param size OUTPUT (if sequence type): the size of the array, 0 if not specified
 *                    (example: for double[3][5] returns 3)
 * @returns either type_str is a sequence type ('[' and ']' found)
 * @throws std::runtime_error if the brackets are malformed or the size is not a positive integer
 */
bool isArrayType(const std::string& type_str, size_t& size);

/**
 * @brief evaluates string to check if it is a derived type ("derived" or "derived[..]")
 * @param type_str INPUT string containing type
 * @returns either type_str is derived
 */
bool isDerivedType(const std::string& type_str);

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
 * @param node specification node
 * @returns If TYPE does not start by "derived", returns TYPE.
 *          If TYPE start by "derived", substitutes "derived" by BASE (e.g. "derived[]" -> "MyBase[]")
 * @throws std::runtime_error if node does not have TYPE, or BASE (if derived)
 */
std::string getCheckType(const YAML::Node& node);

/**
 * @brief Compare two nodes interpreting all scalars automatically.
 * First try as integers, second as doubles, finally as strings. Maps and sequences are compared recursively.
 * Two undefined nodes or two null nodes are equal.
 *
 * @param node1 node to be compared
 * @param node2 node to be compared
 * @return if both nodes are equal
 */
bool compareNodesAutoType(const YAML::Node& node1, const YAML::Node& node2);

/**
 * @brief Compare two nodes interpreted as a given type
 * Array types are compared element by element. Trivial types are compared with compareTrivial() and the rest with
 * compareNonTrivial().
 *
 * @param node1 node to be compared
 * @param node2 node to be compared
 * @param type string containing the type
 * @param folders_schema string vector containing folders where to search for schema files
 * @return if both nodes are equal (false if any of them is undefined or null)
 */
bool compare(const YAML::Node&               node1,
             const YAML::Node&               node2,
             const std::string&              type,
             const std::vector<std::string>& folders_schema);
/**
 * @brief Compare two nodes interpreted as a given type (trivial: no schema needed)
 * Eigen types are compared with a tolerance of 1e-9.
 *
 * @param node1 node to be compared
 * @param node2 node to be compared
 * @param type string containing the type
 * @return if both nodes are equal (false if any of them is undefined, null or cannot be converted)
 * @throws std::runtime_error if type is not a trivial type
 */
bool compareTrivial(const YAML::Node& node1, const YAML::Node& node2, const std::string& type);

/**
 * @brief Compare two nodes interpreted as a given type (nontrivial: schema needed)
 * The schema is loaded and flattened (but not checked) and the nodes are compared with compareNonTrivialSchema().
 *
 * @param node1 node to be compared
 * @param node2 node to be compared
 * @param type string containing the type (name of the schema)
 * @param folders_schema string vector containing folders where to search for schema files
 * @return if both nodes are equal (false if any of them is undefined or null, or the schema cannot be loaded)
 */
bool compareNonTrivial(const YAML::Node&               node1,
                       const YAML::Node&               node2,
                       const std::string&              type,
                       const std::vector<std::string>& folders_schema);

/**
 * @brief Compare two nodes interpreted as a given type using a schema
 * Only the fields specified in the schema are compared. Optional fields (MANDATORY false or expression) missing in
 * both nodes are considered equal.
 *
 * @param node1 node to be compared
 * @param node2 node to be compared
 * @param node_schema schema specifying structure and types of the nodes
 * @param folders_schema string vector containing folders where to search for schema files
 * @return if both nodes are equal
 */
bool compareNonTrivialSchema(const YAML::Node&               node1,
                             const YAML::Node&               node2,
                             const YAML::Node&               node_schema,
                             const std::vector<std::string>& folders_schema);
/**
 * @brief Return string with zero value of given type
 * Arrays of unspecified size (and dynamic Eigen types) have 3 elements.
 * @param type string containing the type
 * @return the zero value as YAML text (e.g. "0.0", "[0.0, 0.0]", "false"), or "type unknown!" for non-trivial types
 */
std::string getZeroString(const std::string& type);

}  // namespace yaml_schema_cpp
