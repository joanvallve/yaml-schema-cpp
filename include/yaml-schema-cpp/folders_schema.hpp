#pragma once

#include <vector>
#include <string>

namespace yaml_schema_cpp
{

class FoldersSchema
{
  public:
    FoldersSchema() = delete;

    static bool add_folder(std::string folder_path);

    static const std::vector<std::string>& get_folders();

  private:
    static std::vector<std::string>& folders();

    // To register your class in FolderSchema, add in its .cpp file:
    // const bool derived_class_folder_schema_registered = FoldersSchema::add_folder("/path/of/my/schema/folder");

};

}