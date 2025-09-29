#include <stdexcept>

#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/expression.hpp"

namespace yaml_schema_cpp
{

YAML::Node loadSchema(std::string                     name_schema,
                      const std::vector<std::string>& folders_schema,
                      std::stringstream&              log,
                      bool                            override)
{
    // Check extension
    if (filesystem::extension(name_schema).empty())
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        log << "ERROR in loadSchema(): Wrong schema file extension " + name_schema + ", it should be '" +
                   SCHEMA_EXTENSION + "'\n";
        return YAML::Node(YAML::NodeType::Undefined);
    }

    // Find schema file
    filesystem::path path_schema;
    try
    {
        path_schema = findFileRecursive(name_schema, folders_schema);
    }
    catch (const std::exception& e)
    {
        log << std::string("ERROR in loadSchema(): ") + e.what() << "\n";
        return YAML::Node(YAML::NodeType::Undefined);
    }

    // write schema file in log
    log << "schema file found: " << path_schema.string() << std::endl;

    // Load schema yaml
    YAML::Node node_schema;
    try
    {
        node_schema = YAML::LoadFile(path_schema.string());
    }
    catch (const std::exception& e)
    {
        log << "ERROR in loadSchema(): Couldn't load the schema yaml file " + path_schema.string() +
                   ". Error: " + e.what()
            << "\n";
        return YAML::Node(YAML::NodeType::Undefined);
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information

    try
    {
        flattenNode(node_schema, path_schema.parent_path().string(), folders_schema, true, override);
    }
    catch (const std::exception& e)
    {
        log << "ERROR in loadSchema(): Couldn't flatten schema file " + path_schema.string() + ". Error: " + e.what()
            << "\n";

        return YAML::Node(YAML::NodeType::Undefined);
    }

    // Check schema
    try
    {
        checkSchema(node_schema, "", node_schema, folders_schema);
    }
    catch (const std::exception& e)
    {
        log << "ERROR in loadSchema(): The schema file " + path_schema.string() + " is not valid. Error: " + e.what()
            << "\n";
        return YAML::Node(YAML::NodeType::Undefined);
    }

    return node_schema;
}

void checkSchema(const YAML::Node&               node_schema,
                 const std::string&              node_field,
                 const YAML::Node&               node_schema_parent,
                 const std::vector<std::string>& folders_schema)
{
    // skip scalars and not defined (empty schemas)
    if (node_schema.IsScalar() or node_schema.IsNull()) return;

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
        if (not tryNodeAs(node_schema[TYPE], "string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + TYPE + " should be a string");
        }
        auto type =
            isSequenceSchema(node_schema) ? getTypeOfSequence(node_schema) : node_schema[TYPE].as<std::string>();
        // If type=="derived" or "derived[]", 'BASE' of type string is required
        if (type == "derived")
        {
            if (not node_schema[BASE])
            {
                throw std::runtime_error("YAML schema: " + node_field + " of derived type does not contain " + BASE);
            }
            if (not tryNodeAs(node_schema[BASE], "string"))
            {
                throw std::runtime_error("YAML schema: In " + node_field + ", " + BASE + " should be a string");
            }
            // store type as BASE to check OPTIONS, VALUE & DEFAULT
            type = node_schema[BASE].as<std::string>();
        }
        // Required 'doc' of type string
        if (not node_schema[DOC])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain " + DOC);
        }
        if (not tryNodeAs(node_schema[DOC], "string"))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + DOC + " should be a string");
        }
        // Required 'mandatory' of type bool or expression
        if (not node_schema[MANDATORY])
        {
            throw std::runtime_error("YAML schema: " + node_field + " does not contain " + MANDATORY);
        }
        if (not tryNodeAs(node_schema[MANDATORY], "bool") and not isExpression(node_schema[MANDATORY]))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + MANDATORY +
                                     " should be a bool or an expression.");
        }
        // check expression
        std::string err_msg;
        if (isExpression(node_schema[MANDATORY]) and
            not checkExpression(node_schema[MANDATORY], node_schema_parent, err_msg))
        {
            throw std::runtime_error("YAML schema: In " + node_field + ", " + MANDATORY +
                                     " wrong expression: " + err_msg);
        }
        // OPTIONAL 'value'
        if (node_schema[VALUE])
        {
            // schema is a sequence type
            if (isSequenceSchema(node_schema))
            {
                // check value is sequence as well
                if (not node_schema[VALUE].IsSequence())
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE + " should be a sequence");
                }

                // check type for each node
                for (auto n_i = 0; n_i < node_schema[VALUE].size(); n_i++)
                {
                    // CHECK TYPE (taking BASE if "derived")
                    // trivial type
                    if (isTrivialType(type))
                    {
                        if (not tryNodeAs(node_schema[VALUE][n_i], type))
                        {
                            throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE + "[" +
                                                     std::to_string(n_i) + "] of wrong type");
                        }
                    }
                    // type with schema
                    else if (isNonTrivialType(type, folders_schema))
                    {
                        std::stringstream log;
                        YAML::Node        node_schema_value = node_schema[VALUE][n_i];

                        if (not applySchema(node_schema_value, type, folders_schema, log, ""))
                        {
                            throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE + "[" +
                                                     std::to_string(n_i) +
                                                     "] did not pass the schema check with error: " + log.str());
                        }
                    }
                    else
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE + " value unknown type.");
                    }
                }
            }
            // schema is a scalar type
            else
            {
                // CHECK TYPE (taking BASE if "derived")
                // trivial type
                if (isTrivialType(type))
                {
                    if (not tryNodeAs(node_schema[VALUE], type))
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE + " of wrong type");
                    }
                }
                // type with schema
                else if (isNonTrivialType(type, folders_schema))
                {
                    std::stringstream log;
                    YAML::Node        node_schema_value = node_schema[VALUE];

                    if (not applySchema(node_schema_value, type, folders_schema, log, ""))
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE +
                                                 " did not pass the schema check with error: " + log.str());
                    }
                }
                else
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE + " value unknown type.");
                }
            }

            // if 'value', 'mandatory' should be false (no sense requiring user to define something already
            // defined)
            if (isExpression(node_schema[MANDATORY]) or node_schema[MANDATORY].as<bool>())
            {
                throw std::runtime_error("YAML schema: " + node_field + ", if " + VALUE + " defined, " + MANDATORY +
                                         " should be false");
            }
        }

        // OPTIONAL 'default' only if 'mandatory'=false or expression
        if (node_schema[DEFAULT])
        {
            if (not isExpression(node_schema[MANDATORY]) and node_schema[MANDATORY].as<bool>())
            {
                throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT +
                                         " value cannot be defined in mandatory field");
            }

            // schema is a sequence type
            if (isSequenceSchema(node_schema))
            {
                // check value is sequence as well
                if (not node_schema[DEFAULT].IsSequence())
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT + " should be a sequence");
                }

                // check type for each node
                for (auto n_i = 0; n_i < node_schema[DEFAULT].size(); n_i++)
                {
                    // CHECK TYPE (take BASE if "derived")
                    // trivial type
                    if (isTrivialType(type))
                    {
                        if (not tryNodeAs(node_schema[DEFAULT][n_i], type))
                        {
                            throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT + "[" +
                                                     std::to_string(n_i) + "] wrong type");
                        }
                    }
                    // type with schema
                    else if (isNonTrivialType(type, folders_schema))
                    {
                        std::stringstream log;
                        YAML::Node        node_schema_default = node_schema[DEFAULT][n_i];
                        // type
                        if (not applySchema(node_schema_default, type, folders_schema, log, ""))
                        {
                            throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT + "[" +
                                                     std::to_string(n_i) +
                                                     "] did not pass the schema check with error: " + log.str());
                        }
                    }
                    else
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT + "[" +
                                                 std::to_string(n_i) + "] unknown type.");
                    }
                }
            }
            // schema is a scalar type
            else
            {
                // CHECK TYPE (take BASE if "derived")
                // trivial type
                if (isTrivialType(type))
                {
                    if (not tryNodeAs(node_schema[DEFAULT], type))
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT + " value wrong type");
                    }
                }
                // type with schema
                else if (isNonTrivialType(type, folders_schema))
                {
                    std::stringstream log;
                    YAML::Node        node_schema_default = node_schema[DEFAULT];
                    if (not applySchema(node_schema_default, type, folders_schema, log, ""))
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT +
                                                 " value did not pass the schema check with error: " + log.str());
                    }
                }
                else
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT + " value unknown type.");
                }
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
            // CHECK TYPE (take BASE if "derived")
            for (auto n_i = 0; n_i < node_schema[OPTIONS].size(); n_i++)
            {
                // trivial type
                if (isTrivialType(type))
                {
                    if (not tryNodeAs(node_schema[OPTIONS][n_i], type))
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + OPTIONS + "[" +
                                                 std::to_string(n_i) + "] has wrong type (should be " + type + ")");
                    }
                }
                // type with schema
                else if (isNonTrivialType(type, folders_schema))
                {
                    std::stringstream log;
                    YAML::Node        node_schema_i = node_schema[OPTIONS][n_i];
                    if (not applySchema(node_schema_i, type, folders_schema, log, ""))
                    {
                        throw std::runtime_error("YAML schema: " + node_field + ", " + OPTIONS + "[" +
                                                 std::to_string(n_i) +
                                                 "] did not pass the schema check with error: " + log.str());
                    }
                }
                else
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + OPTIONS + "[" +
                                             std::to_string(n_i) + "] unknown type.");
                }
            }
            // If default, check it is included in options
            if (node_schema[DEFAULT])
            {
                bool default_found = false;
                for (auto valid_n : node_schema[OPTIONS])
                {
                    if (compare(valid_n, node_schema[DEFAULT], type))
                    {
                        default_found = true;
                        break;
                    }
                }
                if (not default_found)
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT +
                                             " value should be one of the specified in " + OPTIONS);
                }
            }
            // If value, check it is included in options
            if (node_schema[VALUE])
            {
                bool value_found = false;
                for (auto valid_n : node_schema[OPTIONS])
                {
                    if (compare(valid_n, node_schema[VALUE], type))
                    {
                        value_found = true;
                        break;
                    }
                }
                if (not value_found)
                {
                    throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE +
                                             " value should be one of the specified in " + OPTIONS);
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
                throw std::runtime_error(
                    "YAML schema: " + node_field + " is not interpreted as specification (any of the required items " +
                    MANDATORY + ", " + TYPE + ", and " + DOC +
                    " is missing) but has a key reserved for schema specifications: " + reserved_key);
            }
        }
        // check the children nodes
        for (auto node_schema_child : node_schema)
        {
            checkSchema(
                node_schema_child.second, node_schema_child.first.as<std::string>(), node_schema, folders_schema);
        }
    }
}

bool validateAllSchemas(const std::vector<std::string>& folders_schema, bool verbose, bool override)
{
    bool all_valid = true;

    for (auto folder : folders_schema)
    {
        if (filesystem::exists(folder) and filesystem::is_directory(folder))
        {
            for (auto const& entry : filesystem::recursive_directory_iterator(folder))
            {
                if (filesystem::is_regular_file(entry) and filesystem::extension(entry) == SCHEMA_EXTENSION)
                {
                    std::string schema_file = entry.path().string();

                    if (verbose) std::cout << "Validating " << schema_file << "... ";

                    // Load schema yaml
                    YAML::Node node_schema;
                    try
                    {
                        node_schema = YAML::LoadFile(schema_file);
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "ERROR!\n\tCouldn't load schema" + (verbose ? "" : ": " + schema_file) + "\n\t" +
                                         e.what()
                                  << std::endl
                                  << std::endl;
                        all_valid = false;
                        continue;
                    }

                    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the
                    // information
                    try
                    {
                        flattenNode(node_schema, entry.path().parent_path().string(), folders_schema, true, override);
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "ERROR!\n\tCouldn't flatten schema" + (verbose ? "" : ": " + schema_file) +
                                         "\n\t" + e.what()
                                  << std::endl
                                  << std::endl;
                        all_valid = false;
                        continue;
                    }

                    // Check schema
                    try
                    {
                        checkSchema(node_schema, "", node_schema, folders_schema);
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "ERROR!\n\tInvalid schema" + (verbose ? "" : ": " + schema_file) + "\n\t" +
                                         e.what()
                                  << std::endl
                                  << std::endl;
                        all_valid = false;
                        continue;
                    }

                    if (verbose) std::cout << "OK!\n";
                }
            }
        }
    }

    return all_valid;
}

bool applySchema(YAML::Node&                     node_input,
                 const std::string&              name_schema,
                 const std::vector<std::string>& folders,
                 std::stringstream&              log,
                 const std::string&              acc_field,
                 bool                            override)
{
    // Load and check schema
    YAML::Node node_schema(YAML::NodeType::Undefined);
    node_schema = loadSchema(name_schema, folders, log, override);
    if (not node_schema.IsDefined()) return false;

    // Check node_input against node_schema
    return applySchemaRecursive(node_input, node_input, node_schema, folders, log, acc_field, override);
}

bool applySchemaRecursive(YAML::Node&                     node_input,
                          YAML::Node&                     node_input_parent,
                          const YAML::Node&               node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream&              log,
                          const std::string&              acc_field,
                          bool                            override)
{
    bool is_valid = true;

    // Param schema (has mandatory and type)
    if (isSpecification(node_schema))
    {
        // Exists
        if (node_input.IsDefined())
        {
            // If VALUE defined in schema, complain if different
            if (node_schema[VALUE] and
                not compare(node_schema[VALUE], node_input, node_schema[TYPE].as<std::string>()))
            {
                writeErrorToLog(log,
                                acc_field,
                                node_schema,
                                " already defined in schema with a different value. Not allowed to be changed.");
                is_valid = false;
            }

            // Not sequence
            if (not isSequenceSchema(node_schema))
            {
                // check trivial type
                if (isTrivialType(node_schema[TYPE].as<std::string>()))
                {
                    // Wrong type (complain)
                    if (not tryNodeAs(node_input, node_schema[TYPE].as<std::string>()))
                    {
                        writeErrorToLog(log, acc_field, node_schema, "Wrong type.");
                        is_valid = false;
                    }
                    // Check "valid options"
                    else if (node_schema[OPTIONS])
                    {
                        if (not checkOptions(node_input, node_schema[OPTIONS], node_schema[TYPE].as<std::string>()))
                        {
                            writeErrorToLog(
                                log, acc_field, node_schema, "Wrong value. Allowed values defined in OPTIONS.");
                            is_valid = false;
                        }
                    }
                }
                // Derived type
                else if (node_schema[TYPE].as<std::string>() == "derived")
                {
                    // check existence of key type
                    if (not node_input["type"])
                    {
                        writeErrorToLog(log,
                                        acc_field,
                                        node_schema,
                                        "Does not contain key 'type' which is mandatory for 'derived'.");
                        is_valid = false;
                    }
                    else
                    {
                        // Validate with derived schema file
                        is_valid = applySchema(node_input,
                                               node_input["type"].as<std::string>(),
                                               folders,
                                               log,
                                               acc_field,
                                               override) and
                                   is_valid;

                        // Validate with base schema file (after derived since it may complete the input node)
                        is_valid =
                            applySchema(
                                node_input, node_schema[BASE].as<std::string>(), folders, log, acc_field, override) and
                            is_valid;
                    }
                }
                // custom non trivial-type
                else if (isNonTrivialType(node_schema[TYPE].as<std::string>(), folders))
                {
                    is_valid =
                        applySchema(
                            node_input, node_schema[TYPE].as<std::string>(), folders, log, acc_field, override) and
                        is_valid;
                }
                // non trivial type also failed
                else
                {
                    throw std::runtime_error("Not trivial type not found: " + node_schema[TYPE].as<std::string>());
                }
            }
            // sequence
            else
            {
                // First check that the node it is really a sequence
                if (not node_input.IsSequence())
                {
                    writeErrorToLog(log, acc_field, node_schema, "Should be a sequence.");
                    is_valid = false;
                }
                // Then check the validity of each element in the sequence
                else
                {
                    // to check all the entries of the sequence, remove the brackets [] from the end of type
                    YAML::Node node_schema_i = Clone(node_schema);
                    node_schema_i[TYPE]      = getTypeOfSequence(node_schema);

                    for (auto i = 0; i < node_input.size(); i++)
                    {
                        YAML::Node node_input_i = node_input[i];
                        is_valid                = applySchemaRecursive(node_input_i,
                                                        node_input,
                                                        node_schema_i,
                                                        folders,
                                                        log,
                                                        acc_field + "[" + std::to_string(i) + "]",
                                                        override) and
                                   is_valid;
                    }
                }
            }
        }
        // Does not exist
        else
        {
            // Load VALUE in case defined in schema
            if (node_schema[VALUE])
            {
                // add node with value (if parent is defined)
                if (node_input_parent.IsDefined())
                {
                    auto field               = filesystem::path(acc_field).filename().string();
                    node_input_parent[field] = node_schema[VALUE];
                }
            }
            // Check if it is mandatory
            else
            {
                assert(tryNodeAs(node_schema[MANDATORY], "bool") or isExpression(node_schema[MANDATORY]));
                bool mandatory;
                if (isExpression(node_schema[MANDATORY]))
                {
                    try
                    {
                        mandatory = evalExpression(node_schema[MANDATORY].as<std::string>(), node_input_parent);
                    }
                    catch (const std::exception& e)
                    {
                        writeErrorToLog(log,
                                        acc_field,
                                        node_schema,
                                        "Evaluating schema expression for 'mandatory' of field " + acc_field +
                                            " failed with error: " + e.what() + "\n");
                        is_valid = false;
                    }
                }
                else
                    mandatory = node_schema[MANDATORY].as<bool>();

                // complain if mandatory
                if (mandatory)
                {
                    writeErrorToLog(log,
                                    acc_field,
                                    node_schema,
                                    "Missing mandatory field (" + MANDATORY + ": " +
                                        node_schema[MANDATORY].as<std::string>() + ").");
                    is_valid = false;
                }
                // add node with default value (if parent is defined)
                else if (node_schema[DEFAULT] and node_input_parent.IsDefined())
                {
                    auto field               = filesystem::path(acc_field).filename().string();
                    node_input_parent[field] = node_schema[DEFAULT];
                }
            }
        }
    }
    // map without specification
    else
    {
        // if doesn't exist, we create it. If it should have mandatory fields, it will crash later.
        if (not node_input.IsDefined())
        {
            node_input               = YAML::Node();
            auto field               = filesystem::path(acc_field).filename().string();
            node_input_parent[field] = node_input;
        }

        // iterate all childs
        for (auto node_schema_child : node_schema)
        {
            YAML::Node node_input_child = node_input[node_schema_child.first.as<std::string>()];

            is_valid = applySchemaRecursive(
                           node_input_child,
                           node_input,
                           node_schema_child.second,
                           folders,
                           log,
                           (acc_field.empty() ? "" : acc_field + "/") + node_schema_child.first.as<std::string>(),
                           override) and
                       is_valid;
        }
    }

    return is_valid;
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

void addNodeSchema(YAML::Node&        node,
                   const std::string& key,
                   const YAML::Node&  value,
                   bool               override,
                   std::string        parent_path)
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
                throw std::runtime_error(
                    "addNodeSchema: node[key] has any of the reserved keys but not all required keys");
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

    // relative path input case
    if (node[key].Type() == YAML::NodeType::Scalar)
    {
        std::string value_str = node[key].as<std::string>();
        if ((value_str.size() > 1 and value_str.substr(0, 2) == "./") or
            (value_str.size() > 2 and value_str.substr(0, 3) == "../"))
        {
            filesystem::path path_value = filesystem::path(parent_path) / filesystem::path(value.as<std::string>());
            node[key]                   = path_value.string();
        }
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
    return type.size() > 2 and type.substr(type.size() - 2) == "[]";
}

std::string getTypeOfSequence(const YAML::Node& node_schema)
{
    if (not node_schema[TYPE])
    {
        throw std::runtime_error("getTypeOfSequence: the node does not have the key " + TYPE);
    }
    if (not isSequenceSchema(node_schema))
    {
        throw std::runtime_error("getTypeOfSequence: the node type is not sequence: " +
                                 node_schema[TYPE].as<std::string>());
    }
    auto type_seq = node_schema[TYPE].as<std::string>();
    return type_seq.substr(0, type_seq.size() - 2);
}

}  // namespace yaml_schema_cpp
