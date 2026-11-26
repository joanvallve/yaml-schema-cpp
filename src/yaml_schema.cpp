#include <stdexcept>
#include <cassert>

#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/expression.hpp"

namespace yaml_schema_cpp
{

namespace
{
// applySchema() without the unknown-key check: used by every internal validation, so the check runs once, from the
// top-level call, on the input as written.
bool applySchemaWithoutKeyCheck(YAML::Node&                     node_input,
                                const std::string&              type,
                                const std::vector<std::string>& folders,
                                std::stringstream&              log,
                                const std::string&              acc_field,
                                bool                            override = true);
}  // namespace

YAML::Node loadSchema(std::string                     name_schema,
                      const std::vector<std::string>& folders_schema,
                      std::stringstream&              log,
                      bool                            override)
{
    // Find schema file + check extension
    std::stringstream log_find_schema;
    auto              path_schema = findSchema(name_schema, folders_schema, log_find_schema);
    if (path_schema.empty())
    {
        log << "ERROR in loadSchema(): findSchema() failed with error: " << log_find_schema.str() << "\n";
        return YAML::Node(YAML::NodeType::Undefined);
    }

    // write schema file in log
    log << "schema file found: " << path_schema << std::endl;

    // Load schema yaml
    YAML::Node node_schema;
    try
    {
        node_schema = YAML::LoadFile(path_schema);
    }
    catch (const std::exception& e)
    {
        log << "ERROR in loadSchema(): Couldn't load the schema yaml file " + path_schema + ". Error: " + e.what()
            << "\n";
        return YAML::Node(YAML::NodeType::Undefined);
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information

    try
    {
        flattenNode(node_schema, filesystem::path(path_schema).parent_path().string(), folders_schema, true, override);
    }
    catch (const std::exception& e)
    {
        log << "ERROR in loadSchema(): Couldn't flatten schema file " + path_schema + ". Error: " + e.what() << "\n";

        return YAML::Node(YAML::NodeType::Undefined);
    }

    // Check schema
    try
    {
        checkSchema(node_schema, "", node_schema, folders_schema);
    }
    catch (const std::exception& e)
    {
        log << "ERROR in loadSchema(): The schema file " + path_schema + " is not valid. Error: " + e.what() << "\n";
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

    // specifications node
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

        // If TYPE=="derived" or "derived[]" or "derived[][]"... 'BASE' of type string is required
        if (isDerivedType(node_schema[TYPE].as<std::string>()))
        {
            if (not node_schema[BASE])
            {
                throw std::runtime_error("YAML schema: " + node_field + " of derived type does not contain " + BASE);
            }
            if (not tryNodeAs(node_schema[BASE], "string"))
            {
                throw std::runtime_error("YAML schema: In " + node_field + ", " + BASE + " should be a string");
            }
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

        // check value (optional)
        if (node_schema[VALUE]) checkSchemaValue(node_schema, node_field, folders_schema);

        // check default (optional)
        if (node_schema[DEFAULT]) checkSchemaDefault(node_schema, node_field, folders_schema);

        // check options (optional)
        if (node_schema[OPTIONS]) checkSchemaOptions(node_schema, node_field, folders_schema);

        // check range (optional)
        if (node_schema[MIN] or node_schema[MAX]) checkSchemaRange(node_schema, node_field);
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
        // check the children schema nodes
        for (auto node_schema_child : node_schema)
        {
            checkSchema(
                node_schema_child.second, node_schema_child.first.as<std::string>(), node_schema, folders_schema);
        }
    }
}

void checkSchemaValue(const YAML::Node&               node_schema,
                      const std::string&              node_field,
                      const std::vector<std::string>& folders_schema)
{
    // Check VALUE follows the corresponding schema
    std::string       type = getCheckType(node_schema);  // If derived type take BASE
    std::stringstream log;
    YAML::Node        node_schema_value = node_schema[VALUE];
    if (not applySchemaWithoutKeyCheck(node_schema_value, type, folders_schema, log, ""))
    {
        throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE +
                                 " did not pass the schema check with error: " + log.str());
    }

    // if 'value', 'mandatory' should be false (no sense requiring user to define something already
    // defined)
    if (isExpression(node_schema[MANDATORY]) or node_schema[MANDATORY].as<bool>())
    {
        throw std::runtime_error("YAML schema: " + node_field + ", if " + VALUE + " defined, " + MANDATORY +
                                 " should be false");
    }
}

void checkSchemaDefault(const YAML::Node&               node_schema,
                        const std::string&              node_field,
                        const std::vector<std::string>& folders_schema)
{
    // Check not mandatory
    if (not isExpression(node_schema[MANDATORY]) and node_schema[MANDATORY].as<bool>())
    {
        throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT +
                                 " value cannot be defined in mandatory field");
    }

    // Check default against corresponding schema
    std::string       type = getCheckType(node_schema);  // If derived type take BASE
    std::stringstream log;
    YAML::Node        node_schema_default = node_schema[DEFAULT];
    if (not applySchemaWithoutKeyCheck(node_schema_default, type, folders_schema, log, ""))
    {
        throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT +
                                 " did not pass the schema check with error: " + log.str());
    }
}

void checkSchemaOptions(const YAML::Node&               node_schema,
                        const std::string&              node_field,
                        const std::vector<std::string>& folders_schema)
{
    // Check that it is a sequence
    if (not node_schema[OPTIONS].IsSequence())
    {
        throw std::runtime_error("YAML schema: " + node_field + ", " + OPTIONS + " should be a sequence");
    }

    // Check options against corresponding schema (if "derived" --> BASE)
    std::string type = getCheckType(node_schema);  // If derived type take BASE
    for (size_t n_i = 0; n_i < node_schema[OPTIONS].size(); n_i++)
    {
        std::stringstream log;
        YAML::Node        node_schema_option_i = node_schema[OPTIONS][n_i];
        if (not applySchemaWithoutKeyCheck(node_schema_option_i, type, folders_schema, log, ""))
        {
            throw std::runtime_error("YAML schema: " + node_field + ", " + OPTIONS + "[" + std::to_string(n_i) +
                                     "] did not pass the schema check with error: " + log.str());
        }
    }

    // If default, check it is included in options
    if (node_schema[DEFAULT])
    {
        if (not isInOptions(node_schema[DEFAULT], node_schema[OPTIONS], type, folders_schema))
        {
            throw std::runtime_error("YAML schema: " + node_field + ", " + DEFAULT +
                                     " value should be one of the specified in " + OPTIONS);
        }
    }
    // If value, check it is included in options
    if (node_schema[VALUE])
    {
        if (not isInOptions(node_schema[VALUE], node_schema[OPTIONS], type, folders_schema))
        {
            throw std::runtime_error("YAML schema: " + node_field + ", " + VALUE +
                                     " value should be one of the specified in " + OPTIONS);
        }
    }
}

namespace
{
// Range comparisons are done in the declared type: yaml-cpp reads integers with base detection (hex, octal), which a
// double conversion does not follow, and a double cannot hold every 64-bit integer.
enum class RangeKind
{
    SIGNED,
    UNSIGNED,
    FLOATING
};

RangeKind getRangeKind(const std::string& type)
{
    if (type == "unsigned int" or type == "long unsigned int") return RangeKind::UNSIGNED;
    if (type == "float" or type == "double") return RangeKind::FLOATING;
    return RangeKind::SIGNED;
}

// true if value is outside [MIN, MAX] (NaN is outside any range)
template <typename T>
bool isOutOfRange(const YAML::Node& node_value, const YAML::Node& node_schema)
{
    T value = node_value.as<T>();
    if (value != value) return true;
    return (node_schema[MIN] and value < node_schema[MIN].as<T>()) or
           (node_schema[MAX] and value > node_schema[MAX].as<T>());
}

bool isOutOfRange(const YAML::Node& node_value, const YAML::Node& node_schema, RangeKind kind)
{
    switch (kind)
    {
        case RangeKind::SIGNED:
            return isOutOfRange<long long>(node_value, node_schema);
        case RangeKind::UNSIGNED:
            return isOutOfRange<unsigned long long>(node_value, node_schema);
        case RangeKind::FLOATING:
            return isOutOfRange<double>(node_value, node_schema);
    }
    return false;
}
}  // namespace

void checkSchemaRange(const YAML::Node& node_schema, const std::string& node_field)
{
    // only numeric types (or arrays of them)
    auto type = node_schema[TYPE].as<std::string>();
    if (isDerivedType(type) or not isNumericType(getLowestElementType(type)))
    {
        throw std::runtime_error("YAML schema: " + node_field + ", " + MIN + " and " + MAX +
                                 " are only allowed for numeric types, not " + type);
    }

    // bounds should be values of the element type (not NaN)
    auto element_type = getLowestElementType(type);
    for (auto bound : {MIN, MAX})
    {
        if (not node_schema[bound]) continue;
        if (not node_schema[bound].IsScalar() or not tryNodeAs(node_schema[bound], element_type))
        {
            throw std::runtime_error("YAML schema: " + node_field + ", " + bound + " should be a " + element_type);
        }
        if (getRangeKind(element_type) == RangeKind::FLOATING and
            node_schema[bound].as<double>() != node_schema[bound].as<double>())
        {
            throw std::runtime_error("YAML schema: " + node_field + ", " + bound + " should be a number, not NaN");
        }
    }

    // MIN not greater than MAX
    if (node_schema[MIN] and node_schema[MAX] and
        isOutOfRange(node_schema[MIN], node_schema, getRangeKind(element_type)))
    {
        throw std::runtime_error("YAML schema: " + node_field + ", " + MIN + " is greater than " + MAX);
    }

    // DEFAULT and VALUE inside the range (their type was already checked)
    for (auto key : {DEFAULT, VALUE})
    {
        std::stringstream log;
        if (node_schema[key] and not checkRange(node_schema[key], node_schema, log, node_field))
        {
            throw std::runtime_error("YAML schema: " + node_field + ", " + key + " out of range: " + log.str());
        }
    }
}

bool checkRange(const YAML::Node&  node_input,
                const YAML::Node&  node_schema,
                std::stringstream& log,
                const std::string& acc_field)
{
    // sequence: check all elements
    if (node_input.IsSequence())
    {
        bool is_valid = true;
        for (size_t i = 0; i < node_input.size(); i++)
        {
            is_valid =
                checkRange(node_input[i], node_schema, log, acc_field + "[" + std::to_string(i) + "]") and is_valid;
        }
        return is_valid;
    }

    auto kind = getRangeKind(getLowestElementType(node_schema[TYPE].as<std::string>()));
    if (isOutOfRange(node_input, node_schema, kind))
    {
        writeErrorToLog(log,
                        acc_field,
                        node_schema,
                        "Value " + node_input.Scalar() + " out of range [" +
                            (node_schema[MIN] ? node_schema[MIN].Scalar() : "-inf") + ", " +
                            (node_schema[MAX] ? node_schema[MAX].Scalar() : "inf") + "]");
        return false;
    }
    return true;
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
                if (filesystem::is_regular_file(entry) and filesystem::path(entry).extension() == SCHEMA_EXTENSION)
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

namespace
{
// Unknown-key check (see applySchema()'s error_on_unknown_keys). It walks the input as the user wrote it, before
// applySchema() completes it, so keys added from DEFAULT or VALUE are not reported. Type errors are reported by the
// validation, so nodes of the wrong kind and schemas that fail to load are skipped here.
//
// A key may be governed by several schema nodes at once: a "derived" type is validated against its own schema and its
// BASE, which may both define the same key. The walk therefore carries every schema node that applies, and a key is
// known if any of them lists it.
struct SchemaRef
{
    YAML::Node  node;  // specification or map of schema nodes
    std::string name;  // schema named in the error
};

// Writes each unknown key as an error (as_error) or a warning; returns true if there is none.
bool checkUnknownKeys(const YAML::Node&               node_input,
                      const std::vector<SchemaRef>&   refs,
                      const std::vector<std::string>& folders,
                      std::stringstream&              log,
                      const std::string&              acc_field,
                      bool                            override,
                      bool                            as_error)
{
    auto report = [&](const std::string& field, const std::string& message) {
        if (as_error)
            writeErrorToLog(log, field, YAML::Node(YAML::NodeType::Undefined), message);
        else
            writeWarningToLog(log, field, message);
    };

    if (not node_input.IsDefined()) return true;

    // resolve specifications into the schema maps (or element specifications) they stand for
    std::vector<SchemaRef> maps, elements;
    bool                   allow_type_key = false;
    for (const auto& ref : refs)
    {
        // scalar schema nodes accept any content (as in applySchemaRecursive())
        if (ref.node.IsScalar()) return true;

        if (not isSpecification(ref.node))
        {
            maps.push_back(ref);
            continue;
        }

        auto   type = ref.node[TYPE].as<std::string>();
        size_t size;
        if (isArrayType(type, size))
        {
            YAML::Node node_element = Clone(ref.node);
            node_element[TYPE]      = getLowerElementType(type);
            elements.push_back({node_element, ref.name});
        }
        else if (isDerivedType(type))
        {
            if (not node_input.IsMap() or not node_input["type"] or not tryNodeAs(node_input["type"], "string"))
                continue;
            std::stringstream log_load;
            auto              name_derived = node_input["type"].as<std::string>();
            auto              name_base    = ref.node[BASE].as<std::string>();
            YAML::Node        node_derived = loadSchema(name_derived, folders, log_load, override);
            YAML::Node        node_base    = loadSchema(name_base, folders, log_load, override);
            if (node_derived.IsDefined()) maps.push_back({node_derived, name_derived});
            if (node_base.IsDefined()) maps.push_back({node_base, name_derived});
            allow_type_key = true;
        }
        else if (not isTrivialType(type))
        {
            std::stringstream log_load;
            YAML::Node        node_schema = loadSchema(type, folders, log_load, override);
            if (node_schema.IsDefined()) maps.push_back({node_schema, type});
        }
    }

    bool is_valid = true;

    // sequence: each element against the element specifications
    if (node_input.IsSequence() and not elements.empty())
    {
        for (size_t i = 0; i < node_input.size(); i++)
        {
            is_valid = checkUnknownKeys(node_input[i],
                                        elements,
                                        folders,
                                        log,
                                        acc_field + "[" + std::to_string(i) + "]",
                                        override,
                                        as_error) and
                       is_valid;
        }
        return is_valid;
    }

    // map: every key should be listed by one of the schema maps
    if (not node_input.IsMap() or maps.empty()) return true;

    for (auto node_input_child : node_input)
    {
        if (not node_input_child.first.IsScalar())
        {
            report(acc_field, "Unknown key, not a scalar (not in schema '" + maps.front().name + "')");
            is_valid = false;
            continue;
        }

        auto key       = node_input_child.first.Scalar();
        auto acc_child = (acc_field.empty() ? "" : acc_field + "/") + key;
        if (allow_type_key and key == "type") continue;

        std::vector<SchemaRef> refs_child;
        for (const auto& map : maps)
        {
            if (map.node[key]) refs_child.push_back({map.node[key], map.name});
        }

        if (refs_child.empty())
        {
            report(acc_child, "Unknown key, not in schema '" + maps.front().name + "'");
            is_valid = false;
            continue;
        }

        is_valid =
            checkUnknownKeys(node_input_child.second, refs_child, folders, log, acc_child, override, as_error) and
            is_valid;
    }
    return is_valid;
}
}  // namespace

bool applySchema(YAML::Node&                     node_input,
                 const std::string&              type,
                 const std::vector<std::string>& folders,
                 std::stringstream&              log,
                 const std::string&              acc_field,
                 bool                            override,
                 bool                            error_on_unknown_keys)
{
    // check the input as written: the validation adds DEFAULT and VALUE keys, which are the schema's own
    YAML::Node node_written = Clone(node_input);
    bool       is_valid     = applySchemaWithoutKeyCheck(node_input, type, folders, log, acc_field, override);

    YAML::Node node_type_spec;
    node_type_spec[TYPE]      = type;
    node_type_spec[MANDATORY] = true;
    node_type_spec[DOC]       = "";
    bool keys_known           = checkUnknownKeys(
        node_written, {{node_type_spec, type}}, folders, log, acc_field, override, error_on_unknown_keys);

    return is_valid and (keys_known or not error_on_unknown_keys);
}

namespace
{
bool applySchemaWithoutKeyCheck(YAML::Node&                     node_input,
                                const std::string&              type,
                                const std::vector<std::string>& folders,
                                std::stringstream&              log,
                                const std::string&              acc_field,
                                bool                            override)
{
    // Array type --> recursive call to applySchema
    size_t size;
    if (isArrayType(type, size))
    {
        // If node not sequence complain
        if (not node_input.IsSequence())
        {
            writeErrorToLog(
                log, acc_field, YAML::Node(YAML::NodeType::Undefined), "Wrong type, should be a sequence: " + type);
            return false;
        }

        bool is_valid = true;

        // If size defined in type (!=0), complain if different
        if (size != 0 and node_input.size() != size)
        {
            writeErrorToLog(log,
                            acc_field,
                            YAML::Node(YAML::NodeType::Undefined),
                            "Wrong size, should be " + std::to_string(size));
            is_valid = false;
        }

        // applySchema recursively for all nodes in sequence
        for (size_t i = 0; i < node_input.size(); i++)
        {
            YAML::Node node_input_i = node_input[i];
            is_valid                = applySchemaWithoutKeyCheck(node_input_i,
                                                  getLowerElementType(type),
                                                  folders,
                                                  log,
                                                  acc_field + "[" + std::to_string(i) + "]",
                                                  override) and
                       is_valid;
        }
        return is_valid;
    }
    // not array
    else
    {
        // If trivial type (no schema file)
        if (isTrivialType(type))
        {
            // null is not a valid string (yaml-cpp would convert it to "null")
            if (node_input.IsDefined() and node_input.IsNull() and isStringType(type))
            {
                writeErrorToLog(log,
                                acc_field,
                                YAML::Node(YAML::NodeType::Undefined),
                                "Wrong type, should be " + type + " (found null value)");
                return false;
            }
            if (not tryNodeAs(node_input, type))
            {
                writeErrorToLog(
                    log, acc_field, YAML::Node(YAML::NodeType::Undefined), "Wrong type, should be " + type);
                return false;
            }
            else
                return true;
        }
        // Non-trivial: Load and apply schema
        else
        {
            YAML::Node node_schema = loadSchema(type, folders, log, override);
            if (not node_schema.IsDefined()) return false;

            // Check node_input against node_schema
            return applySchemaRecursive(node_input, node_input, node_schema, folders, log, acc_field, override);
        }
    }
}

}  // namespace

bool applySchemaRecursive(YAML::Node&                     node_input,
                          YAML::Node&                     node_input_parent,
                          const YAML::Node&               node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream&              log,
                          const std::string&              acc_field,
                          bool                            override,
                          const YAML::Node&               node_schema_parent)
{
    // scalar schema nodes are ignored (as in checkSchema())
    if (node_schema.IsScalar()) return true;

    bool is_valid = true;

    // Param schema (has mandatory and type)
    if (isSpecification(node_schema))
    {
        // If exists, check type, VALUE & OPTIONS
        if (node_input.IsDefined())
        {
            // check type:
            bool type_valid;
            // - derived type ( "derived" or "derived[]" or "derived[][]".. )
            if (isDerivedType(node_schema[TYPE].as<std::string>()))
                type_valid =
                    applySchemaDerived(node_input, node_input_parent, node_schema, folders, log, acc_field, override);
            // - type specified (either trivial or custom), check with corresponding schema file or trivial type
            else
                type_valid = applySchemaWithoutKeyCheck(
                    node_input, node_schema[TYPE].as<std::string>(), folders, log, acc_field, override);

            is_valid = type_valid;

            // check VALUE and OPTIONS (only if passed type validation)
            if (type_valid)
            {
                // type to be compared with VALUE and OPTIONS (if derived type, BASE)
                std::string check_type = getCheckType(node_schema);

                // If VALUE defined in schema, complain if different
                if (node_schema[VALUE] and not compare(node_schema[VALUE], node_input, check_type, folders))
                {
                    writeErrorToLog(log,
                                    acc_field,
                                    node_schema,
                                    "Already defined in schema with a different value. Not allowed to be changed.");
                    is_valid = false;
                }

                // check if value is in OPTIONS
                if (node_schema[OPTIONS] and not isInOptions(node_input, node_schema[OPTIONS], check_type, folders))
                {
                    writeErrorToLog(log, acc_field, node_schema, "Wrong value. Allowed values defined in OPTIONS.");
                    is_valid = false;
                }

                // check if value is inside [MIN, MAX]
                if ((node_schema[MIN] or node_schema[MAX]) and not checkRange(node_input, node_schema, log, acc_field))
                {
                    is_valid = false;
                }
            }
        }
        // Does not exist -> add VALUE or check if MANDATORY (or add DEFAULT)
        // NOTE: node_input was obtained via node_input_parent[key], assigning it adds the key to node_input_parent
        else
        {
            // Load VALUE in case defined in schema
            if (node_schema[VALUE])
            {
                node_input = Clone(node_schema[VALUE]);
            }
            // Check if it is mandatory
            else
            {
                bool mandatory = false;
                if (isExpression(node_schema[MANDATORY]))
                {
                    try
                    {
                        mandatory = evalExpression(
                            node_schema[MANDATORY].as<std::string>(), node_input_parent, node_schema_parent);
                    }
                    catch (const std::exception& e)
                    {
                        writeErrorToLog(
                            log,
                            acc_field,
                            node_schema,
                            "Evaluating schema expression for '" + MANDATORY + "' failed with error: " + e.what());
                        return false;
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
                // add node with default value
                else if (node_schema[DEFAULT])
                {
                    node_input = Clone(node_schema[DEFAULT]);
                }
            }
        }
    }
    // map without specification
    else
    {
        // if doesn't exist, we create it. If it should have mandatory fields, it will fail later.
        if (not node_input.IsDefined())
        {
            node_input = YAML::Node();
        }
        // if exists, it should be a map (null is accepted and converted to map)
        else if (node_schema.IsMap() and not node_input.IsMap() and not node_input.IsNull())
        {
            writeErrorToLog(log, acc_field, YAML::Node(YAML::NodeType::Undefined), "Wrong type, should be a map.");
            return false;
        }

        // iterate all childs in two passes: first the ones without expression in MANDATORY, so that their DEFAULT
        // or VALUE are already added when evaluating the expressions of the rest (that may depend on them)
        for (auto expression_pass : {false, true})
        {
            for (auto node_schema_child : node_schema)
            {
                bool has_expression =
                    isSpecification(node_schema_child.second) and isExpression(node_schema_child.second[MANDATORY]);
                if (has_expression != expression_pass) continue;

                YAML::Node node_input_child = node_input[node_schema_child.first.as<std::string>()];

                is_valid = applySchemaRecursive(
                               node_input_child,
                               node_input,
                               node_schema_child.second,
                               folders,
                               log,
                               (acc_field.empty() ? "" : acc_field + "/") + node_schema_child.first.as<std::string>(),
                               override,
                               node_schema) and
                           is_valid;
            }
        }
    }

    return is_valid;
}

bool applySchemaDerived(YAML::Node&                     node_input,
                        YAML::Node&                     node_input_parent,
                        const YAML::Node&               node_schema,
                        const std::vector<std::string>& folders,
                        std::stringstream&              log,
                        const std::string&              acc_field,
                        bool                            override)
{
    bool is_valid = true;

    // array
    size_t size;
    if (isArrayType(node_schema[TYPE].as<std::string>(), size))
    {
        // If node not sequence complain
        if (not node_input.IsSequence())
        {
            writeErrorToLog(log, acc_field, node_schema, "Wrong type, should be a sequence.");
            return false;
        }
        // If size defined in type (!=0), complain if different
        if (size != 0 and node_input.size() != size)
        {
            writeErrorToLog(log,
                            acc_field,
                            YAML::Node(YAML::NodeType::Undefined),
                            "Wrong size, should be " + std::to_string(size));
            is_valid = false;
        }
        // applySchemaDerived recursively for all nodes in sequence
        YAML::Node node_schema_i = YAML::Clone(node_schema);
        node_schema_i[TYPE]      = getLowerElementType(node_schema[TYPE].as<std::string>());
        for (size_t i = 0; i < node_input.size(); i++)
        {
            YAML::Node node_input_i = node_input[i];
            is_valid                = applySchemaDerived(node_input_i,
                                          node_input_parent,
                                          node_schema_i,
                                          folders,
                                          log,
                                          acc_field + "[" + std::to_string(i) + "]",
                                          override) and
                       is_valid;
        }
        return is_valid;
    }
    else
    {
        // check it is a map containing the key 'type' of type string
        if (not node_input.IsMap() or not node_input["type"])
        {
            writeErrorToLog(log,
                            acc_field,
                            node_schema,
                            "Should be a map containing the key 'type', which is mandatory for 'derived'.");
            return false;
        }
        if (not tryNodeAs(node_input["type"], "string"))
        {
            writeErrorToLog(log, acc_field, node_schema, "The key 'type' should be a string.");
            return false;
        }

        // Validate with derived schema file
        is_valid = applySchemaWithoutKeyCheck(
                       node_input, node_input["type"].as<std::string>(), folders, log, acc_field, override) and
                   is_valid;

        // Validate with base schema file (after derived since it may complete the input node)
        is_valid = applySchemaWithoutKeyCheck(
                       node_input, node_schema[BASE].as<std::string>(), folders, log, acc_field, override) and
                   is_valid;

        return is_valid;
    }
}

bool isInOptions(const YAML::Node&               input_node,
                 const YAML::Node&               options_node,
                 const std::string&              type,
                 const std::vector<std::string>& folders_schema)
{
    for (auto option_n : options_node)
    {
        if (compare(option_n, input_node, type, folders_schema))
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
        // existing specification or leaf (scalar or sequence) --> override (or throw)
        if (isSpecification(node[key]) or node[key].IsScalar() or node[key].IsSequence())
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
        // existing map (or null) --> add all children of value
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
                addNodeSchema(
                    node_key, value_map_node.first.as<std::string>(), value_map_node.second, override, parent_path);
            }
        }
    }
    else
    {
        node[key] = value;
    }

    // relative path input case
    if (not parent_path.empty() and node[key].Type() == YAML::NodeType::Scalar)
    {
        std::string value_str = node[key].as<std::string>();
        if (isRelativePath(value_str))
        {
            node[key] = (filesystem::path(parent_path) / filesystem::path(value_str)).string();
        }
    }
}

bool isSpecification(const YAML::Node& node_schema)
{
    // only maps can be specifications
    if (not node_schema.IsDefined() or not node_schema.IsMap()) return false;

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
    // only maps can have keys
    if (not node_schema.IsDefined() or not node_schema.IsMap()) return false;

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
    return isArrayType(node_schema[TYPE].as<std::string>());
}

bool isSequenceSchema(const YAML::Node& node_schema, size_t& size)
{
    if (not node_schema[TYPE])
    {
        throw std::runtime_error("isSequenceSchema: the node does not have the key " + TYPE);
    }
    return isArrayType(node_schema[TYPE].as<std::string>(), size);
}

}  // namespace yaml_schema_cpp
