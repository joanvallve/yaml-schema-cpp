/**
 * @file type_check.hpp
 * @brief Checking if YAML nodes can be interpreted as a given type (specified as a string).
 *
 * Types are specified as strings: basic types ("bool", "char", "int", "unsigned int", "long int",
 * "long unsigned int", "float", "double"), string types ("string", "std::string"), Eigen types ("VectorXd",
 * "MatrixXd", "Vector<N>d", "Matrix<N>d" for N in 1..10, with or without "Eigen::", only if Eigen was found) and array
 * types ("<type>[]" or "<type>[<size>]", e.g. "double[3][]").
 */
#pragma once

#include "yaml-schema-cpp/internal/config.h"

#if _EIGEN_FOUND == 1
#include <Eigen/Dense>
#endif

#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
/**
 * @brief check the sizes of node and type
 * If the type is an array type, it checks the node is a sequence.
 * If the array has size, it checks the sequence size matches.
 *
 * @param node YAML node
 * @param type string specifying a type
 * @return if node and type agree on sizes (both scalars or arrays of correct sizes)
 */
bool checkSizes(const YAML::Node& node, const std::string& type);

/**
 * @brief check if a node can be interpreted as a given basic type (or array of basic types)
 * NOTE: This function may raise exceptions (if the type is basic but the conversion fails)
 *
 * @param node YAML node to be converted
 * @param type string defining the type
 * @return false if type is not basic (or sizes do not match), true if check ok.
 */
bool checkNodeAsBasic(const YAML::Node& node, const std::string& type);

/**
 * @brief check if a node can be interpreted as a given string type (or array of string types)
 * NOTE: This function may raise exceptions (if the type is string but the conversion fails)
 *
 * @param node YAML node to be converted
 * @param type string defining the type
 * @return false if type is not string (or sizes do not match), true if check ok.
 */
bool checkNodeAsString(const YAML::Node& node, const std::string& type);

#if _EIGEN_FOUND == 1
/**
 * @brief check if a node can be interpreted as a given Eigen type (or array of Eigen types)
 * NOTE: This function may raise exceptions (if the type is Eigen but the conversion fails)
 *
 * @param node YAML node to be converted
 * @param type string defining the type
 * @return false if type is not Eigen (or sizes do not match), true if check ok.
 */
bool checkNodeAsEigen(const YAML::Node& node, const std::string& type);
#endif

/**
 * @brief check if a node can be interpreted as a given trivial type (or array of trivial types)
 * NOTE: This function may raise exceptions (if the type is trivial but the conversion fails)
 *
 * @param node YAML node to be converted
 * @param type string defining the type
 * @return false if type is not trivial (or sizes do not match), true if check ok.
 */
bool checkNodeAs(const YAML::Node& node, const std::string& type);

/**
 * @brief try if a node can be interpreted as a given type
 * NOTE: This function does not raise exceptions
 * NOTE: A null node can be interpreted as a string ("null"), see applySchema() for input validation.
 *
 * @param node YAML node to be converted
 * @param type string defining the type (trivial type or array of trivial types)
 * @return if the node can be interpreted as the type
 */
bool tryNodeAs(const YAML::Node& node, const std::string& type);

/**
 * @brief check if a type is trivial: basic, string or Eigen (not requiring a schema)
 * @param type string defining the type
 * @return true if trivial (false for array types and malformed type strings)
 */
bool isTrivialType(const std::string& type);

/**
 * @brief check if a type is basic: bool, char, int, unsigned int, long int, long unsigned int, float or double
 * @param type string defining the type
 * @return true if basic (false for array types and malformed type strings)
 */
bool isBasicType(const std::string& type);

/**
 * @brief check if a type is string: string or std::string
 * @param type string defining the type
 * @return true if string (false for array types and malformed type strings)
 */
bool isStringType(const std::string& type);

#if _EIGEN_FOUND == 1
/**
 * @brief check if a type is Eigen: VectorXd, MatrixXd, Vector<N>d or Matrix<N>d (N from 1 to 10), with or without
 *        "Eigen::"
 * @param type string defining the type
 * @return true if Eigen (false for array types and malformed type strings)
 */
bool isEigenType(const std::string& type);
#endif

/**
 * @brief check if a type is non-trivial: a schema file with that name exists in folders (see findSchema())
 * @param type string defining the type (name of the schema)
 * @param folders folders where the schema files are searched (recursively)
 * @return true if the schema file is found
 */
bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders);

}  // namespace yaml_schema_cpp
