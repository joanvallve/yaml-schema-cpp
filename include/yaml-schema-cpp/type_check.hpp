#pragma once

#include "yaml-schema-cpp/internal/config.h"

#if _EIGEN_FOUND == 1
#include <Eigen/Dense>
#endif

#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_utils.hpp"

namespace yaml_schema_cpp
{
#define CHECK_STRING_TYPE_CASE(yaml_string, TypeName)                                                                 \
    if (type == #yaml_string)                                                                                         \
    {                                                                                                                 \
        node.as<TypeName>();                                                                                          \
        return true;                                                                                                  \
    }

#define CHECK_TYPE_CASE(TypeName) CHECK_STRING_TYPE_CASE(TypeName, TypeName);

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
    CHECK_TYPE_CASE(Eigen::VectorXd)                                                                                  \
    CHECK_TYPE_CASE(Eigen::VectorXd)                                                                                  \
    CHECK_TYPE_CASE(Eigen::MatrixXd)                                                                                  \
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
    CHECK_STRING_TYPE_CASE(VectorXd, Eigen::VectorXd)                                                                 \
    CHECK_STRING_TYPE_CASE(MatrixXd, Eigen::MatrixXd)
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
    CHECK_STRING_TYPE_CASE(string, std::string)

static bool checkSizes(const YAML::Node& node, const std::string& type)
{
    size_t size;

    // not array --> ok
    if (not isArrayType(type, size)) return true;

    // array type --> node should be sequence
    if (node.IsSequence())
    {
        // array size not specified --> ok to be a sequence
        if (size == 0) return true;

        // array size specified --> sequence should have same size
        if (node.size() == size) return true;
    }
    // otherwise
    return false;
}

static bool checkNodeAsBasic(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsBasic(node[i], getLowerElementType(type));
    }
    // scalar
    else
        CHECK_TYPE_BASIC_CASES
    return false;
}

static bool checkNodeAsString(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsString(node[i], getLowerElementType(type));
    }
    // scalar
    else
        CHECK_TYPE_STRING_CASES
    return false;
}

#if _EIGEN_FOUND == 1
static bool checkNodeAsEigen(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;
    
    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsString(node[i], getLowerElementType(type));
    }
    else
        CHECK_TYPE_EIGEN_CASES
    return false;
}
#endif

static bool checkNodeAs(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsString(node[i], getLowerElementType(type));
    }
    else
    {
        CHECK_TYPE_BASIC_CASES
        CHECK_TYPE_STRING_CASES
#if _EIGEN_FOUND == 1
        CHECK_TYPE_EIGEN_CASES
#endif
    }
    return false;
}

bool tryNodeAs(const YAML::Node& node, const std::string& type);

bool isTrivialType(const std::string& type);
bool isBasicType(const std::string& type);
bool isStringType(const std::string& type);
#if _EIGEN_FOUND == 1
bool isEigenType(const std::string& type);
#endif

bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders);

#define COMPARE_STRING_TYPE(yaml_string, TypeName)                                                                    \
    if (type == #yaml_string)                                                                                         \
    {                                                                                                                 \
        try                                                                                                           \
        {                                                                                                             \
            return node1.as<TypeName>() == node2.as<TypeName>();                                                      \
        }                                                                                                             \
        catch (const std::exception& e)                                                                               \
        {                                                                                                             \
            return false;                                                                                             \
        }                                                                                                             \
    }

#define COMPARE_TYPE(TypeName) COMPARE_STRING_TYPE(TypeName, TypeName);

#if _EIGEN_FOUND == 1
#define COMPARE_EIGEN(size)                                                                                           \
    if (type == "Vector" #size "d" or type == "Eigen::Vector" #size "d")                                              \
    {                                                                                                                 \
        try                                                                                                           \
        {                                                                                                             \
            return (node1.as<Eigen::Matrix<double, size, 1> >() - node2.as<Eigen::Matrix<double, size, 1> >())        \
                       .cwiseAbs()                                                                                    \
                       .maxCoeff() < 1e-9;                                                                            \
        }                                                                                                             \
        catch (const std::exception& e)                                                                               \
        {                                                                                                             \
            return false;                                                                                             \
        }                                                                                                             \
    }                                                                                                                 \
    if (type == "Matrix" #size "d" or type == "Eigen::Matrix" #size "d")                                              \
    {                                                                                                                 \
        try                                                                                                           \
        {                                                                                                             \
            return (node1.as<Eigen::Matrix<double, size, size> >() - node2.as<Eigen::Matrix<double, size, size> >())  \
                       .cwiseAbs()                                                                                    \
                       .maxCoeff() < 1e-9;                                                                            \
        }                                                                                                             \
        catch (const std::exception& e)                                                                               \
        {                                                                                                             \
            return false;                                                                                             \
        }                                                                                                             \
    }
#endif

static bool compare(const YAML::Node& node1, const YAML::Node& node2, const std::string& type)
{
    // sequences if array type, and same size as array type (if specified)
    if (not checkSizes(node1, type)) return false;
    if (not checkSizes(node2, type)) return false;

    size_t size;
    // array type --> call recursively compare
    if (isArrayType(type, size)) 
    {
        // same size
        if (node1.size() != node2.size()) return false;

        // compare all elements
        for (auto i = 0; i < node1.size(); i++)
        {
            if (not compare(node1[i], node2[i], getLowerElementType(type))) return false;
        }
        return true;
    }
    // scalar
    else
    {
        COMPARE_TYPE(bool)
        COMPARE_TYPE(char)
        COMPARE_TYPE(int)
        COMPARE_TYPE(unsigned int)
        COMPARE_TYPE(long int)
        COMPARE_TYPE(long unsigned int)
        COMPARE_TYPE(float)
        COMPARE_TYPE(double)
        COMPARE_TYPE(std::string)
#if _EIGEN_FOUND == 1
        COMPARE_TYPE(Eigen::VectorXd)
        COMPARE_TYPE(Eigen::MatrixXd)

        COMPARE_EIGEN(1)
        COMPARE_EIGEN(2)
        COMPARE_EIGEN(3)
        COMPARE_EIGEN(4)
        COMPARE_EIGEN(5)
        COMPARE_EIGEN(6)
        COMPARE_EIGEN(7)
        COMPARE_EIGEN(8)
        COMPARE_EIGEN(9)
        COMPARE_EIGEN(10)

        COMPARE_STRING_TYPE(VectorXd, Eigen::VectorXd)
        COMPARE_STRING_TYPE(VectorXd, Eigen::VectorXd)
#endif
        COMPARE_STRING_TYPE(string, std::string)
    }

    throw std::runtime_error("compare() not implemented for type " + type);
}

#if _EIGEN_FOUND == 1
#define SETZERO_EIGEN(size)                                                                                           \
    if (type == "Vector" #size "d" or type == "Eigen::Vector" #size "d")                                              \
    {                                                                                                                 \
        node = Eigen::Matrix<double, size, 1>::Zero();                                                                \
    }                                                                                                                 \
    if (type == "Matrix" #size "d" or type == "Eigen::Matrix" #size "d")                                              \
    {                                                                                                                 \
        node = Eigen::Matrix<double, size, size>::Zero();                                                             \
    }
#endif

static void setZero(YAML::Node& node, const std::string& type)
{
    // check sequence if array type, and same size as array type (if specified)
    if (not checkSizes(node, type)) return;

    size_t size;
    // array type --> call recursively setZero
    if (isArrayType(type, size))
    {
        auto lower_type = getLowerElementType(type);
        for (auto i = 0; i < node.size(); i++)
        {
            setZero(node, lower_type);
        }
    }
    // not array
    else
    {
        if (type == "bool") node = "false";
        if (type == "int" or type == "unsigned int" or type == "long int" or type == "long unsigned int") node = "0";
        if (type == "float" or type == "double") node = "0.0";
        if (type == "char") node = "A";
        if (type == "string" or type == "std::string") node = "whatever";
#if _EIGEN_FOUND == 1
        if (type == "VectorXd" or type == "Eigen::VectorXd") node = Eigen::VectorXd::Zero(3);
        if (type == "MatrixXd" or type == "Eigen::MatrixXd") node = Eigen::MatrixXd::Zero(3, 2);

        SETZERO_EIGEN(1)
        SETZERO_EIGEN(2)
        SETZERO_EIGEN(3)
        SETZERO_EIGEN(4)
        SETZERO_EIGEN(5)
        SETZERO_EIGEN(6)
        SETZERO_EIGEN(7)
        SETZERO_EIGEN(8)
        SETZERO_EIGEN(9)
        SETZERO_EIGEN(10)
#endif
    }
}

static std::string getZeroString(const std::string& type)
{
    size_t size;
    // array type --> call recursively setZero
    if (isArrayType(type, size))
    {
        if (size == 0) size = 3;  // arbitrary size if not specified
        auto lower_type = getLowerElementType(type);
        std::string zero_string = "[";
        for (auto i = 0; i < size; i++)
        {
            zero_string += getZeroString(lower_type);
            if (i < size - 1) zero_string += ", ";
        }
        zero_string += "]";
        return zero_string;
    }
    // not array
    else
    {
        if (type == "bool") return "false";
        if (type == "int" or type == "unsigned int" or type == "long int" or type == "long unsigned int") return "0";
        if (type == "float" or type == "double") return "0.0";
        if (type == "char") return "A";
        if (type == "string" or type == "std::string") return "whatever";
#if _EIGEN_FOUND == 1
        if (type == "VectorXd" or type == "Eigen::VectorXd") return "[0.0, 0.0, 0.0]";
        if (type == "MatrixXd" or type == "Eigen::MatrixXd") return "[[3, 2], [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]]";

        for (auto i = 1; i <= 10; i++)
        {
            if (type == "Vector" + std::to_string(i) + "d" or type == "Eigen::Vector" + std::to_string(i) + "d")
            {
                std::string string_ret = "[0.0";
                for (auto j = 2; j <= i; j++) string_ret += ", 0.0";
                string_ret += "]";

                return string_ret;
            }

            if (type == "Matrix" + std::to_string(i) + "d" or type == "Eigen::Matrix" + std::to_string(i) + "d")
            {
                std::string string_ret = "[0.0";
                for (auto j = 2; j <= i * i; j++) string_ret += ", 0.0";
                string_ret += "]";

                return string_ret;
            }
        }
#endif
    }
    return "type uknown!";
}

}  // namespace yaml_schema_cpp
