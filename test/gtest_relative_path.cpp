#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"
#include "yaml-schema-cpp/yaml_conversion.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(relative_path, same_folder)
{
    yaml_schema_cpp::YamlServer yaml_server({}, ROOT_DIR + "/test/yaml/relative_path_same_folder.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);

    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), ROOT_DIR + "/test/yaml/./base_input.yaml");

    ASSERT_TRUE(filesystem::exists(input_node["map1"]["param2"].as<std::string>()));
}

TEST(relative_path, other_folder)
{
    yaml_schema_cpp::YamlServer yaml_server({}, ROOT_DIR + "/test/yaml/relative_path_other_folder.yaml");

    YAML::Node input_node = yaml_server.getNode();

    std::cout << "input_node: " << std::endl << input_node << std::endl;

    ASSERT_TRUE(input_node["map1"]);
    ASSERT_TRUE(input_node["map1"]["param2"]);

    ASSERT_EQ(input_node["map1"]["param2"].as<std::string>(), ROOT_DIR + "/test/yaml/../../src/yaml_utils.cpp");
    ASSERT_TRUE(filesystem::exists(input_node["map1"]["param2"].as<std::string>()));
}

// Changes the current working directory during the life of the object
struct ScopedCurrentPath
{
    filesystem::path previous_;
    ScopedCurrentPath(const std::string& path) : previous_(filesystem::current_path())
    {
        filesystem::current_path(path);
    }
    ~ScopedCurrentPath()
    {
        filesystem::current_path(previous_);
    }
};

void checkRelativeFollow(const YAML::Node& input_node)
{
    std::cout << "input_node: " << std::endl << input_node << std::endl;

    ASSERT_TRUE(input_node["local_file"]);
    ASSERT_TRUE(input_node["sub_file"]);
    ASSERT_TRUE(input_node["sub_map"]);
    ASSERT_TRUE(input_node["sub_map"]["main_nested_file"]);
    ASSERT_TRUE(input_node["sub_map"]["sub_nested_file"]);

    EXPECT_TRUE(filesystem::exists(input_node["local_file"].as<std::string>()));
    EXPECT_TRUE(filesystem::exists(input_node["sub_file"].as<std::string>()));
    EXPECT_TRUE(filesystem::exists(input_node["sub_map"]["main_nested_file"].as<std::string>()));
    EXPECT_TRUE(filesystem::exists(input_node["sub_map"]["sub_nested_file"].as<std::string>()));
}

TEST(relative_path, relative_input_path)
{
    ScopedCurrentPath scoped_path(ROOT_DIR + "/test/yaml");

    yaml_schema_cpp::YamlServer yaml_server({}, "./relative_follow/main.yaml");
    YAML::Node                  input_node = yaml_server.getNode();

    checkRelativeFollow(input_node);
    EXPECT_EQ(input_node["sub_file"].as<std::string>(), "./relative_follow/sub/./sub.yaml");
}

TEST(relative_path, bare_input_file_name)
{
    ScopedCurrentPath scoped_path(ROOT_DIR + "/test/yaml/relative_follow");

    YAML::Node input_node;
    ASSERT_NO_THROW(input_node = yaml_schema_cpp::YamlServer({}, "main.yaml").getNode());

    checkRelativeFollow(input_node);
}

TEST(relative_path, absolute_follow)
{
    YAML::Node node = YAML::Load("follow: " + ROOT_DIR + "/test/yaml/relative_follow/sub/sub.yaml");

    ASSERT_NO_THROW(flattenNode(node, ROOT_DIR + "/test/yaml", {}, false, true));

    std::cout << "node: " << std::endl << node << std::endl;
    ASSERT_TRUE(node["sub_file"]);
    EXPECT_EQ(node["sub_file"].as<std::string>(), ROOT_DIR + "/test/yaml/relative_follow/sub/./sub.yaml");
    EXPECT_TRUE(filesystem::exists(node["sub_file"].as<std::string>()));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
