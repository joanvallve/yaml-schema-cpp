// Copyright (C) 2022,2023 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
// Authors: Josep Martí Saumell (jmarti@iri.upc.edu) and Joan Vallvé Navarro (jvallve@iri.upc.edu)
// All rights reserved.
//
// YAML-SCHEMA-CPP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <gtest/gtest.h>
#include <Eigen/Dense>

// Macros for testing equalities and inequalities.
//
//    * {ASSERT|EXPECT}_EQ(expected, actual): Tests that expected == actual
//    * {ASSERT|EXPECT}_NE(v1, v2):           Tests that v1 != v2
//    * {ASSERT|EXPECT}_LT(v1, v2):           Tests that v1 < v2
//    * {ASSERT|EXPECT}_LE(v1, v2):           Tests that v1 <= v2
//    * {ASSERT|EXPECT}_GT(v1, v2):           Tests that v1 > v2
//    * {ASSERT|EXPECT}_GE(v1, v2):           Tests that v1 >= v2
//
// C String Comparisons.  All tests treat NULL and any non-NULL string
// as different.  Two NULLs are equal.
//
//    * {ASSERT|EXPECT}_STREQ(s1, s2):     Tests that s1 == s2
//    * {ASSERT|EXPECT}_STRNE(s1, s2):     Tests that s1 != s2
//    * {ASSERT|EXPECT}_STRCASEEQ(s1, s2): Tests that s1 == s2, ignoring case
//    * {ASSERT|EXPECT}_STRCASENE(s1, s2): Tests that s1 != s2, ignoring case
//
// Macros for comparing floating-point numbers.
//
//    * {ASSERT|EXPECT}_FLOAT_EQ(expected, actual):
//         Tests that two float values are almost equal.
//    * {ASSERT|EXPECT}_DOUBLE_EQ(expected, actual):
//         Tests that two double values are almost equal.
//    * {ASSERT|EXPECT}_NEAR(v1, v2, abs_error):
//         Tests that v1 and v2 are within the given distance to each other.
//
// These predicate format functions work on floating-point values, and
// can be used in {ASSERT|EXPECT}_PRED_FORMAT2*(), e.g.
//
//   EXPECT_PRED_FORMAT2(testing::DoubleLE, Foo(), 5.0);
//
// Macros that execute statement and check that it doesn't generate new fatal
// failures in the current thread.
//
//    * {ASSERT|EXPECT}_NO_FATAL_FAILURE(statement);

// http://stackoverflow.com/a/29155677

namespace testing
{
namespace internal
{
enum GTestColor
{
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW
};

extern void ColoredPrintf(GTestColor color, const char* fmt, ...);

#define PRINTF(...)                                                                                                   \
    do                                                                                                                \
    {                                                                                                                 \
        testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");                            \
        testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__);                               \
    } while (0)

#define PRINT_TEST_FINISHED                                                                                           \
    {                                                                                                                 \
        const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();         \
        PRINTF(std::string("Finished test case ")                                                                     \
                   .append(test_info->test_case_name())                                                               \
                   .append(" of test ")                                                                               \
                   .append(test_info->name())                                                                         \
                   .append(".\n")                                                                                     \
                   .c_str());                                                                                         \
    }

// C++ stream interface
class TestCout : public std::stringstream
{
  public:
    ~TestCout() override
    {
        PRINTF("%s\n", str().c_str());
    }
};

/* Usage :

TEST(Test, Foo)
{
  // the following works but prints default stream
  EXPECT_TRUE(false) << "Testing Stream.";

  // or you can play with AINSI color code
  EXPECT_TRUE(false) << "\033[1;31m" << "Testing Stream.";

  // or use the above defined macros

  PRINTF("Hello world");

  // or

  TEST_COUT << "Hello world";
}

*/
#define TEST_COUT testing::internal::TestCout()

}  // namespace internal

/* Macros related to testing Eigen classes:
 */
#define EXPECT_MATRIX_APPROX(C_expect, C_actual, precision)                                                           \
    EXPECT_PRED2([](const Eigen::MatrixXd lhs,                                                                        \
                    const Eigen::MatrixXd rhs) { return (lhs - rhs).isMuchSmallerThan(1, precision); },               \
                 C_expect,                                                                                            \
                 C_actual);

#define ASSERT_MATRIX_APPROX(C_expect, C_actual, precision)                                                           \
    ASSERT_PRED2([](const Eigen::MatrixXd lhs,                                                                        \
                    const Eigen::MatrixXd rhs) { return (lhs - rhs).isMuchSmallerThan(1, precision); },               \
                 C_expect,                                                                                            \
                 C_actual);

#define EXPECT_QUATERNION_APPROX(C_expect, C_actual, precision)                                                       \
    EXPECT_PRED2([](const Eigen::Quaterniond lhs,                                                                     \
                    const Eigen::Quaterniond rhs) { return lhs.angularDistance(rhs) < precision; },                   \
                 C_expect,                                                                                            \
                 C_actual);

#define ASSERT_QUATERNION_APPROX(C_expect, C_actual, precision)                                                       \
    ASSERT_PRED2([](const Eigen::Quaterniond lhs,                                                                     \
                    const Eigen::Quaterniond rhs) { return lhs.angularDistance(rhs) < precision; },                   \
                 C_expect,                                                                                            \
                 C_actual);

#define EXPECT_QUATERNION_VECTOR_APPROX(C_expect, C_actual, precision)                                                \
    EXPECT_PRED2(                                                                                                     \
        [](const Eigen::VectorXd lhs, const Eigen::VectorXd rhs) {                                                    \
            return Eigen::Quaterniond(Eigen::Vector4d(lhs))                                                           \
                       .angularDistance(Eigen::Quaterniond(Eigen::Vector4d(rhs))) < precision;                        \
        },                                                                                                            \
        C_expect,                                                                                                     \
        C_actual);

#define ASSERT_QUATERNION_VECTOR_APPROX(C_expect, C_actual, precision)                                                \
    ASSERT_PRED2(                                                                                                     \
        [](const Eigen::VectorXd lhs, const Eigen::VectorXd rhs) {                                                    \
            return Eigen::Quaterniond(Eigen::Vector4d(lhs))                                                           \
                       .angularDistance(Eigen::Quaterniond(Eigen::Vector4d(rhs))) < precision;                        \
        },                                                                                                            \
        C_expect,                                                                                                     \
        C_actual);

#define EXPECT_POSE2d_APPROX(C_expect, C_actual, precision)                                                           \
    EXPECT_PRED2(                                                                                                     \
        [](const Eigen::VectorXd lhs, const Eigen::VectorXd rhs) {                                                    \
            assert(lhs.size() == 3 and rhs.size() == 3);                                                              \
            Eigen::VectorXd er = lhs - rhs;                                                                           \
            er(2)              = pi2pi((double)er(2));                                                                \
            return er.isMuchSmallerThan(1, precision);                                                                \
        },                                                                                                            \
        C_expect,                                                                                                     \
        C_actual);

#define ASSERT_POSE2d_APPROX(C_expect, C_actual, precision)                                                           \
    ASSERT_PRED2(                                                                                                     \
        [](const Eigen::VectorXd lhs, const Eigen::VectorXd rhs) {                                                    \
            assert(lhs.size() == 3 and rhs.size() == 3);                                                              \
            Eigen::VectorXd er = lhs - rhs;                                                                           \
            er(2)              = pi2pi((double)er(2));                                                                \
            return er.isMuchSmallerThan(1, precision);                                                                \
        },                                                                                                            \
        C_expect,                                                                                                     \
        C_actual);

#define EXPECT_POSE3d_APPROX(C_expect, C_actual, precision)                                                           \
    EXPECT_PRED2(                                                                                                     \
        [](const Eigen::VectorXd lhs, const Eigen::VectorXd rhs) {                                                    \
            assert(lhs.size() == 7 and rhs.size() == 7);                                                              \
            Eigen::Vector4d er;                                                                                       \
            er.head(3) = lhs.head(3) - rhs.head(3);                                                                   \
            er(3)      = Eigen::Quaterniond(Eigen::Vector4d(lhs.tail(4)))                                             \
                        .angularDistance(Eigen::Quaterniond(Eigen::Vector4d(rhs.tail(4))));                           \
            return er.isMuchSmallerThan(1, precision);                                                                \
        },                                                                                                            \
        C_expect,                                                                                                     \
        C_actual);

#define ASSERT_POSE3d_APPROX(C_expect, C_actual, precision)                                                           \
    EXPECT_PRED2(                                                                                                     \
        [](const Eigen::VectorXd lhs, const Eigen::VectorXd rhs) {                                                    \
            assert(lhs.size() == 7 and rhs.size() == 7);                                                              \
            Eigen::Vector4d er;                                                                                       \
            er.head(3) = lhs.head(3) - rhs.head(3);                                                                   \
            er(3)      = Eigen::Quaterniond(Eigen::Vector4d(lhs.tail(4)))                                             \
                        .angularDistance(Eigen::Quaterniond(Eigen::Vector4d(rhs.tail(4))));                           \
            return er.isMuchSmallerThan(1, precision);                                                                \
        },                                                                                                            \
        C_expect,                                                                                                     \
        C_actual);

}  // namespace testing

#endif /* WOLF_UTILS_GTEST_H */
