#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"

std::string ROOT_DIR   = _YAML_SCHEMA_CPP_ROOT_DIR;
std::string SCHEMA_DIR = ROOT_DIR + "/test/schema";

using namespace yaml_schema_cpp;

TEST(flatten_yaml, plain)
{
    YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/base_input.yaml");

    YAML::Node gt_node = YAML::LoadFile(ROOT_DIR + "/test/yaml/base_input.yaml");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(
        compareNodesAutoType(yaml_server.getNode(), gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_yaml, relative_path)
{
    YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/flatten/flatten_relative_path.yaml");

    YAML::Node gt_node = YAML::LoadFile(ROOT_DIR + "/test/yaml/flatten/flatten_relative_path_gt.yaml");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(
        compareNodesAutoType(yaml_server.getNode(), gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_yaml, recursive)
{
    YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/flatten/flatten_recursive.yaml");

    YAML::Node gt_node = YAML::LoadFile(ROOT_DIR + "/test/yaml/flatten/flatten_recursive_gt.yaml");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(
        compareNodesAutoType(yaml_server.getNode(), gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_yaml, merge)
{
    YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/flatten/flatten_merge.yaml");

    YAML::Node gt_node = YAML::LoadFile(ROOT_DIR + "/test/yaml/flatten/flatten_merge_gt.yaml");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(
        compareNodesAutoType(yaml_server.getNode(), gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_yaml, override)
{
    // if override not allowed should crash
    ASSERT_THROW(YamlServer({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/flatten/flatten_override.yaml", false),
                 std::runtime_error);

    // Allow override
    YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/flatten/flatten_override.yaml");

    YAML::Node gt_node = YAML::LoadFile(ROOT_DIR + "/test/yaml/flatten/flatten_override_gt.yaml");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(
        compareNodesAutoType(yaml_server.getNode(), gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_yaml, sequence_follow)
{
    YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/flatten/flatten_sequence_follow.yaml");

    YAML::Node gt_node = YAML::LoadFile(ROOT_DIR + "/test/yaml/flatten/flatten_sequence_follow_gt.yaml");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(
        compareNodesAutoType(yaml_server.getNode(), gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_schema, plain)
{
    std::stringstream log;
    auto              schema_flatten = loadSchema("base_input.schema", {SCHEMA_DIR}, log);

    YAML::Node gt_node = YAML::LoadFile(SCHEMA_DIR + "/folder_schema/base_input.schema");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(compareNodesAutoType(schema_flatten, gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_schema, relative_path)
{
    std::stringstream log;
    auto              schema_flatten = loadSchema("flatten_relative_path.schema", {SCHEMA_DIR}, log);

    YAML::Node gt_node = YAML::LoadFile(SCHEMA_DIR + "/flatten/flatten_relative_path_gt.schema");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(compareNodesAutoType(schema_flatten, gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_schema, recursive)
{
    std::stringstream log;
    auto              schema_flatten = loadSchema("flatten_recursive.schema", {SCHEMA_DIR}, log);

    YAML::Node gt_node = YAML::LoadFile(SCHEMA_DIR + "/flatten/flatten_recursive_gt.schema");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(compareNodesAutoType(schema_flatten, gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_schema, merge)
{
    std::stringstream log;
    auto              schema_flatten = loadSchema("flatten_merge.schema", {SCHEMA_DIR}, log);

    YAML::Node gt_node = YAML::LoadFile(SCHEMA_DIR + "/flatten/flatten_merge_gt.schema");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(compareNodesAutoType(schema_flatten, gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_schema, override)
{
    std::stringstream log;
    auto              schema_flatten = loadSchema("flatten_override.schema", {SCHEMA_DIR}, log);

    YAML::Node gt_node = YAML::LoadFile(SCHEMA_DIR + "/flatten/flatten_override_gt.schema");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(compareNodesAutoType(schema_flatten, gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_schema, sequence_follow)
{
    std::stringstream log;
    auto              schema_flatten = loadSchema("flatten_sequence_follow.schema", {SCHEMA_DIR}, log);

    YAML::Node gt_node = YAML::LoadFile(SCHEMA_DIR + "/flatten/flatten_sequence_follow_gt.schema");

    // check flatten node == groundtruth yaml loaded
    ASSERT_TRUE(compareNodesAutoType(schema_flatten, gt_node));  // compareNodesAutoType validated at gtest_yaml_utils
}

TEST(flatten_yaml, follow_cycle)
{
    // a.yaml follows b.yaml, that follows a.yaml
    try
    {
        YamlServer yaml_server({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/follow_cycle/a.yaml");
        FAIL() << "A 'follow' cycle should throw";
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        EXPECT_NE(std::string(e.what()).find("cycle"), std::string::npos) << e.what();
    }
}

TEST(flatten_yaml, follow_diamond)
{
    // left.yaml and right.yaml follow common.yaml: not a cycle
    YAML::Node node;
    ASSERT_NO_THROW(node = YamlServer({SCHEMA_DIR}, ROOT_DIR + "/test/yaml/follow_diamond/main.yaml").getNode());

    EXPECT_EQ(node["main_param"].as<int>(), 0);
    EXPECT_EQ(node["left_param"].as<int>(), 1);
    EXPECT_EQ(node["right_param"].as<int>(), 2);
    EXPECT_EQ(node["common_param"].as<int>(), 3);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
