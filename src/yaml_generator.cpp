// Copyright (C) 2022,2023,2024 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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

#include "yaml-schema-cpp/yaml_generator.hpp"

#include <iostream>
#include <fstream>

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
    // Check extension
    if (filesystem::extension(filepath).empty())
    {
        filepath += YAML_EXTENSION;
    }
    else if (filesystem::extension(filepath) != YAML_EXTENSION)
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
    // Check extension
    if (filesystem::extension(name_schema).empty())
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        throw std::runtime_error("Wrong schema file extension " + name_schema + ", it should be '" + SCHEMA_EXTENSION +
                                 "'");
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
        throw std::runtime_error("generateYaml: Couldn't load the schema yaml file " + path_schema.string() +
                                 ". Error: " + e.what());
    }

    // Flatten yaml nodes (containing "follow") to a single YAML node containing all the information
    try
    {
        flattenNode(node_schema, folders_schema, true, override);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("generateYaml: Couldn't flatten the schema yaml file " + path_schema.string() +
                                 ". Error: " + e.what());
    }

    // Check schema
    try
    {
        checkSchema(node_schema, "", node_schema);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("In loadSchema: The schema file " + path_schema.string() +
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
    // std::cout << "SCHEMA:\n" << node_schema << std::endl;
    // std::cout << "OUTPUT:\n" << node_output << std::endl;

    // Specification (has _mandatory, _type and _doc)
    if (isSpecification(node_schema))
    {
        // If VALUE defined in schema, not needed to be added to yaml
        if (node_schema[VALUE]) return;

        // Not sequence
        if (not isSequenceSchema(node_schema))
        {
            // check trivial type
            if (isTrivialType(node_schema[TYPE].as<std::string>()))
            {
                std::string value_str;

                // Mandatory string
                std::string mandatory_str = isExpression(node_schema[MANDATORY])
                                                ? "MANDATORY if " + node_schema[MANDATORY].as<std::string>() + " - "
                                                : (node_schema[MANDATORY].as<bool>() ? "" : "OPTIONAL - ");
                // Default
                if (node_schema[DEFAULT])
                {
                    if (not isEigenType(node_schema[TYPE].as<std::string>()))
                        value_str = node_schema[DEFAULT].as<std::string>();
                    else
                        value_str = sequenceToString(node_schema[DEFAULT]);
                }
                // Options
                else if (node_schema[OPTIONS])
                {
                    if (not isEigenType(node_schema[TYPE].as<std::string>()))
                        value_str = node_schema[OPTIONS][0].as<std::string>();
                    else
                        value_str = sequenceToString(node_schema[OPTIONS][0]);
                }
                // Zero value
                else
                    value_str = getZeroString(node_schema[TYPE].as<std::string>());

                // put ' ' envolving the value if type string
                if (isStringType(node_schema[TYPE].as<std::string>())) value_str = "'" + value_str + "'";

                // FILL NODE
                node_output = value_str + "  # " + mandatory_str + "DOC " + node_schema[DOC].as<std::string>() +
                              " - TYPE " + node_schema[TYPE].as<std::string>() +
                              (node_schema[OPTIONS] ? " - OPTIONS " + sequenceToString(node_schema[OPTIONS]) : "");
            }
            // Derived type
            else if (node_schema[TYPE].as<std::string>() == "derived")
            {
                node_output["type"] =
                    "'DerivedType'  # DOC String corresponding to the name of the object class (and its schema "
                    "file).";
                node_output["follow"] = "some/path/to/derived/type/parameters.yaml";
            }
            // custom non trivial-type
            else if (isNonTrivialType(node_schema[TYPE].as<std::string>(), folders_schema))
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
        // sequence
        else
        {
            // to check all the entries of the sequence, remove the brackets [] from the end of type
            YAML::Node node_schema_i = Clone(node_schema);
            node_schema_i[TYPE]      = getTypeOfSequence(node_schema);

            for (auto i = 0; i < N_SEQUENCE_OUTPUT; i++)
            {
                YAML::Node node_output_i;
                schemaToYaml(node_schema_i, node_output_i, folders_schema, override);

                // add node if it was created (if VALUE defined, it won't)
                if (node_output_i.IsDefined()) node_output.push_back(node_output_i);
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
            YAML::Node node_output_child = node_output[node_schema_child.first];
            schemaToYaml(node_schema_child.second, node_output_child, folders_schema, override);

            // remove node if it wasn't created (for example, if VALUE defined)
            if (not node_output_child.IsDefined()) node_output.remove(node_schema_child.first);
        }
    }
    // std::cout << "OUTPUT:\n" << node_output << std::endl;
}

}  // namespace yaml_schema_cpp
