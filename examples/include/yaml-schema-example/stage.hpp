
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