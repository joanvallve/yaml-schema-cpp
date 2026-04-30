#pragma once

#include <list>
#include <iostream>
#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/type_check.hpp"

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

void insertNodes(YAML::Node&              node,
                 const YAML::Node&        node_follow,
                 std::string              current_folder,
                 std::vector<std::string> schema_folders,
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
 * @returns either type_str is a sequence type ('[' and ']' found)
 */
bool isArrayType(const std::string& type_str);

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
 * @returns If TYPE does not start by "derived", returns TYPE.
 *          If TYPE start by "derived", substitutes "derived" by BASE
 */
std::string getCheckType(const YAML::Node& node);

/**
 * @brief Compare nodes of a given type
 *
 */
#define COMPARE_TYPE(TypeName)                                                                                        \
    if (type == #TypeName)                                                                                            \
    {                                                                                                                 \
        try                                                                                                           \
        {                                                                                                             \
            return node1.as<TypeName>() == node2.as<TypeName>();                                                      \
        }                                                                                                             \
        catch (const std::exception& e)                                                                               \
        {                                                                                                             \
            return false;                                                                                             \
        }                                                                                                             \
    }

#if _EIGEN_FOUND == 1
#define COMPARE_EIGEN(size)                                                                                           \
    if (type == "Vector" #size "d" or type == "Eigen::Vector" #size "d")                                              \
    {                                                                                                                 \
        try                                                                                                           \
        {                                                                                                             \
            return (node1.as<Eigen::Matrix<double, size, 1> >() - node2.as<Eigen::Matrix<double, size, 1> >())        \
                       .cwiseAbs()                                                                                    \
                       .maxCoeff() < 1e-9;                                                                            \
        }                                                                                                             \
        catch (const std::exception& e)                                                                               \
        {                                                                                                             \
            return false;                                                                                             \
        }                                                                                                             \
    }                                                                                                                 \
    if (type == "Matrix" #size "d" or type == "Eigen::Matrix" #size "d")                                              \
    {                                                                                                                 \
        try                                                                                                           \
        {                                                                                                             \
            return (node1.as<Eigen::Matrix<double, size, size> >() - node2.as<Eigen::Matrix<double, size, size> >())  \
                       .cwiseAbs()                                                                                    \
                       .maxCoeff() < 1e-9;                                                                            \
        }                                                                                                             \
        catch (const std::exception& e)                                                                               \
        {                                                                                                             \
            return false;                                                                                             \
        }                                                                                                             \
    }
#endif

/**
 * @brief Compare two nodes interpreting all scalars automatically.
 * First try as integers, second as doubles, finally as strings.
 *
 * @param node1, node2 nodes to be compared
 * @return if both nodes are equal
 */
bool compareNodesAutoType(const YAML::Node& node1, const YAML::Node& node2);

/**
 * @brief Compare two nodes interpreted as a given type
 *
 * @param node1, node2 nodes to be compared
 * @param type string containing the type
 * @param folders_schema string vector containing folders where to search for schema files
 * @return if both nodes are equal
 */
bool compare(const YAML::Node&               node1,
             const YAML::Node&               node2,
             const std::string&              type,
             const std::vector<std::string>& folders_schema);
/**
 * @brief Compare two nodes interpreted as a given type (trivial: no schema needed)
 *
 * @param node1, node2 nodes to be compared
 * @param type string containing the type
 * @return if both nodes are equal
 */
bool compareTrivial(const YAML::Node& node1, const YAML::Node& node2, const std::string& type);

/**
 * @brief Compare two nodes interpreted as a given type (nontrivial: schema needed)
 *
 * @param node1, node2 nodes to be compared
 * @param type string containing the type
 * @param folders_schema string vector containing folders where to search for schema files
 * @return if both nodes are equal
 */
bool compareNonTrivial(const YAML::Node&               node1,
                       const YAML::Node&               node2,
                       const std::string&              type,
                       const std::vector<std::string>& folders_schema);

/**
 * @brief Compare two nodes interpreted as a given type using a schema
 *
 * @param node1, node2 nodes to be compared
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
 * @param type string containing the type
 */
std::string getZeroString(const std::string& type);

}  // namespace yaml_schema_cpp
