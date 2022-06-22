
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml-schema-cpp.hpp"
#include "yaml-schema-cpp/internal/config.h"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

TEST(DuplicatedKeys, test1)
{
  yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/duplicated_input.yaml");

  auto input_node = yaml_server.getNodeInput();

  EXPECT_EQ(input_node["map1"].size(), 3);
  EXPECT_TRUE(input_node["map1"]["param1"]);
  EXPECT_TRUE(input_node["map1"]["param2"]);
  EXPECT_TRUE(input_node["map1"]["param3"]);

  //EXPECT_NEAR(input_node["map1"]["param3"].as<double>(), 4.5, 1e-12);
  // EXPECT_EQ(input_node["map1"]["param1"].as<int>(), 1);
  // EXPECT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
  // auto node_aux = input_node["map1"]["param3"];
  //EXPECT_NEAR(input_node["map1"]["param3"].as<double>(), 4.5, 1e-12);

}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
