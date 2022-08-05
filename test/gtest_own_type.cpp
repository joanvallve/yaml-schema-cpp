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
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(TestOwnType, SingleMandatory)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/own_type/single_mandatory.yaml");

    // Check schema is OK
    ASSERT_TRUE(server.applySchema("single_mandatory.schema"));

    YAML::Node node = server.getNode();

    // Check existence default
    ASSERT_TRUE(node["own_type"]["map1"]["param3"]);
    ASSERT_TRUE(node["own_type"]["param4"]);

    // Check value
    ASSERT_DOUBLE_EQ(node["own_type"]["map1"]["param3"].as<double>(), 3.5);
    ASSERT_EQ(node["own_type"]["param4"].as<std::string>(), "hello");
}

TEST(TestOwnType, SequenceMandatory)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/own_type/sequence_mandatory.yaml");

    // Check schema is OK
    ASSERT_TRUE(server.applySchema("sequence_mandatory.schema"));

    YAML::Node node = server.getNode();

    ASSERT_EQ(node["own_type"][0]["map1"]["param1"].as<int>(), 1);
    ASSERT_EQ(node["own_type"][0]["map1"]["param2"].as<std::string>(), "string");
    ASSERT_DOUBLE_EQ(node["own_type"][0]["map1"]["param3"].as<double>(), 3.5);
    ASSERT_EQ(node["own_type"][0]["param4"].as<std::string>(), "hello");
    ASSERT_TRUE(node["own_type"][0]["param5"].as<std::vector<int>>() == std::vector<int>({5, 6, -1, -7, 0}));

    ASSERT_EQ(node["own_type"][1]["map1"]["param1"].as<int>(), 4);
    ASSERT_EQ(node["own_type"][1]["map1"]["param2"].as<std::string>(), "strong");
    ASSERT_DOUBLE_EQ(node["own_type"][1]["map1"]["param3"].as<double>(), 4.4);
    ASSERT_EQ(node["own_type"][1]["param4"].as<std::string>(), "gromenauer");
    ASSERT_TRUE(node["own_type"][1]["param5"].as<std::vector<int>>() == std::vector<int>({4, 6, -1, -7, 10}));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
