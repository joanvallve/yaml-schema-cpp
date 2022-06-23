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
#include "yaml-schema-cpp/yaml_utils.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

class YamlServer
{
  public:
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input);

    bool validate(const std::string& name_schema);

    const std::stringstream& getLog() const;

    const YAML::Node& getNode() const;

  private:
    filesystem::path getPathSchema(const std::string& name_schema) const;

  public:

    bool isValidBase(const std::string& name_schema, 
                     const YAML::Node& node_input, 
                     //std::vector<std::string> folders,
                     std::stringstream& log) const;
    bool isValidNode(const YAML::Node& node_schema,
                     const YAML::Node& node_input,
                     //std::vector<std::string> folders,
                     std::stringstream& log,
                     const std::string& field = "") const;
    
  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;

    std::stringstream log_;

    YAML::Node node_input_;
};

}  // namespace yaml_schema_cpp
