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

TEST(TestTypeDerived, TestFinalSchema)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/type_derived/type_derived.yaml");

    // Check schema is OK
    ASSERT_TRUE(server.applySchema("type_derived_final.schema"));

    YAML::Node node = server.getNode();

    // Check value
    ASSERT_EQ(node["final_1"].as<std::string>(), "test_derived");
    ASSERT_DOUBLE_EQ(node["final_derived"]["param_base"].as<double>(), 2.4);
    ASSERT_EQ(node["final_derived"]["param_derived"].as<std::vector<int>>(), std::vector<int>({1, 2, 3, 4}));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
