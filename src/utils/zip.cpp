#include "zip.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/base64.hpp>
#include "miniz.h"

std::string gzipBase64(std::string const& data) {
#ifndef FANCY_EXPORT_COMPRESS_MINIZ
    return cocos2d::ZipUtils::compressString(data, false);
#else
    auto tdefl_flags = TDEFL_DEFAULT_MAX_PROBES;
    
    size_t compressed_len;
    auto compressed = static_cast<char*>(
        tdefl_compress_mem_to_heap(data.c_str(), data.size(), &compressed_len, tdefl_flags)
    );
    
    // miniz provides raw deflate, we have to add the gzip header/footer ourselves
    uint32_t crc = mz_crc32(
        MZ_CRC32_INIT, reinterpret_cast<unsigned char const*>(data.c_str()), data.size()
    );
    uint32_t size_modulo = static_cast<uint32_t>(data.size() & 0xffff'ffff);
    
    geode::log::debug("miniz crc {}, size_modulo {}", crc, size_modulo);
    
    std::string gzip("\x1f\x8b\x08\0\0\0\0\0\0\xff", 10);
    gzip.append(compressed, compressed_len);
    // don't need that anymore
    free(compressed);
    // there's probably a way to do this that isn't manual but
    gzip.push_back((crc      ) & 0xff);
    gzip.push_back((crc >>  8) & 0xff);
    gzip.push_back((crc >> 16) & 0xff);
    gzip.push_back((crc >> 24) & 0xff);
    gzip.push_back((size_modulo      ) & 0xff);
    gzip.push_back((size_modulo >>  8) & 0xff);
    gzip.push_back((size_modulo >> 16) & 0xff);
    gzip.push_back((size_modulo >> 24) & 0xff);
    
    return geode::utils::base64::encode(gzip);
#endif
}