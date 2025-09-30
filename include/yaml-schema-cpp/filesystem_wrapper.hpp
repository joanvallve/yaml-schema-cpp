#pragma once

#include "yaml-schema-cpp/internal/config.h"

#if _BOOST_FILESYSTEM_LIB == 1
#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;
#else
#include <filesystem>
namespace filesystem = std::filesystem;
#endif