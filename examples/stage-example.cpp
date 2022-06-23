#include <memory>

#include "yaml-schema-example/stage.hpp"
#include "yaml-schema-cpp/yaml_server.h"

int main()
{
    std::string base_path = "/home/pepms/robotics/libraries/yaml-schema-cpp/examples/";

    std::shared_ptr<yaml_schema_example::Stage> stage =
        yaml_schema_example::createStage(base_path + "/yaml/stage-input.yaml");

    // std::cout << *(stage) << std::endl;
}