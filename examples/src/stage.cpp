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
    // std::shared_ptr<Stage> stage;
    // std::string            stage_path = YAML_SCHEMA_EXAMPLE_STAGE;

    // // Check the type to create the schema
    // std::string yaml_schema_path = YAML_SCHEMA_EXAMPLE_STAGE "/base.yaml";

    // YAML::Node node = YAML::LoadFile(yaml_path);

    // yaml_schema_cpp::SchemaValidator validator(yaml_schema_path, yaml_path, false);
    // std::stringstream                log_parse;

    // // If we are here, we have the schema and we can validate the input yaml
    // if (!validator.isValid(log_parse))
    // {
    //     std::cout << log_parse.str() << std::endl;

    //     throw std::runtime_error("Error parsing the yaml file");

    //     return nullptr;
    // }

    // return stage;
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