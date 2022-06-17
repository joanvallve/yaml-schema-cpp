#include <filesystem>

#include "yaml-schema-cpp/yaml-schema-cpp.hpp"

namespace yaml_schema_cpp
{

YamlServer::YamlServer(const std::string& schema_path, const std::string& input_path, bool is_derived)
    : is_derived_(is_derived)
{
    input_node_  = YAML::LoadFile(input_path);
    schema_node_ = YAML::LoadFile(schema_path);

    std::filesystem::path p_s(schema_path);
    parent_schema_path_ = p_s.parent_path();

    // YamlServer(schema_path, is_derived);
}

YamlServer::YamlServer(const std::string& schema_path, const YAML::Node& input_node, bool is_derived)
    : is_derived_(is_derived)
{
    input_node_  = input_node;
    schema_node_ = YAML::LoadFile(schema_path);

    std::filesystem::path p_s(schema_path);
    parent_schema_path_ = p_s.parent_path();
}

YamlServer::YamlServer(const std::string& schema_path, bool is_derived) : is_derived_(is_derived)
{
    // std::cout << "Constructor 2:" << schema_path << std::endl;

    // std::filesystem::path p_i(input_path);

    // parent_input_path_  = p_i.parent_path();
}

void YamlServer::print()
{
    std::cout << "Printing schema:" << std::endl;
    std::cout << parent_schema_path_ << std::endl;
    for (auto it = schema_node_.begin(); it != schema_node_.end(); ++it)
    {
        std::cout << it->first.as<std::string>() << std::endl;
    }

    std::cout << "Printing input:" << std::endl;
    for (auto it = input_node_.begin(); it != input_node_.end(); ++it)
    {
        std::cout << it->first.as<std::string>() << std::endl;
    }
}

bool YamlServer::isValid(std::stringstream& log)
{
    preProcess(schema_node_);  // follow substitution

    log << "Log of YAML parsing procedure of file: \n";
    log << "-------------------------------------- \n";

    if (is_derived_)
    {
        return isValidDerived(log);
    }

    YAML::Node node_aux =
        YAML::LoadFile("/home/pepms/robotics/libraries/yaml-schema-cpp/examples/yaml-schemas/stages/base.yaml");
    // std::cout << parent_schema_path_ << std::endl;

    return isValidBase(log);
}

bool YamlServer::isValidBase(std::stringstream& log)
{
    bool is_valid = true;

    for (auto it = schema_node_.begin(); it != schema_node_.end(); ++it)
    {
        if (it->second["mandatory"].as<bool>())
        {
            // Check existence
            const std::string field_name = it->first.as<std::string>();
            if (YAML::Node i_node = input_node_[field_name])
            {
                const YAML::Node& sch_node = it->second;
                if (!correctType(sch_node, i_node, log))
                {
                    log << "Field '" << it->first.as<std::string>() << "' of wrong type. Should be "
                        << it->second["type"].as<std::string>() << std::endl;
                    is_valid = false;
                }
            }
            else
            {
                log << "Input yaml does not contain field: " << it->first.as<std::string>();
                is_valid = false;
            }
        }
        else
        {
            // If not mandatory it should have a default
        }
    }

    return is_valid;
}

bool YamlServer::isValidDerived(std::stringstream& log)
{
    // Check the base case
    bool is_valid = true;
    is_valid      = is_valid && isValidBase(log);

    // Check if the derived schema exists

    std::string yaml_schema_path;
    try
    {
        const std::string&          type = input_node_["type"].as<std::string>();
        const std::filesystem::path derived_yaml(parent_schema_path_ + "/" + type + ".yaml");
        if (!std::filesystem::exists(derived_yaml))
        {
            // The derived schema does not exists
            std::string error = "Schema for derived type does not exists:" + derived_yaml.string();
            throw std::runtime_error(error);
        }
        yaml_schema_path = derived_yaml.string();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // Validate derived case
    schema_node_ = YAML::LoadFile(yaml_schema_path);
    preProcess(schema_node_);

    return is_valid && isValidBase(log);
}

bool YamlServer::correctType(const YAML::Node& schema_node, const YAML::Node& input_node, std::stringstream& log)
{
    const std::string& type = schema_node["type"].as<std::string>();

    if (type == "string")
    {
        try
        {
            input_node.as<std::string>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "double")
    {
        try
        {
            input_node.as<double>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "eigen_vector")
    {
        try
        {
            input_node.as<Eigen::VectorXd>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "sequence_own_type")
    {
        // First check that the node it is really a sequence
        if (!input_node.IsSequence())
        {
            return false;
        }

        // Then check the validity of each element in the sequence
        bool is_valid = true;
        for (std::size_t i = 0; i < input_node.size(); i++)
        {
            std::string schema_path_own_type =
                parent_schema_path_ + "/" + schema_node["own_type_location"].as<std::string>() + "/base.yaml";
            YamlServer validator(schema_path_own_type, input_node[i], schema_node["polymorphism"].as<bool>());
            is_valid = is_valid && validator.isValid(log);
        }
        return is_valid;
    }

    return true;
}

void YamlServer::preProcess(YAML::Node& yaml_node)
{
    // Substitute 'follow' in the schema node
    // TODO: recursively
    try
    {
        if (yaml_node["follow"])
        {
            std::string file_name = parent_schema_path_ + "/" + yaml_node["follow"].as<std::string>();
            YAML::Node  node_base = YAML::LoadFile(file_name);
            for (auto it = node_base.begin(); it != node_base.end(); ++it)
            {
                yaml_node.force_insert(it->first, it->second);
            }
            yaml_node.remove("follow");
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

}  // namespace yaml_schema_cpp
