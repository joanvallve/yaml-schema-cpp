#pragma once

#include "yaml-schema-cpp/internal/config.h"

#if _FILESYSTEM_LIB == BOOST
    #include <boost/filesystem.hpp>
#elif _FILESYSTEM_LIB == STD
    #include <filesystem>
#else
  #error Unsupported choice setting
#endif

#if _FILESYSTEM_LIB == BOOST
    namespace filesystem = boost::filesystem;
  
#elif _FILESYSTEM_LIB == STD
    namespace filesystem = std::filesystem;
#endif