#pragma once

#include <Geode/Geode.hpp>

#include "../server.hpp"
#include "../layers/OptionInfoLayer.hpp"
#include "../classes/Popup.hpp"
#include "../classes/ProcessingOptions.hpp"
#include "../classes/TextArea.hpp"

using std::string;

class FancyExportLayer : public not_geode::Popup<GJGameLevel const*> {
protected:
    GJGameLevel const* m_level = nullptr;
    string m_level_string;
    ProcessingOptions m_options_cache = ALL_OPTIONS_OFF;
    
    ProcessingOptions m_processing_options = ALL_OPTIONS_OFF;
    
    geode::EventListener<geode::utils::web::WebTask> m_listener;
    
    cocos2d::CCLayer* m_options_layer = nullptr;
    cocos2d::CCLayer* m_export_layer = nullptr;
    cocos2d::CCLayer* m_upload_info_layer = nullptr;
    
    geode::TextInput* m_username_input = nullptr;
    geode::TextInput* m_password_input = nullptr;
    geode::TextInput* m_level_name_input = nullptr;
    CCMenuItemSpriteExtra* m_upload_button = nullptr;
    CCMenuItemToggler* m_unlisted_toggle = nullptr;
    
    geode::LoadingSpinner* m_upload_throbber;
    SolstaTextArea* m_upload_message;
    cocos2d::CCLabelBMFont* m_unlisted_label;
    
    bool m_menus_enabled = true;
    bool m_force_unlisted = false;
    
    bool setup(GJGameLevel const* level) override;
    
    void addOption(ProcessingOption option);
    
    void onOptionsToggle(cocos2d::CCObject* sender) {
        auto toggler = static_cast<CCMenuItemToggler*>(sender);
        auto option = static_cast<ProcessingOption>(toggler->getTag());
        
        m_processing_options.setOption(option, !toggler->m_toggled);
        geode::log::debug("{} {}", ProcessingOptions::getString(option), !toggler->m_toggled);
    }
    
    void onOptionsInfo(cocos2d::CCObject* sender) {
        auto node = static_cast<cocos2d::CCNode*>(sender);
        auto option = static_cast<ProcessingOption>(node->getTag());
        
        OptionInfoLayer::create(option)->show();
    }
    
    void updateLevelString();
    
    void setMenusEnabled(bool new_state);
    void setUploadMessage(string const& message, bool enableThrobber=false, std::optional<cocos2d::ccColor3B> colorPulse=std::nullopt);
    
    void onUploadButton(cocos2d::CCObject*);
    void onLoginResult(geode::Result<server::AccountLogin, string> res);
    void onUploadResult(geode::Result<int, string> res);
    
public:
    static FancyExportLayer* create(GJGameLevel const* level) {
        
        auto ret = new FancyExportLayer();
        if (ret->initAnchored(420.f, 240.f, level, "square-sunset.png"_spr)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
