#include "yaml-schema-cpp/yaml_server.hpp" 

#include <stdexcept>
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

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

bool YamlServer::applySchema(const std::string& name_schema)
{
    log_.clear();
    
    std::string header1 = "Log of applySchema to YAML file";
    std::string header2 = "  yaml file: " + path_input_.string();
    std::string header3 = "  schema: " + name_schema;

    auto max_size = std::max({header1.size(),header2.size(),header3.size()});

    log_ << std::string(max_size,'-') << std::endl;
    log_ << header1 << std::endl;
    log_ << header2 << std::endl;
    log_ << header3 << std::endl;
    log_ << std::string(max_size,'-') << std::endl << std::endl;

    return yaml_schema_cpp::applySchema(node_input_, name_schema, folders_schema_, log_);
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
