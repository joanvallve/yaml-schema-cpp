
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(schema, plain_yaml)
{
  YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

  std::cout << "before: \n" <<  server.getNode() << std::endl;

  ASSERT_TRUE(server.applySchema("base_input.schema"));

  std::cout << "after: \n" <<  server.getNode() << std::endl;
  
  YAML::Node node = server.getNode();

  ASSERT_TRUE(node["map1"]["param3"]);
  ASSERT_TRUE(node["param4"]);
  ASSERT_NEAR(node["map1"]["param3"].as<double>(), 3.5, 1e-12);
  ASSERT_EQ(node["param4"].as<std::string>(),"hello");
}

TEST(schema, follow)
{
  YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

  std::cout << "before: \n" <<  server.getNode() << std::endl;

  ASSERT_TRUE(server.applySchema("base_input_base.schema"));

  std::cout << "after: \n" <<  server.getNode() << std::endl;
  
  YAML::Node node = server.getNode();

  ASSERT_TRUE(node["map1"]["param3"]);
  ASSERT_TRUE(node["param4"]);
  ASSERT_NEAR(node["map1"]["param3"].as<double>(), 3.5, 1e-12);
  ASSERT_EQ(node["param4"].as<std::string>(),"hello");
}

TEST(schema, wrong)
{
  std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/base_input_wrong1.yaml",
                                       ROOT_DIR + "/test/yaml/base_input_wrong2.yaml",
                                       ROOT_DIR + "/test/yaml/base_input_wrong3.yaml",
                                       ROOT_DIR + "/test/yaml/base_input_wrong4.yaml",
                                       ROOT_DIR + "/test/yaml/base_input_wrong5.yaml"};
  for (auto input : input_yamls)
  {
    std::cout << "testing " << input << std::endl;
    YamlServer server = YamlServer({ROOT_DIR}, input);

    ASSERT_FALSE(server.applySchema("base_input.schema"));

    std::cout << server.getLog().str() << std::endl;
  }
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
