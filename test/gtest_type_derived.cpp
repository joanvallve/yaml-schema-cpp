#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_server.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(TestTypeDerived, type_derived_final)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/type_derived/type_derived.yaml");

    // Check schema is OK
    ASSERT_TRUE(server.applySchema("type_derived_final.schema"));

    YAML::Node node = server.getNode();

    // Check value
    ASSERT_EQ(node["final_1"].as<std::string>(), "gromenauer");
    ASSERT_DOUBLE_EQ(node["final_derived"]["param_base"].as<double>(), 2.4);
    ASSERT_EQ(node["final_derived"]["param_derived"].as<std::vector<int>>(), std::vector<int>({1, 2, 3, 4}));

    std::cout << "after type_derived_final.schema: \n" << server.getNode() << std::endl;
    std::cout << "log: \n" << server.getLog() << std::endl;
}

TEST(TestTypeDerived, type_derived_final_wrong)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/type_derived/type_derived_wrong1.yaml",
                                         ROOT_DIR + "/test/yaml/type_derived/type_derived_wrong2.yaml",
                                         ROOT_DIR + "/test/yaml/type_derived/type_derived_wrong3.yaml",
                                         ROOT_DIR + "/test/yaml/type_derived/type_derived_wrong4.yaml"};
    for (auto input : input_yamls)
    {
        YamlServer server = YamlServer({ROOT_DIR}, input);
        std::cout << "testing " << input << ":\n" << server.getNode() << std::endl;

        bool succeed = server.applySchema("type_derived_final.schema");

        if (succeed)
            std::cout << "resulting node: " << server.getNode() << std::endl;
        else
            std::cout << server.getLog() << std::endl;

        ASSERT_FALSE(succeed);
    }
}

TEST(TestTypeDerived, sequence_derived)
{
    YamlServer server = YamlServer({ROOT_DIR}, ROOT_DIR + "/test/yaml/type_derived/sequence_derived.yaml");

    // Check schema is OK
    ASSERT_TRUE(server.applySchema("sequence_derived.schema"));

    std::cout << "after sequence_derived.schema: \n" << server.getNode() << std::endl;
    std::cout << "log: \n" << server.getLog() << std::endl;
}

TEST(TestTypeDerived, sequence_derived_wrong)
{
    std::vector<std::string> input_yamls{ROOT_DIR + "/test/yaml/type_derived/sequence_derived_wrong1.yaml",
                                         ROOT_DIR + "/test/yaml/type_derived/sequence_derived_wrong2.yaml",
                                         ROOT_DIR + "/test/yaml/type_derived/sequence_derived_wrong3.yaml",
                                         ROOT_DIR + "/test/yaml/type_derived/sequence_derived_wrong4.yaml"};
    for (auto input : input_yamls)
    {
        YamlServer server = YamlServer({ROOT_DIR}, input);
        std::cout << "testing " << input << ":\n" << server.getNode() << std::endl;

        bool succeed = server.applySchema("sequence_derived.schema");

        if (succeed)
            std::cout << "resulting node: " << server.getNode() << std::endl;
        else
            std::cout << server.getLog() << std::endl;

        ASSERT_FALSE(succeed);
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
