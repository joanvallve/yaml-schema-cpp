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

#define CHECK_STRING_TYPE(yaml_string, TypeName) \
    if (type == #yaml_string)                    \
    {                                            \
        node.as<TypeName>();                     \
        return true;                             \
    }

#define CHECK_TYPE(TypeName) CHECK_STRING_TYPE(TypeName, TypeName);

#define CHECK_TYPE_EIGEN(size)                                           \
    if (type == "Vector" #size "d" or type == "Eigen::Vector" #size "d") \
    {                                                                    \
        node.as<Eigen::Matrix<double, size, 1> >();                      \
        return true;                                                     \
    }                                                                    \
    if (type == "Matrix" #size "d" or type == "Eigen::Matrix" #size "d") \
    {                                                                    \
        node.as<Eigen::Matrix<double, size, size> >();                   \
        return true;                                                     \
    }

static bool checkTrivialTypeException(const YAML::Node& node, const std::string& type)
{
    CHECK_TYPE(bool)
    CHECK_TYPE(char)
    CHECK_TYPE(int)
    CHECK_TYPE(unsigned int)
    CHECK_TYPE(long int)
    CHECK_TYPE(long unsigned int)
    CHECK_TYPE(float)
    CHECK_TYPE(double)
    CHECK_TYPE(std::string)
    CHECK_TYPE(Eigen::VectorXd)
    CHECK_TYPE(Eigen::VectorXd)
    CHECK_TYPE(Eigen::MatrixXd)

    CHECK_TYPE_EIGEN(1)
    CHECK_TYPE_EIGEN(2)
    CHECK_TYPE_EIGEN(3)
    CHECK_TYPE_EIGEN(4)
    CHECK_TYPE_EIGEN(5)
    CHECK_TYPE_EIGEN(6)
    CHECK_TYPE_EIGEN(7)
    CHECK_TYPE_EIGEN(8)
    CHECK_TYPE_EIGEN(9)
    CHECK_TYPE_EIGEN(10)

    CHECK_STRING_TYPE(string, std::string)
    CHECK_STRING_TYPE(VectorXd, Eigen::VectorXd)
    CHECK_STRING_TYPE(MatrixXd, Eigen::MatrixXd)

    return false;
}

static bool checkTrivialType(const YAML::Node& node, const std::string& type)
{
    try
    {
        return checkTrivialTypeException(node, type);
    }
    catch (const std::exception& e)
    {
        return false;
    }
    return false;
}

static bool isTrivialType(const std::string& type)
{
    try
    {
        return checkTrivialTypeException(YAML::Node(), type);
    }
    catch (const std::exception& e)
    {
        return true;
    }
    return false;
}

static bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders)
{
    std::string name_schema = type;

    if (filesystem::extension(name_schema).empty())
    {
        // circular includes in case I use "SCHEMA_EXTENSION"
        name_schema += ".schema";
    }
    else if (filesystem::extension(name_schema) != ".schema")
    {
        // bad extension
        return false;
    }

    try
    {
        filesystem::path path = findFileRecursive(name_schema, folders);
    }
    catch (const std::exception& e)
    {
        // non existing file
        return false;
    }

    return true;
}

#define COMPARE_STRING_TYPE(yaml_string, TypeName)               \
    if (type == #yaml_string)                                    \
    {                                                            \
        try                                                      \
        {                                                        \
            return node1.as<TypeName>() == node2.as<TypeName>(); \
        }                                                        \
        catch (const std::exception& e)                          \
        {                                                        \
            return false;                                        \
        }                                                        \
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
