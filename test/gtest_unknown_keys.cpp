#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string              ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;
std::vector<std::string> SCHEMA_FOLDERS{ROOT_DIR + "/test/schema/strict", ROOT_DIR + "/test/schema/type_derived"};

using namespace yaml_schema_cpp;

TEST(UnknownKeys, valid_input)
{
    YamlServer server = YamlServer(SCHEMA_FOLDERS, ROOT_DIR + "/test/yaml/strict/strict.yaml");

    bool ok = server.applySchema("strict");
    if (not ok) std::cout << server.getLog();
    ASSERT_TRUE(ok);
    EXPECT_EQ(server.getLog().find("Unknown key"), std::string::npos) << server.getLog();

    // defaults are still added, and they are schema keys
    ASSERT_TRUE(server.getNode()["param_optional"]);
}

TEST(UnknownKeys, unknown_keys)
{
    // input file, path of the unknown key, schema named in the message
    std::vector<std::array<std::string, 3>> cases{
        {"strict_wrong_top.yaml", "param_optinal", "strict"},
        {"strict_wrong_map.yaml", "map1/param_b", "strict"},
        {"strict_wrong_typed.yaml", "typed/valeu", "strict_item"},
        {"strict_wrong_list.yaml", "typed_list[1]/valeu", "strict_item"},
        {"strict_wrong_derived.yaml", "derived_field/param_bse", "type_derived_derived"}};

    for (auto [file, path, schema] : cases)
    {
        std::cout << "Checking " << file << "..." << std::endl;
        std::string message = "in '" + path + "': Unknown key, not in schema '" + schema + "'";

        // by default, it is an error naming the key and the schema
        YamlServer server_error = YamlServer(SCHEMA_FOLDERS, ROOT_DIR + "/test/yaml/strict/" + file);
        EXPECT_FALSE(server_error.applySchema("strict"));
        EXPECT_NE(server_error.getLog().find("ERROR " + message), std::string::npos)
            << "expected: ERROR " << message << "\nlog:\n"
            << server_error.getLog();

        // with error_on_unknown_keys = false, it is a warning and the input is valid
        YamlServer server_warning = YamlServer(SCHEMA_FOLDERS, ROOT_DIR + "/test/yaml/strict/" + file);
        bool       ok_warning     = server_warning.applySchema("strict", false);
        if (not ok_warning) std::cout << server_warning.getLog();
        EXPECT_TRUE(ok_warning);
        EXPECT_NE(server_warning.getLog().find("WARNING " + message), std::string::npos)
            << "expected: WARNING " << message << "\nlog:\n"
            << server_warning.getLog();
        EXPECT_EQ(server_warning.getLog().find("ERROR"), std::string::npos) << server_warning.getLog();
    }
}

TEST(UnknownKeys, free_function)
{
    YAML::Node        node = YAML::LoadFile(ROOT_DIR + "/test/yaml/strict/strict_wrong_top.yaml");
    std::stringstream log;
    EXPECT_FALSE(applySchema(node, "strict", SCHEMA_FOLDERS, log, ""));
    EXPECT_NE(log.str().find("ERROR in 'param_optinal': Unknown key"), std::string::npos) << log.str();

    YAML::Node        node_warning = YAML::LoadFile(ROOT_DIR + "/test/yaml/strict/strict_wrong_top.yaml");
    std::stringstream log_warning;
    EXPECT_TRUE(applySchema(node_warning, "strict", SCHEMA_FOLDERS, log_warning, "", true, false));
    EXPECT_NE(log_warning.str().find("WARNING in 'param_optinal': Unknown key"), std::string::npos)
        << log_warning.str();
}

TEST(UnknownKeys, set_yaml)
{
    // in-memory nodes are checked as files are
    YamlServer server      = YamlServer(SCHEMA_FOLDERS);
    YAML::Node node        = YAML::LoadFile(ROOT_DIR + "/test/yaml/strict/strict.yaml");
    node["typed"]["extra"] = 1;
    server.setYaml(node);
    EXPECT_TRUE(server.applySchema("strict", false));
    EXPECT_FALSE(server.applySchema("strict"));
    EXPECT_NE(server.getLog().find("ERROR in 'typed/extra': Unknown key"), std::string::npos) << server.getLog();
}

TEST(UnknownKeys, schema_inserted_keys_are_not_checked)
{
    // the default carries a key strict_item does not list; the input has none
    YamlServer server = YamlServer(SCHEMA_FOLDERS);
    server.setYaml(YAML::Load("{}"));
    bool ok = server.applySchema("strict_default");
    if (not ok) std::cout << server.getLog();
    EXPECT_TRUE(ok);
    EXPECT_EQ(server.getLog().find("Unknown key"), std::string::npos) << server.getLog();
}

TEST(UnknownKeys, derived_redefining_a_base_key)
{
    // 'sub' is a strict_item in the base and a conflict_other in the derived schema: both apply
    YamlServer server = YamlServer(SCHEMA_FOLDERS);
    server.setYaml(YAML::Load("x: {type: conflict_derived, sub: {name: a, value: 1.0, extra_value: 2.0}}"));
    bool ok = server.applySchema("strict_conflict");
    if (not ok) std::cout << server.getLog();
    EXPECT_TRUE(ok);

    server.setYaml(YAML::Load("x: {type: conflict_derived, sub: {name: a, value: 1.0, extra_value: 2.0, valeu: 3}}"));
    EXPECT_FALSE(server.applySchema("strict_conflict"));
    EXPECT_NE(server.getLog().find("ERROR in 'x/sub/valeu': Unknown key"), std::string::npos) << server.getLog();
}

TEST(UnknownKeys, non_scalar_key)
{
    YamlServer server          = YamlServer(SCHEMA_FOLDERS);
    YAML::Node node            = YAML::LoadFile(ROOT_DIR + "/test/yaml/strict/strict.yaml");
    node[YAML::Load("[1, 2]")] = 3;
    server.setYaml(node);
    bool ok = true;
    EXPECT_NO_THROW(ok = server.applySchema("strict"));
    EXPECT_FALSE(ok);
    EXPECT_NE(server.getLog().find("Unknown key"), std::string::npos) << server.getLog();
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
