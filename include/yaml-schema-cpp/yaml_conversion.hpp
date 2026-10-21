/**
 * @file yaml_conversion.hpp
 * @brief YAML conversions (YAML::convert) of Eigen types: Matrix, Quaternion and expressions (only if Eigen was
 * found).
 */
#pragma once

#include "yaml-schema-cpp/internal/config.h"

#if _EIGEN_FOUND == 1

// Yaml
#include <yaml-cpp/yaml.h>

// Eigen
#include <Eigen/Dense>
#include <Eigen/Geometry>

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
    /** \brief Encode a YAML Sequence (row-major) from an Eigen::Matrix< >
     *  \param matrix matrix to be encoded
     *  \return the YAML sequence
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
     *
     *  An empty sequence is accepted for matrices with at least one dynamic dimension (resized to 0).
     *
     *  \param node YAML node to be decoded
     *  \param matrix OUTPUT decoded matrix
     *  \return false if the node is not well formed or the sizes do not match (YAML::as<>() throws then)
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
                return false;
            }
            // dynamic dimensions resized to 0
            matrix.resize(_Rows == Eigen::Dynamic ? 0 : _Rows, _Cols == Eigen::Dynamic ? 0 : _Cols);
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
                        return false;
                    }
                    matrix.resize(rows, Eigen::NoChange);
                }
                // only cols dynamic
                else if (_Cols == Eigen::Dynamic)
                {
                    if (_Rows != rows)
                    {
                        return false;
                    }
                    matrix.resize(Eigen::NoChange, cols);
                }
                else  // fixed size
                {
                    if (_Rows != rows || _Cols != cols)
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
        }
        // sizes deduced
        else
        {
            // If full dynamic -> error
            if (_Rows == Eigen::Dynamic && _Cols == Eigen::Dynamic)
            {
                return false;
            }
            values = node;

            // If one dimension is dynamic -> calculate and resize

            // _Rows is Dynamic
            if (_Rows == Eigen::Dynamic)
            {
                if (values.size() % _Cols != 0)
                {
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
                    return false;
                }

                int nDynCols = values.size() / _Rows;
                matrix.resize(Eigen::NoChange, nDynCols);
            }
        }

        // final check for good size
        if (values.size() != (unsigned int)(matrix.rows() * matrix.cols()))
        {
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
    /** \brief Encode a YAML Sequence [x, y, z, w] from an Eigen::Quaternion
     *  \param quaternion quaternion to be encoded
     *  \return the YAML sequence
     */
    static Node encode(const Eigen::Quaternion<_Scalar, _Options>& quaternion)
    {
        Node node(NodeType::Sequence);

        node[0] = quaternion.x();
        node[1] = quaternion.y();
        node[2] = quaternion.z();
        node[3] = quaternion.w();

        return node;
    }

    /** \brief Decode a YAML sequence [x, y, z, w] into an Eigen::Quaternion (not normalized)
     *
     *  \param node YAML node to be decoded
     *  \param quaternion OUTPUT decoded quaternion
     *  \return false if the node is not a sequence of 4 elements
     */
    static bool decode(const Node& node, Eigen::Quaternion<_Scalar, _Options>& quaternion)
    {
        int nSize = node.size();  // Sequence check is implicit
        if (nSize != 4)
        {
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
    /** \brief Encode a YAML Sequence (row-major) from an Eigen::CwiseNullaryOp
     *  \param matrix expression to be encoded
     *  \return the YAML sequence
     */
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
    /** \brief Encode a YAML Sequence (row-major) from an Eigen::CwiseBinaryOp
     *  \param matrix expression to be encoded
     *  \return the YAML sequence
     */
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
    /** \brief Encode a YAML Sequence (row-major) from an Eigen::CwiseUnaryOp
     *  \param matrix expression to be encoded
     *  \return the YAML sequence
     */
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

#endif