#pragma once

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/binding/GJGameLevel.hpp>

using std::string;

namespace server {
    struct AccountLogin {
        string m_username;
        string m_gjp2;
        string m_account_id;
        string m_player_id;
        
        AccountLogin(string username, string gjp2, string account_id, string player_id)
            : m_username(username), m_gjp2(gjp2), m_account_id(account_id), m_player_id(player_id)
        {}
    };
    
    void attempt_login(
        string username, string password,
        geode::EventListener<geode::utils::web::WebTask> &listener,
        std::function<void(geode::Result<AccountLogin, string>)> callback
    );
    
    void attempt_upload_level(
        const GJGameLevel* level, string name, const string &level_string,
        AccountLogin &login,
        geode::EventListener<geode::utils::web::WebTask> &listener,
        std::function<void(geode::Result<int, string>)> callback
    );
}
