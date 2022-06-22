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
    std::cout << "YamlServer::YamlServer: node_input_\n" << node_input_ << std::endl;

    // flatten
    flattenNode(node_input_, path_input_.parent_path());
    std::cout << "YamlServer::YamlServer: flattened node_input_\n" << node_input_ << std::endl;
}

void YamlServer::flattenNode(YAML::Node node, const filesystem::path& root_path) const
{
    switch (node.Type())
    {
        case YAML::NodeType::Map:
            flattenMap(node, root_path);
            break;
        case YAML::NodeType::Sequence:
            flattenSequence(node, root_path);
            break;
        case YAML::NodeType::Scalar:
        default:
            break;
    }
}

void YamlServer::flattenSequence(YAML::Node node, const filesystem::path& root_path) const
{
    for (auto node_i : node)
    {
        flattenNode(node_i, root_path);
    }
}

void YamlServer::flattenMap(YAML::Node node, const filesystem::path& root_path) const
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (auto n : node)
    {
        if (n.first.as<std::string>() == "follow")
        {
            filesystem::path path_follow, path_follow_parent;
            
            // Case input yaml
            if (root_path != "")
            {
                path_follow        = root_path / filesystem::path(n.second.as<std::string>());
                path_follow_parent = path_follow.parent_path();
            }
            // Case schema yaml
            else
                path_follow = getPathSchema(n.second.as<std::string>());

            // Check file exists
            if (not filesystem::exists(path_follow))
            {
                throw std::runtime_error("YAML file does not exists. Non-existing path '" + 
                                         path_follow.string() + 
                                         "' included in '" + 
                                         root_path.string() + "'\n");
            }

            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            std::cout << __LINE__ << std::endl << node_child << std::endl;
            flattenNode(node_child, path_follow_parent);

            std::cout << __LINE__ << std::endl << node_child << std::endl;
            for (auto nc : node_child)
            {
                addNode(node_aux,nc.first.as<std::string>(),nc.second);
            }
            std::cout << __LINE__ << std::endl << node_aux << std::endl;
        }
        else
        {
            std::cout << __LINE__ << std::endl << n.second << std::endl;
            flattenNode(n.second, root_path);
            std::cout << __LINE__ << std::endl << n.second << std::endl;
            addNode(node_aux,n.first.as<std::string>(),n.second);
            std::cout << __LINE__ << std::endl << node_aux << std::endl;
        }
        std::cout << __LINE__ << std::endl << node_aux << std::endl;
    }

    std::cout << __LINE__ << std::endl << node_aux << std::endl;
    node = node_aux;
    std::cout << __LINE__ << std::endl << node << std::endl;
}

void YamlServer::addNode(YAML::Node node, const std::string& key, const YAML::Node& value)
{
    std::cout << __LINE__ << std::endl << node << std::endl;
    if (node[key])
    {
        switch (value.Type()) 
        {
            case YAML::NodeType::Scalar:
            {
                throw std::runtime_error("Trying to add an already existing scalar node.");
                break;
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
                    addNode(node[key], value_map_node.first.as<std::string>(), value_map_node.second);
                }
                break;
            }
            default:
            {
                throw std::runtime_error("Trying to add a node of type not known.");
                break;
            }
            
        }
    }
    else
    {
        node.force_insert(key,value);
    }
    std::cout << __LINE__ << std::endl << node << std::endl;
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
    // Check extension
    if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        throw std::runtime_error("Wrong schema file extension " + 
                                 name_schema + 
                                 ", it should be '." + 
                                 SCHEMA_EXTENSION + 
                                 "'.");
    }

    // Load schema node
    YAML::Node node_schema;
    try
    {
        filesystem::path path_schema = getPathSchema(name_schema);
        node_schema = YAML::LoadFile(path_schema.string());
    }
    catch(const std::exception& e)
    {
        log << e.what() << '\n';
        return false;
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
    flattenNode(node_schema);
    checkSchema(node_schema);

    // Check node_input against node_schema
    return isValidNode(node_schema, node_input, log);
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
        // Check 'mandatory'
        if (not node_schema["mandatory"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'mandatory'.");
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
        // Check 'mandatory'
        if (not node_schema["mandatory"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'mandatory'.");
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
        throw std::runtime_error("YAML schema: " + node_field + " has unknown structure.");
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
    for (auto base_p : folders_schema_)
    {
        filesystem::path path_schema = filesystem::path(base_p) / filesystem::path(name_schema);
        if (filesystem::exists(path_schema))
        {
            return path_schema;
        }
    }

    throw std::runtime_error("Schema with the name '" + name_schema + "' not found inside the YamlServer folders");
}

bool YamlServer::checkType(const YAML::Node& node, const std::string& type)
{
    if (node.IsMap())
    {
        return false;
    }

    if (type == "string")
    {
        try
        {
            node.as<std::string>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "double")
    {
        try
        {
            node.as<double>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "eigen_vector")
    {
        try
        {
            node.as<Eigen::VectorXd>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }

    return false;
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
    return node_schema["yaml_type"] and node_schema["yaml_type"].as<std::string>() != "scalar";
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
