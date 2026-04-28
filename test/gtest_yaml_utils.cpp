#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml_utils.hpp"

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

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
    //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
    return RUN_ALL_TESTS();
}
