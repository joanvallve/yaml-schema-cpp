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

#include "yaml-schema-cpp/expression.hpp"
#include "yaml-schema-cpp/exprtk/exprtk.hpp"
#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"

namespace yaml_schema_cpp
{
typedef exprtk::expression<double>   expression_t;
typedef exprtk::parser<double>       parser_t;
typedef exprtk::symbol_table<double> symbol_table_t;

bool isExpression(const YAML::Node& node)
{
    return node.as<std::string>().front() == '$';
}

bool isExpression(const std::string& expression)
{
    return expression.front() == '$';
}

bool checkExpressionSyntax(std::string expression)
{
    if (not isExpression(expression)) return false;

    std::vector<std::string> variable_list;

    // remove "$"
    expression.erase(expression.begin());

    return exprtk::collect_variables(expression, variable_list);
}

bool checkExpressionSchema(const YAML::Node& node_expression, const YAML::Node& node_schema_parent, std::string& err)
{
    std::string expression = node_expression.as<std::string>();

    if (not isExpression(expression))
    {
        err = "checkExpression: expression should start by '$': " + expression;
        return false;
    }

    // remove "$"
    expression.erase(expression.begin());

    // check exprtk syntax validity
    std::vector<std::string> variable_list;
    if (not exprtk::collect_variables(expression, variable_list))
    {
        err = "checkExpression: bad syntax: " + expression;
        return false;
    }
    // check variables
    else
    {
        for (const auto& var : variable_list)
        {
            // existence
            if (not node_schema_parent[var])
            {
                err = "checkExpression: coudn't find '" + var +
                      "' parameter in the schema at the same level, evaluating expression: " + expression;
                return false;
            }
            // is specification
            if (not isSpecification(node_schema_parent[var]))
            {
                err = "checkExpression: The parameter '" + var +
                      "' does not have specifications in the schema, evaluating expression: " + expression;
                return false;
            }
            // trivial type
            if (not isTrivialType(node_schema_parent[var][TYPE].as<std::string>()))
            {
                err = "checkExpression: The parameter '" + var +
                      "' is not a trivial type, evaluating expression: " + expression;
                return false;
            }
            // mandatory (bool and true)
            if (not tryNodeAs(node_schema_parent[var][MANDATORY], "bool") or
                not node_schema_parent[var][MANDATORY].as<bool>())
            {
                err = "checkExpression: The parameter '" + var +
                      "' is not mandatory (expressions with optional parameters not implemented yet), evaluating "
                      "expression: " +
                      expression;
                return false;
            }
        }
    }

    return true;
}

bool evalExpression(std::string expression_str, const YAML::Node& node_input_parent)
{
    assert(isExpression(expression_str) and "evalExpression: expression does not contain an expression");

    // remove "$"
    expression_str.erase(expression_str.begin());

    // VARIABLES
    symbol_table_t symbol_table;
    double         true_double(1), false_double(0);
    // get variable list
    std::vector<std::string> variable_list;
    std::vector<double>      values_list;
    if (exprtk::collect_variables(expression_str, variable_list))
    {
        // Add variables
        for (const auto& var : variable_list)
        {
            if (not node_input_parent[var])
            {
                throw std::runtime_error("evalExpression: coudn't find '" + var +
                                         "' parameter in the yaml file evaluating expression: " + expression_str);
            }
            if (tryNodeAs(node_input_parent[var], "bool"))
            {
                values_list.push_back(node_input_parent[var].as<bool>() ? true_double : false_double);
            }
            else
                values_list.push_back(node_input_parent[var].as<double>());

            symbol_table.add_variable(var, values_list.back(), true);
        }
    }
    else
        throw std::runtime_error("evalExpression: An error occurred collecting variables of expression: " +
                                 expression_str);

    // EXPRESSION
    expression_t expression;
    expression.register_symbol_table(symbol_table);

    // COMPILE
    parser_t parser;
    if (not parser.compile(expression_str, expression))
    {
        throw std::runtime_error("evalExpression: An error occurred compiling expression: " + expression_str);
    }

    return (bool)expression.value();
}

}  // namespace yaml_schema_cpp
