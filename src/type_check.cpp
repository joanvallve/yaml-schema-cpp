#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"
#include "yaml-schema-cpp/yaml_utils.hpp"

// Macros used by checkNodeAs*() to check a node depending on the type string
/*
 * If type is TypeName, converts node to TypeName (it may throw) and returns true.
 */
#define CHECK_TYPE_CASE(TypeName)                                                                                     \
    if (type == #TypeName)                                                                                            \
    {                                                                                                                 \
        node.as<TypeName>();                                                                                          \
        return true;                                                                                                  \
    }

#if _EIGEN_FOUND == 1
/*
 * If type is "[Eigen::]Vector<size>d" or "[Eigen::]Matrix<size>d", converts node to it (it may throw) and
 * returns true.
 */
#define CHECK_TYPE_EIGEN_CASE(size)                                                                                   \
    if (type == "Vector" #size "d" or type == "Eigen::Vector" #size "d")                                              \
    {                                                                                                                 \
        node.as<Eigen::Matrix<double, size, 1> >();                                                                   \
        return true;                                                                                                  \
    }                                                                                                                 \
    if (type == "Matrix" #size "d" or type == "Eigen::Matrix" #size "d")                                              \
    {                                                                                                                 \
        node.as<Eigen::Matrix<double, size, size> >();                                                                \
        return true;                                                                                                  \
    }

/*
 * CHECK_TYPE_CASE for all Eigen types (fixed size from 1 to 10, and dynamic size).
 */
#define CHECK_TYPE_EIGEN_CASES                                                                                        \
    CHECK_TYPE_EIGEN_CASE(1)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(2)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(3)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(4)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(5)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(6)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(7)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(8)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(9)                                                                                          \
    CHECK_TYPE_EIGEN_CASE(10)                                                                                         \
    CHECK_TYPE_CASE(Eigen::VectorXd)                                                                                  \
    CHECK_TYPE_CASE(Eigen::MatrixXd)                                                                                  \
    {                                                                                                                 \
        using namespace Eigen;                                                                                        \
        CHECK_TYPE_CASE(VectorXd)                                                                                     \
        CHECK_TYPE_CASE(MatrixXd)                                                                                     \
    }
#endif

/*
 * CHECK_TYPE_CASE for all basic types.
 */
#define CHECK_TYPE_BASIC_CASES                                                                                        \
    CHECK_TYPE_CASE(bool)                                                                                             \
    CHECK_TYPE_CASE(char)                                                                                             \
    CHECK_TYPE_CASE(int)                                                                                              \
    CHECK_TYPE_CASE(unsigned int)                                                                                     \
    CHECK_TYPE_CASE(long int)                                                                                         \
    CHECK_TYPE_CASE(long unsigned int)                                                                                \
    CHECK_TYPE_CASE(float)                                                                                            \
    CHECK_TYPE_CASE(double)

/*
 * CHECK_TYPE_CASE for all string types.
 */
#define CHECK_TYPE_STRING_CASES                                                                                       \
    CHECK_TYPE_CASE(std::string)                                                                                      \
    {                                                                                                                 \
        using std::string;                                                                                            \
        CHECK_TYPE_CASE(string)                                                                                       \
    }

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

    // array type
    if (isArrayType(type))
    {
        // the lowest element type has to be a basic type
        if (not isBasicType(getLowestElementType(type))) return false;

        // call recursively checkNodeAsBasic for each element of sequence
        for (size_t i = 0; i < node.size(); i++)
            if (not checkNodeAsBasic(node[i], getLowerElementType(type))) return false;
        return true;
    }
    // scalar
    CHECK_TYPE_BASIC_CASES
    return false;
}

bool checkNodeAsString(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    // array type
    if (isArrayType(type))
    {
        // the lowest element type has to be a string type
        if (not isStringType(getLowestElementType(type))) return false;

        // call recursively checkNodeAsString for each element of sequence
        for (size_t i = 0; i < node.size(); i++)
            if (not checkNodeAsString(node[i], getLowerElementType(type))) return false;
        return true;
    }
    // scalar
    CHECK_TYPE_STRING_CASES
    return false;
}

#if _EIGEN_FOUND == 1
bool checkNodeAsEigen(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    // array type
    if (isArrayType(type))
    {
        // the lowest element type has to be an Eigen type
        if (not isEigenType(getLowestElementType(type))) return false;

        // call recursively checkNodeAsEigen for each element of sequence
        for (size_t i = 0; i < node.size(); i++)
            if (not checkNodeAsEigen(node[i], getLowerElementType(type))) return false;
        return true;
    }
    // scalar
    CHECK_TYPE_EIGEN_CASES
    return false;
}
#endif

bool checkNodeAs(const YAML::Node& node, const std::string& type)
{
    if (not checkSizes(node, type)) return false;

    // array type
    if (isArrayType(type))
    {
        // the lowest element type has to be a trivial type
        if (not isTrivialType(getLowestElementType(type))) return false;

        // call recursively checkNodeAs for each element of sequence
        for (size_t i = 0; i < node.size(); i++)
            if (not checkNodeAs(node[i], getLowerElementType(type))) return false;
        return true;
    }
    // scalar
    CHECK_TYPE_BASIC_CASES
    CHECK_TYPE_STRING_CASES
#if _EIGEN_FOUND == 1
    CHECK_TYPE_EIGEN_CASES
#endif
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

// A type string with malformed array brackets (e.g. "int[", "int[x]") is not a known type
static bool isWellFormedType(const std::string& type)
{
    try
    {
        std::string lower_type = type;
        while (isArrayType(lower_type)) lower_type = getLowerElementType(lower_type);
    }
    catch (const std::exception& e)
    {
        return false;
    }
    return true;
}

// NOTE: is*Type() functions call checkNodeAs*() with a null node. If the type is known, either the conversion throws
// or succeeds (returning true). Otherwise, false is returned.
bool isTrivialType(const std::string& type)
{
    if (not isWellFormedType(type)) return false;
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
    if (not isWellFormedType(type)) return false;
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
    if (not isWellFormedType(type)) return false;
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
    if (not isWellFormedType(type)) return false;
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
