#pragma once

#include <Geode/Geode.hpp>

#ifdef GEODE_IS_ANDROID
#define FANCY_EXPORT_COMPRESS_MINIZ
#endif

std::string gzipBase64(std::string const& data);
