#include <stdexcept>

#include "yaml-schema-example/costs.hpp"
#include "yaml-schema-example/path.hpp"
#include "yaml-schema-example/converter.hpp"

namespace yaml_schema_example
{

std::shared_ptr<CostBase> createCost(const std::string& yaml_path)
{
    std::shared_ptr<CostBase> cost;
    std::vector<std::string>  cost_folders_path = {YAML_SCHEMA_EXAMPLE_COSTS};

    yaml_schema_cpp::YamlServer yaml_server(cost_folders_path, yaml_path);

    if (!yaml_server.isValid("cost"))
    {
        std::cout << yaml_server.getLog().str() << std::endl;
        throw std::runtime_error("Error parsing the yaml file");

        return nullptr;
    }

    const YAML::Node& node_input = yaml_server.getNodeInput();
    const CostTypes   cost_type  = CostTypes_map.at(node_input["type"].as<std::string>());
    switch (cost_type)
    {
        case CostTypes::CostState: {
            cost = std::make_shared<CostState>(node_input.as<CostState>());
            break;
        }
        case CostTypes::CostFramePlacement:
            /* code */
            break;
        default:
            break;
    }

    return cost;
}

CostBase::CostBase(const std::string& name, double weight) : name_(name), weight_(weight) {}

CostBase::CostBase() {}

CostBase::~CostBase() {}

std::ostream& operator<<(std::ostream& os, const CostBase& cost)
{
    os << "Cost: " << cost.name_ << std::endl;
    os << "\t weight: " << cost.weight_ << std::endl;
    return os;
}

const std::string& CostBase::get_name() const
{
    return name_;
}

double CostBase::get_weight() const
{
    return weight_;
}

CostState::CostState(const std::string&     name,
                     double                 weight,
                     const Eigen::VectorXd& reference,
                     const Eigen::VectorXd& weights)
    : CostBase(name, weight), reference_(reference), weights_(weights)
{
}

CostState::CostState() : CostBase() {}

CostState::~CostState() {}

std::ostream& operator<<(std::ostream& os, const CostState& cost)
{
    os << static_cast<CostBase>(cost);
    os << "\t reference: " << cost.reference_.transpose() << std::endl;
    os << "\t weights: " << cost.weights_.transpose() << std::endl;
    return os;
}

const Eigen::VectorXd& CostState::get_reference() const
{
    return reference_;
}

const Eigen::VectorXd& CostState::get_weights() const
{
    return weights_;
}

CostFramePlacement::CostFramePlacement(const std::string&        name,
                                       double                    weight,
                                       const std::string&        link_name,
                                       const Eigen::VectorXd&    position,
                                       const Eigen::Quaterniond& orientation)
    : CostBase(name, weight), link_name_(link_name), position_(position), orientation_(orientation)
{
}

CostFramePlacement::~CostFramePlacement() {}

}  // namespace yaml_schema_example