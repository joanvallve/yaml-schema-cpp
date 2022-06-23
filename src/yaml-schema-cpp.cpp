#include <stdexcept>

#include "yaml-schema-cpp/yaml-schema-cpp.hpp"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

std::string SCHEMA_EXTENSION = ".schema";

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

void YamlServer::flattenNode(YAML::Node& node, std::vector<std::string> folders, bool is_schema)
{
    switch (node.Type())
    {
        case YAML::NodeType::Map:
            flattenMap(node, folders, is_schema);
            break;
        case YAML::NodeType::Sequence:
            flattenSequence(node, folders, is_schema);
            break;
        case YAML::NodeType::Scalar:
        default:
            break;
    }
}

void YamlServer::flattenSequence(YAML::Node& node, std::vector<std::string> folders, bool is_schema)
{
    for (auto node_i : node)
    {
        flattenNode(node_i, folders, is_schema);
    }
}

void YamlServer::flattenMap(YAML::Node& node, std::vector<std::string> folders, bool is_schema)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (auto n : node)
    {
        if (n.first.as<std::string>() == "follow")
        {
            // Try to find the file
            filesystem::path path_follow = findFile(n.second.as<std::string>(), folders);

            // Case input yaml, folders has to be modified to keep relative paths
            if (not is_schema)
            {
                folders.front() = path_follow.parent_path().string();
            }
            
            // load "follow" file
            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            // Recursively flatten the "follow" file
            flattenNode(node_child, folders, is_schema);

            for (auto nc : node_child)
            {
                // Case schema
                if (is_schema)
                {
                    addNodeSchema(node_aux,nc.first.as<std::string>(),nc.second);
                }
                // Case input yaml
                else
                {
                    addNodeYaml(node_aux,nc.first.as<std::string>(),nc.second);
                }
            }
        }
        else
        {
            // Recursively flatten the "follow" file
            flattenNode(n.second, folders, is_schema);

            // Case schema
            if (is_schema)
            {
                addNodeSchema(node_aux,n.first.as<std::string>(),n.second);
            }
            // Case input yaml
            else
            {
                addNodeYaml(node_aux,n.first.as<std::string>(),n.second);
            }
        }
    }

    node = node_aux;
}

void YamlServer::addNodeYaml(YAML::Node& node, const std::string& key, const YAML::Node& value)
{
    if (node[key])
    {
        switch (value.Type()) 
        {
            case YAML::NodeType::Scalar:
            {
                throw std::runtime_error("Trying to add an already existing scalar node.");
            }
            case YAML::NodeType::Sequence:
            {
                for (auto value_seq_node : value)
                {
                    node[key].push_back(value_seq_node);
                }
                break;
            }
            case YAML::NodeType::Map:
            {
                for (auto value_map_node : value)
                {
                    YAML::Node node_key = node[key];
                    addNodeYaml(node_key, value_map_node.first.as<std::string>(), value_map_node.second);
                }
                break;
            }
            default:
            {
                throw std::runtime_error("Trying to add a node of type not known.");
            }
        }
    }
    else
    {
        node[key] = value;
    }
}

void YamlServer::addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value)
{
    if (node[key])
    {
        if (isScalarSchema(node[key]))
        {
            throw std::runtime_error("Trying to add something into an already existing scalar schema node.");
        }
        if (isSequenceSchema(node[key]))
        {
            throw std::runtime_error("Trying to add something into an already existing sequence schema node.");
        }
        if (isMapSchema(node[key]))
        {
            if (not isMapSchema(value))
            {
                throw std::runtime_error("Trying to add a sequence, scalar or unknown schema node into an already existing map schema node.");
            }
            else
            {
                for (auto value_map_node : value)
                {
                    YAML::Node node_key = node[key];
                    addNodeSchema(node_key, value_map_node.first.as<std::string>(), value_map_node.second);
                }
            }
        }
        else
        {
            throw std::runtime_error("Trying to add something into an schema node of unknown yaml_type.");
        }
    }
    else
    {
        node[key] = value;
    }
}


bool YamlServer::isValid(const std::string& name_schema)
{
    log_.clear();
    
    log_ << "-------------------------------------- \n";
    log_ << "Log of YAML parsing procedure of file: \n";
    log_ << "-------------------------------------- \n";

    return isValidBase(name_schema, node_input_, log_);
}

bool YamlServer::isValidBase(const std::string& name_schema,
                             const YAML::Node&  node_input,
                             std::stringstream& log) const
{
    // Load and check schema
    YAML::Node node_schema = loadSchema(name_schema, folders_schema_);

    // Check node_input against node_schema
    return isValidNode(node_schema, node_input, log);
}

YAML::Node YamlServer::loadSchema(const std::string& name_schema, const std::vector<std::string>& folders_schema)
{
    // Check extension
    if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        throw std::runtime_error("Wrong schema file extension " + 
                                 name_schema + 
                                 ", it should be '." + 
                                 SCHEMA_EXTENSION + 
                                 "'.");
    }

    // Load schema yaml
    filesystem::path path_schema = findFile(name_schema, folders_schema);
    YAML::Node node_schema = YAML::LoadFile(path_schema.string());

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
    flattenNode(node_schema, folders_schema, true);

    // Check schema
    checkSchema(node_schema);

    return node_schema;
}

void YamlServer::checkSchema(const YAML::Node& node_schema, const std::string& node_field)
{
    // skip scalars
    if (node_schema.IsScalar())
        return;

    // Check that node_schema is map
    if (not node_schema.IsMap())
    {
        throw std::runtime_error("YAML schema: " + node_field + " should be a map.");
    }

    // scalar
    if (isScalarSchema(node_schema))
    {
        // Check 'type'
        if (not node_schema["type"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'type'.");
        }
        else if (not checkType(node_schema["type"],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'type' should be a string.");
        }
        // Check 'mandatory'
        if (not node_schema["mandatory"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'mandatory'.");
        }
        else if (not checkType(node_schema["mandatory"],"bool"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'mandatory' should be a bool.");
        }
        // not mandatory params should have a default of correct type
        if (not node_schema["mandatory"].as<bool>())
        {
            if (not node_schema["default"])
            {
                throw std::runtime_error("YAML schema: " + node_field + " does not contain default value for a non mandatory parameter.");
            }
            if (not checkType(node_schema["default"], node_schema["type"].as<std::string>()))
            {
                throw std::runtime_error("YAML schema: " + node_field + " default value wrong type.");
            }
        }
    }
    // sequence
    else if (isSequenceSchema(node_schema))
    {
        // Check 'type'
        if (not node_schema["type"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'type'.");
        }
        else if (not checkType(node_schema["type"],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'type' should be a string.");
        }
        // Check 'mandatory'
        if (not node_schema["mandatory"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'mandatory'.");
        }
        else if (not checkType(node_schema["mandatory"],"bool"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'mandatory' should be a bool.");
        }
    }
    // Map
    else if (isMapSchema(node_schema))
    {
        for (auto node_schema_child : node_schema)
        {
            checkSchema(node_schema_child.second, node_schema_child.first.as<std::string>());
        }
    }
    else
    {
        throw std::runtime_error("YAML schema: " + node_field + " has unknown yaml_type.");
    }
}

bool YamlServer::isValidNode(const YAML::Node& node_schema,
                             const YAML::Node& node_input,
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
                        auto file_schema = getPathSchema(type + SCHEMA_EXTENSION);

                        // Check file exists
                        if (not filesystem::exists(file_schema))
                        {
                            writeToLog(log, 
                                       "Sequence " + field + 
                                       ", element " + std::to_string(i) + 
                                       " of type" + type +
                                       ": couldn't check against trivial types and " + 
                                       type + SCHEMA_EXTENSION + " file was not found.");
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

filesystem::path YamlServer::findFile(const std::string& name, const std::vector<std::string>& folders)
{
    for (auto folder : folders)
    {
        filesystem::path file_path = filesystem::path(folder) / filesystem::path(name);
        if (filesystem::exists(file_path))
        {
            return file_path;
        }
    }

    throw std::runtime_error("File '" + name + "' not found in provided folters");
}

void YamlServer::writeToLog(std::stringstream& log, const std::string& message)
{
    log << message << std::endl;
}

const std::stringstream& YamlServer::getLog() const
{
    return log_;
}

const YAML::Node& YamlServer::getNodeInput() const
{
    return node_input_;
}

bool YamlServer::isScalarSchema(const YAML::Node& node_schema)
{
    return node_schema["yaml_type"] and node_schema["yaml_type"].as<std::string>() == "scalar";
}

bool YamlServer::isSequenceSchema(const YAML::Node& node_schema)
{
    return node_schema["yaml_type"] and node_schema["yaml_type"].as<std::string>() == "sequence";
}

bool YamlServer::isMapSchema(const YAML::Node& node_schema)
{
    return node_schema.IsMap() and not node_schema["yaml_type"];
}

}  // namespace yaml_schema_cpp
