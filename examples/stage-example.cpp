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

#include <memory>

#include "yaml-schema-example/stage.hpp"
#include "yaml-schema-cpp/yaml_server.hpp"

int main()
{
    std::string base_path = "/home/pepms/robotics/libraries/yaml-schema-cpp/examples/";

    std::shared_ptr<yaml_schema_example::Stage> stage =
        yaml_schema_example::createStage(base_path + "/yaml/stage-input.yaml");

    // std::cout << *(stage) << std::endl;
}
