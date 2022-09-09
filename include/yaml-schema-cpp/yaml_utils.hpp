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

#include <list>
#include <boost/filesystem.hpp>
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
namespace filesystem = boost::filesystem;
// 'follow' behavior:
// -> schema node (is_schema = true): only allowed single file names. It will be searched inside folders specified by
// 'folders'
// -> input node (is_schema = false): path to file allowd (either relative or absolute). Relative paths will be done
// with respect to the first item specified in folders. No search will be performed.
void flattenNode(YAML::Node& node, std::vector<std::string> folders, bool is_schema, bool override);
void flattenMap(YAML::Node& node, std::vector<std::string> folders, bool is_schema, bool override);
void flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema, bool override);

void writeToLog(std::stringstream& log, const std::string& message);

void addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value, bool override);

filesystem::path findFileRecursive(const std::string& name_with_extension, const std::vector<std::string>& folders);

std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key);

}  // namespace yaml_schema_cpp
