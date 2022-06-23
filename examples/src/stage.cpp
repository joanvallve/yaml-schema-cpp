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

    if (!yaml_server.validate("stage_schema"))
    {
        std::cout << yaml_server.getLog().str() << std::endl;
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