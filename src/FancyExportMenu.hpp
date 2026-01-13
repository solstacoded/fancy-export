#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/utils/cocos.hpp>

#include "server.hpp"
#include "LevelObject.hpp"
#include "ObjectHelper.hpp"

class FancyExportMenu : public geode::Popup<GJGameLevel const*> {
protected:
    GJGameLevel const* m_level = nullptr;
    geode::EventListener<geode::utils::web::WebTask> m_listener;
    geode::TextInput* m_username_input = nullptr;
    geode::TextInput* m_password_input = nullptr;
    geode::TextInput* m_level_name_input = nullptr;
    CCMenuItemSpriteExtra* m_login_button = nullptr;
    
    // whatever the fuck geode::Popup<...> does here fucks up touch prio. so we just don't do that
    void registerWithTouchDispatcher() override {
        FLAlertLayer::registerWithTouchDispatcher();
    }
    
    bool setup(GJGameLevel const* level) override {
        if (level == nullptr) {
            return false;
        }
        m_level = level;
        this->setTitle("Fancy Export");
        
        auto login_layer = cocos2d::CCLayer::create();
        login_layer->setLayout(
            geode::ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAutoScale(false)
                ->setAutoGrowAxis(0.0f)
                ->setGap(14.0f)
        );
        //auto label = cocos2d::CCLabelBMFont::create(":33", "bigFont.fnt");
        //label->limitLabelWidth(120.0f, 0.8f, 0.1f);
        //label->setScale(0.001f);
        //login_layer->addChild(label);
        
        m_username_input = geode::TextInput::create(160.0f, "Username");
        m_username_input->setLabel("Username");
        m_username_input->setCommonFilter(geode::CommonFilter::Name);
        m_username_input->setMaxCharCount(16);
        m_username_input->setEnabled(true);
        login_layer->addChild(m_username_input);
        
        m_password_input = geode::TextInput::create(160.0f, "Password");
        m_password_input->setLabel("Password");
        // not technically correct for passwords but i can't be bothered doing it properly right now
        m_password_input->setCommonFilter(geode::CommonFilter::Any);
        m_password_input->setMaxCharCount(20);
        m_password_input->setPasswordMode(true);
        m_password_input->setEnabled(true);
        login_layer->addChild(m_password_input);
        
        m_level_name_input = geode::TextInput::create(160.0f, "Level Name");
        m_level_name_input->setLabel("Level Name");
        m_level_name_input->setCommonFilter(geode::CommonFilter::Name);
        m_level_name_input->setMaxCharCount(20);
        m_level_name_input->setEnabled(true);
        login_layer->addChild(m_level_name_input);
        
        m_login_button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Upload", "goldFont.fnt", "GJ_button_04.png", 0.6f),
            this, menu_selector(FancyExportMenu::onLoginButton)
        );
        auto login_button_menu = cocos2d::CCMenu::create();
        login_button_menu->setTouchPriority(-503);
        login_button_menu->setLayout(
            geode::ColumnLayout::create()
                ->setAutoGrowAxis(0.0f)
        );
        login_button_menu->addChild(m_login_button);
        login_button_menu->updateLayout();
        login_layer->addChild(login_button_menu);
        
        login_layer->updateLayout();
        auto x_offset = 10.0f + login_layer->getContentSize().width * 0.5f;
        auto y_offset = 45.0f + login_layer->getContentSize().height * 0.5f;
        m_mainLayer->addChildAtPosition(
            login_layer,
            geode::Anchor::TopRight,
            cocos2d::CCPoint(-x_offset, -y_offset)
        );
        
        // testing
        auto helper = obj_helper::get_shared_helper();
        
        return true;
    }
    
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
    }
    
public:
    static FancyExportMenu* create(GJGameLevel const* level) {
        
        auto ret = new FancyExportMenu();
        if (ret->initAnchored(400.f, 240.f, level, "square-sunset.png"_spr)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
