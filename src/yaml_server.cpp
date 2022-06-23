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

    return isValidBase(name_schema, node_input_, log_);
}

bool YamlServer::isValidBase(const std::string& name_schema,
                             const YAML::Node&  node_input,
                             //std::vector<std::string> folders,
                             std::stringstream& log) const
{
    // Load and check schema
    YAML::Node node_schema = loadSchema(name_schema, folders_schema_);

    // Check node_input against node_schema
    return isValidNode(node_schema, node_input, log);
}

bool YamlServer::isValidNode(const YAML::Node& node_schema,
                             const YAML::Node& node_input,
                             //std::vector<std::string> folders,
                             std::stringstream& log,
                             const std::string& field) const
{
    bool is_valid = true;
    
    // Param schema (has mandatory and type)
    if (isScalarSchema(node_schema))
    {
        if (node_input.IsDefined())
        {
            if (not checkType(node_input, node_schema["type"].as<std::string>()))
            {
                writeToLog(log, 
                           "Field '" + field + "' of wrong type. Should be " + node_schema["type"].as<std::string>());
                is_valid = false;
            }
        }
        else if(node_schema["mandatory"].as<bool>()) // complain inexistence if mandatory
        {
            writeToLog(log, "Input yaml does not contain field: " + field);
            is_valid = false;
        }
    }
    // sequence
    else if (isSequenceSchema(node_schema))
    {
        if(node_input.IsDefined())
        {
            // First check that the node it is really a sequence
            if (not node_input.IsSequence())
            {
                writeToLog(log, "Input yaml does not contain a sequence in: " + field);
                is_valid = false;
            }
            // Then check the validity of each element in the sequence
            else
            {
                for (auto i = 0; i < node_input.size(); i++)
                {
                    auto type = node_schema["type"].as<std::string>();

                    // Find derived type
                    if (type == "derived")
                    {
                        // check existence of key type
                        if (not node_input[i]["type"])
                        {
                            writeToLog(log, 
                                       "Sequence " + 
                                       field + 
                                       ", element " + 
                                       std::to_string(i) + 
                                       ": does not contain key 'type'");
                            is_valid = false; 
                            continue;
                        }
                        else
                        {
                            type = node_input[i]["type"].as<std::string>();
                        }
                    }
                     
                    // If check for trivial types doesn't succeed, try to load a schema of this type
                    if (not checkType(node_input[i], type))
                    {
                        auto file_schema = getPathSchema(type + ".schema");

                        // Check file exists
                        if (not filesystem::exists(file_schema))
                        {
                            writeToLog(log, 
                                       "Sequence " + field + 
                                       ", element " + std::to_string(i) + 
                                       " of type" + type +
                                       ": couldn't check against trivial types and " + 
                                       type + ".schema" + " file was not found.");
                            is_valid = false;
                            continue;
                        }
                        // Validate with the schema file
                        is_valid = is_valid and isValidBase(file_schema.string(), 
                                                            node_input[i],
                                                            log);
                  
                    }
                }
            }
        }
        else if (node_schema["mandatory"]) // complain inexistence if mandatory
        {
            writeToLog(log, "Input yaml does not contain field: " + field);
            is_valid = false;
        }
    }
    // map of params
    else if (isMapSchema(node_schema))
    {
        for (auto node_schema_child : node_schema)
        {
            const YAML::Node& node_input_child = (node_input.IsDefined() ? 
                                                  node_input[node_schema_child.first.as<std::string>()] : 
                                                  node_input);

            is_valid = is_valid and isValidNode(node_schema_child.second, 
                                                node_input_child,
                                                log,
                                                node_schema_child.first.as<std::string>());
        }
    }
    else
    {
        throw std::runtime_error("unknown schema");
    }

    return is_valid;
}

filesystem::path YamlServer::getPathSchema(const std::string& name_schema) const
{
    filesystem::path schema_path;
    try
    {
        schema_path = findFile(name_schema, folders_schema_);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Schema with the name '" + name_schema + "' not found inside the YamlServer folders");
    }
    return schema_path;
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
