#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/internal/config.h"
#include "yaml-schema-cpp/yaml_utils.hpp"

std::string ROOT_DIR = _YAML_SCHEMA_CPP_ROOT_DIR;

using namespace yaml_schema_cpp;

TEST(TestYamlUtils, isArrayType)
{
    size_t size;

    EXPECT_FALSE(isArrayType("double", size));

    EXPECT_TRUE(isArrayType("double[]", size));
    EXPECT_EQ(size, 0);

    EXPECT_TRUE(isArrayType("double[5]", size));
    EXPECT_EQ(size, 5);

    EXPECT_TRUE(isArrayType("double[5][]", size));
    EXPECT_EQ(size, 5);

    EXPECT_TRUE(isArrayType("double[][67]", size));
    EXPECT_EQ(size, 0);
}

TEST(TestYamlUtils, getLowerElementType)
{
    EXPECT_EQ(getLowerElementType("double"), "double");
    EXPECT_EQ(getLowerElementType("double[]"), "double");
    EXPECT_EQ(getLowerElementType("double[67]"), "double");
    EXPECT_EQ(getLowerElementType("double[946213]"), "double");
    EXPECT_EQ(getLowerElementType("double[67][]"), "double[]");
    EXPECT_EQ(getLowerElementType("double[67][5]"), "double[5]");
    EXPECT_EQ(getLowerElementType("double[][34876]"), "double[34876]");
    EXPECT_EQ(getLowerElementType("double[23487][5]"), "double[5]");
}

TEST(TestYamlUtils, getLowestElementType)
{
    EXPECT_EQ(getLowestElementType("double"), "double");
    EXPECT_EQ(getLowestElementType("double[]"), "double");
    EXPECT_EQ(getLowestElementType("double[67]"), "double");
    EXPECT_EQ(getLowestElementType("double[946213]"), "double");
    EXPECT_EQ(getLowestElementType("double[67][]"), "double");
    EXPECT_EQ(getLowestElementType("double[67][5]"), "double");
    EXPECT_EQ(getLowestElementType("double[][34876]"), "double");
    EXPECT_EQ(getLowestElementType("double[23487][5]"), "double");
}

TEST(compare, compare_trivial)
{
    /*
    map1:
        param1: 1
        param2: "string"
    param5:
        - 5
        - 6
        - -1
        - -7
        - 0
    param6: 3.14 #same as value defined
    param7: [0, 2, 3] #same as value defined
    param8: [[3],[1,2.0,4],[2, 5.9]]
    */
    YAML::Node node_input = YAML::LoadFile(ROOT_DIR + "/test/yaml/base_input.yaml");

    // param 1
    EXPECT_TRUE(compare(node_input["map1"]["param1"], node_input["map1"]["param1"], "int", {}));
    EXPECT_TRUE(compare(node_input["map1"]["param1"], node_input["map1"]["param1"], "double", {}));
    EXPECT_TRUE(compare(node_input["map1"]["param1"], node_input["map1"]["param1"], "unsigned int", {}));
    EXPECT_TRUE(compare(node_input["map1"]["param1"], node_input["map1"]["param1"], "float", {}));
    EXPECT_TRUE(compare(node_input["map1"]["param1"], node_input["map1"]["param1"], "string", {}));

    // param 2
    EXPECT_TRUE(compare(node_input["map1"]["param2"], node_input["map1"]["param2"], "string", {}));

    // param 5
    EXPECT_TRUE(compare(node_input["param5"], node_input["param5"], "int[]", {}));
    EXPECT_TRUE(compare(node_input["param5"], node_input["param5"], "double[]", {}));
    EXPECT_TRUE(compare(node_input["param5"], node_input["param5"], "float[]", {}));
    EXPECT_TRUE(compare(node_input["param5"], node_input["param5"], "int[5]", {}));
    EXPECT_TRUE(compare(node_input["param5"], node_input["param5"], "double[5]", {}));
    EXPECT_TRUE(compare(node_input["param5"], node_input["param5"], "float[5]", {}));

    // param7
    EXPECT_TRUE(compare(node_input["param7"], node_input["param7"], "int[]", {}));
    EXPECT_TRUE(compare(node_input["param7"], node_input["param7"], "double[]", {}));
    EXPECT_TRUE(compare(node_input["param7"], node_input["param7"], "float[]", {}));
    EXPECT_TRUE(compare(node_input["param7"], node_input["param7"], "int[3]", {}));
    EXPECT_TRUE(compare(node_input["param7"], node_input["param7"], "double[3]", {}));
    EXPECT_TRUE(compare(node_input["param7"], node_input["param7"], "float[3]", {}));

    // param8
    EXPECT_TRUE(compare(node_input["param8"], node_input["param8"], "double[][]", {}));
    EXPECT_TRUE(compare(node_input["param8"], node_input["param8"], "float[][]", {}));
    EXPECT_TRUE(compare(node_input["param8"], node_input["param8"], "double[3][]", {}));
    EXPECT_TRUE(compare(node_input["param8"], node_input["param8"], "float[3][]", {}));
}

TEST(compare, compare_non_trivial)
{
    // base_input
    YAML::Node node_input = YAML::LoadFile(ROOT_DIR + "/test/yaml/base_input.yaml");
    EXPECT_TRUE(compare(node_input, node_input, "base_input", {ROOT_DIR}));

    // sequence_mandatory
    YAML::Node node_input2 = YAML::LoadFile(ROOT_DIR + "/test/yaml/own_type/sequence_mandatory.yaml");
    EXPECT_TRUE(compare(node_input2, node_input2, "sequence_mandatory", {ROOT_DIR}));
}

TEST(compare, compare_trivial_wrong)
{
    /*
    map1:
        param1: 1
        param2: "string"
    param5:
        - 5
        - 6
        - -1
        - -7
        - 0
    param6: 3.14 #same as value defined
    param7: [0, 2, 3] #same as value defined
    param8: [[3],[1,2.0,4],[2, 5.9]]
    */
    YAML::Node node_input = YAML::LoadFile(ROOT_DIR + "/test/yaml/base_input.yaml");
    YAML::Node node_comp;

    node_comp["map1"]["param1"] = 2;         // different
    node_comp["map1"]["param2"] = "strong";  // different
    node_comp["param5"][0]      = 5;
    node_comp["param5"][1]      = 6;
    node_comp["param5"][2]      = -1;
    node_comp["param5"][3]      = -7;      // fifth element missing
    node_comp["param6"]         = "true";  // wrong format
    node_comp["param7"][0]      = 0;
    node_comp["param7"][1]      = 2;
    node_comp["param7"][2]      = 4;  // different
    node_comp["param8"][0][0]   = 3;
    node_comp["param8"][1][0]   = 1;
    node_comp["param8"][1][1]   = 2;
    node_comp["param8"][1][2]   = 4;
    node_comp["param8"][2]   = 2; // double instead of double[]

    // param 1
    EXPECT_FALSE(compare(node_input["map1"]["param1"], node_comp["map1"]["param1"], "int", {}));
    EXPECT_FALSE(compare(node_input["map1"]["param1"], node_comp["map1"]["param1"], "double", {}));
    EXPECT_FALSE(compare(node_input["map1"]["param1"], node_comp["map1"]["param1"], "unsigned int", {}));
    EXPECT_FALSE(compare(node_input["map1"]["param1"], node_comp["map1"]["param1"], "float", {}));
    EXPECT_FALSE(compare(node_input["map1"]["param1"], node_comp["map1"]["param1"], "string", {}));

    // param 2
    EXPECT_FALSE(compare(node_input["map1"]["param2"], node_comp["map1"]["param2"], "string", {}));

    // param 5
    EXPECT_FALSE(compare(node_input["param5"], node_comp["param5"], "int[]", {}));
    EXPECT_FALSE(compare(node_input["param5"], node_comp["param5"], "double[]", {}));
    EXPECT_FALSE(compare(node_input["param5"], node_comp["param5"], "float[]", {}));
    EXPECT_FALSE(compare(node_input["param5"], node_comp["param5"], "int[5]", {}));
    EXPECT_FALSE(compare(node_input["param5"], node_comp["param5"], "double[5]", {}));
    EXPECT_FALSE(compare(node_input["param5"], node_comp["param5"], "float[5]", {}));

    // param7
    EXPECT_FALSE(compare(node_input["param7"], node_comp["param7"], "int[]", {}));
    EXPECT_FALSE(compare(node_input["param7"], node_comp["param7"], "double[]", {}));
    EXPECT_FALSE(compare(node_input["param7"], node_comp["param7"], "float[]", {}));
    EXPECT_FALSE(compare(node_input["param7"], node_comp["param7"], "int[3]", {}));
    EXPECT_FALSE(compare(node_input["param7"], node_comp["param7"], "double[3]", {}));
    EXPECT_FALSE(compare(node_input["param7"], node_comp["param7"], "float[3]", {}));

    // param8
    EXPECT_FALSE(compare(node_input["param8"], node_comp["param8"], "double[][]", {}));
    EXPECT_FALSE(compare(node_input["param8"], node_comp["param8"], "float[][]", {}));
    EXPECT_FALSE(compare(node_input["param8"], node_comp["param8"], "double[3][]", {}));
    EXPECT_FALSE(compare(node_input["param8"], node_comp["param8"], "float[3][]", {}));
}

TEST(compare, compare_non_trivial_wrong)
{
    // base_input
    YAML::Node node_input  = YAML::LoadFile(ROOT_DIR + "/test/yaml/base_input.yaml");
    YAML::Node node_comp   = YAML::Clone(node_input);
    node_comp["param8"][2]   = 2; // double instead of double[]

    EXPECT_FALSE(compare(node_input, node_comp, "base_input", {ROOT_DIR}));

    // sequence_mandatory
    YAML::Node node_input2 = YAML::LoadFile(ROOT_DIR + "/test/yaml/own_type/sequence_mandatory.yaml");
    YAML::Node node_comp2  = YAML::Clone(node_input2);
    node_comp["own_type"][1]["param5"][2] = 4;  // different

    EXPECT_TRUE(compare(node_input2, node_input2, "sequence_mandatory", {ROOT_DIR}));
}

TEST(compare, compare_nodes_auto_type)
{
    /* base_input:
    map1:
        param1: 1
        param2: "string"
    param5:
        - 5
        - 6
        - -1
        - -7
        - 0
    param6: 3.14 #same as value defined
    param7: [0, 2, 3] #same as value defined
    param8: [[3],[1,2.0,4],[2, 5.9]]
    */
   
    YAML::Node node_input  = YAML::LoadFile(ROOT_DIR + "/test/yaml/base_input.yaml");
    YAML::Node node_comp   = YAML::Clone(node_input);
    
    EXPECT_TRUE(compareNodesAutoType(node_input, node_comp));

    // remove param2
    node_comp["map1"].remove("param2");
    EXPECT_FALSE(node_comp["map1"]["param2"]);
    EXPECT_FALSE(compareNodesAutoType(node_input, node_comp));

    // change param5[2]
    node_comp   = YAML::Clone(node_input);
    node_comp["param5"][2] = 9;
    EXPECT_NE(node_comp["param5"][2].as<int>(), node_input["param5"][2].as<int>());
    EXPECT_FALSE(compareNodesAutoType(node_input, node_comp));

    // add elements to param7 
    node_comp   = YAML::Clone(node_input);
    node_comp["param7"][3] = 9;
    EXPECT_NE(node_comp["param7"].size(), node_input["param7"].size());
    EXPECT_FALSE(compareNodesAutoType(node_input, node_comp));

    // change type of param6
    node_comp   = YAML::Clone(node_input);
    node_comp["param6"] = "pi";
    EXPECT_FALSE(compareNodesAutoType(node_input, node_comp));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
