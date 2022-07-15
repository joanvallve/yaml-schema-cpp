
#include "gtest/utils_gtest.h"

TEST(TestTest, DummyTestExample)
{
  EXPECT_FALSE(false);

  ASSERT_TRUE(true);

  int my_int = 5;

  ASSERT_EQ(my_int, 5);

//  PRINTF("All good at TestTest::DummyTestExample !\n");
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "TestTest.DummyTestExample"; // Test only this one
  //::testing::GTEST_FLAG(filter) = "TestTest.*"; // Test only the tests in this group
  return RUN_ALL_TESTS();
}
