// Copyright (C) 2022 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/internal/config.h"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(addNodeSchema, add_scalars)
{
  YAML::Node node1, node2;
  node1["param1"][MANDATORY] = true;
  node1["param1"][TYPE] = "bool";
  node1["param1"][DOC] = "scalar";
  node2["param2"][MANDATORY] = true;
  node2["param2"][TYPE] = "string";
  node2["param2"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_TRUE(node2["param2"]);
  ASSERT_TRUE(node2["param2"][MANDATORY]);
  ASSERT_TRUE(node2["param2"][TYPE]);
  ASSERT_TRUE(node2["param2"][DOC]);

  addNodeSchema(node2, "param1", node1["param1"], true);
  
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"][MANDATORY]);
  ASSERT_TRUE(node2["param1"][TYPE]);
  ASSERT_TRUE(node2["param1"][DOC]);
  ASSERT_TRUE(node2["param2"]);
  ASSERT_TRUE(node2["param2"][MANDATORY]);
  ASSERT_TRUE(node2["param2"][TYPE]);
  ASSERT_TRUE(node2["param2"][DOC]);
}

TEST(addNodeSchema, merge_map)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"][MANDATORY] = true;
  node1["namespace"]["param1"][TYPE] = "bool";
  node1["namespace"]["param1"][DOC] = "scalar";
  node2["namespace"]["param2"][MANDATORY] = true;
  node2["namespace"]["param2"][TYPE] = "string";
  node2["namespace"]["param2"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"][DOC]);
  ASSERT_TRUE(node2["namespace"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param2"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param2"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param2"][DOC]);

  addNodeSchema(node2, "namespace", node1["namespace"], true);
  
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param1"][DOC]);
  ASSERT_TRUE(node2["namespace"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param2"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param2"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param2"][DOC]);
}

TEST(addNodeSchema, existing_scalar_override)
{
  YAML::Node node1, node2;
  node1["param1"][MANDATORY] = true;
  node1["param1"][TYPE] = "bool";
  node1["param1"][DOC] = "scalar";
  node2["param1"][MANDATORY] = true;
  node2["param1"][TYPE] = "string";
  node2["param1"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"][MANDATORY]);
  ASSERT_TRUE(node2["param1"][TYPE]);
  ASSERT_TRUE(node2["param1"][DOC]);

  addNodeSchema(node1, "param1", node2["param1"], true);

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_EQ(node1["param1"][TYPE].as<std::string>(),"string");
}

TEST(addNodeSchema, existing_scalar_dont_override)
{
  YAML::Node node1, node2;
  node1["param1"][MANDATORY] = true;
  node1["param1"][TYPE] = "bool";
  node1["param1"][DOC] = "scalar";
  node2["param1"][MANDATORY] = true;
  node2["param1"][TYPE] = "string";
  node2["param1"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"][MANDATORY]);
  ASSERT_TRUE(node2["param1"][TYPE]);
  ASSERT_TRUE(node2["param1"][DOC]);

  try
  {
    addNodeSchema(node1, "param1", node2["param1"], false);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "param1", node2["param1"], false), std::runtime_error);
}

TEST(addNodeSchema, existing_sequence_override)
{
  YAML::Node node1, node2;
  node1["param1"][MANDATORY] = true;
  node1["param1"][TYPE] = "bool";
  node1["param1"][DOC] = "sequence";
  node2["param1"][MANDATORY] = true;
  node2["param1"][TYPE] = "string";
  node2["param1"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"][MANDATORY]);
  ASSERT_TRUE(node2["param1"][TYPE]);
  ASSERT_TRUE(node2["param1"][DOC]);

  addNodeSchema(node1, "param1", node2["param1"], true);

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_EQ(node1["param1"][TYPE].as<std::string>(),"string");
}

TEST(addNodeSchema, existing_sequence_dont_override)
{
  YAML::Node node1, node2;
  node1["param1"][MANDATORY] = true;
  node1["param1"][TYPE] = "bool";
  node1["param1"][DOC] = "sequence";
  node2["param1"][MANDATORY] = true;
  node2["param1"][TYPE] = "string";
  node2["param1"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["param1"]);
  ASSERT_TRUE(node1["param1"][MANDATORY]);
  ASSERT_TRUE(node1["param1"][TYPE]);
  ASSERT_TRUE(node1["param1"][DOC]);
  ASSERT_TRUE(node2["param1"]);
  ASSERT_TRUE(node2["param1"][MANDATORY]);
  ASSERT_TRUE(node2["param1"][TYPE]);
  ASSERT_TRUE(node2["param1"][DOC]);

  try
  {
    addNodeSchema(node1, "param1", node2["param1"], false);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "param1", node2["param1"], false), std::runtime_error);
}

TEST(addNodeSchema, existing_map_scalar_override)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"][MANDATORY] = true;
  node1["namespace"]["param1"][TYPE] = "bool";
  node1["namespace"]["param1"][DOC] = "scalar";
  node2["namespace"]["param1"][MANDATORY] = true;
  node2["namespace"]["param1"][TYPE] = "string";
  node2["namespace"]["param1"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"][DOC]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param1"][DOC]);

  addNodeSchema(node1, "namespace", node2["namespace"], true);

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"][DOC]);
  ASSERT_EQ(node1["namespace"]["param1"][TYPE].as<std::string>(),"string");
}

TEST(addNodeSchema, existing_map_scalar_dont_override)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"][MANDATORY] = true;
  node1["namespace"]["param1"][TYPE] = "bool";
  node1["namespace"]["param1"][DOC] = "scalar";
  node2["namespace"]["param1"][MANDATORY] = true;
  node2["namespace"]["param1"][TYPE] = "string";
  node2["namespace"]["param1"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"][DOC]);
  ASSERT_TRUE(node2["namespace"]["param1"]);
  ASSERT_TRUE(node2["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param1"][DOC]);

  try
  {
    addNodeSchema(node1, "namespace", node2["namespace"], false);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "namespace", node2["namespace"], false), std::runtime_error);
}

TEST(addNodeSchema, existing_diff_types_override)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"][MANDATORY] = true;
  node1["namespace"]["param1"][TYPE] = "bool";
  node1["namespace"]["param1"][DOC] = "scalar";
  node2["namespace"]["param1"]["param2"][MANDATORY] = true;
  node2["namespace"]["param1"]["param2"][TYPE] = "string";
  node2["namespace"]["param1"]["param2"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"][DOC]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"][DOC]);

  addNodeSchema(node1, "namespace", node2["namespace"], true);

  ASSERT_TRUE(node1["namespace"]["param1"]["param2"]);
  ASSERT_TRUE(node1["namespace"]["param1"]["param2"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"]["param2"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"]["param2"][DOC]);
}

TEST(addNodeSchema, existing_diff_types_dont_override)
{
  YAML::Node node1, node2;
  node1["namespace"]["param1"][MANDATORY] = true;
  node1["namespace"]["param1"][TYPE] = "bool";
  node1["namespace"]["param1"][DOC] = "scalar";
  node2["namespace"]["param1"]["param2"][MANDATORY] = true;
  node2["namespace"]["param1"]["param2"][TYPE] = "string";
  node2["namespace"]["param1"]["param2"][DOC] = "scalar";

  std::cout << "node1: " << std::endl << node1 << std::endl;
  std::cout << "node2: " << std::endl << node2 << std::endl;

  ASSERT_TRUE(node1["namespace"]["param1"]);
  ASSERT_TRUE(node1["namespace"]["param1"][MANDATORY]);
  ASSERT_TRUE(node1["namespace"]["param1"][TYPE]);
  ASSERT_TRUE(node1["namespace"]["param1"][DOC]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"][MANDATORY]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"][TYPE]);
  ASSERT_TRUE(node2["namespace"]["param1"]["param2"][DOC]);

  try
  {
    addNodeSchema(node1, "namespace", node2["namespace"], false);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node1, "namespace", node2["namespace"], false), std::runtime_error);

  try
  {
    addNodeSchema(node2, "namespace", node1["namespace"], false);
  }
  catch (const std::exception& e)
  {
    std::cout << "exception: " << e.what() << std::endl;
  }
  ASSERT_THROW(addNodeSchema(node2, "namespace", node1["namespace"], false), std::runtime_error);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
