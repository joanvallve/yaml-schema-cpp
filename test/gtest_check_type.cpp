
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/type_check.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(check_type, num)
{
  YAML::Node node;
  node = 3;
  
  EXPECT_FALSE(checkTrivialType(node, "unknown_class"));
  EXPECT_FALSE(checkTrivialType(node, "bool"));
  EXPECT_TRUE(checkTrivialType(node, "char"));
  EXPECT_TRUE(checkTrivialType(node, "int"));
  EXPECT_TRUE(checkTrivialType(node, "unsigned int"));
  EXPECT_TRUE(checkTrivialType(node, "long int"));
  EXPECT_TRUE(checkTrivialType(node, "long unsigned int"));
  EXPECT_TRUE(checkTrivialType(node, "float"));
  EXPECT_TRUE(checkTrivialType(node, "double"));
  EXPECT_TRUE(checkTrivialType(node, "string"));
  EXPECT_TRUE(checkTrivialType(node, "std::string"));
  EXPECT_FALSE(checkTrivialType(node, "VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "MatrixXd"));
}

TEST(check_type, char)
{
  YAML::Node node;
  node = 'g';
  
  EXPECT_FALSE(checkTrivialType(node, "unknown_class"));
  EXPECT_FALSE(checkTrivialType(node, "bool"));
  EXPECT_TRUE(checkTrivialType(node, "char"));
  EXPECT_FALSE(checkTrivialType(node, "int"));
  EXPECT_FALSE(checkTrivialType(node, "unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "long int"));
  EXPECT_FALSE(checkTrivialType(node, "long unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "float"));
  EXPECT_FALSE(checkTrivialType(node, "double"));
  EXPECT_TRUE(checkTrivialType(node, "string"));
  EXPECT_TRUE(checkTrivialType(node, "std::string"));
  EXPECT_FALSE(checkTrivialType(node, "VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "MatrixXd"));
}

TEST(check_type, string)
{
  YAML::Node node;
  node = "gromenauer";
  
  EXPECT_FALSE(checkTrivialType(node, "unknown_class"));
  EXPECT_FALSE(checkTrivialType(node, "bool"));
  EXPECT_FALSE(checkTrivialType(node, "char"));
  EXPECT_FALSE(checkTrivialType(node, "int"));
  EXPECT_FALSE(checkTrivialType(node, "unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "long int"));
  EXPECT_FALSE(checkTrivialType(node, "long unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "float"));
  EXPECT_FALSE(checkTrivialType(node, "double"));
  EXPECT_TRUE(checkTrivialType(node, "string"));
  EXPECT_TRUE(checkTrivialType(node, "std::string"));
  EXPECT_FALSE(checkTrivialType(node, "VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "MatrixXd"));
}

TEST(check_type, Eigen)
{
  YAML::Node node;
  node[0] = 0;
  node[1] = 0.2;
  node[2] = 1e-5;
  node[3] = 1000;
  
  EXPECT_FALSE(checkTrivialType(node, "unknown_class"));
  EXPECT_FALSE(checkTrivialType(node, "bool"));
  EXPECT_FALSE(checkTrivialType(node, "char"));
  EXPECT_FALSE(checkTrivialType(node, "int"));
  EXPECT_FALSE(checkTrivialType(node, "unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "long int"));
  EXPECT_FALSE(checkTrivialType(node, "long unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "float"));
  EXPECT_FALSE(checkTrivialType(node, "double"));
  EXPECT_FALSE(checkTrivialType(node, "string"));
  EXPECT_FALSE(checkTrivialType(node, "std::string"));
  EXPECT_TRUE(checkTrivialType(node, "VectorXd"));
  EXPECT_TRUE(checkTrivialType(node, "Eigen::VectorXd"));
  EXPECT_TRUE(checkTrivialType(node, "Vector4d"));
  EXPECT_TRUE(checkTrivialType(node, "Matrix2d"));
  EXPECT_FALSE(checkTrivialType(node, "MatrixXd"));
}

TEST(check_type, EigenEmpty)
{
  YAML::Node node_loaded = YAML::LoadFile(ROOT_DIR + "/test/yaml/empty_vectorXd.yaml");
  
  YAML::Node node = node_loaded["vector"];
  EXPECT_TRUE(node.IsDefined());
  EXPECT_EQ(node.size(),0);
  EXPECT_TRUE(node.IsSequence());

  EXPECT_FALSE(checkTrivialType(node, "unknown_class"));
  EXPECT_FALSE(checkTrivialType(node, "bool"));
  EXPECT_FALSE(checkTrivialType(node, "char"));
  EXPECT_FALSE(checkTrivialType(node, "int"));
  EXPECT_FALSE(checkTrivialType(node, "unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "long int"));
  EXPECT_FALSE(checkTrivialType(node, "long unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "float"));
  EXPECT_FALSE(checkTrivialType(node, "double"));
  EXPECT_FALSE(checkTrivialType(node, "string"));
  EXPECT_FALSE(checkTrivialType(node, "std::string"));
  EXPECT_TRUE(checkTrivialType(node, "VectorXd"));
  EXPECT_TRUE(checkTrivialType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "Vector4d"));
  EXPECT_FALSE(checkTrivialType(node, "Matrix2d"));
  EXPECT_TRUE(checkTrivialType(node, "MatrixXd"));
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
  
  EXPECT_FALSE(checkTrivialType(node, "unknown_class"));
  EXPECT_FALSE(checkTrivialType(node, "bool"));
  EXPECT_FALSE(checkTrivialType(node, "char"));
  EXPECT_FALSE(checkTrivialType(node, "int"));
  EXPECT_FALSE(checkTrivialType(node, "unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "long int"));
  EXPECT_FALSE(checkTrivialType(node, "long unsigned int"));
  EXPECT_FALSE(checkTrivialType(node, "float"));
  EXPECT_FALSE(checkTrivialType(node, "double"));
  EXPECT_FALSE(checkTrivialType(node, "string"));
  EXPECT_FALSE(checkTrivialType(node, "std::string"));
  EXPECT_FALSE(checkTrivialType(node, "VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "Eigen::VectorXd"));
  EXPECT_FALSE(checkTrivialType(node, "Matrix2d"));
  EXPECT_TRUE(checkTrivialType(node, "MatrixXd"));
  EXPECT_TRUE(checkTrivialType(node, "Eigen::MatrixXd"));
}

TEST(check_type, trivial_types)
{
  EXPECT_FALSE(isTrivialType("unknown_class"));
  EXPECT_FALSE(isTrivialType("some_weird_schema"));
  EXPECT_TRUE(isTrivialType("bool"));
  EXPECT_TRUE(isTrivialType("char"));
  EXPECT_TRUE(isTrivialType("int"));
  EXPECT_TRUE(isTrivialType("unsigned int"));
  EXPECT_TRUE(isTrivialType("long int"));
  EXPECT_TRUE(isTrivialType("long unsigned int"));
  EXPECT_TRUE(isTrivialType("float"));
  EXPECT_TRUE(isTrivialType("double"));
  EXPECT_TRUE(isTrivialType("string"));
  EXPECT_TRUE(isTrivialType("std::string"));
  EXPECT_TRUE(isTrivialType("VectorXd"));
  EXPECT_TRUE(isTrivialType("Eigen::VectorXd"));
  EXPECT_TRUE(isTrivialType("Matrix2d"));
  EXPECT_TRUE(isTrivialType("MatrixXd"));
  EXPECT_TRUE(isTrivialType("Eigen::MatrixXd"));
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
