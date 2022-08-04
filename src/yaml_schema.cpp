// Copyright (C) 2022 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
// Authors: Josep Martí Saumell (jmarti@iri.upc.edu) and Joan Vallvé Navarro (jvallve@iri.upc.edu)
// All rights reserved.
//
// YAML-SCHEMA-CPP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdexcept>

#include "yaml-schema-cpp/yaml_schema.hpp"

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

YAML::Node loadSchema(std::string name_schema, const std::vector<std::string>& folders_schema, bool override)
{
    // Check extension
    if (filesystem::extension(name_schema).empty() )
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        throw std::runtime_error("Wrong schema file extension " + name_schema + 
                                 ", it should be '" + SCHEMA_EXTENSION + "'");
    }

    // Find schema file
    filesystem::path path_schema = findFileRecursive(name_schema, folders_schema);

    // Load schema yaml
    YAML::Node node_schema;
    try
    {
        node_schema = YAML::LoadFile(path_schema.string());
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("In loadSchema: Couldn't load the schema yaml file " + 
                                 path_schema.string() + ". Error: " + e.what());
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
    flattenNode(node_schema, folders_schema, true, override);

    // Check schema
    try
    {
        checkSchema(node_schema, "");
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("In loadSchema: The schema file " + 
                                 path_schema.string() + " is not valid. Error: " + e.what());
    }

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

    // specifications
    if (isSpecification(node_schema))
    {
        // Required 'type' of type string
        if (not node_schema[TYPE])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain " + TYPE);
        }
        if (not checkTrivialType(node_schema[TYPE],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + TYPE + " should be a string");
        }
        if (node_schema[TYPE].as<std::string>().find("derived") != std::string::npos)
        {
            // If type=="derived" or "derived[]", 'base' of type string is required
            if (not node_schema[BASE])
            {
                throw std::runtime_error("YAML schema: " + node_field + " of derived type does not contain " + BASE);
            }
            if (not checkTrivialType(node_schema[BASE],"string"))
            {
                throw std::runtime_error("YAML schema: In " + node_field + ", " + BASE + " should be a string");
            }
        }
        // Required 'doc' of type string
        if (not node_schema[DOC])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain " + DOC);
        }
        if (not checkTrivialType(node_schema[DOC],"string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + DOC + " should be a string");
        }
        // Required 'mandatory' of type bool
        if (not node_schema[MANDATORY])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain " + MANDATORY);
        }
        if (not checkTrivialType(node_schema[MANDATORY],"bool"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + MANDATORY + " should be a bool");
        }
        // OPTIONAL 'default'
        if (not node_schema[MANDATORY].as<bool>() and node_schema[DEFAULT])
        {
            auto type = isSequenceSchema(node_schema) ? getTypeOfSequence(node_schema) : node_schema[TYPE].as<std::string>();
            if (not checkTrivialType(node_schema[DEFAULT], type))
            {
                throw std::runtime_error("YAML schema: " + node_field + " default value wrong type");
            }
        }
        // OPTIONAL sequence 'options'
        if (node_schema[OPTIONS])
        {
            // Check that it is a sequence
            if (not node_schema[OPTIONS].IsSequence())
            {
                throw std::runtime_error("YAML schema: " + node_field + ", " + OPTIONS + " should be a sequence");
            }
            // check that all items have valid type
            auto type = isSequenceSchema(node_schema) ? getTypeOfSequence(node_schema) : node_schema[TYPE].as<std::string>();
            for (auto n_i = 0; n_i < node_schema[OPTIONS].size(); n_i++)
            {
                if (not checkTrivialType(node_schema[OPTIONS][n_i],type))
                {
                    throw std::runtime_error("YAML schema: " + node_field + 
                                             ", " + OPTIONS + "[" + std::to_string(n_i) + 
                                             "] has wrong type (should be " + type + ")");
                }
            }
            // If default, check that it is in options
            if (node_schema[DEFAULT])
            {
                bool option_found = false;
                for (auto valid_n : node_schema[OPTIONS])
                {
                    if (compare(valid_n, node_schema[DEFAULT], type))
                    {
                        option_found = true;
                        break;
                    }
                }
                if (not option_found)
                {
                    throw std::runtime_error("YAML schema: " + node_field + 
                                             ", " + DEFAULT + " value should be one of the specified in " + OPTIONS);
                }
            }
        }
    }
    // no specifications
    else
    {
        // check that there are no any of the specification keys
        for (auto reserved_key : RESERVED_KEYS)
        {
            if (node_schema[reserved_key])
            {
                throw std::runtime_error("YAML schema: " + node_field + 
                                         " is not interpreted as specification (any of the required items " +
                                         MANDATORY + ", " + TYPE + ", and " + DOC +
                                         " is missing) but has a key reserved for schema specifications: " +
                                         reserved_key);
            }
        }
        // check the children nodes
        for (auto node_schema_child : node_schema)
        {
            checkSchema(node_schema_child.second, node_schema_child.first.as<std::string>());
        }
    }
}

bool validateAllSchemas(const std::vector<std::string>& folders_schema, bool override)
{
    bool all_valid = true;

    for (auto folder : folders_schema)
    {
        if (filesystem::exists(folder) and filesystem::is_directory(folder))
        {
            for (auto const & entry : filesystem::recursive_directory_iterator(folder))
            {
                if (filesystem::is_regular_file(entry) and 
                    filesystem::extension(entry) == SCHEMA_EXTENSION)
                {
                    std::string schema_file = entry.path().string();

                    // Load schema yaml
                    YAML::Node node_schema;
                    try
                    {
                        node_schema = YAML::LoadFile(schema_file);
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "Couldn't load schema: " + schema_file + "\nError: " + e.what() << std::endl << std::endl;
                        all_valid = false;
                        continue;
                    }

                    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
                    try
                    {
                        flattenNode(node_schema, folders_schema, true, override);
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "Couldn't flatten schema: " + schema_file + "\nError: " + e.what() << std::endl << std::endl;
                        all_valid = false;
                        continue;
                    }

                    // Check schema
                    try
                    {
                        checkSchema(node_schema, "");
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "Invalid schema: " + schema_file + "\nError: " + e.what() << std::endl << std::endl;
                        all_valid = false;
                    }
                }
            }
        }
    }

    return all_valid;
}

bool applySchema(YAML::Node&  node_input,
                 const std::string& name_schema,
                 const std::vector<std::string>& folders,
                 std::stringstream& log,
                 const std::string& acc_field,
                 bool override)
{
    // Load and check schema
    YAML::Node node_schema;
    try
    {
        node_schema = loadSchema(name_schema, folders, override);
    }
    catch(const std::exception& e)
    {
        writeToLog(log, acc_field + ": schema '" + name_schema + "' couldn't be correctly loaded. Error: " + e.what() + "\n");
        return false;
    }

    // Check node_input against node_schema
    return applySchemaRecursive(node_input, node_input, node_schema, folders, log, acc_field, override);
}

bool applySchemaRecursive(YAML::Node& node_input,
                          YAML::Node& node_input_parent,
                          const YAML::Node& node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream& log,
                          const std::string& acc_field,
                          bool override)
{
    bool is_valid_current = true;
    bool is_valid_children = true;
    
    // Param schema (has mandatory and type)
    if (isSpecification(node_schema))
    {
        // Exists
        if (node_input.IsDefined())
        {
            // Not sequence
            if (not isSequenceSchema(node_schema))
            {
                // check trivial type
                if (isTrivialType(node_schema[TYPE].as<std::string>()))
                {  
                    // Wrong type (complain)
                    if (not checkTrivialType(node_input, node_schema[TYPE].as<std::string>()))
                    {
                        writeToLog(log, acc_field + ": wrong type, it should be " + node_schema[TYPE].as<std::string>() + "\n");
                        is_valid_current = false;
                    }
                    // Check "valid options"
                    else if (node_schema[OPTIONS])
                    {
                        if (not checkOptions(node_input, node_schema[OPTIONS], node_schema[TYPE].as<std::string>()))
                        {
                            std::stringstream options; 
                            options << node_schema[OPTIONS];
                            writeToLog(log, acc_field + 
                                       ": wrong value, it should be one of the following: \n" + options.str() + "\n");
                            is_valid_current = false;
                        }
                    }
                }
                // Derived type
                else if (node_schema[TYPE].as<std::string>() == "derived")
                {
                    // check existence of key type
                    if (not node_input["type"])
                    {
                        writeToLog(log, acc_field + ": does not contain key 'type'\n");
                        is_valid_current = false;
                    }
                    else
                    {
                        // Validate with the base schema file
                        is_valid_children = applySchema(node_input, 
                                                        node_schema[BASE].as<std::string>(), 
                                                        folders, 
                                                        log, 
                                                        acc_field,
                                                        override)
                                            and is_valid_children;
                        
                        // Validate with the derived schema file
                        is_valid_children = applySchema(node_input, 
                                                        node_input["type"].as<std::string>(), 
                                                        folders, 
                                                        log, 
                                                        acc_field,
                                                        override)
                                            and is_valid_children;
                    }
                }
                // non trivial type known in schema
                else
                {
                    throw std::runtime_error("not trivial type nor 'derived', not implemented: " + node_schema[TYPE].as<std::string>());
                }
            }
            // sequence
            else 
            {
                // First check that the node it is really a sequence
                if (not node_input.IsSequence())
                {
                    writeToLog(log, "Input yaml does not contain a sequence in: " + acc_field + "\n");
                    is_valid_current = false;
                }
                // Then check the validity of each element in the sequence
                else
                {
                    // to check all the entries of the sequence, remove the brackets [] from the end of type 
                    YAML::Node node_schema_i = node_schema;
                    node_schema_i[TYPE] = getTypeOfSequence(node_schema);

                    for (auto i = 0; i < node_input.size(); i++)
                    {
                        YAML::Node node_input_i = node_input[i];
                        is_valid_children = applySchemaRecursive(node_input_i,
                                                                 node_input,
                                                                 node_schema_i, 
                                                                 folders,
                                                                 log,
                                                                 acc_field + "[" + std::to_string(i) + "]",
                                                                 override)
                                            and is_valid_children;
                    }
                }
            }
        }
        // Does not exist
        else 
        {
            // complain if mandatory
            if(node_schema[MANDATORY].as<bool>())
            {
                writeToLog(log, "Input yaml does not contain field: " + acc_field + "\n");
                is_valid_current = false;
            }
            // add node with default value (if parent is defined)
            else if (node_schema[DEFAULT] and node_input_parent.IsDefined())
            {
                auto field = filesystem::path(acc_field).filename().string();
                node_input_parent[field] = node_schema[DEFAULT];
            }
        }
    }
    // map without specification
    else
    {
        for (auto node_schema_child : node_schema)
        {
            YAML::Node node_input_child = (node_input.IsDefined() ? 
                                           node_input[node_schema_child.first.as<std::string>()] : 
                                           node_input);

            is_valid_children = applySchemaRecursive(node_input_child,
                                                     node_input,
                                                     node_schema_child.second, 
                                                     folders,
                                                     log,
                                                     (acc_field.empty() ? "" : acc_field + "/") + node_schema_child.first.as<std::string>(),
                                                     override)
                                and is_valid_children;
        }
    }

    // If not valid, print the doc
    if (not is_valid_current)
    {
        writeToLog(log, "-- '" + acc_field + "':\n");
        writeToLog(log, "\tDoc: " + node_schema[DOC].as<std::string>() + "\n");
        writeToLog(log, "\tType: " + node_schema[TYPE].as<std::string>() + "\n");
        if (node_schema[OPTIONS])
        {
            writeToLog(log, "\tAccepted values: ");
            for (auto option_n : node_schema[OPTIONS])
            {
                writeToLog(log, option_n.as<std::string>() + ", ");
            }
            writeToLog(log, "\n");
        }
        writeToLog(log, "\n");
    }

    return is_valid_current and is_valid_children;
}

bool checkOptions(const YAML::Node& input_node, const YAML::Node& options_node, const std::string& type)
{
    for (auto option_n : options_node)
    {
        if (compare(option_n, input_node, type))
        {
            return true;
        }
    }
    return false;
}

void addNodeSchema(YAML::Node& node, const std::string& key, const YAML::Node& value, bool override)
{
    if (node[key])
    {
        if (isSpecification(node[key]))
        {
            if (override)
            {
                node[key] = value;
            }
            else
            {
                throw std::runtime_error("Trying to add something into an already existing schema node");
            }
        }
        else
        {
            if (hasAnyReservedKey(node[key]))
            {
                throw std::runtime_error("addNodeSchema: node[key] has any of the reserved keys but not all required keys");
            }

            for (auto value_map_node : value)
            {
                YAML::Node node_key = node[key];
                addNodeSchema(node_key, value_map_node.first.as<std::string>(), value_map_node.second, override);
            }
        }
    }
    else
    {
        node[key] = value;
    }
}

bool isSpecification(const YAML::Node& node_schema)
{
    for (auto required_key : REQUIRED_KEYS)
    {
        if (not node_schema[required_key])
        {
            return false;
        }
    }
    return true;
}

bool hasAnyReservedKey(const YAML::Node& node_schema)
{
    for (auto reserved_key : RESERVED_KEYS)
    {
        if (node_schema[reserved_key])
        {
            return true;
        }
    }
    return false;
}

bool isSequenceSchema(const YAML::Node& node_schema)
{
    if (not node_schema[TYPE])
    {
        throw std::runtime_error("isSequenceSchema: the node does not have the key " + TYPE);
    }
    auto type = node_schema[TYPE].as<std::string>();
    return type.size() > 2 and type.substr(type.size()-2) == "[]";
}

std::string getTypeOfSequence(const YAML::Node& node_schema)
{
    if (not node_schema[TYPE])
    {
        throw std::runtime_error("getTypeOfSequence: the node does not have the key " + TYPE);
    }
    if (not isSequenceSchema(node_schema))
    {
        throw std::runtime_error("getTypeOfSequence: the node type is not sequence: " + node_schema[TYPE].as<std::string>());
    }
    auto type_seq = node_schema[TYPE].as<std::string>();
    return type_seq.substr(0, type_seq.size()-2);
}

}  // namespace yaml_schema_cpp
