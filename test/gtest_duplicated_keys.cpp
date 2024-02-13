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
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

TEST(DuplicatedKeys, test1)
{
    yaml_schema_cpp::YamlServer yaml_server({ROOT_DIR + "/test/yaml"}, ROOT_DIR + "/test/yaml/duplicated_input.yaml");

    auto input_node = yaml_server.getNode();

    EXPECT_EQ(input_node["map1"].size(), 3);
    EXPECT_TRUE(input_node["map1"]["param1"]);
    EXPECT_TRUE(input_node["map1"]["param2"]);
    EXPECT_TRUE(input_node["map1"]["param3"]);

    // EXPECT_NEAR(input_node["map1"]["param3"].as<double>(), 4.5, 1e-12);
    // EXPECT_EQ(input_node["map1"]["param1"].as<int>(), 1);
    // EXPECT_EQ(input_node["map1"]["param2"].as<std::string>(), "string");
    // auto node_aux = input_node["map1"]["param3"];
    // EXPECT_NEAR(input_node["map1"]["param3"].as<double>(), 4.5, 1e-12);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
