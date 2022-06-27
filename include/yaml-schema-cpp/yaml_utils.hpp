#pragma once

#include <list>
#include <boost/filesystem.hpp>
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

void flattenNode    (YAML::Node& node, std::vector<std::string> folders, bool is_schema);
void flattenMap     (YAML::Node& node, std::vector<std::string> folders, bool is_schema);
void flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema);

void writeToLog(std::stringstream& log, const std::string& message);

void addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value);

filesystem::path findFile(const std::string& name, const std::vector<std::string>& folders);

std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key);

}  // namespace yaml_schema_cpp
