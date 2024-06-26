cmake_minimum_required(VERSION 3.16)
project(yaml-schema-cpp VERSION 0.0.0)

# ------ PROJECT REQUIREMENTS ------
#Set compiler according C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
IF(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()
IF(CMAKE_CXX_STANDARD)
    IF(CMAKE_CXX_STANDARD LESS 11)
        message(FATAL_ERROR "Required standard C++14 or higher.")
    ENDIF(CMAKE_CXX_STANDARD LESS 11)
ELSE(CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
ENDIF(CMAKE_CXX_STANDARD)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Paths
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
SET(LIB_INSTALL_DIR lib)

# ------ PROJECT OPTIONS ------
IF(NOT BUILD_TESTS)
    OPTION(BUILD_TESTS "Build Unit tests" ON)
ENDIF(NOT BUILD_TESTS)

# ============ config.h ============
# Define the directory where will be the configured config.h
SET(YAML_SCHEMA_CONFIG_DIR ${PROJECT_BINARY_DIR}/conf/${PROJECT_NAME}/internal)

# Create the specified output directory if it does not exist.
IF(NOT EXISTS "${YAML_SCHEMA_CONFIG_DIR}")
    message(STATUS "Creating config output directory: ${YAML_SCHEMA_CONFIG_DIR}")
    file(MAKE_DIRECTORY "${YAML_SCHEMA_CONFIG_DIR}")
ENDIF()

IF(EXISTS "${YAML_SCHEMA_CONFIG_DIR}" AND NOT IS_DIRECTORY "${YAML_SCHEMA_CONFIG_DIR}")
    message(FATAL_ERROR "Bug: Specified CONFIG_DIR: "
        "${YAML_SCHEMA_CONFIG_DIR} exists, but is not a directory.")
ENDIF()

# Configure config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/internal/config.h.in "${YAML_SCHEMA_CONFIG_DIR}/config.h")
include_directories("${PROJECT_BINARY_DIR}/conf")

# ------ DEPENDENCIES ------
find_package(yaml-cpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)

# ------ LIBRARY ------
list(APPEND LIB_SRCS src/expression.cpp)
list(APPEND LIB_SRCS src/type_check.cpp)
list(APPEND LIB_SRCS src/yaml_generator.cpp)
list(APPEND LIB_SRCS src/yaml_schema.cpp)
list(APPEND LIB_SRCS src/yaml_server.cpp)
list(APPEND LIB_SRCS src/yaml_utils.cpp)
add_library(${PROJECT_NAME} SHARED ${LIB_SRCS})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<BUILD_INTERFACE:${YAML_SCHEMA_CONFIG_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)

# ------ TESTS ------
IF(BUILD_TESTS)
    MESSAGE(STATUS "Will build tests.")
    enable_testing()
    add_subdirectory(test)
ENDIF(BUILD_TESTS)

# ------ YAML GENERATOR ------
MESSAGE(STATUS "Building YAML generator.")
add_executable(yaml_template_generator src/yaml_template_generator.cpp)

target_link_libraries(yaml_template_generator PUBLIC ${PROJECT_NAME})
target_link_libraries(yaml_template_generator PUBLIC Boost::filesystem Boost::system)

# ------ INSTALL ------
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION lib)
install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE yaml-schema-cpp::
    DESTINATION lib/cmake/${PROJECT_NAME})
install(
    TARGETS yaml_template_generator
    DESTINATION bin)

# ------ Find ------
include(CMakePackageConfigHelpers)

# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${yaml-schema-cpp_VERSION_MAJOR}.${yaml-schema-cpp_VERSION_MINOR}"
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# So that it can be used without installing
export(EXPORT ${PROJECT_NAME}-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
)

# ---- UNINSTALL ----
if(NOT TARGET uninstall)
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()