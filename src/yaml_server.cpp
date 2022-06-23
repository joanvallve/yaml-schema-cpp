#include <stdexcept>

#include "yaml-schema-cpp/yaml_server.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

YamlServer::YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input)
    : folders_schema_(folders_schema),
      path_input_(filesystem::path(path_input))
{
    // Check file exists
    if (not filesystem::exists(path_input_))
    {
        throw std::runtime_error("YAML file does not exists. Non-existing path: " + path_input_.string() );
    }
    // load yamlfile
    node_input_ = YAML::LoadFile(path_input);

    // flatten
    flattenNode(node_input_, {path_input_.parent_path().string()}, false);
}

bool YamlServer::validate(const std::string& name_schema)
{
    log_.clear();
    
    log_ << "-------------------------------------- \n";
    log_ << "Log of YAML parsing procedure of file: \n";
    log_ << "-------------------------------------- \n";

    return isValidBase(name_schema, node_input_, folders_schema_, log_);
}

const std::stringstream& YamlServer::getLog() const
{
    return log_;
}

const YAML::Node& YamlServer::getNode() const
{
    return node_input_;
}

}  // namespace yaml_schema_cpp
