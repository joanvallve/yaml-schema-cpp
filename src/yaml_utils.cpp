#include "yaml-schema-cpp/yaml_utils.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <memory>

#include "yaml-schema-cpp/type_check.hpp"
#include "yaml-schema-cpp/filesystem_wrapper.hpp"
#include "yaml-schema-cpp/yaml_schema.hpp"

namespace yaml_schema_cpp
{

void flattenNode(YAML::Node&              node,
                 std::string              current_folder,
                 std::vector<std::string> schema_folders,
                 bool                     is_schema,
                 bool                     override)
{
    switch (node.Type())
    {
        case YAML::NodeType::Map:
            flattenMap(node, current_folder, schema_folders, is_schema, override);
            break;
        case YAML::NodeType::Sequence:
            flattenSequence(node, current_folder, schema_folders, is_schema, override);
            break;
        case YAML::NodeType::Scalar:
        default:
            break;
    }
}

void flattenSequence(YAML::Node&              node,
                     std::string              current_folder,
                     std::vector<std::string> schema_folders,
                     bool                     is_schema,
                     bool                     override)
{
    for (auto node_i : node)
    {
        flattenNode(node_i, current_folder, schema_folders, is_schema, override);
    }
}

void flattenMap(YAML::Node&              node,
                std::string              current_folder,
                std::vector<std::string> schema_folders,
                bool                     is_schema,
                bool                     override)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (auto n : node)
    {
        if (n.first.as<std::string>() == "follow")
        {
            std::string path_follow_str = n.second.as<std::string>();
            std::string path_follow;

            // follow schema: empty or .schema
            bool followed_is_schema = filesystem::path(path_follow_str).extension().empty() or
                                      filesystem::path(path_follow_str).extension() == SCHEMA_EXTENSION;
            if (followed_is_schema)
            {
                path_follow = findFileRecursive(path_follow_str, schema_folders);
                if (path_follow.empty())
                {
                    throw std::runtime_error("In flattenNode: file '" + path_follow + "' not found");
                }
            }
            // follow yaml file
            else if (filesystem::path(path_follow_str).extension() == ".yaml")
            {
                // const filesystem::path follow_value(path_follow_str);
                path_follow = current_folder + "/" + path_follow_str;
            }
            else
            {
                throw std::runtime_error("In flattenNode: follow '" + path_follow_str +
                                         "' bad extension, should be '.yaml', '.schema' or empty (assumed '.schema')");
            }

            if (not filesystem::exists(path_follow))
            {
                throw std::runtime_error("In flattenNode: the file '" + path_follow + "' does not exists");
            }

            // load "follow" file
            YAML::Node node_child = YAML::LoadFile(path_follow);

            // Recursively flatten the "follow" file
            if (followed_is_schema)
            {
                flattenNode(node_child, current_folder, schema_folders, followed_is_schema, override);
            }
            else
            {
                flattenNode(node_child,
                            filesystem::path(path_follow).parent_path().string(),
                            {},
                            followed_is_schema,
                            override);
            }

            for (auto nc : node_child)
            {
                // Case schema
                if (followed_is_schema)
                {
                    addNodeSchema(node_aux,
                                  nc.first.as<std::string>(),
                                  nc.second,
                                  override,
                                  filesystem::path(path_follow).parent_path().string());
                }
                // Case input yaml
                else
                {
                    addNodeYaml(node_aux,
                                nc.first.as<std::string>(),
                                nc.second,
                                override,
                                filesystem::path(path_follow).parent_path().string());
                }
            }
        }
        else
        {
            flattenNode(n.second, current_folder, schema_folders, is_schema, override);

            // Case schema
            if (is_schema)
            {
                addNodeSchema(node_aux, n.first.as<std::string>(), n.second, override);
            }
            // Case input yaml
            else
            {
                addNodeYaml(node_aux, n.first.as<std::string>(), n.second, override, current_folder);
            }
        }
    }

    node = node_aux;
}

void addNodeYaml(YAML::Node&        node,
                 const std::string& key,
                 const YAML::Node&  value,
                 bool               override,
                 std::string        parent_path)
{
    if (node[key])
    {
        switch (value.Type())
        {
            case YAML::NodeType::Scalar: {
                if (override)
                {
                    node[key] = value;
                }
                else
                {
                    throw std::runtime_error("Trying to add an already existing scalar node.");
                }
            }
            case YAML::NodeType::Sequence: {
                for (auto value_seq_node : value)
                {
                    node[key].push_back(value_seq_node);
                }
                break;
            }
            case YAML::NodeType::Map: {
                for (auto value_map_node : value)
                {
                    YAML::Node node_key = node[key];
                    addNodeYaml(node_key,
                                value_map_node.first.as<std::string>(),
                                value_map_node.second,
                                override,
                                parent_path);
                }
                break;
            }
            default: {
                throw std::runtime_error("Trying to add a node of type not known.");
            }
        }
    }
    else
    {
        node[key] = value;
    }

    // relative path input case
    if (node[key].Type() == YAML::NodeType::Scalar)
    {
        std::string value_str = node[key].as<std::string>();
        if ((value_str.size() > 1 and value_str.substr(0, 2) == "./") or
            (value_str.size() > 2 and value_str.substr(0, 3) == "../"))
        {
            filesystem::path path_value = filesystem::path(parent_path) / filesystem::path(value.as<std::string>());
            node[key]                   = path_value.string();
        }
    }
}

std::string findFileRecursive(const std::string& name_with_extension, const std::vector<std::string>& folders)
{
    for (auto folder : folders)
    {
        if (filesystem::exists(folder) and filesystem::is_directory(folder))
        {
            for (auto const& entry : filesystem::recursive_directory_iterator(folder))
            {
                if (filesystem::is_regular_file(entry) and entry.path().filename().string() == name_with_extension)
                {
                    return entry.path().string();
                }
            }
        }
    }
    std::string folders_str;
    for (auto folder : folders) folders_str += folder + ", ";
    if (not folders_str.empty())
    {
        folders_str.pop_back();
        folders_str.back() = '.';
    }
    throw std::runtime_error("File '" + name_with_extension + "' not found in provided folders: " + folders_str);
}

std::string findSchema(std::string name_schema, const std::vector<std::string>& folders, std::ostream& log)
{
    // Check extension
    if (filesystem::path(name_schema).extension().empty())
    {
        name_schema += SCHEMA_EXTENSION;
    }
    else if (filesystem::path(name_schema).extension() != SCHEMA_EXTENSION)
    {
        log << "Wrong schema file extension " << name_schema << ", it should be '" << SCHEMA_EXTENSION;
        return "";
    }

    // Find schema file
    try
    {
        return findFileRecursive(name_schema, folders);
    }
    catch (const std::exception& e)
    {
        log << name_schema << " was NOT found in the provided folders.";
        return "";
    }
}

std::vector<std::string> getAllSchemas(const std::vector<std::string>& root_folders)
{
    std::vector<std::string> schemas_found;
    for (auto root_folder : root_folders)
        for (auto const& entry : filesystem::recursive_directory_iterator(root_folder))
            if (filesystem::is_regular_file(entry) and entry.path().extension() == SCHEMA_EXTENSION)
                schemas_found.push_back(entry.path().filename().string());

    return schemas_found;
}

void writeErrorToLog(std::stringstream& log,
                     const std::string& _acc_field,
                     const YAML::Node   _node_schema,
                     const std::string& _error_message)
{
    log << "ERROR in '" << _acc_field << "': " << _error_message << "\n";
    if (_node_schema.IsDefined())
    {
        log << "Schema specification:\n";
        writeNodeSchemaToLog(log, _acc_field, _node_schema, "  ");
    }
    log << "\n";
}

void writeNodeSchemaToLog(std::stringstream& log,
                          const std::string& _acc_field,
                          const YAML::Node   _node_schema,
                          std::string        _tabs)
{
    if (isSpecification(_node_schema))
    {
        log << _tabs << "DOC: " << _node_schema[DOC] << "\n";
        log << _tabs << "MANDATORY: " << _node_schema[MANDATORY] << "\n";
        if (_node_schema[VALUE]) log << _tabs << "VALUE: " << _node_schema[VALUE] << "\n";
        if (_node_schema[DEFAULT]) log << _tabs << "DEFAULT: " << _node_schema[DEFAULT] << "\n";
        log << _tabs << "TYPE: " << _node_schema[TYPE] << "\n";
        if (_node_schema[OPTIONS])
        {
            log << _tabs << "OPTIONS:\n";
            _tabs += " ";
            for (auto opt : _node_schema[OPTIONS]) log << _tabs << "- " << opt << "\n";
        }
    }
    else if (_node_schema.IsMap())
    {
        for (auto node_schema_child : _node_schema)
        {
            auto acc_field_child =
                (_acc_field.empty() ? "" : _acc_field + "/") + node_schema_child.first.as<std::string>();
            log << _tabs << acc_field_child << ":\n";
            writeNodeSchemaToLog(log, acc_field_child, node_schema_child.second, _tabs + "  ");
        }
    }
}

std::list<YAML::Node> findNodesWithKey(const YAML::Node root_node, const std::string& key)
{
    std::list<YAML::Node> nodes_with_key;

    if (root_node.IsSequence())
    {
        for (auto i = 0; i < root_node.size(); i++)
        {
            auto nodes_with_key_i = findNodesWithKey(root_node[i], key);
            nodes_with_key.insert(nodes_with_key.end(), nodes_with_key_i.begin(), nodes_with_key_i.end());
        }
    }
    if (root_node.IsMap())
    {
        if (root_node[key])
        {
            nodes_with_key.push_back(root_node[key]);
        }

        for (auto node_child : root_node)
        {
            auto node_child_with_key = findNodesWithKey(node_child.second, key);
            nodes_with_key.insert(nodes_with_key.end(), node_child_with_key.begin(), node_child_with_key.end());
        }
    }
    return nodes_with_key;
}

std::string sequenceToString(const YAML::Node& node)
{
    if (not node.IsSequence()) throw std::runtime_error("sequenceToString is only for sequence nodes");

    std::string ret("[");
    for (auto i = 0; i < node.size(); i++)
    {
        if (i > 0) ret += ", ";

        if (not node[i].IsSequence())
            ret += node[i].as<std::string>();
        else
            ret += sequenceToString(node[i]);
    }
    ret += "]";
    return ret;
}

bool isArrayType(const std::string& type_str, size_t& size)
{
    size_t pos_open  = type_str.find('[');
    size_t pos_close = type_str.find(']');

    if ((pos_open == std::string::npos) != (pos_close == std::string::npos))
        throw std::runtime_error("isArrayType: type_str (" + type_str + ") contains '[' but not ']' or vice-versa.");

    // not sequence string ('[' not found)
    if (pos_open == std::string::npos) return false;

    // array
    // 0: size not specified
    if (pos_open + 1 == pos_close) size = 0;
    // Return specified size
    else
        size = std::stoi(type_str.substr(pos_open + 1, pos_close - pos_open - 1));

    return true;
}

bool isDerivedType(const std::string& type_str)
{
    return getLowestElementType(type_str) == "derived";
}

std::string getLowerElementType(const std::string type_str)
{
    size_t pos_open  = type_str.find('[');
    size_t pos_close = type_str.find(']');

    if ((pos_open == std::string::npos) != (pos_close == std::string::npos))
        throw std::runtime_error("getLowerElementType: type_str (" + type_str +
                                 ") contains '[' but not ']' or vice-versa.");

    if (pos_open == std::string::npos) return type_str;

    // remove from first [ to first ]
    return type_str.substr(0, pos_open) + type_str.substr(pos_close + 1, type_str.size() - pos_close - 1);
}

std::string getLowestElementType(const std::string type_str)
{
    size_t pos_open  = type_str.find('[');
    size_t pos_close = type_str.find(']');

    if ((pos_open == std::string::npos) != (pos_close == std::string::npos))
        throw std::runtime_error("getLowestElementType: type_str (" + type_str +
                                 ") contains '[' but not ']' or vice-versa.");

    if (pos_open == std::string::npos) return type_str;

    // remove from first '[' to the end
    return type_str.substr(0, pos_open);
}

std::string getCheckType(const YAML::Node& node)
{
    if (not node[TYPE]) throw std::runtime_error("getCheckType: node does not have " + TYPE + " key.");

    // if does not start by "derived", return TYPE
    if (node[TYPE].as<std::string>().size() < std::string("derived").size() or
        node[TYPE].as<std::string>().substr(0, std::string("derived").size()) != "derived")
        return node[TYPE].as<std::string>();

    // If TYPE starts by "derived" substitute by BASE
    if (not node[BASE]) throw std::runtime_error("getCheckType: node does not have " + BASE + " key.");

    return node[BASE].as<std::string>() +
           node[TYPE].as<std::string>().substr(std::string("derived").size(),
                                               node[TYPE].as<std::string>().size() - std::string("derived").size());
}

bool compare(const YAML::Node&               node1,
             const YAML::Node&               node2,
             const std::string&              type,
             const std::vector<std::string>& folders_schema)
{
    if (not node1.IsDefined() or node1.IsNull() or not node2.IsDefined() or node2.IsNull()) return false;

    std::cout << "Compare with type " << type << "\n------- node1:\n"
              << node1 << "\n------- node2:\n"
              << node2 << std::endl;

    // sequences if array type, and same size as array type (if specified)
    if (not checkSizes(node1, type)) return false;
    if (not checkSizes(node2, type)) return false;

    size_t size;
    // array type --> call recursively compare
    if (isArrayType(type, size))
    {
        // same size
        if (node1.size() != node2.size()) return false;

        // compare all elements
        for (auto i = 0; i < node1.size(); i++)
        {
            if (not compare(node1[i], node2[i], getLowerElementType(type), folders_schema)) return false;
        }
        return true;
    }
    // scalar
    else
    {
        if (isTrivialType(type))
            return compareTrivial(node1, node2, type);
        else
            return compareNonTrivial(node1, node2, type, folders_schema);
    }
}

bool compareTrivial(const YAML::Node& node1, const YAML::Node& node2, const std::string& type)
{
    if (not node1.IsDefined() or node1.IsNull() or not node2.IsDefined() or node2.IsNull()) return false;

    std::cout << "Compare with type " << type << "\n------- node1:\n"
              << node1 << "\n------- node2:\n"
              << node2 << std::endl;

    COMPARE_TYPE(bool)
    COMPARE_TYPE(char)
    COMPARE_TYPE(int)
    COMPARE_TYPE(unsigned int)
    COMPARE_TYPE(long int)
    COMPARE_TYPE(long unsigned int)
    COMPARE_TYPE(float)
    COMPARE_TYPE(double)
    COMPARE_TYPE(std::string)
    {
        using std::string;
        COMPARE_TYPE(string)
    }
#if _EIGEN_FOUND == 1
    COMPARE_TYPE(Eigen::VectorXd)
    COMPARE_TYPE(Eigen::MatrixXd)

    COMPARE_EIGEN(1)
    COMPARE_EIGEN(2)
    COMPARE_EIGEN(3)
    COMPARE_EIGEN(4)
    COMPARE_EIGEN(5)
    COMPARE_EIGEN(6)
    COMPARE_EIGEN(7)
    COMPARE_EIGEN(8)
    COMPARE_EIGEN(9)
    COMPARE_EIGEN(10)
    {
        using namespace Eigen;
        COMPARE_TYPE(VectorXd)
        COMPARE_TYPE(VectorXd)
    }
#endif

    throw std::runtime_error("compareTrivial() not implemented for type " + type);
}

bool compareNonTrivialSchema(const YAML::Node&               node1,
                             const YAML::Node&               node2,
                             const YAML::Node&               node_schema,
                             const std::vector<std::string>& folders_schema)
{

    if (isSpecification(node_schema))
    {
        // If one defined and not the other --> not equal
        if (node1.IsDefined() != node2.IsDefined()) return false;

        // Compare if MANDATORY or if both node1 and node2 are defined
        if (node_schema[MANDATORY].as<bool>() or (node1.IsDefined() and node2.IsDefined()))
            return compare(node1, node2, node_schema[TYPE].as<std::string>(), folders_schema);
    }
    else
    {
        for (auto node_schema_child : node_schema)
        {

            YAML::Node node1_child = node1[node_schema_child.first.as<std::string>()]
                                         ? node1[node_schema_child.first.as<std::string>()]
                                         : YAML::Node(YAML::NodeType::Undefined);
            YAML::Node node2_child = node2[node_schema_child.first.as<std::string>()]
                                         ? node2[node_schema_child.first.as<std::string>()]
                                         : YAML::Node(YAML::NodeType::Undefined);

            if (not compareNonTrivialSchema(node1_child, node2_child, node_schema_child.second, folders_schema))
                return false;
        }
    }
    return true;
}

bool compareNonTrivial(const YAML::Node&               node1,
                       const YAML::Node&               node2,
                       const std::string&              type,
                       const std::vector<std::string>& folders_schema)
{
    if (not node1.IsDefined() or node1.IsNull() or not node2.IsDefined() or node2.IsNull()) return false;

    std::cout << "compareNonTrivial with type " << type << "\n------- node1:\n"
              << node1 << "\n------- node2:\n"
              << node2 << std::endl;

    // Find schema file
    auto path_schema = findSchema(type, folders_schema);
    if (path_schema.empty()) return false;

    // Load schema yaml and flatten (include all nodes indicated with "follow")
    YAML::Node node_schema;
    try
    {
        node_schema = YAML::LoadFile(path_schema);
        flattenNode(node_schema, filesystem::path(path_schema).parent_path().string(), folders_schema, true, true);
    }
    catch (const std::exception& e)
    {
        return false;
    }

    // compare following schema node
    return compareNonTrivialSchema(node1, node2, node_schema, folders_schema);
}

std::string getZeroString(const std::string& type)
{
    size_t size;
    // array type --> call recursively setZero
    if (isArrayType(type, size))
    {
        if (size == 0) size = 3;  // arbitrary size if not specified
        auto        lower_type  = getLowerElementType(type);
        std::string zero_string = "[";
        for (auto i = 0; i < size; i++)
        {
            zero_string += getZeroString(lower_type);
            if (i < size - 1) zero_string += ", ";
        }
        zero_string += "]";
        return zero_string;
    }
    // not array
    else
    {
        if (type == "bool") return "false";
        if (type == "int" or type == "unsigned int" or type == "long int" or type == "long unsigned int") return "0";
        if (type == "float" or type == "double") return "0.0";
        if (type == "char") return "A";
        if (type == "string" or type == "std::string") return "whatever";
#if _EIGEN_FOUND == 1
        if (type == "VectorXd" or type == "Eigen::VectorXd") return "[0.0, 0.0, 0.0]";
        if (type == "MatrixXd" or type == "Eigen::MatrixXd") return "[[3, 2], [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]]";

        for (auto i = 1; i <= 10; i++)
        {
            if (type == "Vector" + std::to_string(i) + "d" or type == "Eigen::Vector" + std::to_string(i) + "d")
            {
                std::string string_ret = "[0.0";
                for (auto j = 2; j <= i; j++) string_ret += ", 0.0";
                string_ret += "]";

                return string_ret;
            }

            if (type == "Matrix" + std::to_string(i) + "d" or type == "Eigen::Matrix" + std::to_string(i) + "d")
            {
                std::string string_ret = "[0.0";
                for (auto j = 2; j <= i * i; j++) string_ret += ", 0.0";
                string_ret += "]";

                return string_ret;
            }
        }
#endif
    }
    return "type uknown!";
}

}  // namespace yaml_schema_cpp
