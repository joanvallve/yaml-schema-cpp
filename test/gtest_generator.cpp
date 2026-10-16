#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_generator.hpp"

#include <fstream>
#include <sstream>

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(generator, plain_yaml)
{
    std::cout << "SCHEMA:\n"
              << YAML::LoadFile(ROOT_DIR + "/test/schema/folder_schema/test1.schema") << std::endl
              << std::endl;

    std::cout << "GENERATED YAML:\n" << generateYaml("test1.schema", {ROOT_DIR}) << std::endl;
}

TEST(generator, expression)
{
    std::cout << "SCHEMA:\n"
              << YAML::LoadFile(ROOT_DIR + "/test/schema/folder_schema/expression.schema") << std::endl
              << std::endl;

    std::cout << "GENERATED YAML:\n" << generateYaml("expression.schema", {ROOT_DIR}) << std::endl;
}

TEST(generator, follow)
{
    std::cout << "SCHEMA:\n"
              << YAML::LoadFile(ROOT_DIR + "/test/schema/other_folder_schema/test2.schema") << std::endl
              << std::endl;

    std::cout << "GENERATED YAML:\n" << generateYaml("test2.schema", {ROOT_DIR}) << std::endl;
}

TEST(generator, own_type)
{
    std::cout << "SCHEMA:\n"
              << YAML::LoadFile(ROOT_DIR + "/test/schema/own_type/single_mandatory.schema") << std::endl
              << std::endl;

    std::cout << "GENERATED YAML:\n" << generateYaml("single_mandatory.schema", {ROOT_DIR}) << std::endl;
}

TEST(generator, derived)
{
    std::cout << "SCHEMA:\n"
              << YAML::LoadFile(ROOT_DIR + "/test/schema/type_derived/type_derived_final.schema") << std::endl
              << std::endl;

    std::cout << "GENERATED YAML:\n" << generateYaml("type_derived_final.schema", {ROOT_DIR}) << std::endl;
}

TEST(generator, sequence_of_derived)
{
    std::cout << "SCHEMA:\n"
              << YAML::LoadFile(ROOT_DIR + "/test/schema/type_derived/sequence_derived.schema") << std::endl
              << std::endl;

    std::cout << "GENERATED YAML:\n" << generateYaml("sequence_derived.schema", {ROOT_DIR}) << std::endl;
}

TEST(generator, file_template)
{
    // Create temporary folder at home
    std::string temporary_folder = ROOT_DIR + "/test/temp_folder";
    if (not filesystem::exists(temporary_folder)) filesystem::create_directory(temporary_folder);

    generateTemplate(temporary_folder + "/test1_template.yaml", "test1.schema", {ROOT_DIR});
    generateTemplate(temporary_folder + "/test1_template.yaml", "test1.schema", {ROOT_DIR});  // SHOULD RAISE WARNING
    generateTemplate(temporary_folder + "/test1_template.yaml", "test1.schema", {ROOT_DIR});  // SHOULD RAISE WARNING
    generateTemplate(temporary_folder + "/expression.yaml", "expression.schema", {ROOT_DIR});
    generateTemplate(temporary_folder + "/single_mandatory_template.yaml", "single_mandatory.schema", {ROOT_DIR});
    generateTemplate(temporary_folder + "/type_derived_final_template.yaml", "type_derived_final.schema", {ROOT_DIR});
    generateTemplate(temporary_folder + "/sequence_derived_template.yaml", "sequence_derived.schema", {ROOT_DIR});

    // remove temporary folder
    filesystem::remove_all(temporary_folder);
}

TEST(generator, no_output)
{
    testing::internal::CaptureStdout();
    generateYaml("test1.schema", {ROOT_DIR + "/test/schema"});
    generateYaml("type_derived_final.schema", {ROOT_DIR + "/test/schema"});
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST(generator, scalar_leaf_in_schema)
{
    YAML::Node node;
    ASSERT_NO_THROW(node = generateYaml("regression_scalar_leaf", {ROOT_DIR + "/test/schema"}));
    EXPECT_TRUE(node["param"]);
    EXPECT_FALSE(node["some_scalar"]);
}

TEST(generator, template_override)
{
    std::string temporary_folder = ROOT_DIR + "/test/temp_folder_override";
    if (not filesystem::exists(temporary_folder)) filesystem::create_directory(temporary_folder);

    // 'same_param' has the same parameter twice: only valid if override
    EXPECT_NO_THROW(
        generateTemplate(temporary_folder + "/same_param.yaml", "same_param", {ROOT_DIR + "/test/schema"}));
    EXPECT_THROW(
        generateTemplate(temporary_folder + "/same_param.yaml", "same_param", {ROOT_DIR + "/test/schema"}, false),
        std::runtime_error);

    filesystem::remove_all(temporary_folder);
}

TEST(generator, template_quotes)
{
    std::string temporary_folder = ROOT_DIR + "/test/temp_folder_quotes";
    if (not filesystem::exists(temporary_folder)) filesystem::create_directory(temporary_folder);

    auto file_path = generateTemplate(
        temporary_folder + "/template_quotes.yaml", "regression_template_quotes", {ROOT_DIR + "/test/schema"});

    // file content
    std::ifstream     file(file_path);
    std::stringstream content;
    content << file.rdbuf();
    std::cout << content.str() << std::endl;

    EXPECT_NE(content.str().find("the owner's name, e.g. \"John\""), std::string::npos);
    EXPECT_NE(content.str().find("a doc on two lines"), std::string::npos);

    // the generated template is a valid YAML with the expected values
    YAML::Node node;
    ASSERT_NO_THROW(node = YAML::LoadFile(file_path));
    EXPECT_EQ(node["name"].as<std::string>(), "whatever");
    EXPECT_EQ(node["mode"].as<std::string>(), "auto");
    EXPECT_EQ(node["count"].as<int>(), 3);
    EXPECT_EQ(node.size(), 3);

    filesystem::remove_all(temporary_folder);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
