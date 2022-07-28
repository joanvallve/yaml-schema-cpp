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

#include <map>
#include <memory>

#include <Eigen/Dense>

#include "yaml-schema-cpp/yaml_server.hpp"

namespace yaml_schema_example
{

enum class CostTypes
{
    CostState,
    CostFramePlacement,
    NbCostTypes
};

static std::map<std::string, CostTypes> initMapCostTypes()
{
    std::map<std::string, CostTypes> m;
    m.clear();
    m.insert({"state", CostTypes::CostState});
    m.insert({"frame-placement", CostTypes::CostFramePlacement});

    return m;
}

static const std::map<std::string, CostTypes> CostTypes_map = initMapCostTypes();

class CostBase
{
  public:
    CostBase(const std::string& name, double weight);
    virtual ~CostBase();

    friend std::ostream& operator<<(std::ostream& os, const CostBase& cost);

    const std::string& get_name() const;
    double             get_weight() const;

  protected:
    CostBase();
    std::string name_;
    double      weight_;
};

class CostState : public CostBase
{
  public:
    CostState(const std::string&     name,
              double                 weight,
              const Eigen::VectorXd& reference,
              const Eigen::VectorXd& weights);
    virtual ~CostState();

    const Eigen::VectorXd& get_reference() const;
    const Eigen::VectorXd& get_weights() const;

    friend std::ostream& operator<<(std::ostream& os, const CostState& cost);

    // For converter
    friend YAML::as_if<CostState, void>;
    friend YAML::convert<CostState>;

  private:
    CostState(); // private constructor for converter
    Eigen::VectorXd reference_;
    Eigen::VectorXd weights_;
};

class CostFramePlacement : public CostBase
{
  public:
    CostFramePlacement(const std::string&        name,
                       double                    weight,
                       const std::string&        link_name,
                       const Eigen::VectorXd&    position,
                       const Eigen::Quaterniond& orientation);
    virtual ~CostFramePlacement();

  private:
    std::string        link_name_;
    Eigen::Vector3d    position_;
    Eigen::Quaterniond orientation_;
};

std::shared_ptr<CostBase> createCost(const std::string& yaml_path);
}  // namespace yaml_schema_example