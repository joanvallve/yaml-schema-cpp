#include <stdexcept>

#include "yaml-schema-cpp/yaml-schema-cpp.hpp"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

std::string SEQUENCE_OWN_TYPE = "sequence_own_type";

YamlServer::YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input)
    : folders_schema_(folders_schema),
      path_input_(filesystem::path(path_input)),
      path_input_parent_(path_input_.parent_path()),
      node_input_(YAML::LoadFile(path_input))
{
    flattenInputNode(node_input_, path_input_parent_);
}

void YamlServer::flattenNode(YAML::Node& node, const filesystem::path& root_path)
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

void YamlServer::flattenSequence(YAML::Node& node, const filesystem::path& root_path)
{
    for (std::size_t i = 0; i < node.size(); ++i)
    {
        YAML::Node n = node[i];
        flattenInputNode(n, root_path);
    }
}

void YamlServer::flattenMap(YAML::Node& node, const filesystem::path& root_path)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (YAML::iterator n = node.begin(); n != node.end(); ++n)
    {
        if (n->first.as<std::string>() == "follow")
        {
            filesystem::path path_follow, path_follow_parent;
            
            // Case input yaml
            if (root_path == "")
            {
                path_follow        = root_path / filesystem::path(n->second.as<std::string>());
                path_follow_parent = path_follow.parent_path();
            }
            // Case schema yaml
            else
                path_follow = getPathSchema(n->second.as<std::string>());

            // Check file exists
            if (!filesystem::exists(path_follow))
            {
                throw std::runtime_error("YAML file does not exists. Non-existing path '" + 
                                         path_follow.string() + 
                                         "' included in '" + 
                                         root_path + "'\n");
            }

            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            flattenInputNode(node_child, path_follow_parent);

            for (auto nc = node_child.begin(); nc != node_child.end(); ++nc)
            {
                node_aux.force_insert(nc->first, nc->second);
            }
        }
        else
        {
            flattenInputNode(n->second, root_path);
            node_aux[n->first] = n->second;
        }
    }

    node = node_aux;
}

bool YamlServer::isValid(const std::string& name_schema)
{
    log_.clear();
    
    log_ << "-------------------------------------- \n";
    log_ << "Log of YAML parsing procedure of file: \n";
    log_ << "-------------------------------------- \n";

    return isValidBase(name_schema, node_input_, header_log.str());
}

bool YamlServer::isValidBase(const std::string& name_schema,
                             const YAML::Node&  node_input,
                             std::string& header_log)
{
    // Load schema node
    YAML::Node node_schema
    try
    {
        filesystem::path path_schema = getPathSchema(name_schema);
        node_schema = YAML::LoadFile(path_schema.string());
    }
    catch(const std::exception& e)
    {
        header_log << e.what() << '\n';
        return false;
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
    flattenNode(node_schema);
    checkSchema(node_schema);

    // Check node_input against node_schema
    return isValidNode(node_schema, node_input, header_log);
}

void YamlServer::checkSchema(const YAML::Node node_schema, bool first_level) const
{
    // Check that node_schema is map
    if (first_level and not node_schema.IsMap())
    {
        throw std::runtime_error("The first level of a YAML schema should contain a map.");
    }
    if (node_schema.IsMap())
    {
        // Param schema (has mandatory and type)
        if (node_schema.isParamSchema())
        {
            // not mandatory params should have a default of correct type
            if (not node_schema["mandatory"].as<bool>())
            {
                if (not node_schema["default"])
                {
                    throw std::runtime_error("YAML schema: does not contain default value for a non mandatory parameter.");
                }
                if (not checkType(node_schema["default"], node_schema["type"].as<std::string>()))
                {
                    throw std::runtime_error("YAML schema: Default value wrong type.");
                }
            }
        }
        // sequence of maps defined by schemas
        else if (isSequenceOwnType(node_schema))
        {
            if (not node_schema["schema"])
            {
                throw std::runtime_error("YAML schema: for type " +
                                         SEQUENCE_OWN_TYPE + 
                                         " the yaml should contain the key 'schema' with the value 'derived' or the yaml file.");
            }
        }
        // Map
        else
        {
            for (auto node_schema_child : node_schema)
            {
                checkSchema(node_schema_child, false);
            }
        }
    }
}

bool YamlServer::isValidNode(const std::string& field,
                             const YAML::Node& node_schema,
                             const YAML::Node& node_input,
                             std::string& header_log)
{
    bool is_valid = true;
    
    // Param schema (has mandatory and type)
    if (isParamSchema(node_schema))
    {
        if (YAML::Node i_node = node_input[field])// TODO Change by node_input.isValid() or something like this
        {
            if (not checkType(node_schema, i_node, log_))
            {
                std::stringstream message;
                message << "Field '" << field << "' of wrong type. Should be "
                        << node_schema["type"].as<std::string>();
                writeToLog(header_log, message.str());
                is_valid = false;
            }
        }
        else if(node_schema["mandatory"]) // complain inexistence if mandatory
        {
            writeToLog(header_log, "Input yaml does not contain field: " + field);
            is_valid = false;
        }
    }
    // sequence of maps defined by schemas
    else if (isSequenceOwnType(node_schema))
    {
        // First check that the node it is really a sequence
        if (!node_input.IsSequence())
        {
            writeToLog(header_log, "Input yaml does not contain a sequence in: " + field);
            is_valid = false;
        }
        // Then check the validity of each element in the sequence
        else
        {
            for (std::size_t i = 0; i < node_input.size(); i++)
            {
                if (node_schema["schema"].as<std::string>() == "derived")
                {

                }
                else

                filesystem::path  path_schema_own_type;
                std::stringstream header_log;
                header_log << "[own_type/element_" << i << ": " << schema_node["schema"].as<std::string>() << "]";
                if (schema_node["polymorphism"].as<bool>())
                {
                    is_valid = isValidDerived(schema_node["schema"].as<std::string>(), node_input[i], header_log.str()) &&
                            is_valid;
                }
                else
                {
                    is_valid =
                        isValidBase(schema_node["schema"].as<std::string>(), node_input[i], header_log.str()) && is_valid;
                }
            }
        }
    }
    // map of params
    else
    {
        for (auto node_schema_child : node_schema)
        {
            YAML::Node node_input_child = node_input[field_child];
            is_valid = is_valid and isValidNode(node_schema_child.first.as<std::string>(), 
                                                node_schema_child.second, 
                                                node_input_child,
                                                header_log);
        }
    }

    return is_valid;
}

bool YamlServer::isValidDerived(const std::string& name_schema,
                                const YAML::Node&  node_input,
                                const std::string& header_log)
{
    // Check the base case
    bool is_valid = true;

    if (!node_input["type"])
    {
        std::stringstream message;
        message << "Derived input yaml does not contain field: "
                << "type";
        writeToLog(header_log, message.str());

        return false;
    }

    return isValidBase(node_input["type"].as<std::string>() + ".yaml", node_input, header_log) && is_valid;
}

filesystem::path YamlServer::getPathSchema(const std::string& name_schema)
{
    filesystem::path file_schema = filesystem::path(name_schema);
    for (auto base_p = folders_schema_.begin(); base_p != folders_schema_.end(); ++base_p)
    {
        path_schema = filesystem::path(*base_p) / file_schema;
        if (filesystem::exists(path_schema))
        {
            return file_schema;
        }
    }

    throw std::runtime_error("Schema with the name '" + name_schema + "' not found inside the YamlServer folders");
}

bool YamlServer::correctType(const YAML::Node& schema_node, const YAML::Node& node_input, std::stringstream& log)
{
    const std::string& type = schema_node["type"].as<std::string>();

    if (type == "string")
    {
        try
        {
            node_input.as<std::string>();
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
            node_input.as<double>();
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
            node_input.as<Eigen::VectorXd>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "sequence_own_type")
    {
        // First check that the node it is really a sequence
        if (!node_input.IsSequence())
        {
            return false;
        }

        // Then check the validity of each element in the sequence
        bool is_valid = true;
        for (std::size_t i = 0; i < node_input.size(); i++)
        {
            filesystem::path  path_schema_own_type;
            std::stringstream header_log;
            header_log << "[own_type/element_" << i << ": " << schema_node["schema"].as<std::string>() << "]";
            if (schema_node["polymorphism"].as<bool>())
            {
                is_valid = isValidDerived(schema_node["schema"].as<std::string>(), node_input[i], header_log.str()) &&
                           is_valid;
            }
            else
            {
                is_valid =
                    isValidBase(schema_node["schema"].as<std::string>(), node_input[i], header_log.str()) && is_valid;
            }
        }
        return is_valid;
    }

    return true;
}

bool YamlServer::checkType(const YAML::Node& node, const std::string& type) const
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
}

void YamlServer::writeToLog(const std::string& header, const std::string& message)
{
    log_ << header << ": " << message << std::endl;
}

const std::stringstream& YamlServer::getLog() const
{
    return log_;
}

const YAML::Node& YamlServer::getNodeInput() const
{
    return node_input_;
}

bool YamlServer::isParamSchema(const YAML::Node& node_schema) const
{
    return node_schema["mandatory"] and 
           node_schema["type"] and 
           node_schema["type"].as<std::string>() != SEQUENCE_OWN_TYPE;
}

bool YamlServer::isSequenceOwnType(const YAML::Node& node_schema) const
{
    return node_schema["type"] and 
           node_schema["type"].as<std::string>() == SEQUENCE_OWN_TYPE;
}

}  // namespace yaml_schema_cpp
