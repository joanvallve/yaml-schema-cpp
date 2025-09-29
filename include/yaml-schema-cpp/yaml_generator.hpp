#pragma once

#include "yaml-schema-cpp/filesystem_wrapper.hpp"

#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
const std::string red("\033[0;31m");
const std::string yellow("\033[1;33m");
const std::string reset("\033[0m");

std::string generateTemplate(std::string                     filepath,
                             const std::string&              name_schema,
                             const std::vector<std::string>& folders_schema,
                             bool                            override = true);

YAML::Node generateYaml(std::string schema_file, const std::vector<std::string>& folders_schema, bool override = true);

void schemaToYaml(const YAML::Node&               node_schema,
                  YAML::Node&                     node_output,
                  const std::vector<std::string>& folders_schema,
                  bool                            override = true);

}  // namespace yaml_schema_cpp
