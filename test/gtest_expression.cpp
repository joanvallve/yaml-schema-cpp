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

#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"
#include "yaml-schema-cpp/expression.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(TestExpression, isExpression)
{
    ASSERT_FALSE(isExpression("enabled"));
    ASSERT_TRUE(isExpression("$ It only checks if first char is '$' !·$%&/()^\n:-*+-"));
}

TEST(TestExpression, checkExpressionSyntax)
{
    ASSERT_TRUE(checkExpressionSyntax("$enable"));
    ASSERT_TRUE(checkExpressionSyntax("$not(enable)"));
    ASSERT_FALSE(checkExpressionSyntax("3*2-4"));    // missing $
    ASSERT_FALSE(checkExpressionSyntax("$3-*2-4"));  // wrong expression
    ASSERT_FALSE(checkExpressionSyntax("$not 3"));   // wrong expression
}

TEST(TestExpression, evalExpression)
{
    YAML::Node node_input;
    node_input["enabled"]      = true;
    node_input["disabled"]     = false;
    node_input["param_int"]    = 3;
    node_input["param_double"] = 3.1;

    ASSERT_TRUE(evalExpression("$enabled", node_input));
    ASSERT_FALSE(evalExpression("$disabled", node_input));

    ASSERT_TRUE(evalExpression("$not( disabled )", node_input));
    ASSERT_FALSE(evalExpression("$ not(enabled)", node_input));

    ASSERT_TRUE(evalExpression("$disabled or  not(disabled)", node_input));
    ASSERT_FALSE(evalExpression("$enabled  and (not(enabled))", node_input));

    ASSERT_FALSE(evalExpression("$param_int  < 3", node_input));
    ASSERT_TRUE(evalExpression("$param_int <= 3", node_input));
    ASSERT_TRUE(evalExpression("$param_int>= 3", node_input));
    ASSERT_FALSE(evalExpression("$param_int >   3", node_input));

    ASSERT_FALSE(evalExpression("$param_double  < 3", node_input));
    ASSERT_FALSE(evalExpression("$param_double <= 3", node_input));
    ASSERT_TRUE(evalExpression("$param_double>= 3", node_input));
    ASSERT_TRUE(evalExpression("$param_double >   3", node_input));

    ASSERT_FALSE(evalExpression("$enabled and param_int <3", node_input));
    ASSERT_TRUE(evalExpression("$enabled and param_int <=  3", node_input));
    ASSERT_TRUE(evalExpression("$enabled and param_int  >= 3", node_input));
    ASSERT_FALSE(evalExpression("$enabled and param_int>3", node_input));

    ASSERT_THROW(evalExpression("$param2 > 3", node_input), std::runtime_error);
}

TEST(TestExpression, isExpreYaml)
{
    YAML::Node node_schema = YAML::LoadFile(ROOT_DIR + "/test/schema/folder_schema/expression.schema");

    ASSERT_FALSE(isExpression(node_schema["enabled"][MANDATORY]));
    ASSERT_TRUE(isExpression(node_schema["param_expr1"][MANDATORY]));
}

TEST(TestExpression, checkExpressionYaml)
{
    YAML::Node node_schema        = YAML::LoadFile(ROOT_DIR + "/test/schema/folder_schema/expression.schema");
    YAML::Node node_schema_wrong  = YAML::LoadFile(ROOT_DIR + "/test/wrong_schema/wrong_expression.schema");
    YAML::Node node_schema_wrong2 = YAML::LoadFile(ROOT_DIR + "/test/wrong_schema/wrong_expression2.schema");

    std::string err_msg;
    ASSERT_TRUE(checkExpressionSchema(node_schema["param_expr1"][MANDATORY], node_schema, err_msg));
    ASSERT_FALSE(checkExpressionSchema(node_schema_wrong["param_expr1"][MANDATORY], node_schema_wrong, err_msg));
    std::cout << err_msg << std::endl;
    ASSERT_FALSE(checkExpressionSchema(node_schema_wrong2["param_expr2"][MANDATORY], node_schema_wrong2, err_msg));
    std::cout << err_msg << std::endl;
}

TEST(TestExpression, evalExpressionYaml)
{
    YAML::Node node_input;
    node_input["enabled"]         = true;
    node_input["disabled"]        = false;
    node_input["param_int"]       = 3;
    node_input["param_double"]    = 3.1;
    YAML::Node node_schema        = YAML::LoadFile(ROOT_DIR + "/test/schema/folder_schema/expression.schema");
    YAML::Node node_schema_wrong  = YAML::LoadFile(ROOT_DIR + "/test/wrong_schema/wrong_expression.schema");
    YAML::Node node_schema_wrong2 = YAML::LoadFile(ROOT_DIR + "/test/wrong_schema/wrong_expression2.schema");

    ASSERT_TRUE(evalExpression(node_schema["param_expr1"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr2"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr3"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr4"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr5"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr6"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr7"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr8"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr9"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr10"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr11"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr12"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr13"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr14"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr15"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr16"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr17"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr18"][MANDATORY].as<std::string>(), node_input));

    ASSERT_THROW(evalExpression(node_schema_wrong["param_expr1"][MANDATORY].as<std::string>(), node_input),
                 std::runtime_error);
    ASSERT_THROW(evalExpression(node_schema_wrong2["param_expr2"][MANDATORY].as<std::string>(), node_input),
                 std::runtime_error);
}

TEST(TestExpression, applySchema)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/expression_input1.yaml",
                                         ROOT_DIR + "/test/yaml/expression_input2.yaml"};
    for (auto input : input_yamls)
    {
        std::cout << "testing " << input << std::endl;
        YamlServer server = YamlServer({ROOT_DIR}, input);

        std::cout << "before: \n" << server.getNode() << std::endl;

        ASSERT_TRUE(server.applySchema("expression.schema"));

        std::cout << "after: \n" << server.getNode() << std::endl;
    }
}

TEST(TestExpression, applySchemaWrong)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/expression_input_wrong1.yaml",
                                         ROOT_DIR + "/test/yaml/expression_input_wrong2.yaml"};
    for (auto input : input_yamls)
    {
        std::cout << "testing " << input << std::endl;
        YamlServer server = YamlServer({ROOT_DIR}, input);

        bool succeed = server.applySchema("expression.schema");

        if (succeed)
            std::cout << "resulting node: " << server.getNode() << std::endl;
        else
            std::cout << server.getLog() << std::endl;

        ASSERT_FALSE(succeed);
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
