#include <stdexcept>

#include "yaml-schema-cpp/yaml_generator.hpp"

#include <iostream>
#include <fstream>

#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"
#include "yaml-schema-cpp/expression.hpp"

namespace yaml_schema_cpp
{
int N_SEQUENCE_OUTPUT = 2;

std::string generateTemplate(std::string                     filepath,
                             const std::string&              name_schema,
                             const std::vector<std::string>& folders_schema,
                             bool                            override)
{
    std::cout << "generateTemplate " << name_schema << std::endl;
    // Check extension
    if (filesystem::path(filepath).extension().empty())
    {
        filepath += YAML_EXTENSION;
    }
    else if (filesystem::path(filepath).extension() != YAML_EXTENSION)
    {
        std::cout << yellow
                  << "WARNING: storing template YAML file with an extension different than '.yaml': " + filepath
                  << reset << std::endl;
    }

    // Check that directory of the file exists
    filesystem::path fp = filepath;
    if (not filesystem::exists(fp.parent_path()))
        throw std::runtime_error("generateTemplateYaml: wrong file path: " + filepath);

    // WARN if file already exists?
    auto new_fp = fp;
    if (filesystem::exists(fp))
    {
        auto suffix_id = 0;
        while (filesystem::exists(new_fp))
            new_fp =
                fp.parent_path() / (fp.stem().string() + "_" + std::to_string(++suffix_id) + fp.extension().string());
        fp = new_fp;
        std::cout << yellow << "WARNING: The provided filename already exists. The template will be saved in: " << fp
                  << reset << std::endl;
    }

    // Create Template Node
    auto template_node = generateYaml(name_schema, folders_schema);

    // Modify text
    std::stringstream template_ss;
    template_ss << template_node;
    // remove " "
    std::string template_string = template_ss.str();
    template_string.erase(std::remove(template_string.begin(), template_string.end(), '"'), template_string.end());
    // change ' ' by " "
    std::replace(template_string.begin(), template_string.end(), '\'', '"');

    // Create file
    std::ofstream template_file;
    template_file = std::ofstream(fp.c_str(), std::ofstream::out);  // open log file
    if (not template_file.is_open())
        throw std::runtime_error(std::string("Failed to open the template_file: ") + fp.c_str());
    else
        template_file << template_string;

    // close file
    template_file.close();

    return fp.string();
}

YAML::Node generateYaml(std::string name_schema, const std::vector<std::string>& folders_schema, bool override)
{
    std::cout << "generateYaml " << name_schema << std::endl;

    // Find schema file + check extension
    std::stringstream log;
    auto              path_schema = findSchema(name_schema, folders_schema, log);
    if (path_schema.empty())
    {
        throw std::runtime_error("findSchema ERROR: " + log.str());
    }

    // Load schema yaml
    YAML::Node node_schema;
    try
    {
        node_schema = YAML::LoadFile(path_schema);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("generateYaml: Couldn't load the schema yaml file " + path_schema +
                                 ". Error: " + e.what());
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
    try
    {
        flattenNode(node_schema, filesystem::path(path_schema).parent_path().string(), folders_schema, true, override);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("generateYaml: Couldn't flatten the schema yaml file " + path_schema +
                                 ". Error: " + e.what());
    }

    // Check schema
    try
    {
        checkSchema(node_schema, "", node_schema, folders_schema);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("In loadSchema: The schema file " + path_schema +
                                 " is not valid. Error: " + e.what());
    }

    // Generate YAML from schema
    YAML::Node node_output;

    schemaToYaml(node_schema, node_output, folders_schema, override);

    return node_output;
}

void schemaToYaml(const YAML::Node&               node_schema,
                  YAML::Node&                     node_output,
                  const std::vector<std::string>& folders_schema,
                  bool                            override)
{
    std::cout << "schemaToYaml:\n" << node_schema << std::endl;

    // Specification (has _mandatory, _type and _doc)
    if (isSpecification(node_schema))
    {
        // If VALUE defined in schema, not needed to be added to yaml
        if (node_schema[VALUE])
        {
            node_output = YAML::Null;
            return;
        }

        // trivial type
        auto lowest_type = getLowestElementType(node_schema[TYPE].as<std::string>());
        if (isTrivialType(lowest_type))
        {
            bool sequence_value = isSequenceSchema(node_schema);
#if _EIGEN_FOUND == 1
            sequence_value = sequence_value or isEigenType(lowest_type);
#endif
            std::string value_str;

            // Mandatory string
            std::string mandatory_str = isExpression(node_schema[MANDATORY])
                                            ? "MANDATORY if " + node_schema[MANDATORY].as<std::string>() + " - "
                                            : (node_schema[MANDATORY].as<bool>() ? "" : "OPTIONAL - ");
            // If Default, fill value with default
            if (node_schema[DEFAULT])
            {
                if (sequence_value)
                    value_str = sequenceToString(node_schema[DEFAULT]);
                else
                    value_str = node_schema[DEFAULT].as<std::string>();
            }
            // If not default, but Options, fill value with first option
            else if (node_schema[OPTIONS])
            {
                if (sequence_value)
                    value_str = sequenceToString(node_schema[OPTIONS][0]);
                else
                    value_str = node_schema[OPTIONS][0].as<std::string>();
            }
            // If not default and not options, fill with zero value
            else
                value_str = getZeroString(node_schema[TYPE].as<std::string>());

            // put ' ' envolving the value if type string
            if (isStringType(node_schema[TYPE].as<std::string>())) value_str = "'" + value_str + "'";

            // FILL NODE
            node_output = value_str + "  # " + mandatory_str + "DOC " + node_schema[DOC].as<std::string>() +
                          " - TYPE " + node_schema[TYPE].as<std::string>() +
                          (node_schema[OPTIONS] ? " - OPTIONS " + sequenceToString(node_schema[OPTIONS]) : "");
        }
        // custom non trivial-type or Derived type
        else
        {
            // scalar
            size_t seq_size;
            if (not isSequenceSchema(node_schema, seq_size))
            {
                if (lowest_type == "derived")
                {
                    node_output["type"] =
                        "'DerivedType'  # DOC String corresponding to the name of the object class (and its schema "
                        "file). Should be a class derived from base class: " +
                        node_schema[BASE].as<std::string>();
                    node_output["follow"] = "some/path/to/derived/type/parameters.yaml";
                }
                else if (isNonTrivialType(lowest_type, folders_schema))
                {
                    // find trivial-type schema and apply
                    node_output = generateYaml(node_schema[TYPE].as<std::string>(), folders_schema, override);
                }
                // non trivial type also failed
                else
                {
                    throw std::runtime_error("Not trivial type not found: " + node_schema[TYPE].as<std::string>());
                }
            }
            // array
            else
            {
                YAML::Node node_schema_i = Clone(node_schema);
                if (seq_size == 0) seq_size = N_SEQUENCE_OUTPUT;
                node_schema_i[TYPE] = getLowerElementType(node_schema[TYPE].as<std::string>());

                for (auto i = 0; i < seq_size; i++)
                {
                    // if VALUE --> copy corresponding
                    if (node_schema[VALUE]) node_schema_i[VALUE] = node_schema[VALUE][i];

                    // if OPTIONS --> copy corresponding
                    if (node_schema[OPTIONS]) node_schema_i[OPTIONS][0] = node_schema[OPTIONS][0][i];

                    YAML::Node node_output_i;
                    schemaToYaml(node_schema_i, node_output_i, folders_schema, override);

                    // add node if it was created (if VALUE defined, it wasn't)
                    if (not node_output_i.IsNull()) node_output.push_back(node_output_i);
                }
            }
        }
    }
    // not specification (should be a map)
    else
    {
        if (not node_schema.IsMap()) throw std::runtime_error("not specification schema node should be map.");

        // iterate all childs
        for (auto node_schema_child : node_schema)
        {
            YAML::Node node_output_child;
            schemaToYaml(node_schema_child.second, node_output_child, folders_schema, override);

            // add node if it was created (if VALUE defined, it wasn't)
            if (not node_output_child.IsNull()) node_output[node_schema_child.first] = node_output_child;
        }
    }
}

}  // namespace yaml_schema_cpp
