#pragma once

#include <map>
#include <memory>

#include <Eigen/Dense>

#include "yaml-schema-cpp/yaml-schema-cpp.hpp"

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