/**
 * @file yaml_server.hpp
 * @brief YamlServer: main interface to load input YAML files and validate them against schemas.
 */
#pragma once

#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{

/**
 * @brief Loads an input YAML (file or node) and validates and completes it against schemas.
 *
 * Example:
 * @code
 * YamlServer server({"/path/to/schemas"}, "/path/to/input.yaml");
 * if (not server.applySchema("MySchema"))
 *     std::cout << server.getLog() << std::endl;
 * YAML::Node node = server.getNode();
 * @endcode
 */
class YamlServer
{
  public:
    /**
     * @brief Constructor without schema folders and without input.
     * @param override if true, repeated keys while flattening (input and schemas) are overridden, otherwise it is an
     *        error
     */
    YamlServer(bool override = true);

    /**
     * @brief Constructor without input.
     * @param folders_schema folders where the schema files are searched (recursively)
     * @param override if true, repeated keys while flattening (input and schemas) are overridden, otherwise it is an
     *        error
     */
    YamlServer(const std::vector<std::string>& folders_schema, bool override = true);

    /**
     * @brief Constructor loading an input YAML file (see loadYaml()).
     * @param folders_schema folders where the schema files are searched (recursively)
     * @param path_input path of the input YAML file
     * @param override if true, repeated keys while flattening (input and schemas) are overridden, otherwise it is an
     *        error
     * @throws std::runtime_error if the file does not exist or cannot be flattened
     */
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input, bool override = true);

    /**
     * @brief Constructor loading an input YAML file (see loadYaml()).
     *
     * Overload for string literals (otherwise, they would be converted to bool, selecting the constructor without
     * input).
     *
     * @param folders_schema folders where the schema files are searched (recursively)
     * @param path_input path of the input YAML file
     * @param override if true, repeated keys while flattening (input and schemas) are overridden, otherwise it is an
     *        error
     * @throws std::runtime_error if the file does not exist or cannot be flattened
     */
    YamlServer(const std::vector<std::string>& folders_schema, const char* path_input, bool override = true);

    /**
     * @brief Validate and complete the input node against a schema (see yaml_schema_cpp::applySchema()).
     *
     * Missing optional fields with default values and fields with values fixed in the schema are added to the input
     * node. Null values are not accepted for string fields. The log is reset and filled with the errors found.
     *
     * @param name_schema name of the schema, with or without extension (without path)
     * @return true if the input node is valid. Otherwise, the errors can be retrieved with getLog().
     */
    bool applySchema(const std::string& name_schema);

    /**
     * @brief Add folders where the schema files are searched (recursively).
     * @param folders_schema folders to be added
     * @param before if true, added at the beginning (searched first), otherwise at the end
     */
    void addFolderSchema(const std::vector<std::string>& folders_schema, bool before = false);

    /**
     * @brief Add a folder where the schema files are searched (recursively).
     * @param folder_schema folder to be added
     * @param before if true, added at the beginning (searched first), otherwise at the end
     */
    void addFolderSchema(const std::string& folder_schema, bool before = false);

    /**
     * @brief Get the folders where the schema files are searched.
     * @return the schema folders, in search order
     */
    std::vector<std::string> getFolderSchema() const;

    /**
     * @brief Load an input YAML file.
     *
     * The 'follow' keys are resolved, and relative paths (values starting with "./" or "../") are prepended with the
     * folder of the file where they are defined (see flattenNode()).
     *
     * @param path_input path of the input YAML file (absolute or relative to the current working directory)
     * @throws std::runtime_error if the file does not exist or cannot be flattened (YAML::Exception if it cannot be
     *         parsed)
     */
    void loadYaml(const std::string& path_input);

    /**
     * @brief Set the input node (a copy of it).
     *
     * NOTE: Unlike loadYaml(), the 'follow' keys and relative paths are not resolved.
     *
     * @param _node_input input node
     */
    void setYaml(const YAML::Node _node_input);

    /**
     * @brief Get the log of the last call to applySchema().
     * @return the log, including the errors found (if any)
     */
    std::string getLog() const;

    /**
     * @brief Get the input node (completed by applySchema(), if called).
     * @return a copy of the input node
     */
    YAML::Node getNode() const;

  private:
    std::vector<std::string> folders_schema_;  ///< folders where the schema files are searched (recursively)

    std::string path_input_;  ///< path of the input YAML file (empty if set via setYaml())

    std::stringstream log_;  ///< log of the last call to applySchema()

    YAML::Node node_input_;  ///< input node

    bool override_;  ///< if true, repeated keys while flattening are overridden, otherwise it is an error
};

}  // namespace yaml_schema_cpp
