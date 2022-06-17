cmake_minimum_required(VERSION 3.10.0)
project(yaml-schema-cpp VERSION 0.0.0)

# ------ PROJECT REQUIREMENTS ------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------ DEPENDENCIES ------
find_package(yaml-cpp REQUIRED)
find_package(Eigen3 REQUIRED)

# ------ LIBRARY ------
add_library(${PROJECT_NAME} SHARED src/yaml-schema-cpp.cpp)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)

# ------ EXAMPLE LIBRARY ------
list(APPEND LIB_EXAMPLE_SRCS examples/src/costs.cpp)
list(APPEND LIB_EXAMPLE_SRCS examples/src/stage.cpp)

add_library(${PROJECT_NAME}_example SHARED ${LIB_EXAMPLE_SRCS})

target_include_directories(${PROJECT_NAME}_example PUBLIC examples/include)

target_link_libraries(${PROJECT_NAME}_example PUBLIC ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_example PUBLIC Eigen3::Eigen)

# ------ EXECUTABLES ------
set(COST_EX_NAME cost-example)
add_executable(${COST_EX_NAME} examples/cost-example.cpp)

target_link_libraries(${COST_EX_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${COST_EX_NAME} PUBLIC ${PROJECT_NAME}_example)

set(STAGE_EX_NAME stage-example)
add_executable(${STAGE_EX_NAME} examples/stage-example.cpp)

target_link_libraries(${STAGE_EX_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${STAGE_EX_NAME} PUBLIC ${PROJECT_NAME}_example)

# ------ INSTALL ------
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)
include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION lib)
install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE YamlSchemaCpp::
    DESTINATION lib/cmake/${PROJECT_NAME})
install(FILES cmake/${PROJECT_NAME}Config.cmake DESTINATION lib/cmake/cmake_test)
