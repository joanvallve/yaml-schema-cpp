
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_schema.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(schema, plain_yaml)
{
  loadSchema("test1.schema",{ROOT_DIR + "/test/yaml/schema", ROOT_DIR + "/test/yaml/other_schema"});
}

TEST(schema, follow)
{
  loadSchema("test2.schema",{ROOT_DIR + "/test/yaml/schema", ROOT_DIR + "/test/yaml/other_schema"});
}

TEST(schema, wrong)
{
  std::list<std::string> wrong_schemas{"not_default",
                                       "not_mandatory",
                                       "not_type",
                                       "not_doc",
                                       "wrong_default",
                                       "wrong_default_options",
                                       "wrong_mandatory", 
                                       "wrong_yaml_type",
                                       "wrong_doc",
                                       "wrong_options",
                                       "wrong_options2"};

  for (auto w_schema : wrong_schemas)
  {
    std::cout << "Checking " << w_schema << ".schema..." << std::endl;
    ASSERT_THROW(loadSchema(w_schema + ".schema",
                            { ROOT_DIR + "/test/yaml/schema", 
                              ROOT_DIR + "/test/yaml/other_schema"}), 
                 std::runtime_error);
  }
}

TEST(schema, override)
{
  std::list<std::string> wrong_schemas{"same_param",
                                       "same_param_diff_type"};

  for (auto w_schema : wrong_schemas)
  {
    std::cout << "Checking " << w_schema << ".schema..." << std::endl;
    loadSchema(w_schema + ".schema",
               {ROOT_DIR + "/test/yaml/schema", 
                ROOT_DIR + "/test/yaml/other_schema"});
  }
}

TEST(schema, dont_override)
{
  std::list<std::string> wrong_schemas{"same_param",
                                       "same_param_diff_type"};

  for (auto w_schema : wrong_schemas)
  {
    std::cout << "Checking " << w_schema << ".schema..." << std::endl;
    ASSERT_THROW(loadSchema(w_schema + ".schema",
                            { ROOT_DIR + "/test/yaml/schema", 
                              ROOT_DIR + "/test/yaml/other_schema"},
                            false), 
                 std::runtime_error);
  }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
