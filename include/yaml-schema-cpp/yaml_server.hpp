//--------LICENSE_START--------
//
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.//
//--------LICENSE_END--------
#pragma once

#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <boost/filesystem.hpp>

namespace yaml_schema_cpp
{

namespace filesystem = boost::filesystem;

class YamlServer
{
  public:
    YamlServer(bool override=true);
    YamlServer(const std::vector<std::string>& folders_schema, bool override=true);
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input, bool override=true);

    bool applySchema(const std::string& name_schema);

    void addFolderSchema(const std::vector<std::string>& folders_schema, bool before = false);
    void addFolderSchema(const std::string& folder_schema, bool before = false);

    void loadYaml(const std::string& path_input);

    std::string getLog() const;

    const YAML::Node& getNode() const;

  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;

    std::stringstream log_;

    YAML::Node node_input_;

    bool override_;
};

}  // namespace yaml_schema_cpp
