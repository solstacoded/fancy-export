#include "servers.hpp"
#include "sha1.hpp"

std::string make_gjp2(std::string &password) {
    SHA1 checksum;
    checksum.update(password);
    checksum.update("mI29fmAnxgTs");
    return checksum.final();
}
