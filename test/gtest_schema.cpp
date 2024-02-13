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
#include "yaml-schema-cpp/yaml_schema.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(schema, plain_yaml)
{
    std::stringstream log;
    auto              node_schema = loadSchema(
        "test1.schema", {ROOT_DIR + "/test/schema/folder_schema", ROOT_DIR + "/test/schema/other_folder_schema"}, log);
    ASSERT_TRUE(node_schema.IsDefined());
    std::cout << log.str() << std::endl;
}

TEST(schema, follow)
{
    std::stringstream log;
    auto              node_schema = loadSchema(
        "test2.schema", {ROOT_DIR + "/test/schema/folder_schema", ROOT_DIR + "/test/schema/other_folder_schema"}, log);
    ASSERT_TRUE(node_schema.IsDefined());
    std::cout << log.str() << std::endl;
}

TEST(schema, empty_yaml)
{
    std::stringstream log;
    auto              node_schema = loadSchema(
        "empty.schema", {ROOT_DIR + "/test/schema/folder_schema", ROOT_DIR + "/test/schema/other_folder_schema"}, log);
    ASSERT_TRUE(node_schema.IsDefined());
    std::cout << log.str() << std::endl;
}

TEST(schema, wrong)
{
    std::list<std::string> wrong_schemas{"not_mandatory",
                                         "not_type",
                                         "not_doc",
                                         "not_base",
                                         "wrong_default",
                                         "wrong_default_options",
                                         "wrong_value",
                                         "wrong_value_options",
                                         "wrong_expression",
                                         "wrong_expression2",
                                         "wrong_expression3",
                                         "wrong_mandatory",
                                         "wrong_doc",
                                         "wrong_options",
                                         "wrong_options2"};

    for (auto w_schema : wrong_schemas)
    {
        std::stringstream log;
        std::cout << "Checking " << w_schema << ".schema..." << std::endl;
        auto node_schema = loadSchema(w_schema + ".schema", {ROOT_DIR + "/test/wrong_schema"}, log);
        ASSERT_FALSE(node_schema.IsDefined());
        std::cout << log.str() << std::endl;
    }
}

TEST(schema, override)
{
    std::list<std::string> wrong_schemas{"same_param", "same_param_diff_type"};

    for (auto w_schema : wrong_schemas)
    {
        std::stringstream log;
        std::cout << "Checking " << w_schema << ".schema..." << std::endl;
        auto node_schema = loadSchema(w_schema + ".schema", {ROOT_DIR + "/test/schema"}, log);
        ASSERT_TRUE(node_schema.IsDefined());
        std::cout << log.str() << std::endl;
    }
}

TEST(schema, dont_override)
{
    std::list<std::string> wrong_schemas{"same_param", "same_param_diff_type"};

    for (auto w_schema : wrong_schemas)
    {
        std::stringstream log;
        std::cout << "Checking " << w_schema << ".schema..." << std::endl;
        auto node_schema = loadSchema(w_schema + ".schema", {ROOT_DIR + "/test/schema"}, log);
        ASSERT_TRUE(node_schema.IsDefined());
        std::cout << log.str() << std::endl;
    }
}

TEST(schema, validate_all_schemas)
{
    ASSERT_TRUE(validateAllSchemas({ROOT_DIR + "/test/schema"}, true));
    ASSERT_FALSE(validateAllSchemas({ROOT_DIR + "/test/wrong_schema"}, true));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
