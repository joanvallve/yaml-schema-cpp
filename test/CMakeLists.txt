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

add_gtest(gtest_duplicated_keys gtest_duplicated_keys.cpp)