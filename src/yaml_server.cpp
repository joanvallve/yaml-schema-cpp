#include "yaml-schema-cpp/yaml_server.hpp" 

#include <stdexcept>
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

YamlServer::YamlServer(bool override)
    : folders_schema_(),
      override_(override)
{
}

YamlServer::YamlServer(const std::vector<std::string>& folders_schema, bool override)
    : folders_schema_(folders_schema),
      override_(override)
{
}

YamlServer::YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input, bool override)
    : folders_schema_(folders_schema),
      override_(override)
{
    loadYaml(path_input);
}

void YamlServer::addFolderSchema(const std::vector<std::string>& folders_schema, bool before)
{
    folders_schema_.insert(before ? folders_schema_.begin() : folders_schema_.end(), folders_schema.begin(), folders_schema.end());
}

void YamlServer::addFolderSchema(const std::string& folder_schema, bool before)
{
    folders_schema_.insert(before ? folders_schema_.begin() : folders_schema_.end(), folder_schema);
}

void YamlServer::loadYaml(const std::string& path_input)
{
    // Check file exists
    if (not filesystem::exists(path_input))
    {
        throw std::runtime_error("YAML file does not exists. Non-existing path: " + path_input);
    }

    // load yamlfile
    path_input_ = filesystem::path(path_input);
    node_input_ = YAML::LoadFile(path_input);

    // flatten
    flattenNode(node_input_, {path_input_.parent_path().string()}, false, override_);
}

bool YamlServer::applySchema(const std::string& name_schema)
{
    log_.clear();
    
    std::string header1 = "Log of applySchema to YAML file";
    std::string header2 = "  yaml file: " + path_input_.string();
    std::string header3 = "  schema: " + name_schema;

    auto max_size = std::max({header1.size(),header2.size(),header3.size()});

    log_ << std::endl;
    log_ << std::string(max_size,'-') << std::endl;
    log_ << header1 << std::endl;
    log_ << header2 << std::endl;
    log_ << header3 << std::endl;
    log_ << std::string(max_size,'-') << std::endl << std::endl;

    return yaml_schema_cpp::applySchema(node_input_, name_schema, folders_schema_, log_, "", override_);
}

std::string YamlServer::getLog() const
{
    return log_.str();
}

const YAML::Node& YamlServer::getNode() const
{
    return node_input_;
}

}  // namespace yaml_schema_cpp
