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

static std::string SCHEMA_EXTENSION = ".schema";

namespace filesystem = boost::filesystem;

YAML::Node loadSchema(std::string schema_file, const std::vector<std::string>& folders_schema, bool override = true);
void checkSchema(const YAML::Node& node_schema, const std::string& field);

bool validateAllSchemas(const std::vector<std::string>& folders_schema, bool override = true);

bool applySchema(YAML::Node& node_input, 
                 const std::string& name_schema, 
                 const std::vector<std::string>& folders,
                 std::stringstream& log,
                 const std::string& acc_field, 
                 bool override=true);
                 
bool applySchemaRecursive(YAML::Node& node_input,
                          YAML::Node& node_input_parent,
                          const YAML::Node& node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream& log,
                          const std::string& acc_field, 
                          bool override);

bool checkOptions(const YAML::Node& input_node, const YAML::Node& options_node, const std::string& type);

bool isScalarSchema(const YAML::Node& node_schema);
bool isMapSchema(const YAML::Node& node_schema);
bool isSequenceSchema(const YAML::Node& node_schema);

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value, bool override);



}  // namespace yaml_schema_cpp
