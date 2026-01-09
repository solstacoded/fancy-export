#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

#include "servers.hpp"

class FancyExportMenu : public geode::Popup<GJGameLevel const*> {
protected:
    GJGameLevel const* m_level;
    geode::EventListener<geode::utils::web::WebTask> m_listener;
    geode::TextInput* m_username_input;
    geode::TextInput* m_password_input;
    
    bool setup(GJGameLevel const* level) override {
        if (level == nullptr) {
            return false;
        }
        m_level = level;
        this->setTitle("Empty Window !!!");
        
        auto login_layer = cocos2d::CCLayer::create();
        login_layer->setLayout(geode::ColumnLayout::create());
        
        m_username_input = geode::TextInput::create(160.0f, "username");
        m_username_input->setCommonFilter(geode::CommonFilter::Name);
        m_username_input->setMaxCharCount(16);
        login_layer->addChild(m_username_input);
        
        m_password_input = geode::TextInput::create(160.0f, "password");
        // not technically correct for passwords but i can't be bothered doing it properly right now
        m_password_input->setCommonFilter(geode::CommonFilter::Any);
        m_password_input->setMaxCharCount(20);
        m_password_input->setPasswordMode(true);
        login_layer->addChild(m_password_input);
        
        login_layer->updateLayout();
        auto offset = -1.0f - login_layer->getContentSize().width * 0.5f;
        m_mainLayer->addChildAtPosition(
            login_layer,
            geode::Anchor::Right,
            cocos2d::CCPoint(offset, 0.0f)
        );
        
        return true;
    }
    
    void onLoginButton(cocos2d::CCObject*) {
        
    }
    
    void loginCallback(geode::Result<servers::AccountLogin, string> res) {
        
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
