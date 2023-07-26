// Copyright (C) 2022,2023 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
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
#include <iostream>

#include "yaml-schema-cpp/yaml_generator.hpp"

using std::cerr;
using std::cout;
using std::endl;
using namespace yaml_schema_cpp;

int main(int argc, char* argv[])
{
    /* CALL:
     *
     *   yaml_template_generator schema_name schema_folders output_file
     *
     *   'schema_name':    Schema to be applied (just its name, without path or extension)
     *   'schema_folders': Path to the folder(s) that contains all the schema files (they are searched recursively).
     *                     Provide more than one folder with '[path1 path2 ...]'
     *   'output_file':    (OPTIONAL) Path and name of the output file.
     *                     If not provided, it will be placed in $HOME with a default
     *                     name based on schema_name (requires $HOME to be defined).
     *
     *   NOTE 1: Paths can be absolute (starting by '/') or relative.
     *   NOTE 2: 'output_file' will be modified to avoid overriding existing files.
     */

    // HELP
    if (argc == 2 and (std::string(argv[1]) == "-h" or std::string(argv[1]) == "--help"))
    {
        cout << "--------- yaml_template_generator HELP ---------- \nCall it with:" << endl;
        cout << "yaml_template_generator schema_name schema_folders output_file" << endl << endl;
        cout << "'schema_name':    Schema to be applied (just its name, without path or extension)" << endl;
        cout << "'schema_folders': Path to the folder(s) that contains all the schema files (they are searched "
                "recursively)."
             << endl;
        cout << "                  Provide more than one folder with '[path1 path2 ...]'" << endl;
        cout << "'output_file':    (OPTIONAL) Path and name of the output file." << endl;
        cout << "                  If not provided, it will be placed in $HOME with a default" << endl;
        cout << "                  name based on schema_name (requires `$HOME` to be defined)." << endl << endl;
        cout << "NOTE 1: Paths can be absolute (starting by '/') or relative." << endl;
        cout << "NOTE 2: 'output_file' will be modified to avoid overriding existing files." << endl;
        return 0;
    }
    // Call
    if (argc == 3 or argc == 4)
    {
        filesystem::path current_path(filesystem::current_path());

        // Schema name
        std::string schema(argv[1]);

        // schema folders
        std::string              schema_folders_input(argv[2]);
        std::vector<std::string> schema_folders;
        if (schema_folders_input.front() == '[' and schema_folders_input.back() == ']')
        {
            // remove '[' and ']'
            schema_folders_input.erase(0, 1);
            schema_folders_input.pop_back();

            size_t pos = 0;
            while (not schema_folders_input.empty())
            {
                pos = schema_folders_input.find(" ");
                schema_folders.push_back(schema_folders_input.substr(0, pos));

                // next folder
                schema_folders_input.erase(0, pos);

                // remove spaces
                while (schema_folders_input.front() == ' ') schema_folders_input.erase(0, 1);
            }
        }
        else
            schema_folders.push_back(schema_folders_input);

        // Correct paths to absolute
        for (auto i = 0; i<schema_folders.size(); i++)
        {
            // HOME char '~'
            if (schema_folders[i].front() == '~')
                if (getenv("HOME") == NULL)
                {
                    cout << red << "ERROR: path contains '~' but environment variable HOME not defined." << reset;
                    return 1;
                }
                else
                    schema_folders[i] = std::string(getenv("HOME")) + schema_folders[i].substr(1);

            // relative path
            if (schema_folders[i].front() != '/')
                schema_folders[i] = current_path.string() + "/" + schema_folders[i];
        }

        // default output_file
        std::string output_file;
        if (argc == 3)
        {
            if (getenv("HOME") == NULL)
            {
                cout << red
                     << "ERROR: Environment variable HOME not defined (mandatory if 'output_file' not provided)."
                     << reset;
                return 1;
            }
            output_file = std::string(getenv("HOME")) + "/" + schema + "_template.yaml";
        }
        else
            output_file = std::string(argv[3]);

        // Generate template
        try
        {
            output_file = generateTemplate(output_file, schema, schema_folders);
        }
        catch (const std::exception& e)
        {
            cout << red << "ERROR: Failed to generate YAML template file with error: " << e.what() << '\n' << reset;
            return 1;
        }

        // print result and exit
        cout << "Template YAML file created correctly: " << output_file << endl;
        return 0;
    }
}