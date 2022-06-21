#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/filesystem.hpp>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

class YamlServer
{
  public:
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input);

    bool isValid(const std::string& name_schema, bool polymorphism = false);

    bool correctType(const YAML::Node& schema_node, const YAML::Node& node_input, std::stringstream& log);

    const std::stringstream& getLog() const;

    const YAML::Node& getNodeInput() const;

  private:
    void flattenInputNode(YAML::Node& node, const filesystem::path& root_path="");
    void flattenMap(YAML::Node& node, const filesystem::path& root_path="");
    void flattenSequence(YAML::Node& node, const filesystem::path& root_path="");

    // void flattenSchemaNode(YAML::Node& node);

    bool isValidBase(const std::string& name_schema, const YAML::Node& node_input, const std::string& header_log = "");
    bool isValidDerived(const std::string& name_schema, const YAML::Node& node_input, const std::string& header_log = "");
    bool isValidNode(const std::string& field,
                     const YAML::Node& node_schema,
                     const YAML::Node& node_input,
                     std::string& header_log);

    filesystem::path getPathSchema(const std::string& name_schema);
    
    void writeToLog(const std::string& header, const std::string& message);

    void checkSchema(const YAML::Node node_schema, bool first_level) const;
    bool checkType(const YAML::Node& node, const std::string& type) const;

    bool isParamSchema(const YAML::Node& node_schema) const;
    bool isSequenceOwnType(const YAML::Node& node_schema) const;

  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;
    filesystem::path path_input_parent_;

    std::stringstream log_;

    YAML::Node node_input_;
};

}  // namespace yaml_schema_cpp
