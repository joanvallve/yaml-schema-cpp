#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

namespace yaml_schema_cpp
{

bool checkSizes(const YAML::Node& node, const std::string& type)
{
    size_t size;

    // not array --> ok
    if (not isArrayType(type, size)) return true;

    // array type --> node should be sequence
    if (node.IsSequence())
    {
        // array size not specified --> ok to be a sequence
        if (size == 0) return true;

        // array size specified --> sequence should have same size
        if (node.size() == size) return true;
    }
    // otherwise
    return false;
}

bool checkNodeAsBasic(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsBasic(node[i], getLowerElementType(type));
    }
    // scalar
    else
        CHECK_TYPE_BASIC_CASES
    return false;
}

bool checkNodeAsString(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsString(node[i], getLowerElementType(type));
    }
    // scalar
    else
        CHECK_TYPE_STRING_CASES
    return false;
}

#if _EIGEN_FOUND == 1
bool checkNodeAsEigen(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;
    
    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsString(node[i], getLowerElementType(type));
    }
    else
        CHECK_TYPE_EIGEN_CASES
    return false;
}
#endif

bool checkNodeAs(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    size_t size;
    // array type
    if (isArrayType(type, size))  
    {
        // call recursively checkNodeAsBasic for each element of sequence
        for (auto i = 0; node.size(); i++) checkNodeAsString(node[i], getLowerElementType(type));
    }
    else
    {
        CHECK_TYPE_BASIC_CASES
        CHECK_TYPE_STRING_CASES
#if _EIGEN_FOUND == 1
        CHECK_TYPE_EIGEN_CASES
#endif
    }
    return false;
}

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

#if _EIGEN_FOUND == 1
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
#endif

bool isNonTrivialType(const std::string& type, const std::vector<std::string>& folders)
{
    std::stringstream log;
    return not findSchema(type, folders, log).empty();
}


}  // namespace yaml_schema_cpp
