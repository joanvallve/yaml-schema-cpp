// Copyright (C) 2022,2023 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(addNodeYaml, empty)
{
    YAML::Node node1, node2;
    node1["param1"] = 1.0;
    node1["param2"] = 2.0;

    std::cout << "node1: " << std::endl << node1 << std::endl;
    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1["param1"]);
    ASSERT_TRUE(node1["param2"]);

    addNodeYaml(node2, "namespace", node1, true);

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);
}

TEST(addNodeYaml, empty2)
{
    YAML::Node node1, node2, node3;
    node1["param1"]    = 1.0;
    node1["param2"]    = 2.0;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;
    std::cout << "node3: " << std::endl << node3 << std::endl;

    ASSERT_TRUE(node1["param1"]);
    ASSERT_TRUE(node1["param2"]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);

    addNodeYaml(node3, "namespace", node2["namespace"], true);

    std::cout << "node3: " << std::endl << node3 << std::endl;

    ASSERT_TRUE(node3["namespace"]);
    ASSERT_TRUE(node3["namespace"]["param1"]);
    ASSERT_TRUE(node3["namespace"]["param2"]);
}

TEST(addNodeYaml, empty_sequence)
{
    YAML::Node node1, node2, node3;
    node1[0]           = 1.0;
    node1[1]           = 2.0;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;
    std::cout << "node3: " << std::endl << node3 << std::endl;

    ASSERT_TRUE(node1[0]);
    ASSERT_TRUE(node1[1]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"][0]);
    ASSERT_TRUE(node2["namespace"][1]);

    addNodeYaml(node3, "namespace", node2["namespace"], true);

    std::cout << "node3: " << std::endl << node3 << std::endl;

    ASSERT_TRUE(node3["namespace"]);
    ASSERT_TRUE(node3["namespace"][0]);
    ASSERT_TRUE(node3["namespace"][1]);
}

TEST(addNodeYaml, add)
{
    YAML::Node node1, node2, node3, node4;
    node1["param1"]    = 1.0;
    node1["param2"]    = 2.0;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1["param1"]);
    ASSERT_TRUE(node1["param2"]);
    ASSERT_FALSE(node1["param3"]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);
    ASSERT_FALSE(node2["namespace"]["param3"]);

    node3["param3"]    = 3.0;
    node4["namespace"] = node3;

    std::cout << "node4: " << std::endl << node4 << std::endl;

    ASSERT_TRUE(node3["param3"]);
    ASSERT_TRUE(node4["namespace"]);
    ASSERT_TRUE(node4["namespace"]["param3"]);

    addNodeYaml(node2, "namespace", node4["namespace"], true);

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1["param1"]);
    ASSERT_TRUE(node1["param2"]);
    ASSERT_TRUE(node1["param3"]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);
    ASSERT_TRUE(node2["namespace"]["param3"]);
}

TEST(addNodeYaml, same_key_override)
{
    YAML::Node node1, node2, node3, node4;
    node1["param1"]    = 1;
    node1["param2"]    = 2;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1["param1"]);
    ASSERT_TRUE(node1["param2"]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);

    node3["param1"]    = 3;
    node4["namespace"] = node3;

    std::cout << "node4: " << std::endl << node4 << std::endl;

    ASSERT_TRUE(node3["param1"]);
    ASSERT_TRUE(node4["namespace"]);
    ASSERT_TRUE(node4["namespace"]["param1"]);

    addNodeYaml(node2, "namespace", node4["namespace"], true);

    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);
    ASSERT_EQ(node2["namespace"]["param1"].as<int>(), 3);
}

TEST(addNodeYaml, same_key_dont_override)
{
    YAML::Node node1, node2, node3, node4;
    node1["param1"]    = 1.0;
    node1["param2"]    = 2.0;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1["param1"]);
    ASSERT_TRUE(node1["param2"]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"]["param1"]);
    ASSERT_TRUE(node2["namespace"]["param2"]);

    node3["param1"]    = 3.0;
    node4["namespace"] = node3;

    std::cout << "node4: " << std::endl << node4 << std::endl;

    ASSERT_TRUE(node3["param1"]);
    ASSERT_TRUE(node4["namespace"]);
    ASSERT_TRUE(node4["namespace"]["param1"]);

    ASSERT_THROW(addNodeYaml(node2, "namespace", node4["namespace"], false), std::runtime_error);
}

TEST(addNodeYaml, compose_sequence_override)
{
    YAML::Node node1, node2, node3, node4;
    node1[0]           = 1.0;
    node1[1]           = 2.0;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1[0]);
    ASSERT_TRUE(node1[1]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"][0]);
    ASSERT_TRUE(node2["namespace"][1]);

    node3[0]           = 3.0;
    node3[1]           = 4.0;
    node4["namespace"] = node3;

    std::cout << "node3: " << std::endl << node3 << std::endl;

    ASSERT_TRUE(node3[0]);
    ASSERT_TRUE(node3[1]);
    ASSERT_TRUE(node4["namespace"]);
    ASSERT_TRUE(node4["namespace"][0]);
    ASSERT_TRUE(node4["namespace"][1]);

    addNodeYaml(node2, "namespace", node4["namespace"], true);

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"][0]);
    ASSERT_TRUE(node2["namespace"][1]);
    ASSERT_TRUE(node2["namespace"][2]);
    ASSERT_TRUE(node2["namespace"][3]);
}

TEST(addNodeYaml, compose_sequence_dont_override)
{
    YAML::Node node1, node2, node3, node4;
    node1[0]           = 1.0;
    node1[1]           = 2.0;
    node2["namespace"] = node1;

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node1[0]);
    ASSERT_TRUE(node1[1]);
    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"][0]);
    ASSERT_TRUE(node2["namespace"][1]);

    node3[0]           = 3.0;
    node3[1]           = 4.0;
    node4["namespace"] = node3;

    std::cout << "node3: " << std::endl << node3 << std::endl;

    ASSERT_TRUE(node3[0]);
    ASSERT_TRUE(node3[1]);
    ASSERT_TRUE(node4["namespace"]);
    ASSERT_TRUE(node4["namespace"][0]);
    ASSERT_TRUE(node4["namespace"][1]);

    addNodeYaml(node2, "namespace", node4["namespace"], false);

    std::cout << "node2: " << std::endl << node2 << std::endl;

    ASSERT_TRUE(node2["namespace"]);
    ASSERT_TRUE(node2["namespace"][0]);
    ASSERT_TRUE(node2["namespace"][1]);
    ASSERT_TRUE(node2["namespace"][2]);
    ASSERT_TRUE(node2["namespace"][3]);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
