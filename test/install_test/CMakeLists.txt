# Minimal project using an INSTALLED yaml-schema-cpp, to test the installed package config.
# It is not part of the yaml-schema-cpp build. Usage:
#   cmake -S test/install_test -B build_install_test -DCMAKE_PREFIX_PATH=<install prefix>
#   cmake --build build_install_test
cmake_minimum_required(VERSION 3.16)
project(yaml-schema-cpp-install-test CXX)

find_package(yaml-schema-cpp REQUIRED)
message(STATUS "yaml-schema-cpp found in: ${yaml-schema-cpp_DIR}")

add_executable(install_test main.cpp)
target_link_libraries(install_test yaml-schema-cpp::yaml-schema-cpp)
