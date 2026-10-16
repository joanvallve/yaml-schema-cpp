/**
 * @file yaml_schema.hpp
 * @brief Loading and checking of schema files, and validation of input YAML nodes against them.
 *
 * A schema is a YAML file (with extension SCHEMA_EXTENSION) that specifies the structure and contents of input YAML
 * files. Nodes containing all REQUIRED_KEYS are "specifications" of an input field, the rest of the nodes are maps of
 * other nodes. See the README for a description of the schema syntax.
 */
#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

/**
 * @brief Specification and validation of YAML files via schema files.
 */
namespace yaml_schema_cpp
{
/// Extension of schema files
static const std::string SCHEMA_EXTENSION = ".schema";
/// Extension of input YAML files
static const std::string YAML_EXTENSION = ".yaml";
/// Prefix of all keys reserved for specifications
static const std::string SCHEMA_PREFIX = "_";
/// Specification key (required): type of the field
static const std::string TYPE = SCHEMA_PREFIX + "type";
/// Specification key (required): if the field is mandatory, bool or expression (see expression.hpp)
static const std::string MANDATORY = SCHEMA_PREFIX + "mandatory";
/// Specification key (required): documentation of the field
static const std::string DOC = SCHEMA_PREFIX + "doc";
/// Specification key (optional): sequence of accepted values
static const std::string OPTIONS = SCHEMA_PREFIX + "options";
/// Specification key (optional): value added if the field is missing (only if not mandatory)
static const std::string DEFAULT = SCHEMA_PREFIX + "default";
/// Specification key (optional): value fixed by the schema (the field cannot be changed in the input)
static const std::string VALUE = SCHEMA_PREFIX + "value";
/// Specification key (required for "derived" types): base schema that the derived type should follow
static const std::string BASE = SCHEMA_PREFIX + "base";
/// Keys reserved for specifications (not allowed in nodes that are not specifications)
static const std::list<std::string> RESERVED_KEYS{TYPE, MANDATORY, DOC, OPTIONS, DEFAULT, VALUE, BASE};
/// Keys required for a node to be a specification
static const std::list<std::string> REQUIRED_KEYS{TYPE, MANDATORY, DOC};

/**
 * @brief Find, load, flatten and check a schema.
 *
 * The schema file is searched recursively in folders_schema (see findSchema()). Then, all 'follow' keys are resolved
 * (see flattenNode()) and the resulting schema is checked (see checkSchema()).
 *
 * @param name_schema name of the schema file, with or without extension (without path)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @param log OUTPUT stream where the path of the schema file and the errors (if any) are written
 * @param override if true, repeated specifications while flattening are overridden, otherwise it is an error
 * @return the flattened schema node, or an undefined node if any of the steps failed
 */
YAML::Node loadSchema(std::string                     name_schema,
                      const std::vector<std::string>& folders_schema,
                      std::stringstream&              log,
                      bool                            override = true);

/**
 * @brief Check recursively that a (flattened) schema node is valid.
 *
 * For each specification node, it checks that:
 *  - TYPE, DOC and BASE (required if TYPE is "derived" or "derived[..]") are strings,
 *  - MANDATORY is a bool or a valid expression (see checkExpression()),
 *  - VALUE, DEFAULT and OPTIONS (if defined) are valid (see checkSchemaValue(), checkSchemaDefault() and
 *    checkSchemaOptions()).
 *
 * Nodes that are not specifications should be maps without any of the RESERVED_KEYS. Scalar and null nodes are
 * skipped.
 *
 * @param node_schema schema node to be checked
 * @param field name of the field (used in error messages)
 * @param node_schema_parent parent node of node_schema (used to check expressions)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @throws std::runtime_error if the schema is not valid
 */
void checkSchema(const YAML::Node&               node_schema,
                 const std::string&              field,
                 const YAML::Node&               node_schema_parent,
                 const std::vector<std::string>& folders_schema);

/**
 * @brief Check the VALUE of a specification node.
 *
 * VALUE should pass applySchema() for the TYPE (or BASE for "derived" types, see getCheckType()), and MANDATORY should
 * be false. NOTE: applySchema() may complete VALUE with the defaults of its type.
 *
 * @param node_schema specification node containing VALUE
 * @param node_field name of the field (used in error messages)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @throws std::runtime_error if VALUE is not valid
 */
void checkSchemaValue(const YAML::Node&               node_schema,
                      const std::string&              node_field,
                      const std::vector<std::string>& folders_schema);

/**
 * @brief Check the DEFAULT of a specification node.
 *
 * MANDATORY should not be true, and DEFAULT should pass applySchema() for the TYPE (or BASE for "derived" types, see
 * getCheckType()). NOTE: applySchema() may complete DEFAULT with the defaults of its type.
 *
 * @param node_schema specification node containing DEFAULT
 * @param node_field name of the field (used in error messages)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @throws std::runtime_error if DEFAULT is not valid
 */
void checkSchemaDefault(const YAML::Node&               node_schema,
                        const std::string&              node_field,
                        const std::vector<std::string>& folders_schema);

/**
 * @brief Check the OPTIONS of a specification node.
 *
 * OPTIONS should be a sequence, each option should pass applySchema() for the TYPE (or BASE for "derived" types, see
 * getCheckType()), and DEFAULT and VALUE (if defined) should be one of the options.
 *
 * @param node_schema specification node containing OPTIONS
 * @param node_field name of the field (used in error messages)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @throws std::runtime_error if OPTIONS is not valid
 */
void checkSchemaOptions(const YAML::Node&               node_schema,
                        const std::string&              node_field,
                        const std::vector<std::string>& folders_schema);

/**
 * @brief Load, flatten and check all schema files found (recursively) in the given folders.
 *
 * The result of each schema is printed in std::cout: errors are always printed, the rest only if verbose.
 * Folders that do not exist are skipped.
 *
 * @param folders_schema folders where the schema files are searched (recursively), also used to resolve 'follow'
 * @param verbose if true, prints the name of each schema file and "OK!" for the valid ones
 * @param override if true, repeated specifications while flattening are overridden, otherwise it is an error
 * @return true if all schemas are valid
 */
bool validateAllSchemas(const std::vector<std::string>& folders_schema, bool verbose, bool override = true);

/**
 * @brief Validate an input node against a type and complete it.
 *
 * The type can be:
 *  - a trivial type (see isTrivialType()), checked with tryNodeAs(). Null values are not accepted for string types.
 *  - an array type (e.g. "int[]", "MyType[3]"): the input should be a sequence (of the given size, if specified) and
 *    each element is validated against the lower element type (see getLowerElementType()).
 *  - otherwise, the name of a schema: it is loaded (see loadSchema()) and applied (see applySchemaRecursive()).
 *
 * Applying a schema completes the input node: missing fields with DEFAULT (and not mandatory) or VALUE are added.
 *
 * @param node_input input node to be validated (and completed)
 * @param type type of node_input: trivial type, array type or name of a schema
 * @param folders folders where the schema files are searched (recursively)
 * @param log OUTPUT stream where the errors are written
 * @param acc_field accumulated path of the field (e.g. "map1/param2[0]"), used in error messages ("" at top level)
 * @param override if true, repeated specifications while flattening schemas are overridden, otherwise it is an error
 * @return true if node_input is valid
 */
bool applySchema(YAML::Node&                     node_input,
                 const std::string&              type,
                 const std::vector<std::string>& folders,
                 std::stringstream&              log,
                 const std::string&              acc_field,
                 bool                            override = true);

/**
 * @brief Validate an input node against a schema node and complete it (recursively).
 *
 * If node_schema is a specification:
 *  - if node_input is defined, it is validated against TYPE (see applySchema() and applySchemaDerived()). If valid,
 *    it should be equal to VALUE and be one of the OPTIONS (if defined).
 *  - otherwise: if VALUE is defined, node_input is set to VALUE. If not, an error is reported if MANDATORY (bool or
 *    expression evaluated with evalExpression()) is true, or node_input is set to DEFAULT (if defined).
 *
 * Otherwise (map of other schema nodes), node_input should be a map (it is created if not defined) and each child is
 * validated recursively. The children with an expression in MANDATORY are processed after the rest, so that the
 * values and defaults of the parameters involved in the expressions are already set.
 *
 * Scalar schema nodes are ignored.
 *
 * @param node_input input node to be validated. If not defined, it should be obtained via node_input_parent[key], so
 *        that setting it (VALUE, DEFAULT or empty map) adds the key to node_input_parent.
 * @param node_input_parent parent node of node_input (used to evaluate expressions)
 * @param node_schema schema node: either a specification or a map of other schema nodes
 * @param folders folders where the schema files are searched (recursively)
 * @param log OUTPUT stream where the errors are written
 * @param acc_field accumulated path of the field (e.g. "map1/param2[0]"), used in error messages
 * @param override if true, repeated specifications while flattening schemas are overridden, otherwise it is an error
 * @param node_schema_parent parent node of node_schema (used to get the types of the parameters in expressions).
 *        Undefined if not available.
 * @return true if node_input is valid
 */
bool applySchemaRecursive(YAML::Node&                     node_input,
                          YAML::Node&                     node_input_parent,
                          const YAML::Node&               node_schema,
                          const std::vector<std::string>& folders,
                          std::stringstream&              log,
                          const std::string&              acc_field,
                          bool                            override,
                          const YAML::Node&               node_schema_parent = YAML::Node(YAML::NodeType::Undefined));

/**
 * @brief Validate an input node against a "derived" type specification and complete it.
 *
 * If TYPE is an array ("derived[]", "derived[3]", ...), node_input should be a sequence (of the given size, if
 * specified) and each element is validated recursively. Otherwise, node_input should be a map containing the key
 * 'type' (string) with the name of the derived schema. It is validated against the derived schema and then against
 * the BASE schema.
 *
 * @param node_input input node to be validated (and completed)
 * @param node_input_parent parent node of node_input
 * @param node_schema specification node with a "derived" TYPE and BASE
 * @param folders folders where the schema files are searched (recursively)
 * @param log OUTPUT stream where the errors are written
 * @param acc_field accumulated path of the field (e.g. "map1/param2[0]"), used in error messages
 * @param override if true, repeated specifications while flattening schemas are overridden, otherwise it is an error
 * @return true if node_input is valid
 */
bool applySchemaDerived(YAML::Node&                     node_input,
                        YAML::Node&                     node_input_parent,
                        const YAML::Node&               node_schema,
                        const std::vector<std::string>& folders,
                        std::stringstream&              log,
                        const std::string&              acc_field,
                        bool                            override);

/**
 * @brief Check if an input node is equal to any of the options (see compare()).
 *
 * @param input_node input node
 * @param options_node sequence of options
 * @param type type used to compare the nodes
 * @param folders_schema folders where the schema files are searched (recursively)
 * @return true if input_node is equal to any of the options
 */
bool isInOptions(const YAML::Node&               input_node,
                 const YAML::Node&               options_node,
                 const std::string&              type,
                 const std::vector<std::string>& folders_schema = {});

/**
 * @brief Check if a schema node has any of the RESERVED_KEYS.
 * @param node_schema schema node
 * @return true if node_schema is a map containing any of the RESERVED_KEYS
 */
bool hasAnyReservedKey(const YAML::Node& node_schema);

/**
 * @brief Check if a schema node is a specification.
 * @param node_schema schema node
 * @return true if node_schema is a map containing all REQUIRED_KEYS
 */
bool isSpecification(const YAML::Node& node_schema);

/**
 * @brief Check if a specification node has an array type (see isArrayType()).
 * @param node_schema specification node
 * @return true if TYPE is an array type
 * @throws std::runtime_error if node_schema does not have TYPE
 */
bool isSequenceSchema(const YAML::Node& node_schema);

/**
 * @brief Check if a specification node has an array type (see isArrayType()).
 * @param node_schema specification node
 * @param size OUTPUT (if array type): size of the array, 0 if not specified
 * @return true if TYPE is an array type
 * @throws std::runtime_error if node_schema does not have TYPE
 */
bool isSequenceSchema(const YAML::Node& node_schema, size_t& size);

/**
 * @brief Add a schema node to node[key], merging it with the existing one (used to flatten schemas).
 *
 *  - if node[key] does not exist, it is set to value.
 *  - if node[key] is a specification or a scalar or a sequence, it is replaced by value if override, otherwise an
 *    exception is thrown.
 *  - otherwise (map), all children of value are added recursively.
 *
 * If parent_path is not empty and the resulting node[key] is a relative path (see isRelativePath()), parent_path is
 * prepended.
 *
 * @param node schema node where value is added
 * @param key key in node where value is added
 * @param value schema node to be added
 * @param override if true, existing specifications are overridden, otherwise it is an error
 * @param parent_path path prepended to relative path values ("" to leave them unchanged)
 * @throws std::runtime_error if a specification exists and not override, or node[key] has reserved keys but is not
 *         a specification
 */
void addNodeSchema(YAML::Node&        node,
                   const std::string& key,
                   const YAML::Node&  value,
                   bool               override,
                   std::string        parent_path = "");

}  // namespace yaml_schema_cpp
