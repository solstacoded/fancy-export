#include "servers.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/base64.hpp>

#include "sha1.hpp"

std::string make_gjp2(std::string &password) {
    SHA1 checksum;
    checksum.update(password);
    checksum.update("mI29fmAnxgTs");
    return checksum.final();
}

const char* LEVEL_CHK_XOR_CIPHER = "41274";
std::string make_level_seed(std::string &level_string) {
    SHA1 checksum;
    
    if (level_string.length() <= 50) {
        checksum.update(level_string);
    } else {
        auto delta = level_string.length() / 50;
        for (size_t i = 0; i < 50; i++) {
            auto pos = i * delta;
            checksum.update(level_string.substr(pos, 1));
        }
    }
    checksum.update("xI25fpAapCQg");
    
    auto hash = checksum.final();
    
    size_t j = 0;
    for (size_t i = 0; i < hash.length(); i++) {
        hash[i] ^= LEVEL_CHK_XOR_CIPHER[j];
        if (j == 4) { j = 0; } else { j++; }
    }
    return geode::utils::base64::encode(hash);
}
