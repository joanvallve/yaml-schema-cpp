include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git 
  GIT_TAG main)

SET(BUILD_GMOCK OFF)  # Disable gmock
SET(INSTALL_GTEST OFF) # Disable installation of googletest

FetchContent_MakeAvailable(googletest)

function(add_gtest target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} PUBLIC gtest_main ${PROJECT_NAME})
  add_test(NAME ${target} COMMAND ${target})
endfunction()
