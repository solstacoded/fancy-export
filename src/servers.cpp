#include "servers.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/base64.hpp>

#include "sha1.hpp"
#include "spliterator.hpp"

using std::string;

const string gameVersion = "22";
const string binaryVersion = "42";
const string SERVER_ROOT = "https://www.boomlings.com/database/";


string make_gjp2(string &password) {
    SHA1 checksum;
    checksum.update(password);
    checksum.update("mI29fmAnxgTs");
    return checksum.final();
}


const char* LEVEL_CHK_XOR_CIPHER = "41274";

string make_level_seed(string &level_string) {
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


string make_uuid4() {
    // i probably shouldn't be using std rand but ¯\_(ツ)_/¯
    return std::format(
        "{:04x}{:04x}-{:04x}-{:04x}-{:04x}-{:04x}{:04x}{:04x}",
        std::rand() & 0xffff, std::rand() & 0xffff, std::rand() & 0xffff,
        (std::rand() & 0x0fff) | 0x4000, (std::rand() & 0x3fff) | 0x8000,
        std::rand() & 0xffff, std::rand() & 0xffff, std::rand() & 0xffff
    );
    
}

// taken from https://github.com/geode-sdk/geode/blob/d21fd36bc77f299d88d704b34dd4380e8f401511/loader/src/utils/web.cpp#L377
static std::string urlParamEncode(std::string_view input) {
    std::ostringstream ss;
    ss << std::hex << std::uppercase;
    for (char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            ss << c;
        } else {
            ss << '%' << static_cast<int>(c);
        }
    }
    return ss.str();
}

inline string key_val(string key, string val, bool first=false) {
    if (first) {
        return urlParamEncode(key) + "=" + urlParamEncode(val);
    }
    else {
        return "&" + urlParamEncode(key) + "=" + urlParamEncode(val);
    }
}

void servers::attempt_login(
    string username, string password,
    geode::EventListener<geode::utils::web::WebTask> &listener,
    std::function<void(geode::Result<servers::AccountLogin, string>)> callback
) {
    geode::utils::web::WebRequest req = geode::utils::web::WebRequest();
    
    auto gjp2 = make_gjp2(password);
    auto params = key_val("secret", "Wmfv3899gc9", true)
        + key_val("gameVersion", gameVersion)
        + key_val("binaryVersion", binaryVersion)
        + key_val("gdw", "0")
        + key_val("udid", make_uuid4())
        + key_val("userName", username)
        + key_val("password", password)
        + key_val("gjp2", gjp2);
    
    req.bodyString(params);
    
    
    listener.bind([username, gjp2, callback] (geode::utils::web::WebTask::Event* e) {
        if (geode::utils::web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                auto str = res->string().unwrapOr("");
                if (str.empty()) {
                    callback(geode::Err("Unknown server error"));
                    return;
                }
                geode::log::debug("Login returned response {}", str);
                Spliterator split(str, ',', false);
                auto account_id = split.next();
                if (!account_id.has_value()) {
                    callback(geode::Err("Unknown server error"));
                    return;
                }
                auto player_id = split.next();
                if (!player_id.has_value()) {
                    auto error_code = *account_id;
                    if (error_code == "-1") {
                        callback(geode::Err("Server replied with generic error code"));
                    }
                    else if (error_code == "-8") {
                        callback(geode::Err("Password too short"));
                    }
                    else if (error_code == "-9") {
                        callback(geode::Err("Username too short"));
                    }
                    else if (error_code == "-11") {
                        callback(geode::Err("Incorrect login"));
                    }
                    else if (error_code == "-12") {
                        callback(geode::Err("Your account is marked as disabled"));
                    }
                    else if (error_code == "-13") {
                        callback(geode::Err("Steam ID mismatch??? You should never get this error"));
                    }
                    else {
                        callback(geode::Err("Unknown server error"));
                    }
                    return;
                }
                callback(geode::Ok(AccountLogin(username, gjp2, *account_id, *player_id)));
                
            }
            else {
                callback(geode::Err(res->string().unwrapOr("Unknown server error")));
            }
        }
        else if (geode::utils::web::WebProgress* p = e->getProgress()) {
            // don't care
        }
        else if (e->isCancelled()) {
            callback(geode::Err("Web task cancelled"));
        }
    });
    listener.setFilter(req.post(SERVER_ROOT + "accounts/loginGJAccount.php"));
}
