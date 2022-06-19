#include <stdexcept>

#include "yaml-schema-cpp/yaml-schema-cpp.hpp"

namespace yaml_schema_cpp
{

namespace filesystem = std::experimental::filesystem;

YamlServer::YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input)
    : folders_schema_(folders_schema),
      path_input_(filesystem::path(path_input)),
      path_input_parent_(path_input_.parent_path()),
      node_input_(YAML::LoadFile(path_input))
{
    flattenInputNode(node_input_, path_input_parent_);
}

void YamlServer::flattenInputNode(YAML::Node& node, const filesystem::path& root_path)
{
    switch (node.Type())
    {
        case YAML::NodeType::Map:
            flattenMap(node, root_path);
            break;
        case YAML::NodeType::Sequence:
            flattenSequence(node, root_path);
            break;
        case YAML::NodeType::Scalar:
        default:
            break;
    }
}

void YamlServer::flattenSequence(YAML::Node& node, const filesystem::path& root_path)
{
    for (std::size_t i = 0; i < node.size(); ++i)
    {
        YAML::Node n = node[i];
        flattenInputNode(n, root_path);
    }
}

void YamlServer::flattenMap(YAML::Node& node, const filesystem::path& root_path)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (YAML::iterator n = node.begin(); n != node.end(); ++n)
    {
        if (n->first.as<std::string>() == "follow")
        {
            filesystem::path path_follow        = root_path / filesystem::path(n->second.as<std::string>());
            filesystem::path path_follow_parent = path_follow.parent_path();

            if (!filesystem::exists(path_follow))
            {
                std::stringstream error;
                error << "YAML file does not exists. Non-existing path '" << path_follow.string() << "' included in '"
                      << root_path << "'" << std::endl;
                // Cannot retrieve the name of the parent file
                throw std::runtime_error(error.str());
            }

            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            flattenInputNode(node_child, path_follow_parent);

            for (auto nc = node_child.begin(); nc != node_child.end(); ++nc)
            {
                node_aux.force_insert(nc->first, nc->second);
            }
        }
        else
        {
            flattenInputNode(n->second, root_path);
            node_aux[n->first] = n->second;
        }
    }

    node = node_aux;
}

bool YamlServer::isValid(const std::string& name_schema, bool polymorphism)
{
    log_ << "Log of YAML parsing procedure of file: \n";
    log_ << "-------------------------------------- \n";

    if (polymorphism)
    {
        return isValidDerived(name_schema, node_input_);
    }

    return isValidBase(name_schema, node_input_);
}

bool YamlServer::isValidBase(const std::string& name_schema, const YAML::Node& node_input)
{
    filesystem::path path_schema;
    getPathSchema(name_schema, path_schema);

    YAML::Node node_schema = YAML::LoadFile(path_schema.string());

    if (!node_schema.IsMap())
    {
        throw std::runtime_error("YAML schema should be of type Map");
    }

    flattenSchemaNode(node_schema);

    bool is_valid = true;

    for (auto it = node_schema.begin(); it != node_schema.end(); ++it)
    {
        if (it->second["mandatory"].as<bool>())
        {
            // Check existence
            const std::string field_name = it->first.as<std::string>();
            if (YAML::Node i_node = node_input[field_name])
            {
                const YAML::Node& sch_node = it->second;
                if (!correctType(sch_node, i_node, log_))
                {
                    log_ << "Field '" << it->first.as<std::string>() << "' of wrong type. Should be "
                         << it->second["type"].as<std::string>() << std::endl;
                    is_valid = false;
                }
            }
            else
            {
                log_ << "Input yaml does not contain field: " << it->first.as<std::string>();
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

bool YamlServer::isValidDerived(const std::string& name_schema, const YAML::Node& node_input)
{
    // Check the base case
    bool is_valid = true;
    is_valid      = is_valid && isValidBase(name_schema, node_input);

    // Check if the derived schema exists
    filesystem::path path_derived;
    try
    {
        const std::string& type = node_input["type"].as<std::string>();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return is_valid && isValidBase(node_input["type"].as<std::string>(), node_input);
}

void YamlServer::getPathSchema(const std::string& name_schema, filesystem::path& path_schema)
{
    filesystem::path file_schema;
    if (name_schema.size() > 5 && name_schema.compare(name_schema.size() - 5, 5, ".yaml") == 0)
    {
        file_schema = filesystem::path(name_schema);
    }
    else
    {
        file_schema = filesystem::path(name_schema + ".yaml");
    }

    for (auto base_p = folders_schema_.begin(); base_p != folders_schema_.end(); ++base_p)
    {
        path_schema = filesystem::path(*base_p) / file_schema;
        if (filesystem::exists(path_schema))
        {
            return;
        }
    }

    std::stringstream error;
    error << "Schema with the name '" << name_schema << "' not found inside the YamlServer folders";
    throw std::runtime_error(error.str());
}

void YamlServer::flattenSchemaNode(YAML::Node& node)
{
    YAML::Node node_aux;  // Done using copy to preserve order of follow
    for (YAML::iterator n = node.begin(); n != node.end(); ++n)
    {
        if (n->first.as<std::string>() == "follow")
        {
            filesystem::path path_follow;
            getPathSchema(n->second.as<std::string>(), path_follow);

            YAML::Node node_child = YAML::LoadFile(path_follow.string());

            flattenSchemaNode(node_child);

            for (auto nc = node_child.begin(); nc != node_child.end(); ++nc)
            {
                node_aux.force_insert(nc->first, nc->second);
            }
        }
        else
        {
            node_aux[n->first] = n->second;
        }
    }

    node = node_aux;
}

bool YamlServer::correctType(const YAML::Node& schema_node, const YAML::Node& node_input, std::stringstream& log)
{
    const std::string& type = schema_node["type"].as<std::string>();

    if (type == "string")
    {
        try
        {
            node_input.as<std::string>();
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
            node_input.as<double>();
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
            node_input.as<Eigen::VectorXd>();
        }
        catch (const std::exception& e)
        {
            return false;
        }
    }
    else if (type == "sequence_own_type")
    {
        // First check that the node it is really a sequence
        if (!node_input.IsSequence())
        {
            return false;
        }

        // Then check the validity of each element in the sequence
        bool is_valid = true;
        for (std::size_t i = 0; i < node_input.size(); i++)
        {
            filesystem::path path_schema_own_type;
            is_valid = is_valid && isValid(schema_node["schema"].as<std::string>(), node_input[i]);
        }
        return is_valid;
    }

    return true;
}

const std::stringstream& YamlServer::get_log() const
{
    return log_;
}

const YAML::Node& YamlServer::get_node_input() const
{
    return node_input_;
}

}  // namespace yaml_schema_cpp
