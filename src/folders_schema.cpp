#include "yaml-schema-cpp/folders_schema.hpp"

namespace yaml_schema_cpp
{

bool FoldersSchema::add_folder(std::string folder_path)
{
    folders().push_back(folder_path);

    return true;
}

const std::vector<std::string>& FoldersSchema::get_folders()
{
    return folders();
}

std::vector<std::string>& FoldersSchema::folders()
{
    static std::vector<std::string> folders;

    return folders;
}

}