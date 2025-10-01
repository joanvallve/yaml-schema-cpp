#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_generator.hpp"

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

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
