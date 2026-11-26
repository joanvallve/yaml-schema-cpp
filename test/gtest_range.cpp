#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string              ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;
std::vector<std::string> SCHEMA_FOLDERS{ROOT_DIR + "/test/schema/range"};

using namespace yaml_schema_cpp;

TEST(Range, valid_input)
{
    YamlServer server = YamlServer(SCHEMA_FOLDERS, ROOT_DIR + "/test/yaml/range/range.yaml");

    bool ok = server.applySchema("range");
    if (not ok) std::cout << server.getLog();
    ASSERT_TRUE(ok);
    ASSERT_DOUBLE_EQ(server.getNode()["param_default"].as<double>(), 2.0);
}

TEST(Range, out_of_range)
{
    // input file, expected error
    std::vector<std::array<std::string, 2>> cases{
        {"range_wrong_int.yaml", "ERROR in 'param_int': Value 11 out of range [0, 10]"},
        {"range_wrong_min.yaml", "ERROR in 'param_min': Value 0.4 out of range [0.5, inf]"},
        {"range_wrong_sequence.yaml", "ERROR in 'param_sequence[2]': Value 1.5 out of range [-1, 1]"},
        {"range_wrong_default_overridden.yaml", "ERROR in 'param_default': Value 3.5 out of range [-inf, 3.0]"}};

    for (auto [file, expected] : cases)
    {
        std::cout << "Checking " << file << "..." << std::endl;
        YamlServer server = YamlServer(SCHEMA_FOLDERS, ROOT_DIR + "/test/yaml/range/" + file);
        EXPECT_FALSE(server.applySchema("range"));
        EXPECT_NE(server.getLog().find(expected), std::string::npos) << "expected: " << expected << "\nlog:\n"
                                                                     << server.getLog();
    }
}

TEST(Range, compared_in_declared_type)
{
    // input, expected valid
    std::vector<std::pair<std::string, bool>> cases{
        {"param_int: 0x9", true},                              // hex: 9
        {"param_int: 0xA", false},                             // hex: 10
        {"param_int: 010", true},                              // octal: 8
        {"param_int: 012", false},                             // octal: 10
        {"param_long: 9007199254740993", false},               // rounds to the bound as a double
        {"param_unsigned_long: 18446744073709551615", false},  // rounds to the bound as a double
        {"param_unsigned_long: 18446744073709551614", true},
        {"param_double: .nan", false},
        {"param_double: 1.0", true}};

    for (auto [input, valid] : cases)
    {
        YamlServer server = YamlServer(SCHEMA_FOLDERS);
        server.setYaml(YAML::Load(input));
        bool ok = false;
        EXPECT_NO_THROW(ok = server.applySchema("range_types")) << input;
        EXPECT_EQ(ok, valid) << input << "\n" << server.getLog();
    }
}

TEST(Range, wrong_schemas)
{
    for (auto w_schema : {"range_not_numeric",
                          "range_not_number",
                          "range_min_greater",
                          "range_default_outside",
                          "range_value_outside",
                          "range_nan_bound",
                          "range_bound_type",
                          "range_bound_unsigned"})
    {
        std::stringstream log;
        std::cout << "Checking " << w_schema << ".schema..." << std::endl;
        EXPECT_FALSE(loadSchema(w_schema, {ROOT_DIR + "/test/wrong_schema"}, log).IsDefined());
        std::cout << log.str() << std::endl;
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
