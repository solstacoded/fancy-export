#pragma once

#include <string>

#define FANCY_EXPORT_COMPRESS_MINIZ

std::string gzipBase64(std::string const& data);
