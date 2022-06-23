#include <stdexcept>

#include "yaml-schema-cpp/yaml_schema.h"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value)
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

YAML::Node loadSchema(const std::string& name_schema, const std::vector<std::string>& folders_schema)
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

void checkSchema(const YAML::Node& node_schema, const std::string& node_field)
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
        // Check 'doc'
        if (not node_schema["doc"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'doc'.");
        }
        else if (not checkType(node_schema["doc"],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'doc' should be a string.");
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

bool isScalarSchema(const YAML::Node& node_schema)
{
    return node_schema["yaml_type"] and node_schema["yaml_type"].as<std::string>() == "scalar";
}

bool isSequenceSchema(const YAML::Node& node_schema)
{
    return node_schema["yaml_type"] and node_schema["yaml_type"].as<std::string>() == "sequence";
}

bool isMapSchema(const YAML::Node& node_schema)
{
    return node_schema.IsMap() and not node_schema["yaml_type"];
}

}  // namespace yaml_schema_cpp
