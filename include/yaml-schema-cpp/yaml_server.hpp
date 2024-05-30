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
    YamlServer(bool override = true);
    YamlServer(const std::vector<std::string>& folders_schema, bool override = true);
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input, bool override = true);

    bool applySchema(const std::string& name_schema);

    void addFolderSchema(const std::vector<std::string>& folders_schema, bool before = false);
    void addFolderSchema(const std::string& folder_schema, bool before = false);
    std::vector<std::string> getFolderSchema() const;

    void loadYaml(const std::string& path_input);
    void setYaml(const YAML::Node _node_input);

    std::string getLog() const;

    const YAML::Node& getNode() const;

  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;

    std::stringstream log_;

    YAML::Node node_input_;

    bool override_;
};

}  // namespace yaml_schema_cpp
