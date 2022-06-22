
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml-schema-cpp.hpp"
#include "yaml-schema-cpp/internal/config.h"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(addNode, empty)
{
  YAML::Node node1, node2, node3;
  node1["param1"] = 1.0;
  node1["param2"] = 2.0;
  node2["namespace"] = node1;

  std::cout << "node2: " << std::endl << node2 << std::endl;
  std::cout << "node3: " << std::endl << node3 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param2"]);
  ASSERT_TRUE(node2["namespace"]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param2"]);

  YamlServer::addNode(node3, "namespace", node2["namespace"]);
  
  std::cout << "node3: " << std::endl << node3 << std::endl;

  ASSERT_TRUE(node3["namespace"]);
  ASSERT_TRUE(node3["namespace"]["param1"]);
  ASSERT_TRUE(node3["namespace"]["param2"]);
}

TEST(addNode, add)
{
  YAML::Node node1, node2, node3, node4;
  node1["param1"] = 1.0;
  node1["param2"] = 2.0;
  node2["namespace"] = node1;

  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param2"]);
  ASSERT_FALSE(node1["param3"]);
  ASSERT_TRUE(node2["namespace"]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param2"]);
  ASSERT_FALSE(node2["namespace"]["param3"]);

  node3["param3"] = 3.0;
  node4["namespace"] = node3;

  std::cout << "node4: " << std::endl << node4 << std::endl;

  ASSERT_TRUE(node3["param3"]);
  ASSERT_TRUE(node4["namespace"]);
  ASSERT_TRUE(node4["namespace"]["param3"]);

  YamlServer::addNode(node2, "namespace", node4["namespace"]);
  
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param2"]);
  ASSERT_TRUE(node1["param3"]);
  ASSERT_TRUE(node2["namespace"]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param3"]);
}

TEST(addNode, same_key)
{
  YAML::Node node1, node2, node3, node4;
  node1["param1"] = 1.0;
  node1["param2"] = 2.0;
  node2["namespace"] = node1;

  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param2"]);
  ASSERT_TRUE(node2["namespace"]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param2"]);

  node3["param1"] = 3.0;
  node4["namespace"] = node3;

  std::cout << "node4: " << std::endl << node4 << std::endl;

  ASSERT_TRUE(node3["param1"]);
  ASSERT_TRUE(node4["namespace"]);
  ASSERT_TRUE(node4["namespace"]["param1"]);

  ASSERT_THROW(YamlServer::addNode(node2, "namespace", node4["namespace"]),std::runtime_error);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
