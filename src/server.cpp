#include "server.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/base64.hpp>

#include "utils/sha1.hpp"
#include "utils/Spliterator.hpp"

using std::string;
using std::string_view;

const string gameVersion = "22";
const string binaryVersion = "42";
const string SERVER_ROOT = "https://www.boomlings.com/database/";


string make_gjp2(string const& password) {
    SHA1 checksum;
    checksum.update(password);
    checksum.update("mI29fmAnxgTs");
    return checksum.final();
}


const char* LEVEL_CHK_XOR_CIPHER = "41274";

string make_level_seed(string const& level_string) {
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
static std::string urlParamEncode(string_view input) {
    std::ostringstream ss;
    ss << std::hex << std::uppercase;
    for (char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            ss << c;
        } else if (c == ' ') {
            ss << '+';
        } else {
            ss << '%' << static_cast<int>(c);
        }
    }
    return ss.str();
}

inline string key_val(string_view const key, string_view const val, bool first=false) {
    if (first) {
        return urlParamEncode(key) + "=" + urlParamEncode(val);
    }
    else {
        return "&" + urlParamEncode(key) + "=" + urlParamEncode(val);
    }
}

void server::attempt_login(
    string const username, string const password,
    geode::EventListener<geode::utils::web::WebTask>& listener,
    std::function<void(geode::Result<server::AccountLogin, string>)> callback
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
                    callback(geode::Err("You should never get this error"));
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
                        callback(geode::Err("Server returned unexpected value"));
                    }
                    return;
                }
                callback(geode::Ok(AccountLogin(username, gjp2, string(*account_id), string(*player_id))));
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

void server::attempt_upload_level(
    GJGameLevel const* level, string const name, string const& level_string,
    server::AccountLogin const& login, bool unlisted,
    geode::EventListener<geode::utils::web::WebTask>& listener,
    std::function<void(geode::Result<int, string>)> callback
) {
    geode::utils::web::WebRequest req = geode::utils::web::WebRequest();
    
    auto level_seed = make_level_seed(level_string);
    
    auto params = key_val("gameVersion", gameVersion, true)
        + key_val("accountID", login.m_account_id)
        + key_val("gjp2", login.m_gjp2)
        + key_val("userName", login.m_username)
        + key_val("levelID", "0")
        + key_val("levelName", name)
        + key_val("levelDesc", geode::utils::base64::encode(level->m_levelDesc))
        + key_val("levelVersion", "1")
        + key_val("levelLength", std::to_string(level->m_levelLength))
        + key_val("audioTrack", std::to_string(level->m_audioTrack))
        + key_val("auto", "0")
        + key_val("password", "1")
        + key_val("original", "0")
        + key_val("twoPlayer", level->m_twoPlayerMode ? "1" : "0")
        + key_val("songID", std::to_string(level->m_songID))
        + key_val("objects", std::to_string(level->m_objectCount))
        + key_val("coins", "0")
        + key_val("requestedStars", "0")
        + key_val("unlisted", unlisted ? "1" : "0")
        + key_val("ldm", "0")
        + key_val("levelString", level_string)
        + key_val("seed2", level_seed)
        + key_val("secret", "Wmfd2893gb7")
        + key_val("binaryVersion", binaryVersion)
        + key_val("gdw", "0");
    
    //geode::log::debug("{}", params);
    
    req.bodyString(params);
    
    listener.bind([callback] (geode::utils::web::WebTask::Event* e) {
        if (geode::utils::web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                auto str = res->string().unwrapOr("");
                if (str.empty()) {
                    callback(geode::Err("Unknown server error"));
                    return;
                }
                geode::log::debug("Upload returned response {}", str);
                int response_id;
                try {
                    response_id = std::stoi(str);
                }
                catch (std::invalid_argument const& ex) {
                    callback(geode::Err("Server returned unexpected value"));
                    return;
                }
                if (response_id < 0) {
                    callback(geode::Err("Failed to upload level"));
                }
                else {
                    callback(geode::Ok(response_id));
                }
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
    listener.setFilter(req.post(SERVER_ROOT + "uploadGJLevel21.php"));
}
