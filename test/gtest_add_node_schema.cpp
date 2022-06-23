#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/internal/config.h"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(addNodeSchema, add_scalars)
{
  YAML::Node node1, node2;
  node1["param1"]["mandatory"] = true;
  node1["param1"]["type"] = "bool";
  node1["param1"]["yaml_type"] = "scalar";
  node2["param2"]["mandatory"] = true;
  node2["param2"]["type"] = "string";
  node2["param2"]["yaml_type"] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"]["mandatory"]);
  ASSERT_TRUE(node1["param1"]["type"]);
  ASSERT_TRUE(node1["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["param2"]);
  ASSERT_TRUE(node2["param2"]["mandatory"]);
  ASSERT_TRUE(node2["param2"]["type"]);
  ASSERT_TRUE(node2["param2"]["yaml_type"]);

  addNodeSchema(node2, "param1", node1["param1"]);
  
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"]["mandatory"]);
  ASSERT_TRUE(node2["param1"]["type"]);
  ASSERT_TRUE(node2["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["param2"]);
  ASSERT_TRUE(node2["param2"]["mandatory"]);
  ASSERT_TRUE(node2["param2"]["type"]);
  ASSERT_TRUE(node2["param2"]["yaml_type"]);
}

TEST(addNodeSchema, merge_map)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"]["mandatory"] = true;
  node1["namespace"]["param1"]["type"] = "bool";
  node1["namespace"]["param1"]["yaml_type"] = "scalar";
  node2["namespace"]["param2"]["mandatory"] = true;
  node2["namespace"]["param2"]["type"] = "string";
  node2["namespace"]["param2"]["yaml_type"] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["mandatory"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["type"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["namespace"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param2"]["mandatory"]);
  ASSERT_TRUE(node2["namespace"]["param2"]["type"]);
  ASSERT_TRUE(node2["namespace"]["param2"]["yaml_type"]);

  addNodeSchema(node2, "namespace", node1["namespace"]);
  
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["mandatory"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["type"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["namespace"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param2"]["mandatory"]);
  ASSERT_TRUE(node2["namespace"]["param2"]["type"]);
  ASSERT_TRUE(node2["namespace"]["param2"]["yaml_type"]);
}

TEST(addNodeSchema, existing_scalar)
{
  YAML::Node node1, node2;
  node1["param1"]["mandatory"] = true;
  node1["param1"]["type"] = "bool";
  node1["param1"]["yaml_type"] = "scalar";
  node2["param1"]["mandatory"] = true;
  node2["param1"]["type"] = "string";
  node2["param1"]["yaml_type"] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"]["mandatory"]);
  ASSERT_TRUE(node1["param1"]["type"]);
  ASSERT_TRUE(node1["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"]["mandatory"]);
  ASSERT_TRUE(node2["param1"]["type"]);
  ASSERT_TRUE(node2["param1"]["yaml_type"]);

  try
  {
    addNodeSchema(node1, "param1", node2["param2"]);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "param1", node2["param2"]), std::runtime_error);
}

TEST(addNodeSchema, existing_sequence)
{
  YAML::Node node1, node2;
  node1["param1"]["mandatory"] = true;
  node1["param1"]["type"] = "bool";
  node1["param1"]["yaml_type"] = "sequence";
  node2["param1"]["mandatory"] = true;
  node2["param1"]["type"] = "string";
  node2["param1"]["yaml_type"] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"]["mandatory"]);
  ASSERT_TRUE(node1["param1"]["type"]);
  ASSERT_TRUE(node1["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"]["mandatory"]);
  ASSERT_TRUE(node2["param1"]["type"]);
  ASSERT_TRUE(node2["param1"]["yaml_type"]);

  try
  {
    addNodeSchema(node1, "param1", node2["param2"]);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "param1", node2["param2"]), std::runtime_error);
}

TEST(addNodeSchema, existing_map_scalar)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"]["mandatory"] = true;
  node1["namespace"]["param1"]["type"] = "bool";
  node1["namespace"]["param1"]["yaml_type"] = "scalar";
  node2["namespace"]["param1"]["mandatory"] = true;
  node2["namespace"]["param1"]["type"] = "string";
  node2["namespace"]["param1"]["yaml_type"] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["mandatory"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["type"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["mandatory"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["type"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["yaml_type"]);

  try
  {
    addNodeSchema(node1, "namespace", node2["namespace"]);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "namespace", node2["namespace"]), std::runtime_error);
}

TEST(addNodeSchema, existing_diff_types)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"]["mandatory"] = true;
  node1["namespace"]["param1"]["type"] = "bool";
  node1["namespace"]["param1"]["yaml_type"] = "scalar";
  node2["namespace"]["param1"]["param2"]["mandatory"] = true;
  node2["namespace"]["param1"]["param2"]["type"] = "string";
  node2["namespace"]["param1"]["param2"]["yaml_type"] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["mandatory"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["type"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["yaml_type"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"]["mandatory"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"]["type"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"]["yaml_type"]);

  try
  {
    addNodeSchema(node1, "namespace", node2["namespace"]);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "namespace", node2["namespace"]), std::runtime_error);

  try
  {
    addNodeSchema(node2, "namespace", node1["namespace"]);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node2, "namespace", node1["namespace"]), std::runtime_error);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
