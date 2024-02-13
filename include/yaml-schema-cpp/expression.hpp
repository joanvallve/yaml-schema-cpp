// Copyright (C) 2022,2023,2024 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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

#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
bool isExpression(const YAML::Node& node);

bool isExpression(const std::string& expression);

bool checkExpression(const YAML::Node& node_expression, const YAML::Node& node_schema_parent, std::string& err);

bool evalExpression(std::string expression_str, const YAML::Node& node_input_parent);

void preProcessExpression(std::string& expression_str);

}  // namespace yaml_schema_cpp
