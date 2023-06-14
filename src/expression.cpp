// Copyright (C) 2022,2023 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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
typedef exprtk::expression<double>                 expression_t;
typedef exprtk::parser<double>                     parser_t;
typedef exprtk::symbol_table<double>               symbol_table_t;
typedef typename parser_t::unknown_symbol_resolver usr_t;

struct yaml_USR : public parser_t::unknown_symbol_resolver
{
    YAML::Node node_;

    yaml_USR(const YAML::Node& _node) : usr_t(usr_t::e_usrmode_extended), node_(_node) {}

    virtual bool process(const std::string& unknown_symbol, symbol_table_t& symbol_table, std::string& error_message)
    {
        bool result = false;

        // inexistent
        if (not node_[unknown_symbol])
        {
            error_message = "Indeterminable symbol type.";
        }
        // boolean
        else if (tryNodeAs(node_[unknown_symbol], "bool"))
        {
            double value = node_[unknown_symbol].as<bool>() ? 1 : 0;
            result       = symbol_table.create_variable(unknown_symbol, value);

            if (not result)
            {
                error_message = "Failed to create variable " + unknown_symbol + " with value " + std::to_string(value);
            }
        }
        // scalar
        else if (tryNodeAs(node_[unknown_symbol], "double"))
        {
            double value = node_[unknown_symbol].as<double>();
            result       = symbol_table.create_variable(unknown_symbol, value);

            if (not result)
            {
                error_message = "Failed to create variable " + unknown_symbol + " with value " + std::to_string(value);
            }
        }
        // string (last one since it is always possible to take as string)
        else if (tryNodeAs(node_[unknown_symbol], "string"))
        {
            std::string string_val = node_[unknown_symbol].as<std::string>();
            result                 = symbol_table.create_stringvar(unknown_symbol, string_val);

            if (not result)
            {
                error_message = "Failed to create string variable " + unknown_symbol + " with value " + string_val;
            }
        }
        else
        {
            error_message = "yaml_USR: unknown case.";
        }
        return result;
    }
};

struct schema_USR : public parser_t::unknown_symbol_resolver
{
    YAML::Node schema_;

    schema_USR(const YAML::Node& _schema) : usr_t(usr_t::e_usrmode_extended), schema_(_schema) {}

    virtual bool process(const std::string& unknown_symbol, symbol_table_t& symbol_table, std::string& error_message)
    {
        bool result = false;

        // inexistent
        if (not schema_[unknown_symbol])
        {
            error_message = "coudn't find '" + unknown_symbol + "' parameter in the schema at the same level.";
        }
        // is specification
        else if (not isSpecification(schema_[unknown_symbol]))
        {
            error_message = "The parameter '" + unknown_symbol + "' does not have specifications in the schema.";
        }
        // basic type (bool, scalar) or string
        else if (not isBasicType(schema_[unknown_symbol][TYPE].as<std::string>()) and
                 not isStringType(schema_[unknown_symbol][TYPE].as<std::string>()))
        {
            error_message = "checkExpression: The parameter '" + unknown_symbol + "' is not a basic type";
        }
        // mandatory (bool and true or with default
        else if (not(tryNodeAs(schema_[unknown_symbol][MANDATORY], "bool") and
                     (schema_[unknown_symbol][MANDATORY].as<bool>() or schema_[unknown_symbol][DEFAULT])))
        {
            error_message = "checkExpression: The parameter '" + unknown_symbol +
                            "' is not mandatory (expressions with optional parameters not implemented yet)";
        }
        // scalar
        else if (isBasicType(schema_[unknown_symbol][TYPE].as<std::string>()))
        {
            result = symbol_table.create_variable(unknown_symbol, 0);  // initialize to 0/false, just checking syntax

            if (not result)
            {
                error_message = "Failed to create variable " + unknown_symbol;
            }
        }
        // string
        else if (isStringType(schema_[unknown_symbol][TYPE].as<std::string>()))
        {
            result = symbol_table.create_stringvar(unknown_symbol, "");

            if (not result)
            {
                error_message = "Failed to create string variable " + unknown_symbol;
            }
        }
        else
        {
            error_message = "schema_USR: unknown case.";
        }
        return result;
    }
};

bool isExpression(const YAML::Node& node)
{
    return node.as<std::string>().front() == '$';
}

bool isExpression(const std::string& expression)
{
    return expression.front() == '$';
}

bool checkExpression(const YAML::Node& node_expression, const YAML::Node& node_schema_parent, std::string& err)
{
    std::string expression_str = node_expression.as<std::string>();

    if (not isExpression(expression_str))
    {
        err = "checkExpression: expression should start by '$': " + expression_str;
        return false;
    }

    // Preprocess: remove '$'
    preProcessExpression(expression_str);

    // Parser with our symbol resolver for schemas
    expression_t   expression;
    symbol_table_t symbol_table;
    schema_USR     schema_usr(node_schema_parent);
    parser_t       parser;
    expression.register_symbol_table(symbol_table);
    parser.enable_unknown_symbol_resolver(&schema_usr);
    parser.compile(expression_str, expression);

    // check exprtk syntax validity
    if (not parser.compile(expression_str, expression))
    {
        err = "checkExpression: bad syntax:";
        for (std::size_t i = 0; i < parser.error_count(); ++i)
        {
            auto error = parser.get_error(i);
            err += "\nError " + std::to_string(i) + " (position " + std::to_string(error.token.position) +
                   "): " + error.diagnostic.c_str();
        }

        return false;
    }

    return true;
}

bool evalExpression(std::string expression_str, const YAML::Node& node_input_parent)
{
    assert(isExpression(expression_str) and "evalExpression: expression does not contain an expression");

    // Preprocess: remove '$'
    preProcessExpression(expression_str);

    // Parser with our symbol resolver for schemas
    expression_t   expression;
    symbol_table_t symbol_table;
    yaml_USR       yaml_usr(node_input_parent);
    parser_t       parser;
    expression.register_symbol_table(symbol_table);
    parser.enable_unknown_symbol_resolver(&yaml_usr);
    parser.compile(expression_str, expression);

    // check exprtk syntax validity
    if (not parser.compile(expression_str, expression))
    {
        std::string err = "bad syntax:";
        for (std::size_t i = 0; i < parser.error_count(); ++i)
        {
            auto error = parser.get_error(i);
            err += "\nError " + std::to_string(i) + " (position " + std::to_string(error.token.position) +
                   "): " + error.diagnostic.c_str();
        }

        throw std::runtime_error("evalExpression: An error occurred compiling expression: " + expression_str + " | " +
                                 err);
    }

    return (bool)expression.value();
}

void preProcessExpression(std::string& expression_str)
{
    // remove first '$' if there
    if (expression_str.front() == '$') expression_str.erase(expression_str.begin());
}

}  // namespace yaml_schema_cpp
