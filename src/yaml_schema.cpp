#include <stdexcept>

#include "yaml-schema-cpp/yaml_schema.hpp"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

YAML::Node loadSchema(std::string name_schema, const std::vector<std::string>& folders_schema)
{
    // Check extension
    if (filesystem::extension(name_schema).empty() )
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        throw std::runtime_error("Wrong schema file extension " + 
                                 name_schema + 
                                 ", it should be '" + 
                                 SCHEMA_EXTENSION + 
                                 "'");
    }

    // Load schema yaml
    filesystem::path path_schema = findFileRecursive(name_schema, folders_schema);
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
        throw std::runtime_error("YAML schema: " + node_field + " should be a map");
    }

    // scalar
    if (isScalarSchema(node_schema))
    {
        // Required 'type' of type string
        if (not node_schema["type"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'type'");
        }
        if (not checkType(node_schema["type"],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'type' should be a string");
        }
        // Required 'doc' of type string
        if (not node_schema["doc"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'doc'");
        }
        if (not checkType(node_schema["doc"],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'doc' should be a string");
        }
        // Required 'mandatory' of type bool
        if (not node_schema["mandatory"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'mandatory'");
        }
        if (not checkType(node_schema["mandatory"],"bool"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'mandatory' should be a bool");
        }
        // If 'mandatory' == false, required 'default' of type "type"
        if (not node_schema["mandatory"].as<bool>())
        {
            if (not node_schema["default"])
            {
                throw std::runtime_error("YAML schema: " + node_field + " does not contain default value for a non mandatory parameter");
            }
            if (not checkType(node_schema["default"], node_schema["type"].as<std::string>()))
            {
                throw std::runtime_error("YAML schema: " + node_field + " default value wrong type");
            }
        }
        // OPTIONAL sequence 'options'
        if (node_schema["options"])
        {
            // Check that it is a sequence
            if (not node_schema["options"].IsSequence())
            {
                throw std::runtime_error("YAML schema: " + node_field + ", 'options' should be a sequence");
            }
            // check that all items have valid type
            for (auto n_i = 0; n_i < node_schema["options"].size(); n_i++)
            {
                if (not checkType(node_schema["options"][n_i],node_schema["type"].as<std::string>()))
                {
                    throw std::runtime_error("YAML schema: " + node_field + 
                                             ", 'options'[" + std::to_string(n_i) + 
                                             "] has wrong type (should be " + node_schema["type"].as<std::string>() + ")");
                }
            }
            // If default, check that it is in options
            if (node_schema["default"])
            {
                bool option_found = false;
                for (auto valid_n : node_schema["options"])
                {
                    if (compare(valid_n, node_schema["default"], node_schema["type"].as<std::string>()))
                    {
                        option_found = true;
                        break;
                    }
                }
                if (not option_found)
                {
                    throw std::runtime_error("YAML schema: " + node_field + 
                                             ", 'default' value should be one of the specified in 'options'");
                }
            }
        }
    }
    // sequence
    else if (isSequenceSchema(node_schema))
    {
        // Check 'type'
        if (not node_schema["type"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'type'");
        }
        else if (not checkType(node_schema["type"],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'type' should be a string");
        }
        // Check 'mandatory'
        if (not node_schema["mandatory"])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain 'mandatory'");
        }
        else if (not checkType(node_schema["mandatory"],"bool"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", 'mandatory' should be a bool");
        }
        // OPTIONAL sequence 'options'
        if (node_schema["options"])
        {
            // Check that it is a sequence
            if (not node_schema["options"].IsSequence())
            {
                throw std::runtime_error("YAML schema: " + node_field + ", 'options' should be a sequence");
            }
            // Check that all items have valid type
            for (auto n_i = 0; n_i < node_schema["options"].size(); n_i++)
            {
                if (not checkType(node_schema["options"][n_i],node_schema["type"].as<std::string>()))
                {
                    throw std::runtime_error("YAML schema: " + node_field + 
                                             ", 'options'[" + std::to_string(n_i) + 
                                             "] has wrong type (should be " + node_schema["type"].as<std::string>() + ")");
                }
            }
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
        throw std::runtime_error("YAML schema: " + node_field + " has unknown yaml_type");
    }
}

bool applySchema(YAML::Node&  node_input,
                 const std::string& name_schema,
                 const std::vector<std::string>& folders,
                 std::stringstream& log,
                 const std::string& acc_field)
{
    // Load and check schema
    YAML::Node node_schema = loadSchema(name_schema, folders);

    // Check node_input against node_schema
    return applySchemaRecursive(node_input, node_input, node_schema, folders, log, acc_field);
}

bool applySchemaRecursive(YAML::Node& node_input,
                          YAML::Node& node_input_parent,
                          const YAML::Node& node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream& log,
                          const std::string& acc_field)
{
    bool is_valid = true;
    
    // Param schema (has mandatory and type)
    if (isScalarSchema(node_schema))
    {
        // Exists
        if (node_input.IsDefined())
        {
            // Wrong type (complain)
            if (not checkType(node_input, node_schema["type"].as<std::string>()))
            {
                writeToLog(log, 
                           acc_field + ": wrong type, it should be " + node_schema["type"].as<std::string>());
                is_valid = false;
            }
            // Check "valid options"
            else if (node_schema["options"])
            {
                bool option_found = false;
                for (auto valid_n : node_schema["options"])
                {
                    if (compare(valid_n, node_input, node_schema["type"].as<std::string>()))
                    {
                        option_found = true;
                        break;
                    }
                }
                if (not option_found)
                {
                    std::stringstream options; 
                    options << node_schema["options"];
                    writeToLog(log, acc_field + ": Wrong value, it should be one of the following: \n" + options.str());
                    is_valid = false;
                }
            }
        }
        // Does not exist
        else 
        {
            // complain if mandatory)
            if(node_schema["mandatory"].as<bool>())
            {
                writeToLog(log, "Input yaml does not contain field: " + acc_field);
                is_valid = false;
            }
            // add node with default value (if parent is defined)
            else if (node_input_parent.IsDefined())
            {
                auto field = filesystem::path(acc_field).filename().string();
                node_input_parent[field] = node_schema["default"];
            }
        }

        // If not valid, print the doc
        if (not is_valid)
        {
            writeToLog(log, "-- Documentation of '" + acc_field + "': " + node_schema["doc"].as<std::string>() + "\n\n");
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
                writeToLog(log, "Input yaml does not contain a sequence in: " + acc_field);
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
                                       acc_field + 
                                       ": Element " + 
                                       std::to_string(i) + 
                                       " does not contain key 'type'");
                            is_valid = false; 
                            continue;
                        }
                        else
                        {
                            type = node_input[i]["type"].as<std::string>();
                        }
                    }
                     
                    // If check for trivial types
                    if (checkType(node_input[i], type))
                    {
                        // Check "valid options"
                        if (node_schema["options"])
                        {
                            bool option_found = false;
                            for (auto valid_n : node_schema["options"])
                            {
                                if (compare(valid_n, node_input[i], node_schema["type"].as<std::string>()))
                                {
                                    option_found = true;
                                    break;
                                }
                            }
                            if (not option_found)
                            {
                                std::stringstream options; 
                                options << node_schema["options"];
                                writeToLog(log,  
                                           acc_field + 
                                           ": Element " + std::to_string(i) + 
                                           " has wrong value, it should be one of the following: \n" + options.str());
                                is_valid = false;
                            }
                        }
                    }
                    // Trivial types didn't check, try to load an schema from 'type'
                    else
                    {
                        auto file_schema = findFileRecursive(type + SCHEMA_EXTENSION, folders);

                        // Check file exists
                        if (not filesystem::exists(file_schema))
                        {
                            writeToLog(log, 
                                       acc_field + 
                                       ": element " + std::to_string(i) + 
                                       " the type " + type +
                                       " couldn't be checked against trivial types and " + 
                                       type + SCHEMA_EXTENSION + " file was not found");
                            is_valid = false;
                            continue;
                        }
                        // Validate with the schema file
                        YAML::Node node_input_i = node_input[i];
                        is_valid = applySchema(node_input_i, 
                                               file_schema.filename().string(), 
                                               folders, 
                                               log, acc_field + "[" + std::to_string(i) + "]") 
                                   and is_valid;
                    }
                }
            }
        }
        else if (node_schema["mandatory"]) // complain inexistence if mandatory
        {
            writeToLog(log, "Input yaml does not contain field: " + acc_field);
            is_valid = false;
        }

        // If not valid, print the doc
        if (not is_valid)
        {
            writeToLog(log, "-- Documentation of '" + acc_field + "': " + node_schema["doc"].as<std::string>() + "\n\n");
        }
    }
    // map of params
    else if (isMapSchema(node_schema))
    {
        for (auto node_schema_child : node_schema)
        {
            YAML::Node node_input_child = (node_input.IsDefined() ? 
                                           node_input[node_schema_child.first.as<std::string>()] : 
                                           node_input);

            is_valid = is_valid and applySchemaRecursive(node_input_child,
                                                         node_input,
                                                         node_schema_child.second, 
                                                         folders,
                                                         log,
                                                         (acc_field.empty() ? "" : acc_field + "/") + node_schema_child.first.as<std::string>());
        }
    }
    else
    {
        throw std::runtime_error("unknown schema");
    }

    return is_valid;
}

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value)
{
    if (node[key])
    {
        if (isScalarSchema(node[key]))
        {
            throw std::runtime_error("Trying to add something into an already existing scalar schema node");
        }
        if (isSequenceSchema(node[key]))
        {
            throw std::runtime_error("Trying to add something into an already existing sequence schema node");
        }
        if (isMapSchema(node[key]))
        {
            if (not isMapSchema(value))
            {
                throw std::runtime_error("Trying to add a sequence, scalar or unknown schema node into an already existing map schema node");
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
            throw std::runtime_error("Trying to add something into an schema node of unknown yaml_type");
        }
    }
    else
    {
        node[key] = value;
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
