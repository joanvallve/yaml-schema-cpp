#pragma once

#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <boost/filesystem.hpp>

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

class YamlServer
{
  public:
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input);

    bool applySchema(const std::string& name_schema);

    const std::stringstream& getLog() const;

    const YAML::Node& getNode() const;

  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;

    std::stringstream log_;

    YAML::Node node_input_;
};

}  // namespace yaml_schema_cpp
