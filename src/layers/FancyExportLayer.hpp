#pragma once

#include <Geode/Geode.hpp>

#include "../server.hpp"
#include "../classes/LevelObject.hpp"
#include "../classes/ObjectHelper.hpp"

struct ProcessingOptions {
    bool fix_layers = false;
    bool fix_white = false;
    bool fix_wavy_blocks = false;
    bool unfix_uncolored_3d = false;
    bool unfix_glow = false;
    
    bool operator==(ProcessingOptions const& other) {
        return (
            fix_layers == other.fix_layers
            && fix_white == other.fix_white
            && fix_wavy_blocks == other.fix_wavy_blocks
            && unfix_uncolored_3d == other.unfix_uncolored_3d
            && unfix_glow == other.unfix_glow
        );
    }
};

class FancyExportLayer : public geode::Popup<GJGameLevel const*> {
protected:
    GJGameLevel const* m_level = nullptr;
    std::string m_level_string;
    ProcessingOptions m_options_cache;
    
    ProcessingOptions m_processing_options;
    
    geode::EventListener<geode::utils::web::WebTask> m_listener;
    
    cocos2d::CCLayer* m_options_layer = nullptr;
    cocos2d::CCLayer* m_export_layer = nullptr;
    
    geode::TextInput* m_username_input = nullptr;
    geode::TextInput* m_password_input = nullptr;
    geode::TextInput* m_level_name_input = nullptr;
    CCMenuItemSpriteExtra* m_login_button = nullptr;
    
    // whatever the fuck geode::Popup<...> does here fucks up touch prio. so we just don't do that
    void registerWithTouchDispatcher() override {
        FLAlertLayer::registerWithTouchDispatcher();
    }
    
    bool setup(GJGameLevel const* level) override;
    
    // i feel like yanderedev
    void onFixLayersToggle(cocos2d::CCObject*) {
        m_processing_options.fix_layers = !m_processing_options.fix_layers;
    }
    
    void onFixWhiteToggle(cocos2d::CCObject*) {
        m_processing_options.fix_white = !m_processing_options.fix_white;
    }
    
    void onFixWavyBlocksToggle(cocos2d::CCObject*) {
        m_processing_options.fix_wavy_blocks = !m_processing_options.fix_wavy_blocks;
    }
    
    void onUnfixGlowToggle(cocos2d::CCObject*) {
        m_processing_options.unfix_glow = !m_processing_options.unfix_glow;
    }
    
    void onUnfixUncolored3DToggle(cocos2d::CCObject*) {
        m_processing_options.unfix_uncolored_3d = !m_processing_options.unfix_uncolored_3d;
    }
    /*
    void onLoginButton(cocos2d::CCObject*) {
        geode::log::debug("login button pressed !!!");
        m_login_button->setEnabled(false);
        
        geode::log::debug("logging into {}...", m_username_input->getString());
        server::attempt_login(
            m_username_input->getString(),
            m_password_input->getString(),
            m_listener,
            [this](auto res) { this->loginCallback(res); }
        );
    }
    
    void loginCallback(geode::Result<server::AccountLogin, string> res) {
        if (res.isOk()) {
            auto login = *res.ok();
            geode::log::debug("Logged in! acc {}, player {}", login.m_account_id, login.m_player_id);
        
            auto objects = LevelObject::from_level_string(m_level->m_levelString);
            
            auto helper = obj_helper::getSharedHelper();
            // first object is startobject and we don't want to change that
            for (int i = 1; i < objects.size(); i++) {
                objects[i].fix_layers(helper);
                objects[i].fix_white(helper);
                objects[i].fix_wavy_blocks();
                objects[i].unfix_uncolored_3d();
                objects[i].unfix_glow();
            }
            
            auto and_back_again = LevelObject::to_level_string(objects);
            
            server::attempt_upload_level(
                m_level,
                m_level_name_input->getString(),
                and_back_again,
                login,
                m_listener,
                [this](auto res) { this->uploadCallback(res); }
            );
        }
        else {
            auto err = *res.err();
            geode::log::debug("Error: {}", err);
            m_login_button->setEnabled(true);
        }
        
    }
    
    void uploadCallback(geode::Result<int, string> res) {
        if (res.isOk()) {
            auto level_id = *res.ok();
            geode::log::debug("Uploaded to ID {}", level_id);
        }
        else {
            auto err = *res.err();
            geode::log::debug("Error: {}", err);
        }
        m_login_button->setEnabled(true);
    }*/
    
public:
    static FancyExportLayer* create(GJGameLevel const* level) {
        
        auto ret = new FancyExportLayer();
        if (ret->initAnchored(400.f, 240.f, level, "square-sunset.png"_spr)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
