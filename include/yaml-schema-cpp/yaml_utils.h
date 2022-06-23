#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.h"
#include "yaml-schema-cpp/type_check.h"
#include "yaml-schema-cpp/yaml_schema.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

void flattenNode    (YAML::Node& node, std::vector<std::string> folders, bool is_schema);
void flattenMap     (YAML::Node& node, std::vector<std::string> folders, bool is_schema);
void flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema);

void writeToLog(std::stringstream& log, const std::string& message);

void addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value);

filesystem::path findFile(const std::string& name, const std::vector<std::string>& folders);

}  // namespace yaml_schema_cpp
