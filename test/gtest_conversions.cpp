//--------LICENSE_START--------
//
// Copyright (C) 2022 Institut de Robòtica i Informàtica Industrial, CSIC-UPC.
// Authors: Josep Martí Saumell (jmarti@iri.upc.edu) and Joan Vallvé Navarro (jvallve@iri.upc.edu)
// All rights reserved.
//
// YAML-SCHEMA-CPP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.//
//--------LICENSE_END--------
#include "gtest/utils_gtest.h"
#include "yaml-schema-cpp/yaml_conversion.hpp"
#include <yaml-cpp/yaml.h>
#include <eigen3/Eigen/Dense>
#include <iostream>

using namespace Eigen;

TEST(MapYaml, save_2d)
{
    MatrixXd M23(2,3);
    MatrixXd M33(3,3);
    VectorXd v3(3);
    M23 << 1, 2, 3, 4, 5, 6;
    M33 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    v3  << 1, 2, 3;

    std::cout << "\nTrying different yaml specs for matrix..." << std::endl;

    YAML::Node mat_sized_23, mat_sized_33, mat_23, mat_33;
    YAML::Node vec_sized_3, vec_3, vec_0;

    mat_sized_23    = YAML::Load("[[2, 3] ,[1, 2, 3, 4, 5, 6] ]"); // insensitive to spacing
    mat_sized_33    = YAML::Load("[[3, 3] ,[1, 2, 3, 4, 5, 6, 7, 8, 9]]"); // insensitive to spacing
    vec_sized_3     = YAML::Load("[[3, 1] ,[1, 2, 3]]"); // insensitive to spacing

    mat_23      = YAML::Load("[1, 2, 3, 4, 5, 6]");             // insensitive to spacing
    mat_33      = YAML::Load("[1, 2, 3, 4, 5, 6, 7, 8, 9]");    // insensitive to spacing
    vec_3       = YAML::Load("[1, 2, 3]");                      // insensitive to spacing
    //vec_0       = YAML::Load("[]");                             // insensitive to spacing

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

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
