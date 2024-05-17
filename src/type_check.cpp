#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"

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
    std::string name_schema = type;

    if (filesystem::extension(name_schema).empty())
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::extension(name_schema) != SCHEMA_EXTENSION)
    {
        // bad extension
        return false;
    }

    try
    {
        filesystem::path path = findFileRecursive(name_schema, folders);
    }
    catch (const std::exception& e)
    {
        // non existing schema file
        return false;
    }

    return true;
}

}  // namespace yaml_schema_cpp
