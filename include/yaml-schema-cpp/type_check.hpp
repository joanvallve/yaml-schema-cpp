// Copyright (C) 2022 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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

static bool checkNodeAs(const YAML::Node& node, const std::string& type)
{
    CHECK_TYPE_CASE(bool)
    CHECK_TYPE_CASE(char)
    CHECK_TYPE_CASE(int)
    CHECK_TYPE_CASE(unsigned int)
    CHECK_TYPE_CASE(long int)
    CHECK_TYPE_CASE(long unsigned int)
    CHECK_TYPE_CASE(float)
    CHECK_TYPE_CASE(double)
    CHECK_TYPE_CASE(std::string)
    CHECK_TYPE_CASE(Eigen::VectorXd)
    CHECK_TYPE_CASE(Eigen::VectorXd)
    CHECK_TYPE_CASE(Eigen::MatrixXd)

    CHECK_TYPE_EIGEN_CASE(1)
    CHECK_TYPE_EIGEN_CASE(2)
    CHECK_TYPE_EIGEN_CASE(3)
    CHECK_TYPE_EIGEN_CASE(4)
    CHECK_TYPE_EIGEN_CASE(5)
    CHECK_TYPE_EIGEN_CASE(6)
    CHECK_TYPE_EIGEN_CASE(7)
    CHECK_TYPE_EIGEN_CASE(8)
    CHECK_TYPE_EIGEN_CASE(9)
    CHECK_TYPE_EIGEN_CASE(10)

    CHECK_STRING_TYPE_CASE(string, std::string)
    CHECK_STRING_TYPE_CASE(VectorXd, Eigen::VectorXd)
    CHECK_STRING_TYPE_CASE(MatrixXd, Eigen::MatrixXd)

    return false;
}

bool tryNodeAs(const YAML::Node& node, const std::string& type);

bool isTrivialType(const std::string& type);

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

static bool compare(const YAML::Node& node1, const YAML::Node& node2, const std::string& type)
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

    COMPARE_STRING_TYPE(string, std::string)
    return false;
}

}  // namespace yaml_schema_cpp
