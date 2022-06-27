#pragma once

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.hpp"

namespace yaml_schema_cpp
{

#define CHECK_STRING_TYPE(yaml_string, TypeName)  \
  if (type == #yaml_string)                       \
  {                                               \
    try                                           \
    {                                             \
      node.as<TypeName>();                        \
    }                                             \
    catch (const std::exception& e)               \
    {                                             \
      return false;                               \
    }                                             \
    return true;                                  \
  }                                               \
  
#define CHECK_TYPE(TypeName)                      \
  CHECK_STRING_TYPE(TypeName,TypeName);           \
  
#define CHECK_TYPE_EIGEN(size)                    \
  if (type == "Vector" #size "d" or               \
      type == "Eigen::Vector" #size "d")          \
  {                                               \
    try                                           \
    {                                             \
      node.as<Eigen::Matrix<double,size,1> >();   \
    }                                             \
    catch (const std::exception& e)               \
    {                                             \
      return false;                               \
    }                                             \
    return true;                                  \
  }                                               \
  if (type == "Matrix" #size "d" or               \
      type == "Eigen::Matrix" #size "d")          \
  {                                               \
    try                                           \
    {                                             \
      node.as<Eigen::Matrix<double,size,size> >();\
    }                                             \
    catch (const std::exception& e)               \
    {                                             \
      return false;                               \
    }                                             \
    return true;                                  \
  }                                               \

static bool checkType(const YAML::Node& node, const std::string& type)
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

#define COMPARE_STRING_TYPE(yaml_string, TypeName)        \
  if (type == #yaml_string)                               \
  {                                                       \
    try                                                   \
    {                                                     \
      return node1.as<TypeName>() == node2.as<TypeName>();\
    }                                                     \
    catch (const std::exception& e)                       \
    {                                                     \
      return false;                                       \
    }                                                     \
  }                                                       \

#define COMPARE_TYPE(TypeName)                      \
  COMPARE_STRING_TYPE(TypeName,TypeName);           \
  
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
