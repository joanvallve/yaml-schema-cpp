#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(schema, plain_yaml)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

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
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

    std::cout << "before: \n" << server.getNode() << std::endl;

    bool ok = server.applySchema("base_input_base.schema");
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
                                         ROOT_DIR + "/test/yaml/base_input_wrong9.yaml"};
    for (auto input : input_yamls)
    {
        std::cout << "testing " << input << std::endl;
        YamlServer server = YamlServer({ROOT_DIR}, input);

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
        YamlServer server = YamlServer({ROOT_DIR}, input);

        std::cout << "before: \n" << server.getNode() << std::endl;

        ASSERT_TRUE(server.applySchema("optional_map.schema"));

        std::cout << "after: \n" << server.getNode() << std::endl;

        std::cout << "log: \n" << server.getLog() << std::endl;
    }
}

TEST(schema, nontrivial_options_default_value)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/nontrivial_options_default_value.yaml");

    std::cout << "before: \n" << server.getNode() << std::endl;

    ASSERT_TRUE(server.applySchema("nontrivial_options_default_value.schema"));

    std::cout << "after: \n" << server.getNode() << std::endl;

    std::cout << "log: \n" << server.getLog() << std::endl;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
