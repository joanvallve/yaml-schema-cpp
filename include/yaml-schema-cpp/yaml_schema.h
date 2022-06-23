#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.h"
#include "yaml-schema-cpp/type_check.h"
#include "yaml-schema-cpp/yaml_utils.h"

namespace yaml_schema_cpp
{

static std::string SCHEMA_EXTENSION = ".schema";

namespace filesystem = boost::filesystem;

YAML::Node loadSchema(const std::string& schema_file, const std::vector<std::string>& folders_schema);
void checkSchema(const YAML::Node& node_schema, const std::string& field = "");

bool isScalarSchema(const YAML::Node& node_schema);
bool isMapSchema(const YAML::Node& node_schema);
bool isSequenceSchema(const YAML::Node& node_schema);

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value);

}  // namespace yaml_schema_cpp
