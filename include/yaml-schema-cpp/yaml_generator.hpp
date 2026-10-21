/**
 * @file yaml_generator.hpp
 * @brief Generation of template YAML files from schemas.
 */
#pragma once

#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{
const std::string red    = "\033[0;31m";  ///< ANSI escape code: red text (console output)
const std::string yellow = "\033[1;33m";  ///< ANSI escape code: yellow text (console output)
const std::string reset  = "\033[0m";     ///< ANSI escape code: reset text color (console output)

/**
 * @brief Generate a template YAML file from a schema.
 *
 * Each field of the template has a value (DEFAULT, the first of OPTIONS or a zero value) and a comment with its
 * documentation, type, options and if it is optional (see generateYaml()). Fields with VALUE are not included.
 *
 * If filepath has no extension, YAML_EXTENSION is added. If the file already exists, a suffix "_N" is added to the
 * file name to avoid overwriting it (a warning is printed).
 *
 * @param filepath path of the template file to be created
 * @param name_schema name of the schema, with or without extension (without path)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @param override if true, repeated specifications while flattening schemas are overridden, otherwise it is an error
 * @return the path of the created template file
 * @throws std::runtime_error if the folder of filepath does not exist, the schema cannot be generated (see
 *         generateYaml()) or the file cannot be created
 */
std::string generateTemplate(std::string                     filepath,
                             const std::string&              name_schema,
                             const std::vector<std::string>& folders_schema,
                             bool                            override = true);

/**
 * @brief Generate the template YAML node of a schema.
 *
 * The schema is found, loaded, flattened and checked, and converted to a YAML node (see schemaToYaml()).
 *
 * @param name_schema name of the schema, with or without extension (without path)
 * @param folders_schema folders where the schema files are searched (recursively)
 * @param override if true, repeated specifications while flattening schemas are overridden, otherwise it is an error
 * @return template node, where each scalar contains the value (as YAML text) followed by a comment
 *         (e.g. "3  # OPTIONAL - DOC some doc - TYPE int")
 * @throws std::runtime_error if the schema is not found, cannot be loaded or flattened, or is not valid
 */
YAML::Node generateYaml(std::string name_schema, const std::vector<std::string>& folders_schema, bool override = true);

/**
 * @brief Convert (recursively) a schema node into a template YAML node.
 *
 * Specifications with VALUE and scalar schema nodes are skipped (node_output is set to null). Custom types are
 * generated from their schemas, "derived" types with a placeholder 'type' and 'follow', and arrays of unspecified
 * size with 2 elements.
 *
 * @param node_schema schema node (flattened)
 * @param node_output OUTPUT template node (see generateYaml())
 * @param folders_schema folders where the schema files are searched (recursively)
 * @param override if true, repeated specifications while flattening schemas are overridden, otherwise it is an error
 * @throws std::runtime_error if a custom type schema is not found or a schema node is not valid
 */
void schemaToYaml(const YAML::Node&               node_schema,
                  YAML::Node&                     node_output,
                  const std::vector<std::string>& folders_schema,
                  bool                            override = true);

}  // namespace yaml_schema_cpp
