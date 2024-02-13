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
#pragma once

#include <vector>
#include <string>

namespace yaml_schema_cpp
{
class FoldersSchema
{
  public:
    FoldersSchema() = delete;

    static bool add_folder(std::string folder_path);

    static const std::vector<std::string>& get_folders();

  private:
    static std::vector<std::string>& folders();

    // To register your class in FolderSchema, add in its .cpp file:
    // const bool derived_class_folder_schema_registered = FoldersSchema::add_folder("/path/of/my/schema/folder");
};

}  // namespace yaml_schema_cpp
