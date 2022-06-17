#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#include <Eigen/Dense>

#include "yaml-cpp/yaml.h"

namespace yaml_schema_cpp
{

class YamlServer
{
  public:
    YamlServer(const std::string& schema_path, const std::string& input_path, bool is_derived);
    YamlServer(const std::string& schema_path, const YAML::Node& input_node, bool is_derived);
    YamlServer(const std::string& schema_path, bool is_derived);

    bool isValid(std::stringstream& log);

    bool correctType(const YAML::Node& schema_node, const YAML::Node& input_node, std::stringstream& log);
    void print();
  private:
    bool isValidDerived(std::stringstream& log);
    bool isValidBase(std::stringstream& log);

    void preProcess(YAML::Node& yaml_node);

    bool is_derived_;

    YAML::Node schema_node_;
    YAML::Node input_node_;

    std::string parent_schema_path_;
    // std::string parent_input_path_;
};

}  // namespace yaml_schema_cpp

namespace YAML
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
}  // namespace YAML
