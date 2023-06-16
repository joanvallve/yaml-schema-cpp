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

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

namespace yaml_schema_cpp
{
namespace filesystem = boost::filesystem;

#define CHECK_STRING_TYPE_CASE(yaml_string, TypeName)                                                                 \
    if (type == #yaml_string)                                                                                         \
    {                                                                                                                 \
        node.as<TypeName>();                                                                                          \
        return true;                                                                                                  \
    }

#define CHECK_TYPE_CASE(TypeName) CHECK_STRING_TYPE_CASE(TypeName, TypeName);

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

static bool checkNodeAsBasic(const YAML::Node& node, const std::string& type)
{
    CHECK_TYPE_BASIC_CASES
    return false;
}
static bool checkNodeAsString(const YAML::Node& node, const std::string& type)
{
    CHECK_TYPE_STRING_CASES
    return false;
}
static bool checkNodeAsEigen(const YAML::Node& node, const std::string& type)
{
    CHECK_TYPE_EIGEN_CASES
    return false;
}
static bool checkNodeAs(const YAML::Node& node, const std::string& type)
{
    CHECK_TYPE_BASIC_CASES
    CHECK_TYPE_STRING_CASES
    CHECK_TYPE_EIGEN_CASES
    return false;
}

bool tryNodeAs(const YAML::Node& node, const std::string& type);

bool isTrivialType(const std::string& type);
bool isBasicType(const std::string& type);
bool isStringType(const std::string& type);
bool isEigenType(const std::string& type);

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

static bool compare(const YAML::Node& node1, const YAML::Node& node2, const std::string& type)
{
    // sequence
    if (type.front() == '[' and type.back() == ']')
    {
        // both sequences
        if (not node1.IsSequence() or not node2.IsSequence()) return false;

        // same size
        if (node1.size() != node2.size()) return false;

        // compare all elements
        for (auto i = 0; i < node1.size(); i++)
        {
            if (not compare(node1[i], node2[i], type.substr(1, type.size() - 2))) return false;
        }
        return true;
    }

    COMPARE_TYPE(bool)
    COMPARE_TYPE(char)
    COMPARE_TYPE(int)
    COMPARE_TYPE(unsigned int)
    COMPARE_TYPE(long int)
    COMPARE_TYPE(long unsigned int)
    COMPARE_TYPE(float)
    COMPARE_TYPE(double)
    COMPARE_TYPE(std::string)
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

    COMPARE_STRING_TYPE(string, std::string)
    COMPARE_STRING_TYPE(VectorXd, Eigen::VectorXd)
    COMPARE_STRING_TYPE(VectorXd, Eigen::VectorXd)

    throw std::runtime_error("compare() not implemented for type " + type);
}

}  // namespace yaml_schema_cpp
