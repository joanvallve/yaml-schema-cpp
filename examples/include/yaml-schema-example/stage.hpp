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

#include <map>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "yaml-schema-cpp/yaml_server.hpp"

#include "yaml-schema-example/costs.hpp"

namespace yaml_schema_example
{
class Stage
{
  public:
    Stage(std::string& name, double duration, bool transition, const std::vector<std::shared_ptr<CostBase>>& costs);

    friend std::ostream& operator<<(std::ostream& os, const Stage& stage);
    // For converter
    friend YAML::as_if<Stage, void>;
    friend YAML::convert<Stage>;

  protected:
    std::string                            name_;
    double                                 duration_;
    bool                                   transition_;
    std::vector<std::shared_ptr<CostBase>> costs_;

  private:
    Stage();
};

std::shared_ptr<Stage> createStage(const std::string& yaml_path);

}  // namespace yaml_schema_example
