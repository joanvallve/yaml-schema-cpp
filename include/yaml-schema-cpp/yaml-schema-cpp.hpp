#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.h"
#include "yaml-schema-cpp/type_check.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

class YamlServer
{
  public:
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input);

    bool isValid(const std::string& name_schema);

    const std::stringstream& getLog() const;

    const YAML::Node& getNodeInput() const;

  private:
    filesystem::path getPathSchema(const std::string& name_schema) const;

  public:

    bool isValidBase(const std::string& name_schema, 
                     const YAML::Node& node_input, 
                     std::stringstream& log) const;
    bool isValidNode(const YAML::Node& node_schema,
                     const YAML::Node& node_input,
                     std::stringstream& log,
                     const std::string& field = "") const;
    
    // STATIC METHODS
    static void flattenNode    (YAML::Node& node, std::vector<std::string> folders, bool is_schema);
    static void flattenMap     (YAML::Node& node, std::vector<std::string> folders, bool is_schema);
    static void flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema);
    
    static void writeToLog(std::stringstream& log, const std::string& message);

    static YAML::Node loadSchema(const std::string& schema_file, const std::vector<std::string>& folders_schema);
    static void checkSchema(const YAML::Node& node_schema, const std::string& field = "");

    static bool isScalarSchema(const YAML::Node& node_schema);
    static bool isMapSchema(const YAML::Node& node_schema);
    static bool isSequenceSchema(const YAML::Node& node_schema);

    static void addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value);
    static void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value);
    
    static filesystem::path findFile(const std::string& name, const std::vector<std::string>& folders);

  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;

    std::stringstream log_;

    YAML::Node node_input_;
};

}  // namespace yaml_schema_cpp
