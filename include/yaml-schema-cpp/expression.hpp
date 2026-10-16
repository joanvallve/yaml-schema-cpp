/**
 * @file expression.hpp
 * @brief Boolean expressions in the MANDATORY key of schema specifications.
 *
 * Expressions start with '$' and follow the exprtk syntax (http://www.partow.net/programming/exprtk). The symbols are
 * the names of other parameters at the same level in the schema/input (e.g. "$mode == 'auto' and n_threads > 0").
 */
#pragma once

#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
/**
 * @brief Check if a node is an expression.
 * @param node YAML node
 * @return true if node is a scalar starting with '$'
 */
bool isExpression(const YAML::Node& node);

/**
 * @brief Check if a string is an expression.
 * @param expression string
 * @return true if expression starts with '$'
 */
bool isExpression(const std::string& expression);

/**
 * @brief Check the syntax of an expression in a schema.
 *
 * All symbols in the expression should be parameters specified in node_schema_parent (i.e. at the same level) of
 * basic type (see isBasicType()) or string type, either mandatory (MANDATORY true) or with DEFAULT.
 *
 * @param node_expression node containing the expression (starting with '$')
 * @param node_schema_parent schema node containing the specifications of the parameters in the expression
 * @param err OUTPUT error message (if not valid)
 * @return true if the expression is valid
 */
bool checkExpression(const YAML::Node& node_expression, const YAML::Node& node_schema_parent, std::string& err);

/**
 * @brief Evaluate an expression with the values of an input node.
 *
 * The values of the symbols are taken from node_input_parent. Their type (bool, number or string) is taken from the
 * specifications in node_schema_parent if available, otherwise it is deduced from the value.
 *
 * @param expression_str expression (starting with '$')
 * @param node_input_parent input node containing the parameters in the expression
 * @param node_schema_parent schema node containing the specifications of the parameters (undefined if not available)
 * @return result of the expression casted to bool (numeric values are true except for 0)
 * @throws std::runtime_error if the expression cannot be compiled (e.g. wrong syntax or a parameter is missing)
 */
bool evalExpression(std::string       expression_str,
                    const YAML::Node& node_input_parent,
                    const YAML::Node& node_schema_parent = YAML::Node(YAML::NodeType::Undefined));

/**
 * @brief Remove the leading '$' of an expression (if any).
 * @param expression_str INPUT/OUTPUT expression
 */
void preProcessExpression(std::string& expression_str);

}  // namespace yaml_schema_cpp
