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

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

namespace yaml_schema_cpp
{
static std::string            SCHEMA_EXTENSION = ".schema";
static std::string            YAML_EXTENSION   = ".yaml";
static std::string            SCHEMA_PREFIX    = "_";
static std::string            TYPE             = SCHEMA_PREFIX + "type";
static std::string            MANDATORY        = SCHEMA_PREFIX + "mandatory";
static std::string            DOC              = SCHEMA_PREFIX + "doc";
static std::string            OPTIONS          = SCHEMA_PREFIX + "options";
static std::string            DEFAULT          = SCHEMA_PREFIX + "default";
static std::string            VALUE            = SCHEMA_PREFIX + "value";
static std::string            BASE             = SCHEMA_PREFIX + "base";
static std::list<std::string> RESERVED_KEYS{TYPE, MANDATORY, DOC, OPTIONS, DEFAULT, BASE};
static std::list<std::string> REQUIRED_KEYS{TYPE, MANDATORY, DOC};

namespace filesystem = boost::filesystem;

YAML::Node loadSchema(std::string                     schema_file,
                      const std::vector<std::string>& folders_schema,
                      std::stringstream&              log,
                      bool                            override = true);
void       checkSchema(const YAML::Node& node_schema, const std::string& field, const YAML::Node& node_schema_parent);

bool validateAllSchemas(const std::vector<std::string>& folders_schema, bool verbose, bool override = true);

bool applySchema(YAML::Node&                     node_input,
                 const std::string&              name_schema,
                 const std::vector<std::string>& folders,
                 std::stringstream&              log,
                 const std::string&              acc_field,
                 bool                            override = true);

bool applySchemaRecursive(YAML::Node&                     node_input,
                          YAML::Node&                     node_input_parent,
                          const YAML::Node&               node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream&              log,
                          const std::string&              acc_field,
                          bool                            override);

bool checkOptions(const YAML::Node& input_node, const YAML::Node& options_node, const std::string& type);

bool hasAnyReservedKey(const YAML::Node& node_schema);
bool isSpecification(const YAML::Node& node_schema);
bool isSequenceSchema(const YAML::Node& node_schema);

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value, bool override);

std::string getTypeOfSequence(const YAML::Node& node_schema);

}  // namespace yaml_schema_cpp
