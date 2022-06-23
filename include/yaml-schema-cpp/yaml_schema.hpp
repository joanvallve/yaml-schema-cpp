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

YAML::Node loadSchema(const std::string& schema_file, const std::vector<std::string>& folders_schema);
void checkSchema(const YAML::Node& node_schema, const std::string& field = "");

bool checkNode(const YAML::Node& node_input, 
               const std::string& name_schema, 
               const std::vector<std::string>& folders,
               std::stringstream& log);
                 
bool compareNodesRecursive(const YAML::Node& node_schema,
                           const YAML::Node& node_input,
                           const std::vector<std::string>& folders,
                           std::stringstream& log,
                           const std::string& field = "");

bool isScalarSchema(const YAML::Node& node_schema);
bool isMapSchema(const YAML::Node& node_schema);
bool isSequenceSchema(const YAML::Node& node_schema);

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value);



}  // namespace yaml_schema_cpp
