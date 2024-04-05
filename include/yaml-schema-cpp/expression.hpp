#pragma once

#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
bool isExpression(const YAML::Node& node);

bool isExpression(const std::string& expression);

bool checkExpression(const YAML::Node& node_expression, const YAML::Node& node_schema_parent, std::string& err);

bool evalExpression(std::string expression_str, const YAML::Node& node_input_parent);

void preProcessExpression(std::string& expression_str);

}  // namespace yaml_schema_cpp
