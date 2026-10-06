#include "yaml-schema-cpp/yaml_server.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

#include <iostream>

int main()
{
    // use symbols from the shared library, so that linking against it is tested
    yaml_schema_cpp::YamlServer server;
    if (not yaml_schema_cpp::isArrayType("int[3]"))
    {
        std::cerr << "unexpected result of isArrayType" << std::endl;
        return 1;
    }
    std::cout << "yaml-schema-cpp install test OK" << std::endl;
    return 0;
}
