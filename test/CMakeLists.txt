# Retrieve googletest from github & compile
add_subdirectory(gtest)

# # Add here the source (.cpp) files in dummy folder
# set(SRC_DUMMY
#   dummy/some_class_dummy.cpp
#   dummy/another_class_dummy.cpp
#   )
# add_library(dummy SHARED ${SRC_DUMMY})
# target_link_libraries(dummy ${PROJECT_NAME})

############# USE THIS TEST AS AN EXAMPLE ###############
#                                                       #
# Create a specific test executable for gtest_example   #
add_gtest(gtest_example gtest_example.cpp)              #
#                                                       #
# OPTIONAL: if the gtest depends on the dummy library   #
# (used for implementing pure virtual base classes),    #
# add a line                                            #
# target_link_libraries(gtest_example dummy)            #
#                                                       #
#########################################################

add_gtest(gtest_add_node_schema gtest_add_node_schema.cpp)
add_gtest(gtest_add_node_yaml gtest_add_node_yaml.cpp)
add_gtest(gtest_apply_schema gtest_apply_schema.cpp)
add_gtest(gtest_check_type gtest_check_type.cpp)
add_gtest(gtest_conversions gtest_conversions.cpp)
add_gtest(gtest_duplicated_keys gtest_duplicated_keys.cpp)
add_gtest(gtest_expression gtest_expression.cpp)
add_gtest(gtest_find_nodes_with_key gtest_find_nodes_with_key.cpp)
add_gtest(gtest_flatten gtest_flatten.cpp)
add_gtest(gtest_generator gtest_generator.cpp)
add_gtest(gtest_own_type gtest_own_type.cpp)
add_gtest(gtest_relative_path gtest_relative_path.cpp)
add_gtest(gtest_schema gtest_schema.cpp)
add_gtest(gtest_type_derived gtest_type_derived.cpp)