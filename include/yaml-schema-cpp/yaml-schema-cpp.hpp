#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <experimental/filesystem>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"
#include "yaml-schema-cpp/yaml_conversion.h"

namespace yaml_schema_cpp
{

namespace filesystem = std::experimental::filesystem;

class YamlServer
{
  public:
    YamlServer(const std::vector<std::string>& folders_schema, const std::string& path_input);

    bool isValid(const std::string& name_schema, bool polymorphism = false);

    bool correctType(const YAML::Node& schema_node, const YAML::Node& node_input, std::stringstream& log);

    const std::stringstream& get_log() const;

    const YAML::Node& get_node_input() const;

  private:
    void flattenInputNode(YAML::Node& node, const filesystem::path& root_path);
    void flattenMap(YAML::Node& node, const filesystem::path& root_path);
    void flattenSequence(YAML::Node& node, const filesystem::path& root_path);

    void flattenSchemaNode(YAML::Node& node);

    bool isValidBase(const std::string& name_schema, const YAML::Node& node_input, const std::string& header_log = "");
    bool isValidDerived(const std::string& name_schema, const YAML::Node& node_input, const std::string& header_log = "");

    void getPathSchema(const std::string& name_schema, filesystem::path& path_schema);

    void writeToLog(const std::string& header, const std::string& message);

  private:
    std::vector<std::string> folders_schema_;

    filesystem::path path_input_;
    filesystem::path path_input_parent_;

    std::stringstream log_;

    YAML::Node node_input_;
};

}  // namespace yaml_schema_cpp

/*namespace YAML
{
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
struct convert<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> >
{
    static Node encode(const Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& matrix)
    {
        Node node(NodeType::Sequence);

        int nValues = matrix.rows() * matrix.cols();

        for (int i = 0; i < nValues; ++i)
        {
            node.push_back(matrix(i / matrix.cols(), i % matrix.cols()));
        }

        return node;
    }

    static bool decode(const Node& node, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& matrix)
    {
        // Totally Dynamic sized arrays are not supported
        if (_Rows == Eigen::Dynamic && _Cols == Eigen::Dynamic)
        {
            // ROS_ERROR("Double Dynamic Matrices are not supported! Matrices may only be dynamic in one dimension!");
            return false;
        }

        if (!node.IsSequence())
        {
            return false;
        }

        int nSize = node.size();  // Sequence check is implicit

        // If one dimension is dynamic -> calculate and resize

        // Rows Dynamic
        if (_Rows == Eigen::Dynamic)
        {
            // Check
            if (nSize % _Cols != 0)
            {
                // ROS_ERROR("Inputsize (%d) of dynamic row matrix is not a multiple of fixed column size (%d)", nSize,
                // _Cols);
                return false;
            }

            int nDynRows = nSize / _Cols;
            matrix.resize(nDynRows, Eigen::NoChange);
        }

        // Cols Dynamic
        if (_Cols == Eigen::Dynamic)
        {
            // Check
            if (nSize % _Rows != 0)
            {
                // ROS_ERROR("Inputsize (%d) of dynamic column matrix is not a multiple of fixed row size (%d)", nSize,
                // _Rows);
                return false;
            }

            int nDynCols = nSize / _Rows;
            matrix.resize(Eigen::NoChange, nDynCols);
        }

        if (nSize != matrix.rows() * matrix.cols())
        {
            // ROS_ERROR_STREAM("Unable to create Eigen::Matrix. Input is of wrong size! Matrix("
            //                  << matrix.rows() << "x" << matrix.cols() << ") != Tokens(" << nSize << ")");
            return false;
        }
        else
        {
            // fill
            for (int i = 0; i < matrix.rows(); i++)
            {
                for (int j = 0; j < matrix.cols(); j++)
                {
                    // ROS_INFO_STREAM("Node pos: " << i*matrix.rows() + j << "Matrix: " << i << "," << j);
                    //                                      YAML::Node field = node[i + j*matrix.cols()];
                    //                                      matrix(i,j) = field.as<_Scalar>();

                    matrix(i, j) = node[(int)(i * matrix.cols() + j)].as<_Scalar>();
                }
            }
        }
        return true;
    }
};
}  // namespace YAML*/
