
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml-schema-cpp.hpp"
#include "yaml-schema-cpp/internal/config.h"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(flatten, plain_yaml)
{
  yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/base_input.yaml");
  
  YAML::Node input_node = yaml_server.getNodeInput();

  std::cout << "input_node: " << std::endl << input_node << std::endl;

  ASSERT_TRUE(input_node["map1"]);
  ASSERT_TRUE(input_node["map1"]["param1"]);
  ASSERT_TRUE(input_node["map1"]["param2"]);

  ASSERT_EQ(input_node["map1"]["param1"].as<int>(),1);
  ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(),"string");
}

TEST(flatten, flatten_independent)
{
  yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten_independent.yaml");
  
  YAML::Node input_node = yaml_server.getNodeInput();

  std::cout << "input_node: " << std::endl << input_node << std::endl;

  ASSERT_TRUE(input_node["map1"]);
  ASSERT_TRUE(input_node["map1"]["param1"]);
  ASSERT_TRUE(input_node["map1"]["param2"]);
  ASSERT_TRUE(input_node["another_param"]);
  ASSERT_TRUE(input_node["map2"]);
  ASSERT_TRUE(input_node["map2"]["param1"]);

  ASSERT_EQ(input_node["map1"]["param1"].as<int>(),1);
  ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(),"string");
  ASSERT_MATRIX_APPROX(input_node["another_param"].as<Eigen::VectorXd>(),(Eigen::VectorXd(3) << 0, 0.3, 1e5).finished(), 1e-12);
  ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
}

TEST(flatten, flatten_recursive)
{
  yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten_recursive.yaml");
  
  YAML::Node input_node = yaml_server.getNodeInput();

  std::cout << "input_node: " << std::endl << input_node << std::endl;

  ASSERT_TRUE(input_node["map1"]);
  ASSERT_TRUE(input_node["map1"]["param1"]);
  ASSERT_TRUE(input_node["map1"]["param2"]);
  ASSERT_TRUE(input_node["another_param"]);
  ASSERT_TRUE(input_node["map2"]);
  ASSERT_TRUE(input_node["map2"]["param1"]);
  ASSERT_TRUE(input_node["yet_another_param"]);
  ASSERT_TRUE(input_node["map3"]);
  ASSERT_TRUE(input_node["map3"]["param1"]);

  ASSERT_EQ(input_node["map1"]["param1"].as<int>(),1);
  ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(),"string");
  ASSERT_MATRIX_APPROX(input_node["another_param"].as<Eigen::VectorXd>(),(Eigen::VectorXd(3) << 0, 0.3, 1e5).finished(), 1e-12);
  ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
  ASSERT_EQ(input_node["yet_another_param"].as<bool>(),true);
  ASSERT_EQ(input_node["map3"]["param1"].as<std::string>(),"gromenauer");
}

TEST(flatten, flatten_merge)
{
  YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/flatten_merge.yaml");
  
  YAML::Node input_node = yaml_server.getNodeInput();

  std::cout << "input_node: " << std::endl << input_node << std::endl;

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

  ASSERT_EQ(input_node["map1"]["param1"].as<int>(),1);
  ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(),"string");
  ASSERT_EQ(input_node["map1"]["param3"].as<int>(),3);
  ASSERT_MATRIX_APPROX(input_node["another_param"].as<Eigen::VectorXd>(),(Eigen::VectorXd(3) << 0, 0.3, 1e5).finished(), 1e-12);
  ASSERT_NEAR(input_node["map2"]["param1"].as<double>(), 4.5, 1e-12);
  ASSERT_EQ(input_node["map2"]["param2"].as<bool>(),false);
  ASSERT_EQ(input_node["yet_another_param"].as<bool>(),true);
  ASSERT_EQ(input_node["map3"]["param1"].as<std::string>(),"gromenauer");
  ASSERT_MATRIX_APPROX(input_node["map3"]["param2"].as<Eigen::VectorXd>(),(Eigen::VectorXd(3) << 9, 0, 1e4).finished(), 1e-12);

}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
