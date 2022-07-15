#include <memory>
#include "yaml-cpp/yaml.h"

#include "yaml-schema-example/costs.hpp"
#include "yaml-schema-example/stage.hpp"
#include "yaml-schema-cpp/yaml_conversion.hpp"

namespace YAML
{

template <>
struct convert<yaml_schema_example::CostState>
{
    static Node encode(const yaml_schema_example::CostState& rhs)
    {
        Node node;
        node["name"]      = rhs.name_;
        node["type"]      = "state";
        node["weight"]    = rhs.weight_;
        node["reference"] = rhs.reference_;
        node["weights"]   = rhs.weight_;

        return node;
    }

    static bool decode(const Node& node, yaml_schema_example::CostState& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.name_      = node["name"].as<std::string>();
        rhs.weight_    = node["weight"].as<double>();
        rhs.reference_ = node["reference"].as<Eigen::VectorXd>();
        rhs.weights_   = node["weights"].as<Eigen::VectorXd>();
        return true;
    }
};

template <>
struct convert<yaml_schema_example::Stage>
{
    static Node encode(const yaml_schema_example::Stage& rhs)
    {
        Node node;
        node["name"]       = rhs.name_;
        node["duration"]   = rhs.duration_;
        node["transition"] = rhs.transition_;

        for (auto cost = rhs.costs_.begin(); cost != rhs.costs_.end(); ++cost)
        {
            // Do this generically. It should consider any kind of existing cost
            std::shared_ptr<yaml_schema_example::CostState> cost_state =
                std::dynamic_pointer_cast<yaml_schema_example::CostState>(*cost);

            if (cost_state != nullptr)
            {
                node["costs"].push_back(*cost_state);
            }
        }

        return node;
    }

    static bool decode(const Node& node, yaml_schema_example::Stage& rhs)
    {
        if (!node.IsMap() && node["costs"].IsSequence())
        {
            return false;
        }
        rhs.name_       = node["name"].as<std::string>();
        rhs.duration_   = node["duration"].as<double>();
        rhs.transition_ = node["transition"].as<bool>();

        for (auto yaml_cost = node["costs"].begin(); yaml_cost != node["costs"].end(); ++yaml_cost)
        {
            std::shared_ptr<yaml_schema_example::CostState> cost =
                std::make_shared<yaml_schema_example::CostState>((*yaml_cost).as<yaml_schema_example::CostState>());
            rhs.costs_.push_back(cost);
        }

        return true;
    }
};

}  // namespace YAML