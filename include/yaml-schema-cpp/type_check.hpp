#pragma once

#include "yaml-schema-cpp/internal/config.h"

#if _EIGEN_FOUND == 1
#include <Eigen/Dense>
#endif

#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
#define CHECK_TYPE_CASE(TypeName)                                                                                     \
    if (type == #TypeName)                                                                                            \
    {                                                                                                                 \
        node.as<TypeName>();                                                                                          \
        return true;                                                                                                  \
    }

#if _EIGEN_FOUND == 1
#define CHECK_TYPE_EIGEN_CASE(size)                                                                                   \
    if (type == "Vector" #size "d" or type == "Eigen::Vector" #size "d")                                              \
    {                                                                                                                 \
        node.as<Eigen::Matrix<double, size, 1> >();                                                                   \
        return true;                                                                                                  \
    }                                                                                                                 \
    if (type == "Matrix" #size "d" or type == "Eigen::Matrix" #size "d")                                              \
    {                                                                                                                 \
        node.as<Eigen::Matrix<double, size, size> >();                                                                \
        return true;                                                                                                  \
    }

#define CHECK_TYPE_EIGEN_CASES                                                                                        \
    CHECK_TYPE_EIGEN_CASE(1)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(2)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(3)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(4)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(5)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(6)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(7)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(8)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(9)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(10)                                                                                         \
    CHECK_TYPE_CASE(Eigen::VectorXd)                                                                                  \
    CHECK_TYPE_CASE(Eigen::VectorXd)                                                                                  \
    CHECK_TYPE_CASE(Eigen::MatrixXd)                                                                                  \
    {                                                                                                                 \
        using namespace Eigen;                                                                                        \
        CHECK_TYPE_CASE(VectorXd)                                                                                     \
        CHECK_TYPE_CASE(MatrixXd)                                                                                     \
    }
#endif

#define CHECK_TYPE_BASIC_CASES                                                                                        \
    CHECK_TYPE_CASE(bool)                                                                                             \
    CHECK_TYPE_CASE(char)                                                                                             \
    CHECK_TYPE_CASE(int)                                                                                              \
    CHECK_TYPE_CASE(unsigned int)                                                                                     \
    CHECK_TYPE_CASE(long int)                                                                                         \
    CHECK_TYPE_CASE(long unsigned int)                                                                                \
    CHECK_TYPE_CASE(float)                                                                                            \
    CHECK_TYPE_CASE(double)

#define CHECK_TYPE_STRING_CASES                                                                                       \
    CHECK_TYPE_CASE(std::string)                                                                                      \
    {                                                                                                                 \
        using std::string;                                                                                            \
        CHECK_TYPE_CASE(string)                                                                                       \
    }

/**
 * @brief check the sizes of node and type
 * If the type is an array type, it checks the node is a sequence.
 * If the array has size, it checks the sequence size matches.
 *
 * @param node YAML node
 * @param type string specifying a type
 * @return if node and type agree on sizes (both scalars or arrays of correct sizes)
 */
bool checkSizes(const YAML::Node& node, const std::string& type);

/**
 * @brief check if a node can be interpreted as a given type
 * NOTE: This function may raise exceptions
 *
 * @param node YAML node to be converted
 * @param type string defining the type
 * @return false if type not checked, true if check ok.
 */
bool checkNodeAsBasic(const YAML::Node& node, const std::string& type);
bool checkNodeAsString(const YAML::Node& node, const std::string& type);
#if _EIGEN_FOUND == 1
bool checkNodeAsEigen(const YAML::Node& node, const std::string& type);
#endif
bool checkNodeAs(const YAML::Node& node, const std::string& type);

/**
 * @brief try if a node can be interpreted as a given type
 * NOTE: This function does not raise exceptions
 *
 * @param node YAML node to be converted
 * @param type string defining the type
 * @return if the node can be interpreted as the type
 */
bool tryNodeAs(const YAML::Node& node, const std::string& type);

/** Functions to evaluate if a string definig type is in these groups:
 *  - Basic: int, float, string, Eigen types (if eigen installed))
 *  - String
 *  - Eigen: Vector or Matrix dynamic of fixed size (until 10)
 *  - Trivial: Basic, string or Eigen
 *  - NonTrivial: rest of types that require schemas to be specified
 */
bool isTrivialType(const std::string& type);
bool isBasicType(const std::string& type);
bool isStringType(const std::string& type);
#if _EIGEN_FOUND == 1
bool isEigenType(const std::string& type);
#endif
bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders);

}  // namespace yaml_schema_cpp
