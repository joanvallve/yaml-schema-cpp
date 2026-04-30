#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"
#include "yaml-schema-cpp/yaml_conversion.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(flatten_yaml, plain)
{
    yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/base_input.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    // check exist
    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);
    ASSERT_TRUE(input_node["param5"]);
    ASSERT_TRUE(input_node["param6"]);
    ASSERT_TRUE(input_node["param7"]);
    ASSERT_TRUE(input_node["param8"]);

    // Check values
    ASSERT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    ASSERT_EQ(input_node["param5"][0].as<int>(), 5);
    ASSERT_EQ(input_node["param5"][1].as<int>(), 6);
    ASSERT_EQ(input_node["param5"][2].as<int>(), -1);
    ASSERT_EQ(input_node["param5"][3].as<int>(), -7);
    ASSERT_EQ(input_node["param5"][4].as<int>(), 0);
    ASSERT_NEAR(input_node["param6"].as<double>(), 3.14, 1e-12);
    ASSERT_EQ(input_node["param7"][0].as<int>(), 0);
    ASSERT_EQ(input_node["param7"][1].as<int>(), 2);
    ASSERT_EQ(input_node["param7"][2].as<int>(), 3);
    ASSERT_NEAR(input_node["param8"][0][0].as<double>(), 3, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][0].as<double>(), 1, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][1].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][2].as<double>(), 4, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][0].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][1].as<double>(), 5.9, 1e-12);
}

TEST(flatten_yaml, relative_path)
{
    YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten/flatten_relative_path.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    // check exist
    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);
    ASSERT_TRUE(input_node["another_param"]);
    ASSERT_TRUE(input_node["map2"]);
    ASSERT_TRUE(input_node["map2"]["param1"]);
    ASSERT_TRUE(input_node["param5"]);
    ASSERT_TRUE(input_node["param6"]);
    ASSERT_TRUE(input_node["param7"]);
    ASSERT_TRUE(input_node["param8"]);

    // Check values
    ASSERT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    ASSERT_NEAR(input_node["another_param"][0].as<double>(), 0, 1e-12);
    ASSERT_NEAR(input_node["another_param"][1].as<double>(), 0.3, 1e-12);
    ASSERT_NEAR(input_node["another_param"][2].as<double>(), 1e5, 1e-12);
    ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
    ASSERT_EQ(input_node["param5"][0].as<int>(), 5);
    ASSERT_EQ(input_node["param5"][1].as<int>(), 6);
    ASSERT_EQ(input_node["param5"][2].as<int>(), -1);
    ASSERT_EQ(input_node["param5"][3].as<int>(), -7);
    ASSERT_EQ(input_node["param5"][4].as<int>(), 0);
    ASSERT_NEAR(input_node["param6"].as<double>(), 3.14, 1e-12);
    ASSERT_EQ(input_node["param7"][0].as<int>(), 0);
    ASSERT_EQ(input_node["param7"][1].as<int>(), 2);
    ASSERT_EQ(input_node["param7"][2].as<int>(), 3);
    ASSERT_NEAR(input_node["param8"][0][0].as<double>(), 3, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][0].as<double>(), 1, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][1].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][2].as<double>(), 4, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][0].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][1].as<double>(), 5.9, 1e-12);
}

TEST(flatten_yaml, recursive)
{
    yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"},
                                            ROOT_DIR + "/test/yaml/flatten/flatten_recursive.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    // check exist
    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);
    ASSERT_TRUE(input_node["another_param"]);
    ASSERT_TRUE(input_node["map2"]);
    ASSERT_TRUE(input_node["map2"]["param1"]);
    ASSERT_TRUE(input_node["yet_another_param"]);
    ASSERT_TRUE(input_node["map3"]);
    ASSERT_TRUE(input_node["map3"]["param1"]);
    ASSERT_TRUE(input_node["param5"]);
    ASSERT_TRUE(input_node["param6"]);
    ASSERT_TRUE(input_node["param7"]);
    ASSERT_TRUE(input_node["param8"]);

    // Check values
    ASSERT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    ASSERT_NEAR(input_node["another_param"][0].as<double>(), 0, 1e-12);
    ASSERT_NEAR(input_node["another_param"][1].as<double>(), 0.3, 1e-12);
    ASSERT_NEAR(input_node["another_param"][2].as<double>(), 1e5, 1e-12);
    ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
    ASSERT_EQ(input_node["yet_another_param"].as<bool>(), true);
    ASSERT_EQ(input_node["map3"]["param1"].as<std::string>(), "gromenauer");
    ASSERT_EQ(input_node["param5"][0].as<int>(), 5);
    ASSERT_EQ(input_node["param5"][1].as<int>(), 6);
    ASSERT_EQ(input_node["param5"][2].as<int>(), -1);
    ASSERT_EQ(input_node["param5"][3].as<int>(), -7);
    ASSERT_EQ(input_node["param5"][4].as<int>(), 0);
    ASSERT_NEAR(input_node["param6"].as<double>(), 3.14, 1e-12);
    ASSERT_EQ(input_node["param7"][0].as<int>(), 0);
    ASSERT_EQ(input_node["param7"][1].as<int>(), 2);
    ASSERT_EQ(input_node["param7"][2].as<int>(), 3);
    ASSERT_NEAR(input_node["param8"][0][0].as<double>(), 3, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][0].as<double>(), 1, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][1].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][2].as<double>(), 4, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][0].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][1].as<double>(), 5.9, 1e-12);
}

TEST(flatten_yaml, merge)
{
    YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten/flatten_merge.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    // check exist
    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);
    ASSERT_TRUE(input_node["map1"]["param3"]);
    ASSERT_TRUE(input_node["another_param"]);
    ASSERT_TRUE(input_node["map2"]);
    ASSERT_TRUE(input_node["map2"]["param1"]);
    ASSERT_TRUE(input_node["map2"]["param2"]);
    ASSERT_TRUE(input_node["yet_another_param"]);
    ASSERT_TRUE(input_node["map3"]);
    ASSERT_TRUE(input_node["map3"]["param1"]);
    ASSERT_TRUE(input_node["map3"]["param2"]);
    ASSERT_TRUE(input_node["param5"]);
    ASSERT_TRUE(input_node["param6"]);
    ASSERT_TRUE(input_node["param7"]);
    ASSERT_TRUE(input_node["param8"]);

    // Check values
    ASSERT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    ASSERT_EQ(input_node["map1"]["param3"].as<int>(), 3);
    ASSERT_NEAR(input_node["another_param"][0].as<double>(), 0, 1e-12);
    ASSERT_NEAR(input_node["another_param"][1].as<double>(), 0.3, 1e-12);
    ASSERT_NEAR(input_node["another_param"][2].as<double>(), 1e5, 1e-12);
    ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
    ASSERT_EQ(input_node["map2"]["param2"].as<bool>(), false);
    ASSERT_EQ(input_node["yet_another_param"].as<bool>(), true);
    ASSERT_EQ(input_node["map3"]["param1"].as<std::string>(), "gromenauer");
    ASSERT_NEAR(input_node["map3"]["param2"][0].as<double>(), 9, 1e-12);
    ASSERT_NEAR(input_node["map3"]["param2"][1].as<double>(), 0, 1e-12);
    ASSERT_NEAR(input_node["map3"]["param2"][2].as<double>(), 1e4, 1e-12);
    ASSERT_EQ(input_node["param5"][0].as<int>(), 5);
    ASSERT_EQ(input_node["param5"][1].as<int>(), 6);
    ASSERT_EQ(input_node["param5"][2].as<int>(), -1);
    ASSERT_EQ(input_node["param5"][3].as<int>(), -7);
    ASSERT_EQ(input_node["param5"][4].as<int>(), 0);
    ASSERT_NEAR(input_node["param6"].as<double>(), 3.14, 1e-12);
    ASSERT_EQ(input_node["param7"][0].as<int>(), 0);
    ASSERT_EQ(input_node["param7"][1].as<int>(), 2);
    ASSERT_EQ(input_node["param7"][2].as<int>(), 3);
    ASSERT_NEAR(input_node["param8"][0][0].as<double>(), 3, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][0].as<double>(), 1, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][1].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][2].as<double>(), 4, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][0].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][1].as<double>(), 5.9, 1e-12);
}

TEST(flatten_yaml, override)
{
    // if override not allowed should crash
    ASSERT_THROW(YamlServer({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten/flatten_override.yaml", false),
                 std::runtime_error);

    // Allow override
    YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten/flatten_override.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    // check exist
    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);
    ASSERT_TRUE(input_node["map1"]["param3"]);
    ASSERT_TRUE(input_node["another_param"]);
    ASSERT_TRUE(input_node["map2"]);
    ASSERT_TRUE(input_node["map2"]["param1"]);
    ASSERT_TRUE(input_node["map2"]["param2"]);
    ASSERT_TRUE(input_node["yet_another_param"]);
    ASSERT_TRUE(input_node["map3"]);
    ASSERT_TRUE(input_node["map3"]["param1"]);
    ASSERT_TRUE(input_node["param5"]);
    ASSERT_TRUE(input_node["param6"]);
    ASSERT_TRUE(input_node["param7"]);
    ASSERT_TRUE(input_node["param8"]);

    // Check values
    ASSERT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    ASSERT_EQ(input_node["map1"]["param3"].as<int>(), 3);
    ASSERT_NEAR(input_node["another_param"][0].as<double>(), 0, 1e-12);
    ASSERT_NEAR(input_node["another_param"][1].as<double>(), 0.3, 1e-12);
    ASSERT_NEAR(input_node["another_param"][2].as<double>(), 1e5, 1e-12);
    ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
    ASSERT_EQ(input_node["map2"]["param2"].as<bool>(), false);
    ASSERT_EQ(input_node["yet_another_param"].as<bool>(), true);
    ASSERT_EQ(input_node["map3"]["param1"].as<std::string>(), "pitihander");
    ASSERT_EQ(input_node["param5"][0].as<int>(), 5);
    ASSERT_EQ(input_node["param5"][1].as<int>(), 6);
    ASSERT_EQ(input_node["param5"][2].as<int>(), -1);
    ASSERT_EQ(input_node["param5"][3].as<int>(), -7);
    ASSERT_EQ(input_node["param5"][4].as<int>(), 0);
    ASSERT_NEAR(input_node["param6"].as<double>(), 3.14, 1e-12);
    ASSERT_EQ(input_node["param7"][0].as<int>(), 0);
    ASSERT_EQ(input_node["param7"][1].as<int>(), 2);
    ASSERT_EQ(input_node["param7"][2].as<int>(), 3);
    ASSERT_NEAR(input_node["param8"][0][0].as<double>(), 3, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][0].as<double>(), 1, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][1].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][2].as<double>(), 4, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][0].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][1].as<double>(), 5.9, 1e-12);
}

TEST(flatten_yaml, sequence_follow)
{
    YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten/flatten_sequence_follow.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    // check exist
    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);
    ASSERT_TRUE(input_node["param5"]);
    ASSERT_TRUE(input_node["param6"]);
    ASSERT_TRUE(input_node["param7"]);
    ASSERT_TRUE(input_node["param8"]);
    ASSERT_TRUE(input_node["int_param"]);
    ASSERT_TRUE(input_node["int_param"]);
    ASSERT_TRUE(input_node["string_param"]);
    ASSERT_TRUE(input_node["another_param"]);
    ASSERT_TRUE(input_node["map2"]);
    ASSERT_TRUE(input_node["map2"]["param1"]);

    // Check values
    ASSERT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    ASSERT_EQ(input_node["param5"][0].as<int>(), 5);
    ASSERT_EQ(input_node["param5"][1].as<int>(), 6);
    ASSERT_EQ(input_node["param5"][2].as<int>(), -1);
    ASSERT_EQ(input_node["param5"][3].as<int>(), -7);
    ASSERT_EQ(input_node["param5"][4].as<int>(), 0);
    ASSERT_NEAR(input_node["param6"].as<double>(), 3.14, 1e-12);
    ASSERT_EQ(input_node["param7"][0].as<int>(), 0);
    ASSERT_EQ(input_node["param7"][1].as<int>(), 2);
    ASSERT_EQ(input_node["param7"][2].as<int>(), 3);
    ASSERT_NEAR(input_node["param8"][0][0].as<double>(), 3, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][0].as<double>(), 1, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][1].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][1][2].as<double>(), 4, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][0].as<double>(), 2, 1e-12);
    ASSERT_NEAR(input_node["param8"][2][1].as<double>(), 5.9, 1e-12);
    ASSERT_EQ(input_node["int_param"].as<int>(), -1);
    ASSERT_EQ(input_node["string_param"].as<std::string>(), "string");
    ASSERT_NEAR(input_node["another_param"][0].as<double>(), 0, 1e-12);
    ASSERT_NEAR(input_node["another_param"][1].as<double>(), 0.3, 1e-12);
    ASSERT_NEAR(input_node["another_param"][2].as<double>(), 1e5, 1e-12);
    ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
