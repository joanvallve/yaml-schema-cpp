#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR   = _YAML_SCHEMA_CPP_ROOT_DIR;
std::string SCHEMA_DIR = ROOT_DIR + "/test/schema";

using namespace yaml_schema_cpp;

TEST(schema, plain_yaml)
{
    YamlServer server = YamlServer({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

    std::cout << "before: \n" << server.getNode() << std::endl;

    bool ok = server.applySchema("base_input.schema");
    if (not ok) std::cout << server.getLog();
    ASSERT_TRUE(ok);

    std::cout << "after: \n" << server.getNode() << std::endl;

    YAML::Node node = server.getNode();

    ASSERT_TRUE(node["map1"]["param3"]);
    ASSERT_TRUE(node["param4"]);
    ASSERT_NEAR(node["map1"]["param3"].as<double>(), 3.5, 1e-12);
    ASSERT_EQ(node["param4"].as<std::string>(), "hello");

    std::cout << "log: \n" << server.getLog() << std::endl;
}

TEST(schema, follow)
{
    YamlServer server = YamlServer({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

    std::cout << "before: \n" << server.getNode() << std::endl;

    bool ok = server.applySchema("base_input_derived.schema");
    if (not ok) std::cout << server.getLog();
    ASSERT_TRUE(ok);

    std::cout << "after: \n" << server.getNode() << std::endl;

    YAML::Node node = server.getNode();

    ASSERT_TRUE(node["map1"]["param3"]);
    ASSERT_TRUE(node["param4"]);
    ASSERT_NEAR(node["map1"]["param3"].as<double>(), 3.5, 1e-12);
    ASSERT_EQ(node["param4"].as<std::string>(), "hello");

    std::cout << "log: \n" << server.getLog() << std::endl;
}

TEST(schema, wrong)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/base_input_wrong1.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong2.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong3.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong4.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong5.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong6.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong7.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong8.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong9.yaml",
                                         ROOT_DIR + "/test/yaml/base_input_wrong10.yaml"};
    for (auto input : input_yamls)
    {
        YamlServer server = YamlServer({SCHEMA_DIR}, input);
        std::cout << "testing " << input << ":\n" << server.getNode() << std::endl;

        bool succeed = server.applySchema("base_input.schema");

        if (succeed)
            std::cout << "resulting node: " << server.getNode() << std::endl;
        else
            std::cout << server.getLog() << std::endl;

        ASSERT_FALSE(succeed);
    }
}

TEST(schema, optional_map)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/optional_map.yaml",
                                         ROOT_DIR + "/test/yaml/optional_map2.yaml"};
    for (auto input : input_yamls)
    {
        std::cout << "testing " << input << std::endl;
        YamlServer server = YamlServer({SCHEMA_DIR}, input);

        std::cout << "before: \n" << server.getNode() << std::endl;

        ASSERT_TRUE(server.applySchema("optional_map.schema"));

        std::cout << "after: \n" << server.getNode() << std::endl;

        std::cout << "log: \n" << server.getLog() << std::endl;
    }
}

TEST(schema, nontrivial_options_default_value)
{
    YamlServer server = YamlServer({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/nontrivial_options_default_value.yaml");

    std::cout << "before: \n" << server.getNode() << std::endl;

    ASSERT_TRUE(server.applySchema("nontrivial_options_default_value.schema"));

    std::cout << "after: \n" << server.getNode() << std::endl;

    std::cout << "log: \n" << server.getLog() << std::endl;
}

#if _EIGEN_FOUND == 1
TEST(schema, complex_case)
{
    YamlServer server = YamlServer({SCHEMA_DIR + "/folder_schema"}, ROOT_DIR + "/test/yaml/complex_case.yaml");

    std::cout << "before: \n" << server.getNode() << std::endl;

    server.addFolderSchema(SCHEMA_DIR + "/complex_case");

    EXPECT_TRUE(server.applySchema("Problem.schema"));

    std::cout << "after Problem.schema: \n" << server.getNode() << std::endl;

    std::cout << "log: \n" << server.getLog() << std::endl;

    bool is2D = server.getNode()["problem"]["dimension"].as<int>() == 2;
    EXPECT_TRUE(server.applySchema(is2D ? "Problem2d.schema" : "Problem3d.schema"));

    std::cout << "after Problem3d.schema: \n" << server.getNode() << std::endl;

    std::cout << "log: \n" << server.getLog() << std::endl;
}
#endif

TEST(schema, wrong_structure_map)
{
    YamlServer server({SCHEMA_DIR});
    bool       succeed = true;

    // scalar where a map is expected
    server.setYaml(YAML::Load("{sub_map: 5}"));
    EXPECT_NO_THROW(succeed = server.applySchema("regression_map"));
    EXPECT_FALSE(succeed);
    EXPECT_NE(server.getLog().find("ERROR in 'sub_map'"), std::string::npos);

    // sequence where a map is expected (and it should not be converted into a map)
    server.setYaml(YAML::Load("{sub_map: [1, 2]}"));
    EXPECT_NO_THROW(succeed = server.applySchema("regression_map"));
    EXPECT_FALSE(succeed);
    EXPECT_TRUE(server.getNode()["sub_map"].IsSequence());

    // missing and null maps are fine
    for (auto input : std::vector<std::string>{"{}", "{sub_map: }"})
    {
        server.setYaml(YAML::Load(input));
        EXPECT_TRUE(server.applySchema("regression_map")) << input;
        EXPECT_EQ(server.getNode()["sub_map"]["param"].as<int>(), 1);
    }
}

TEST(schema, wrong_structure_derived)
{
    // derived type should be a map with 'type'
    YamlServer server({SCHEMA_DIR});
    bool       succeed = true;

    for (auto input : std::vector<std::string>{"{final_1: a, final_derived: 5}",
                                               "{final_1: a, final_derived: [1, 2]}",
                                               "{final_1: a, final_derived: {type: [1, 2]}}"})
    {
        server.setYaml(YAML::Load(input));
        EXPECT_NO_THROW(succeed = server.applySchema("type_derived_final")) << input;
        EXPECT_FALSE(succeed) << input;
        std::cout << server.getLog() << std::endl;
    }
}

TEST(schema, wrong_structure_array)
{
    // a map where an array is expected: a single error, and the map is not modified
    YamlServer server({SCHEMA_DIR});
    server.setYaml(YAML::Load("{arr: {x: 1}}"));

    EXPECT_FALSE(server.applySchema("regression_array"));
    std::cout << server.getLog() << std::endl;
    EXPECT_NE(server.getLog().find("ERROR in 'arr'"), std::string::npos);
    EXPECT_EQ(server.getLog().find("arr["), std::string::npos);
    EXPECT_EQ(server.getNode()["arr"].size(), 1);
}

TEST(schema, keys_with_slash)
{
    YamlServer server({SCHEMA_DIR});
    server.setYaml(YAML::Load("{}"));

    bool succeed = server.applySchema("regression_slash_key");
    if (not succeed) std::cout << server.getLog() << std::endl;
    ASSERT_TRUE(succeed);

    YAML::Node node = server.getNode();
    std::cout << node << std::endl;
    ASSERT_TRUE(node["/odom"]);
    EXPECT_EQ(node["/odom"].as<std::string>(), "odom_topic");
    ASSERT_TRUE(node["a/b"]);
    EXPECT_EQ(node["a/b"].as<int>(), 3);
    ASSERT_TRUE(node["/map/with/slash"]);
    ASSERT_TRUE(node["/map/with/slash"]["param"]);
    EXPECT_EQ(node["/map/with/slash"]["param"].as<int>(), 1);
    EXPECT_FALSE(node["odom"]);
    EXPECT_FALSE(node["b"]);
    EXPECT_FALSE(node["slash"]);
    EXPECT_EQ(node.size(), 3);
}

TEST(schema, scalar_leaf_in_schema)
{
    // scalars in schemas are ignored
    YamlServer server({SCHEMA_DIR});
    server.setYaml(YAML::Load("{}"));

    bool succeed = false;
    EXPECT_NO_THROW(succeed = server.applySchema("regression_scalar_leaf"));
    EXPECT_TRUE(succeed);
    EXPECT_EQ(server.getNode()["param"].as<int>(), 1);
    EXPECT_FALSE(server.getNode()["some_scalar"]);
}

TEST(schema, null_string)
{
    YamlServer server({SCHEMA_DIR});

    // mandatory string with null value
    server.setYaml(YAML::Load("name:"));
    EXPECT_FALSE(server.applySchema("regression_null_string"));
    std::cout << server.getLog() << std::endl;

    server.setYaml(YAML::Load("name: ~"));
    EXPECT_FALSE(server.applySchema("regression_null_string"));

    // empty string is fine
    server.setYaml(YAML::Load("name: ''"));
    EXPECT_TRUE(server.applySchema("regression_null_string"));

    // null element of sequence of strings
    server.setYaml(YAML::Load("{name: a, names: [a, ~]}"));
    EXPECT_FALSE(server.applySchema("regression_null_string"));

    server.setYaml(YAML::Load("{name: a, names: [a, b]}"));
    EXPECT_TRUE(server.applySchema("regression_null_string"));
}

TEST(schema, derived_value)
{
    YamlServer server({SCHEMA_DIR});
    bool       succeed;

    // same value as the schema
    server.setYaml(YAML::Load("final_derived: {type: type_derived_derived, param_base: 2.4, param_derived: [1, 2]}"));
    testing::internal::CaptureStdout();
    succeed = server.applySchema("regression_derived_value");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
    if (not succeed) std::cout << server.getLog() << std::endl;
    EXPECT_TRUE(succeed);

    // different value than the schema
    server.setYaml(YAML::Load("final_derived: {type: type_derived_derived, param_base: 3.0, param_derived: [1, 2]}"));
    EXPECT_FALSE(server.applySchema("regression_derived_value"));

    // not defined: added from schema
    server.setYaml(YAML::Load("{}"));
    EXPECT_TRUE(server.applySchema("regression_derived_value"));
    EXPECT_NEAR(server.getNode()["final_derived"]["param_base"].as<double>(), 2.4, 1e-12);
}

TEST(schema, derived_options)
{
    YamlServer server({SCHEMA_DIR});

    server.setYaml(
        YAML::Load("derived_with_options: {type: type_derived_derived, param_base: 1.0, param_derived: [1]}"));
    bool succeed = server.applySchema("regression_derived_options");
    if (not succeed) std::cout << server.getLog() << std::endl;
    EXPECT_TRUE(succeed);

    server.setYaml(
        YAML::Load("derived_with_options: {type: type_derived_derived, param_base: 2.0, param_derived: [1]}"));
    EXPECT_FALSE(server.applySchema("regression_derived_options"));
    EXPECT_NE(server.getLog().find("OPTIONS"), std::string::npos);
}

TEST(schema, string_literal_input_path)
{
    // a string literal should not be converted to bool (override)
    std::string path = ROOT_DIR + "/test/yaml/base_input.yaml";
    YamlServer  server({SCHEMA_DIR}, path.c_str());

    EXPECT_TRUE(server.getNode()["map1"]);
    EXPECT_TRUE(server.applySchema("base_input.schema"));
}

TEST(schema, set_yaml_log_header)
{
    YamlServer server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");
    server.setYaml(YAML::Load("{}"));
    server.applySchema("regression_map");

    EXPECT_NE(server.getLog().find("yaml node added via setYaml"), std::string::npos);
    EXPECT_EQ(server.getLog().find("base_input.yaml"), std::string::npos);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "schema.complex_case"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "schema.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
