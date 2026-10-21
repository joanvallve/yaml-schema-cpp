

#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"
#include "yaml-schema-cpp/expression.hpp"

std::string ROOT_DIR   = _YAML_SCHEMA_CPP_ROOT_DIR;
std::string SCHEMA_DIR = ROOT_DIR + "/test/schema";

using namespace yaml_schema_cpp;

TEST(TestExpression, isExpression)
{
    ASSERT_FALSE(isExpression("enabled"));
    ASSERT_TRUE(isExpression("$ It only checks if first char is '$' !·$%&/()^\n:-*+-"));
}

TEST(TestExpression, evalExpression)
{
    YAML::Node node_input;
    node_input["enabled"]      = true;
    node_input["disabled"]     = false;
    node_input["mode"]         = "auto";
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

    ASSERT_TRUE(evalExpression("$mode == 'auto'", node_input));
    ASSERT_FALSE(evalExpression("$not(mode == 'auto')", node_input));
    ASSERT_FALSE(evalExpression("$mode <> 'auto'", node_input));
    ASSERT_FALSE(evalExpression("$mode == 'manual'", node_input));
    ASSERT_TRUE(evalExpression("$mode <> 'manual'", node_input));

    ASSERT_THROW(evalExpression("$param2 > 3", node_input), std::runtime_error);
    ASSERT_THROW(evalExpression("$param2 > 'manual'", node_input), std::runtime_error);
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
    ASSERT_TRUE(checkExpression(node_schema["param_expr1"][MANDATORY], node_schema, err_msg));
    ASSERT_FALSE(checkExpression(node_schema_wrong["param_expr1"][MANDATORY], node_schema_wrong, err_msg));
    std::cout << err_msg << std::endl;
    ASSERT_FALSE(checkExpression(node_schema_wrong2["param_expr2"][MANDATORY], node_schema_wrong2, err_msg));
    std::cout << err_msg << std::endl;
}

TEST(TestExpression, evalExpressionYaml)
{
    YAML::Node node_input;
    node_input["enabled"]         = true;
    node_input["disabled"]        = false;
    node_input["param_int"]       = 3;
    node_input["param_double"]    = 3.1;
    node_input["mode"]            = "auto";
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
    ASSERT_TRUE(evalExpression(node_schema["param_expr19"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr20"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr21"][MANDATORY].as<std::string>(), node_input));
    ASSERT_FALSE(evalExpression(node_schema["param_expr22"][MANDATORY].as<std::string>(), node_input));
    ASSERT_TRUE(evalExpression(node_schema["param_expr23"][MANDATORY].as<std::string>(), node_input));

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
        YamlServer server = YamlServer({SCHEMA_DIR}, input);

        // std::cout << "before: \n" << server.getNode() << std::endl;

        bool succeed = server.applySchema("expression.schema");

        if (succeed)
            std::cout << "resulting node: " << server.getNode() << std::endl;
        else
            std::cout << server.getLog() << std::endl;

        ASSERT_TRUE(succeed);
    }
}

TEST(TestExpression, applySchemaWrong)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/expression_input_wrong1.yaml",
                                         ROOT_DIR + "/test/yaml/expression_input_wrong2.yaml"};
    for (auto input : input_yamls)
    {
        std::cout << "testing " << input << std::endl;
        YamlServer server = YamlServer({SCHEMA_DIR}, input);

        bool succeed = server.applySchema("expression.schema");

        if (succeed)
            std::cout << "resulting node: " << server.getNode() << std::endl;
        else
            std::cout << server.getLog() << std::endl;

        ASSERT_FALSE(succeed);
    }
}

TEST(TestExpression, isExpressionEmpty)
{
    EXPECT_FALSE(isExpression(""));
    EXPECT_FALSE(isExpression(YAML::Node("")));
    EXPECT_FALSE(isExpression(YAML::Node()));
    EXPECT_FALSE(isExpression(YAML::Load("[1, 2]")));

    std::string empty_str;
    EXPECT_NO_THROW(preProcessExpression(empty_str));
    EXPECT_TRUE(empty_str.empty());
}

TEST(TestExpression, applySchemaExpressionBeforeDefault)
{
    // the expression of 'param_expr' depends on 'mode', declared after it with a default value ('auto')
    YamlServer server({SCHEMA_DIR});

    // mode not defined -> default 'auto' -> param_expr not mandatory
    server.setYaml(YAML::Load("{}"));
    bool succeed = server.applySchema("regression_expression_order");
    if (not succeed) std::cout << server.getLog() << std::endl;
    EXPECT_TRUE(succeed);
    EXPECT_EQ(server.getNode()["mode"].as<std::string>(), "auto");

    // mode 'manual' -> param_expr mandatory
    server.setYaml(YAML::Load("{mode: manual}"));
    EXPECT_FALSE(server.applySchema("regression_expression_order"));

    server.setYaml(YAML::Load("{mode: manual, param_expr: 3}"));
    EXPECT_TRUE(server.applySchema("regression_expression_order"));
}

TEST(TestExpression, applySchemaExpressionStringType)
{
    // 'switch_mode' is a string, although its values may look like booleans or numbers
    YamlServer server({SCHEMA_DIR});

    for (auto mode : std::vector<std::string>{"on", "yes", "2", "auto"})
    {
        server.setYaml(YAML::Load("{switch_mode: " + mode + "}"));
        bool succeed = server.applySchema("regression_expression_string_type");
        if (not succeed) std::cout << server.getLog() << std::endl;
        EXPECT_TRUE(succeed) << "switch_mode: " << mode;
    }

    // 'off' -> param_off mandatory
    server.setYaml(YAML::Load("{switch_mode: off}"));
    EXPECT_FALSE(server.applySchema("regression_expression_string_type"));
    EXPECT_NE(server.getLog().find("Missing mandatory field"), std::string::npos);

    server.setYaml(YAML::Load("{switch_mode: off, param_off: 1}"));
    EXPECT_TRUE(server.applySchema("regression_expression_string_type"));
}

TEST(TestExpression, applySchemaExpressionEvalFails)
{
    // 'mode' is missing, so the expression of 'param' cannot be evaluated
    YamlServer server({SCHEMA_DIR});
    server.setYaml(YAML::Load("{}"));

    EXPECT_FALSE(server.applySchema("regression_expression_eval_fail"));
    std::string log = server.getLog();
    std::cout << log << std::endl;
    EXPECT_NE(log.find("ERROR in 'mode': Missing mandatory field"), std::string::npos);
    EXPECT_NE(log.find("ERROR in 'param': Evaluating schema expression"), std::string::npos);
    EXPECT_EQ(log.find("ERROR in 'param': Missing mandatory field"), std::string::npos);
    EXPECT_FALSE(server.getNode()["param"]);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
