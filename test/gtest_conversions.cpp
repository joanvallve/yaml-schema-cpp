#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include <yaml-cpp/yaml.h>
#include <eigen3/Eigen/Dense>
#include <iostream>

using namespace Eigen;

TEST(MapYaml, decode)
{
    MatrixXd M23(2, 3);
    MatrixXd M33(3, 3);
    VectorXd v3(3);
    M23 << 1, 2, 3, 4, 5, 6;
    M33 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    v3 << 1, 2, 3;

    std::cout << "\nTrying different yaml specs for matrix..." << std::endl;

    YAML::Node mat_sized_23, mat_sized_33, mat_23, mat_33;
    YAML::Node vec_sized_3, vec_3, vec_0;

    mat_sized_23 = YAML::Load("[[2, 3] ,[1, 2, 3, 4, 5, 6] ]");         // insensitive to spacing
    mat_sized_33 = YAML::Load("[[3, 3] ,[1,2, 3, 4, 5, 6, 7, 8, 9]]");  // insensitive to spacing
    vec_sized_3  = YAML::Load("[[3, 1] , [1, 2, 3]]");                  // insensitive to spacing
    mat_23       = YAML::Load("[1, 2,3, 4,  5, 6]");                    // insensitive to spacing
    mat_33       = YAML::Load("[1, 2, 3, 4,5,6,7, 8, 9]");              // insensitive to spacing
    vec_3        = YAML::Load("[1, 2, 3 ]");                            // insensitive to spacing

    MatrixXd Mx = mat_sized_23.as<MatrixXd>();
    std::cout << "Dyn-Dyn [[2,3] ,[1, 2, 3, 4, 5, 6] ] = \n" << Mx << std::endl;
    ASSERT_MATRIX_APPROX(Mx, M23, 1e-12);

    Matrix<double, 2, Dynamic> M2d = mat_sized_23.as<Matrix<double, 2, Dynamic>>();
    std::cout << "2-Dyn [[2,3] ,[1, 2, 3, 4, 5, 6] ] = \n" << M2d << std::endl;
    ASSERT_MATRIX_APPROX(M2d, M23, 1e-12);

    Matrix<double, Dynamic, 3> MD3 = mat_sized_23.as<Matrix<double, Dynamic, 3>>();
    std::cout << "Dyn-3 [[2,3] ,[1, 2, 3, 4, 5, 6] ] = \n" << MD3 << std::endl;
    ASSERT_MATRIX_APPROX(MD3, M23, 1e-12);

    Matrix3d M3 = mat_sized_33.as<Matrix3d>();
    std::cout << "3-3   [[3,3] ,[1, 2, 3, 4, 5, 6, 7, 8, 9] ] = \n" << M3 << std::endl;
    ASSERT_MATRIX_APPROX(M3, M33, 1e-12);

    M2d = mat_23.as<Matrix<double, 2, Dynamic>>();
    std::cout << "2-Dyn [1, 2, 3, 4, 5, 6] = \n" << M2d << std::endl;
    ASSERT_MATRIX_APPROX(M2d, M23, 1e-12);

    MD3 = mat_23.as<Matrix<double, Dynamic, 3>>();
    std::cout << "Dyn-3 [1, 2, 3, 4, 5, 6] = \n" << MD3 << std::endl;
    ASSERT_MATRIX_APPROX(MD3, M23, 1e-12);

    M3 = mat_33.as<Matrix3d>();
    std::cout << "3-3   [1, 2, 3, 4, 5, 6, 7, 8, 9] = \n" << M3 << std::endl;
    ASSERT_MATRIX_APPROX(M3, M33, 1e-12);

    VectorXd vx = vec_sized_3.as<VectorXd>();
    std::cout << "Dyn [[3, 1] ,[1, 2, 3] ] = \n" << vx << std::endl;
    ASSERT_MATRIX_APPROX(vx, v3, 1e-12);

    Vector3d v3d = vec_sized_3.as<Vector3d>();
    std::cout << "3 [[3, 1] ,[1, 2, 3] ] = \n" << v3d << std::endl;
    ASSERT_MATRIX_APPROX(v3d, v3, 1e-12);

    v3d = vec_3.as<Vector3d>();
    std::cout << "3 [1, 2, 3] = \n" << v3d << std::endl;
    ASSERT_MATRIX_APPROX(v3d, v3, 1e-12);

    vx = vec_3.as<VectorXd>();
    std::cout << "Dyn [1, 2, 3] = \n" << vx << std::endl;
    ASSERT_MATRIX_APPROX(vx, v3, 1e-12);
}

TEST(MapYaml, encode)
{
    // Matrix
    MatrixXd M23(2, 3);
    MatrixXd M33(3, 3);
    VectorXd v3(3);
    M23 << 1, 2, 3, 4, 5, 6;
    M33 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    v3 << 1, 2, 3;

    YAML::Node n;
    n["m23"] = M23;
    n["m33"] = M33;
    n["v3"]  = v3;
    std::cout << n << std::endl;

    // cwiseNullaryOps
    YAML::Node n2;
    n2["zero_d23"]     = MatrixXd::Zero(2, 3);
    n2["zero_33"]      = Matrix3d::Zero();
    n2["zero_d3"]      = VectorXd::Zero(3);
    n2["zero_3"]       = Vector3d::Zero();
    n2["ones_d23"]     = MatrixXd::Ones(2, 3);
    n2["ones_33"]      = Matrix3d::Ones();
    n2["ones_d3"]      = VectorXd::Ones(3);
    n2["ones_3"]       = Vector3d::Ones();
    n2["twohalf_d23"]  = MatrixXd::Constant(2, 3, 2.5);
    n2["twohalf_33"]   = Matrix3d::Constant(2.5);
    n2["twohalf_d3"]   = VectorXd::Constant(3, 2.5);
    n2["twohalf_3"]    = Vector3d::Constant(2.5);
    n2["rand_d23"]     = MatrixXd::Random(2, 3);
    n2["rand_33"]      = Matrix3d::Random();
    n2["rand_d3"]      = VectorXd::Random(3);
    n2["rand_3"]       = Vector3d::Random();
    n2["linspaced_d5"] = VectorXd::LinSpaced(5, 3, 10);
    std::cout << n2 << std::endl;

    // operations
    YAML::Node n3;
    n3["v3twice"]       = v3 * 2;
    n3["v3twiceB"]      = 2 * v3;
    n3["v3plustwicev3"] = v3 + 2 * v3;
    n3["op"]            = M33 * v3 + 2 * v3;
    n3["M33abs"]        = M33.cwiseAbs();
    std::cout << n3 << std::endl;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
