// Copyright (C) 2022,2023,2024 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
// Authors: Josep Martí Saumell (jmarti@iri.upc.edu) and Joan Vallvé Navarro (jvallve@iri.upc.edu)
// All rights reserved.
//
// YAML-SCHEMA-CPP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/type_check.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(check_type, num)
{
    YAML::Node node;
    node = 3;

    EXPECT_FALSE(tryNodeAs(node, "unknown_class"));
    EXPECT_FALSE(tryNodeAs(node, "bool"));
    EXPECT_TRUE(tryNodeAs(node, "char"));
    EXPECT_TRUE(tryNodeAs(node, "int"));
    EXPECT_TRUE(tryNodeAs(node, "unsigned int"));
    EXPECT_TRUE(tryNodeAs(node, "long int"));
    EXPECT_TRUE(tryNodeAs(node, "long unsigned int"));
    EXPECT_TRUE(tryNodeAs(node, "float"));
    EXPECT_TRUE(tryNodeAs(node, "double"));
    EXPECT_TRUE(tryNodeAs(node, "string"));
    EXPECT_TRUE(tryNodeAs(node, "std::string"));
    EXPECT_FALSE(tryNodeAs(node, "VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "Eigen::VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "MatrixXd"));
}

TEST(check_type, char)
{
    YAML::Node node;
    node = 'g';

    EXPECT_FALSE(tryNodeAs(node, "unknown_class"));
    EXPECT_FALSE(tryNodeAs(node, "bool"));
    EXPECT_TRUE(tryNodeAs(node, "char"));
    EXPECT_FALSE(tryNodeAs(node, "int"));
    EXPECT_FALSE(tryNodeAs(node, "unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "long int"));
    EXPECT_FALSE(tryNodeAs(node, "long unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "float"));
    EXPECT_FALSE(tryNodeAs(node, "double"));
    EXPECT_TRUE(tryNodeAs(node, "string"));
    EXPECT_TRUE(tryNodeAs(node, "std::string"));
    EXPECT_FALSE(tryNodeAs(node, "VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "Eigen::VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "MatrixXd"));
}

TEST(check_type, string)
{
    YAML::Node node;
    node = "gromenauer";

    EXPECT_FALSE(tryNodeAs(node, "unknown_class"));
    EXPECT_FALSE(tryNodeAs(node, "bool"));
    EXPECT_FALSE(tryNodeAs(node, "char"));
    EXPECT_FALSE(tryNodeAs(node, "int"));
    EXPECT_FALSE(tryNodeAs(node, "unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "long int"));
    EXPECT_FALSE(tryNodeAs(node, "long unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "float"));
    EXPECT_FALSE(tryNodeAs(node, "double"));
    EXPECT_TRUE(tryNodeAs(node, "string"));
    EXPECT_TRUE(tryNodeAs(node, "std::string"));
    EXPECT_FALSE(tryNodeAs(node, "VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "Eigen::VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "MatrixXd"));
}

TEST(check_type, Eigen)
{
    YAML::Node node;
    node[0] = 0;
    node[1] = 0.2;
    node[2] = 1e-5;
    node[3] = 1000;

    EXPECT_FALSE(tryNodeAs(node, "unknown_class"));
    EXPECT_FALSE(tryNodeAs(node, "bool"));
    EXPECT_FALSE(tryNodeAs(node, "char"));
    EXPECT_FALSE(tryNodeAs(node, "int"));
    EXPECT_FALSE(tryNodeAs(node, "unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "long int"));
    EXPECT_FALSE(tryNodeAs(node, "long unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "float"));
    EXPECT_FALSE(tryNodeAs(node, "double"));
    EXPECT_FALSE(tryNodeAs(node, "string"));
    EXPECT_FALSE(tryNodeAs(node, "std::string"));
    EXPECT_TRUE(tryNodeAs(node, "VectorXd"));
    EXPECT_TRUE(tryNodeAs(node, "Eigen::VectorXd"));
    EXPECT_TRUE(tryNodeAs(node, "Vector4d"));
    EXPECT_TRUE(tryNodeAs(node, "Matrix2d"));
    EXPECT_FALSE(tryNodeAs(node, "MatrixXd"));
}

TEST(check_type, EigenEmpty)
{
    YAML::Node node_loaded = YAML::LoadFile(ROOT_DIR + "/test/yaml/empty_vectorXd.yaml");

    YAML::Node node = node_loaded["vector"];
    EXPECT_TRUE(node.IsDefined());
    EXPECT_EQ(node.size(), 0);
    EXPECT_TRUE(node.IsSequence());

    EXPECT_FALSE(tryNodeAs(node, "unknown_class"));
    EXPECT_FALSE(tryNodeAs(node, "bool"));
    EXPECT_FALSE(tryNodeAs(node, "char"));
    EXPECT_FALSE(tryNodeAs(node, "int"));
    EXPECT_FALSE(tryNodeAs(node, "unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "long int"));
    EXPECT_FALSE(tryNodeAs(node, "long unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "float"));
    EXPECT_FALSE(tryNodeAs(node, "double"));
    EXPECT_FALSE(tryNodeAs(node, "string"));
    EXPECT_FALSE(tryNodeAs(node, "std::string"));
    EXPECT_TRUE(tryNodeAs(node, "VectorXd"));
    EXPECT_TRUE(tryNodeAs(node, "Eigen::VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "Vector4d"));
    EXPECT_FALSE(tryNodeAs(node, "Matrix2d"));
    EXPECT_TRUE(tryNodeAs(node, "MatrixXd"));
}

TEST(check_type, Eigen_matrix)
{
    YAML::Node node_sizes, node_values, node;
    node_sizes[0]  = 3;
    node_sizes[1]  = 2;
    node_values[0] = 1e-5;
    node_values[1] = 1000;
    node_values[2] = 1e-5;
    node_values[3] = 1000;
    node_values[4] = 1e-5;
    node_values[5] = 1000;
    node[0]        = node_sizes;
    node[1]        = node_values;

    EXPECT_FALSE(tryNodeAs(node, "unknown_class"));
    EXPECT_FALSE(tryNodeAs(node, "bool"));
    EXPECT_FALSE(tryNodeAs(node, "char"));
    EXPECT_FALSE(tryNodeAs(node, "int"));
    EXPECT_FALSE(tryNodeAs(node, "unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "long int"));
    EXPECT_FALSE(tryNodeAs(node, "long unsigned int"));
    EXPECT_FALSE(tryNodeAs(node, "float"));
    EXPECT_FALSE(tryNodeAs(node, "double"));
    EXPECT_FALSE(tryNodeAs(node, "string"));
    EXPECT_FALSE(tryNodeAs(node, "std::string"));
    EXPECT_FALSE(tryNodeAs(node, "VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "Eigen::VectorXd"));
    EXPECT_FALSE(tryNodeAs(node, "Matrix2d"));
    EXPECT_TRUE(tryNodeAs(node, "MatrixXd"));
    EXPECT_TRUE(tryNodeAs(node, "Eigen::MatrixXd"));
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
