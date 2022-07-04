
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/type_check.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(check_type, num)
{
  YAML::Node node;
  node = 3;
  
  EXPECT_FALSE(checkType(node, "unknown_class"));
  EXPECT_FALSE(checkType(node, "bool"));
  EXPECT_TRUE(checkType(node, "char"));
  EXPECT_TRUE(checkType(node, "int"));
  EXPECT_TRUE(checkType(node, "unsigned int"));
  EXPECT_TRUE(checkType(node, "long int"));
  EXPECT_TRUE(checkType(node, "long unsigned int"));
  EXPECT_TRUE(checkType(node, "float"));
  EXPECT_TRUE(checkType(node, "double"));
  EXPECT_TRUE(checkType(node, "string"));
  EXPECT_TRUE(checkType(node, "std::string"));
  EXPECT_FALSE(checkType(node, "VectorXd"));
  EXPECT_FALSE(checkType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkType(node, "MatrixXd"));
}

TEST(check_type, char)
{
  YAML::Node node;
  node = 'g';
  
  EXPECT_FALSE(checkType(node, "unknown_class"));
  EXPECT_FALSE(checkType(node, "bool"));
  EXPECT_TRUE(checkType(node, "char"));
  EXPECT_FALSE(checkType(node, "int"));
  EXPECT_FALSE(checkType(node, "unsigned int"));
  EXPECT_FALSE(checkType(node, "long int"));
  EXPECT_FALSE(checkType(node, "long unsigned int"));
  EXPECT_FALSE(checkType(node, "float"));
  EXPECT_FALSE(checkType(node, "double"));
  EXPECT_TRUE(checkType(node, "string"));
  EXPECT_TRUE(checkType(node, "std::string"));
  EXPECT_FALSE(checkType(node, "VectorXd"));
  EXPECT_FALSE(checkType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkType(node, "MatrixXd"));
}

TEST(check_type, string)
{
  YAML::Node node;
  node = "gromenauer";
  
  EXPECT_FALSE(checkType(node, "unknown_class"));
  EXPECT_FALSE(checkType(node, "bool"));
  EXPECT_FALSE(checkType(node, "char"));
  EXPECT_FALSE(checkType(node, "int"));
  EXPECT_FALSE(checkType(node, "unsigned int"));
  EXPECT_FALSE(checkType(node, "long int"));
  EXPECT_FALSE(checkType(node, "long unsigned int"));
  EXPECT_FALSE(checkType(node, "float"));
  EXPECT_FALSE(checkType(node, "double"));
  EXPECT_TRUE(checkType(node, "string"));
  EXPECT_TRUE(checkType(node, "std::string"));
  EXPECT_FALSE(checkType(node, "VectorXd"));
  EXPECT_FALSE(checkType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkType(node, "MatrixXd"));
}

TEST(check_type, Eigen)
{
  YAML::Node node;
  node[0] = 0;
  node[1] = 0.2;
  node[2] = 1e-5;
  node[3] = 1000;
  
  EXPECT_FALSE(checkType(node, "unknown_class"));
  EXPECT_FALSE(checkType(node, "bool"));
  EXPECT_FALSE(checkType(node, "char"));
  EXPECT_FALSE(checkType(node, "int"));
  EXPECT_FALSE(checkType(node, "unsigned int"));
  EXPECT_FALSE(checkType(node, "long int"));
  EXPECT_FALSE(checkType(node, "long unsigned int"));
  EXPECT_FALSE(checkType(node, "float"));
  EXPECT_FALSE(checkType(node, "double"));
  EXPECT_FALSE(checkType(node, "string"));
  EXPECT_FALSE(checkType(node, "std::string"));
  EXPECT_TRUE(checkType(node, "VectorXd"));
  EXPECT_TRUE(checkType(node, "Eigen::VectorXd"));
  EXPECT_TRUE(checkType(node, "Vector4d"));
  EXPECT_TRUE(checkType(node, "Matrix2d"));
  EXPECT_FALSE(checkType(node, "MatrixXd"));
}

TEST(check_type, EigenEmpty)
{
  YAML::Node node_loaded = YAML::LoadFile(ROOT_DIR + "/test/yaml/empty_vectorXd.yaml");
  
  YAML::Node node = node_loaded["vector"];
  EXPECT_TRUE(node.IsDefined());
  EXPECT_EQ(node.size(),0);
  EXPECT_TRUE(node.IsSequence());

  EXPECT_FALSE(checkType(node, "unknown_class"));
  EXPECT_FALSE(checkType(node, "bool"));
  EXPECT_FALSE(checkType(node, "char"));
  EXPECT_FALSE(checkType(node, "int"));
  EXPECT_FALSE(checkType(node, "unsigned int"));
  EXPECT_FALSE(checkType(node, "long int"));
  EXPECT_FALSE(checkType(node, "long unsigned int"));
  EXPECT_FALSE(checkType(node, "float"));
  EXPECT_FALSE(checkType(node, "double"));
  EXPECT_FALSE(checkType(node, "string"));
  EXPECT_FALSE(checkType(node, "std::string"));
  EXPECT_TRUE(checkType(node, "VectorXd"));
  EXPECT_TRUE(checkType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkType(node, "Vector4d"));
  EXPECT_FALSE(checkType(node, "Matrix2d"));
  EXPECT_TRUE(checkType(node, "MatrixXd"));
}

TEST(check_type, Eigen_matrix)
{
  YAML::Node node_sizes, node_values, node;
  node_sizes[0] = 3;
  node_sizes[1] = 2;
  node_values[0] = 1e-5;
  node_values[1] = 1000;
  node_values[2] = 1e-5;
  node_values[3] = 1000;
  node_values[4] = 1e-5;
  node_values[5] = 1000;
  node[0] = node_sizes;
  node[1] = node_values;
  
  EXPECT_FALSE(checkType(node, "unknown_class"));
  EXPECT_FALSE(checkType(node, "bool"));
  EXPECT_FALSE(checkType(node, "char"));
  EXPECT_FALSE(checkType(node, "int"));
  EXPECT_FALSE(checkType(node, "unsigned int"));
  EXPECT_FALSE(checkType(node, "long int"));
  EXPECT_FALSE(checkType(node, "long unsigned int"));
  EXPECT_FALSE(checkType(node, "float"));
  EXPECT_FALSE(checkType(node, "double"));
  EXPECT_FALSE(checkType(node, "string"));
  EXPECT_FALSE(checkType(node, "std::string"));
  EXPECT_FALSE(checkType(node, "VectorXd"));
  EXPECT_FALSE(checkType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkType(node, "Matrix2d"));
  EXPECT_TRUE(checkType(node, "MatrixXd"));
  EXPECT_TRUE(checkType(node, "Eigen::MatrixXd"));
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
