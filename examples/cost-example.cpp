#include <memory>

#include "yaml-schema-example/costs.hpp"
#include "yaml-schema-cpp/yaml-schema-cpp.hpp"

int main()
{
    std::string base_path = "/home/pepms/robotics/libraries/yaml-schema-cpp/examples/";

    std::shared_ptr<yaml_schema_example::CostBase> cost =
        yaml_schema_example::createCost(base_path + "/yaml/cost-state-input.yaml");

    std::cout << *(std::static_pointer_cast<yaml_schema_example::CostState>(cost)) << std::endl;
}