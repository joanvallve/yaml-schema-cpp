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
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(findNodesWithKey, test1)
{
  yaml_schema_cpp::YamlServer yaml_server({}, ROOT_DIR + "/test/yaml/test_find_nodes_with_key.yaml");

  auto input_node = yaml_server.getNode();

  std::list<YAML::Node> nodes_found = findNodesWithKey(input_node, "easter_egg");

  EXPECT_EQ(nodes_found.size(), 5);
  auto nodes_it = nodes_found.begin();
  int node_gt = 0;
  while (nodes_it != nodes_found.end())
  {
    EXPECT_EQ(nodes_it->as<int>(), node_gt);
    nodes_it++;
    node_gt++;
  }
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
