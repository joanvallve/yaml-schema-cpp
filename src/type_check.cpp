#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

namespace yaml_schema_cpp
{
bool tryNodeAs(const YAML::Node& node, const std::string& type)
{
    try
    {
        return checkNodeAs(node, type);
    }
    catch (const std::exception& e)
    {
        return false;
    }
    return false;
}

bool isTrivialType(const std::string& type)
{
    try
    {
        return checkNodeAs(YAML::Node(), type);
    }
    catch (const std::exception& e)
    {
        return true;
    }
    return false;
}

bool isBasicType(const std::string& type)
{
    try
    {
        return checkNodeAsBasic(YAML::Node(), type);
    }
    catch (const std::exception& e)
    {
        return true;
    }
    return false;
}

bool isStringType(const std::string& type)
{
    try
    {
        return checkNodeAsString(YAML::Node(), type);
    }
    catch (const std::exception& e)
    {
        return true;
    }
    return false;
}

bool isEigenType(const std::string& type)
{
    try
    {
        return checkNodeAsEigen(YAML::Node(), type);
    }
    catch (const std::exception& e)
    {
        return true;
    }
    return false;
}

bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders)
{
    std::stringstream log;
    return not findSchema(type, folders, log).empty();
}

}  // namespace yaml_schema_cpp
