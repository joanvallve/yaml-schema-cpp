#pragma once

// Yaml
#include <yaml-cpp/yaml.h>

// Eigen
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

// stl
#include <iostream>

namespace YAML
{
/**\brief Bridge YAML to and from Eigen::Matrix< >.
 *
 */
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
struct convert<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> >
{
    /** \brief Encode a YAML Sequence from an Eigen::Matrix< >
     */
    static Node encode(const Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& matrix)
    {
        Node node;  //(NodeType::Sequence);

        int nValues = matrix.rows() * matrix.cols();

        for (int i = 0; i < nValues; ++i)
        {
            node.push_back(matrix(i / matrix.cols(), i % matrix.cols()));
        }

        return node;
    }

    /** \brief Decode a YAML sequence into a ````Eigen::Matrix<typename _Scalar, int _Rows, int _Cols>````
     *
     * Two YAML formats are accepted:
     *
     *  - For matrices where at least one dimension is not Dynamic:
     *  We use a single sequence of matrix entries: the matrix dimensions are inferred
     *  ````
     *  [ v1, v2, v3, ...]
     *  ````
     *  - For all kinds of matrices. We use a sequence of two sequences.
     *  The first sequence is a vector of dimensions; the second sequence contains the matrix entries
     *  ````
     *  [ [ rows, cols ], [v1, v2, v3, ...] ]
     *  ````
     */
    static bool decode(const Node& node, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& matrix)
    {
        YAML::Node values;

        // ==========================================================================================
        // Special case empty vector/matrix
        if (node.IsSequence() and node.size() == 0)
        {
            if (_Rows != Eigen::Dynamic and _Cols != Eigen::Dynamic)
            {
                std::cout << "Empty input sequence with not dynamic matrix" << std::endl;
                return false;
            }
        }
        // sizes given by YAML
        else if (node[0].Type() == NodeType::Sequence)
        {
            if (node.size() == 2 && node[0].size() == 2 && node[1].Type() == NodeType::Sequence &&
                node[1].size() ==
                    node[0][0].as<unsigned int>() * node[0][1].as<unsigned int>())  // YAML string is well formed
            {
                int rows = node[0][0].as<int>();
                int cols = node[0][1].as<int>();
                values   = node[1];
                // full dynamic
                if (_Rows == Eigen::Dynamic && _Cols == Eigen::Dynamic)
                {
                    matrix.resize(rows, cols);
                }
                // only rows dynamic
                else if (_Rows == Eigen::Dynamic)
                {
                    if (_Cols != cols)
                    {
                        std::cout << "Wrong number of cols" << std::endl;
                        return false;
                    }
                    matrix.resize(rows, Eigen::NoChange);
                }
                // only cols dynamic
                else if (_Cols == Eigen::Dynamic)
                {
                    if (_Rows != rows)
                    {
                        std::cout << "Wrong number of rows" << std::endl;
                        return false;
                    }
                    matrix.resize(Eigen::NoChange, cols);
                }
                else  // fixed size
                {
                    if (_Rows != rows || _Cols != cols)
                    {
                        std::cout << "Wrong size" << std::endl;
                        return false;
                    }
                }
            }
            else
            {
                std::cout << "Bad matrix specification" << std::endl;
                return false;
            }
        }
        // sizes deduced
        else
        {
            // If full dynamic -> error
            if (_Rows == Eigen::Dynamic && _Cols == Eigen::Dynamic)
            {
                std::cout << "Bad yaml format. A full dynamic matrix requires [ [rows, cols], [... values ...] ]"
                          << std::endl;
                return false;
            }
            values = node;

            // If one dimension is dynamic -> calculate and resize

            // _Rows is Dynamic
            if (_Rows == Eigen::Dynamic)
            {
                if (values.size() % _Cols != 0)
                {
                    std::cout << "Input size of dynamic row matrix is not a multiple of fixed column size"
                              << std::endl;
                    return false;
                }

                int nDynRows = values.size() / _Cols;
                matrix.resize(nDynRows, Eigen::NoChange);
            }

            // _Cols is Dynamic
            if (_Cols == Eigen::Dynamic)
            {
                if (values.size() % _Rows != 0)
                {
                    std::cout << "Input size of dynamic column matrix is not a multiple of fixed row size"
                              << std::endl;
                    return false;
                }

                int nDynCols = values.size() / _Rows;
                matrix.resize(Eigen::NoChange, nDynCols);
            }
        }

        // final check for good size
        if (values.size() != (unsigned int)(matrix.rows() * matrix.cols()))
        {
            std::cout << "Wrong input size" << std::endl;
            return false;
        }
        else  // Fill the matrix
        {
            for (int i = 0; i < matrix.rows(); i++)
                for (int j = 0; j < matrix.cols(); j++)
                    matrix(i, j) = values[(int)(i * matrix.cols() + j)].as<_Scalar>();
        }
        return true;
    }
};

/**\brief Bridge YAML <--> Eigen::Quaternion with real component last
 *
 * WARNING: Beware of Eigen constructor order!
 *
 * We use the x-y-z-w order, with the real part at the end. This is consistent with ROS Quaternion.msg,
 * which is good for compatibility against ROS messages and YAML configuration.
 *
 */
template <typename _Scalar, int _Options>
struct convert<Eigen::Quaternion<_Scalar, _Options> >
{
    static Node encode(const Eigen::Quaternion<_Scalar, _Options>& quaternion)
    {
        Node node(NodeType::Sequence);

        node[0] = quaternion.x();
        node[1] = quaternion.y();
        node[2] = quaternion.z();
        node[3] = quaternion.w();

        return node;
    }

    static bool decode(const Node& node, Eigen::Quaternion<_Scalar, _Options>& quaternion)
    {
        int nSize = node.size();  // Sequence check is implicit
        if (nSize != 4)
        {
            std::cout << "Wrong quaternion input size!" << std::endl;
            return false;
        }
        else
        {
            quaternion.x() = node[0].as<_Scalar>();
            quaternion.y() = node[1].as<_Scalar>();
            quaternion.z() = node[2].as<_Scalar>();
            quaternion.w() = node[3].as<_Scalar>();
        }
        return true;
    }
};

/**\brief Encoder Eigen::CwiseNullaryOp --> YAML
 *
 * Useful for using initilizers like: Zero(), Ones(), Constant() or Random()
 *
 */
template <typename A1, typename A2>
struct convert<Eigen::CwiseNullaryOp<A1, A2> >
{
    static Node encode(const Eigen::CwiseNullaryOp<A1, A2>& matrix)
    {
        Node node;

        int nValues = matrix.rows() * matrix.cols();

        for (int i = 0; i < nValues; ++i)
        {
            node.push_back(matrix(i / matrix.cols(), i % matrix.cols()));
        }

        return node;
    }
};

/**\brief Encoder Eigen::CwiseBinaryOp --> YAML
 *
 * Useful for operations * + -
 *
 */
template <typename A1, typename A2, typename A3>
struct convert<Eigen::CwiseBinaryOp<A1, A2, A3> >
{
    static Node encode(const Eigen::CwiseBinaryOp<A1, A2, A3>& matrix)
    {
        Node node;

        int nValues = matrix.rows() * matrix.cols();

        for (int i = 0; i < nValues; ++i)
        {
            node.push_back(matrix(i / matrix.cols(), i % matrix.cols()));
        }

        return node;
    }
};

/**\brief Encoder Eigen::CwiseUnaryOp --> YAML
 *
 * Useful for cwise operators like .cwiseAbs()
 *
 */
template <typename A1, typename A2>
struct convert<Eigen::CwiseUnaryOp<A1, A2> >
{
    static Node encode(const Eigen::CwiseUnaryOp<A1, A2>& matrix)
    {
        Node node;

        int nValues = matrix.rows() * matrix.cols();

        for (int i = 0; i < nValues; ++i)
        {
            node.push_back(matrix(i / matrix.cols(), i % matrix.cols()));
        }

        return node;
    }
};

}  // namespace YAML
