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

#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"

namespace yaml_schema_cpp
{
bool tryNodeAs(const YAML::Node& node, const std::string& type)
{
    try
    {
        return checkNodeAs(node, type);
    }
    catch (const std::exception& e)
    {
        return false;
    }
    return false;
}

bool isTrivialType(const std::string& type)
{
    try
    {
        return checkNodeAs(YAML::Node(), type);
    }
    catch (const std::exception& e)
    {
        return true;
    }
    return false;
}

bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders)
{
    std::string name_schema = type;

    if (filesystem::extension(name_schema).empty())
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        // bad extension
        return false;
    }

    try
    {
        filesystem::path path = findFileRecursive(name_schema, folders);
    }
    catch (const std::exception& e)
    {
        // non existing file
        return false;
    }

    return true;
}

}  // namespace yaml_schema_cpp
