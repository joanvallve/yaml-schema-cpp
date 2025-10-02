cmake_minimum_required(VERSION 3.16)
project(yaml-schema-cpp VERSION 0.0.0)

# ------ PROJECT REQUIREMENTS ------
#Set compiler according C++11 support
include(CheckCXXCompilerFlag)
message(STATUS "Checking compiler ${CMAKE_CXX_COMPILER} C++ standard support...")
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)
IF(CMAKE_CXX_STANDARD)
    IF(CMAKE_CXX_STANDARD LESS 11)
        message(FATAL_ERROR "Required standard C++11 or higher.")
    ENDIF(CMAKE_CXX_STANDARD LESS 11)
ELSE(CMAKE_CXX_STANDARD)
    IF(COMPILER_SUPPORTS_CXX20)
        set(CMAKE_CXX_STANDARD 20)
        message(STATUS "C++ standard used: ${CMAKE_CXX_STANDARD}")
    ELSEIF(COMPILER_SUPPORTS_CXX17)
        set(CMAKE_CXX_STANDARD 17)
        message(STATUS "C++ standard used: ${CMAKE_CXX_STANDARD}")
    ELSEIF(COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_STANDARD 14)
        message(STATUS "C++ standard used: ${CMAKE_CXX_STANDARD}")
    ELSEIF(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_STANDARD 11)
        message(STATUS "C++ standard used: ${CMAKE_CXX_STANDARD}")
    ELSE()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    ENDIF()
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
IF(CMAKE_CXX_STANDARD LESS 17)
    message(STATUS "C++ less than 17, Boost::filesystem is required")
    SET(BOOST_FILESYSTEM_LIB 1)
ELSE()
    SET(BOOST_FILESYSTEM_LIB 0)
ENDIF()
message(STATUS "BOOST_FILESYSTEM_LIB: ${BOOST_FILESYSTEM_LIB}")

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
find_package(yaml-cpp 0.7 REQUIRED)
find_package(Eigen3 REQUIRED)
IF(BOOST_FILESYSTEM_LIB)
    message(STATUS "Finding Boost::filesystem")
    find_package(Boost REQUIRED COMPONENTS filesystem system)
ENDIF()

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
    PUBLIC $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/conf>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
IF(BOOST_FILESYSTEM_LIB)
    message(STATUS "Linking with Boost::filesystem")
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Linking with stdc++fs")
    target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)
ENDIF()

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
IF(BOOST_FILESYSTEM_LIB)
    message(STATUS "Linking with Boost::filesystem")
    target_link_libraries(yaml_template_generator PUBLIC Boost::filesystem Boost::system)
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Linking with stdc++fs")
    target_link_libraries(yaml_template_generator PUBLIC stdc++fs)
ENDIF()

# ------ INSTALL ------
#install headers
install(
    DIRECTORY include/${PROJECT_NAME} 
    DESTINATION include)
#install config.h
INSTALL(
    FILES ${YAML_SCHEMA_CONFIG_DIR}/config.h
    DESTINATION include/${PROJECT_NAME}/internal)
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
IF(BOOST_FILESYSTEM_LIB)
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
ELSE()
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/NoBoostConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
ENDIF()


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