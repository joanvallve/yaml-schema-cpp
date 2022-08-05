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

#include "yaml-schema-example/stage.hpp"

#include "yaml-schema-example/path.hpp"

namespace yaml_schema_example
{
Stage::Stage(std::string& name, double duration, bool transition, const std::vector<std::shared_ptr<CostBase>>& costs)
    : name_(name), duration_(duration), transition_(transition), costs_(costs)
{
}

Stage::Stage() {}

std::shared_ptr<Stage> createStage(const std::string& yaml_path)
{
    std::shared_ptr<Stage>   stage;
    std::vector<std::string> stage_folder_path = {YAML_SCHEMA_EXAMPLE_STAGE, YAML_SCHEMA_EXAMPLE_COSTS};

    yaml_schema_cpp::YamlServer yaml_server(stage_folder_path, yaml_path);
    std::stringstream           log_parse;

    if (!yaml_server.applySchema("stage_schema"))
    {
        std::cout << yaml_server.getLog() << std::endl;
        throw std::runtime_error("Error parsing the yaml file");

        return nullptr;
    }

    return stage;
}

std::ostream& operator<<(std::ostream& os, const Stage& stage)
{
    os << "Stage parameters, " << stage.name_ << std::endl;
    os << "\t duration:" << stage.duration_ << std::endl;
    os << "\t transition:" << stage.transition_ << std::endl;
    os << "\t Costs:" << stage.costs_.size() << std::endl;

    for (auto cost = stage.costs_.begin(); cost != stage.costs_.end(); ++cost)
    {
        os << "\t\t" << *cost;
    }

    return os;
}

}  // namespace yaml_schema_example
