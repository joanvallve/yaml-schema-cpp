#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

TEST(DuplicatedKeys, override)
{
    yaml_schema_cpp::YamlServer yaml_server(
        {ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/duplicated_input.yaml", true);

    YAML::Node input_node = yaml_server.getNode();

    EXPECT_EQ(input_node["map1"].size(), 3);

    // check params in map1 were added
    EXPECT_TRUE(input_node["map1"]["param1"]);
    EXPECT_TRUE(input_node["map1"]["param2"]);
    EXPECT_TRUE(input_node["map1"]["param3"]);

    // check param3 value after overriding
    EXPECT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
}

TEST(DuplicatedKeys, override_not_allowed)
{
    EXPECT_THROW(
        yaml_schema_cpp::YamlServer({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/duplicated_input.yaml", false),
        std::runtime_error);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
