#pragma once

#include <Geode/Geode.hpp>

#include "../server.hpp"
#include "../layers/OptionInfoLayer.hpp"

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

enum ProcessingOption {
    FixLayers = 0,
    FixWhite,
    FixWavyBlocks,
    UnfixGlow,
    UnfixUncolored3D,
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
    //void registerWithTouchDispatcher() override {
    //    FLAlertLayer::registerWithTouchDispatcher();
    //}
    
    bool setup(GJGameLevel const* level) override;
    
    void addOption(ProcessingOption option);
    
    void onOptionsToggle(cocos2d::CCObject* sender) {
        auto toggler = static_cast<CCMenuItemToggler*>(sender);
        auto option = static_cast<ProcessingOption>(toggler->getTag());
        
        switch (option) {
        case ProcessingOption::FixLayers:
            m_processing_options.fix_layers = !toggler->m_toggled;
            geode::log::debug("fix_layers {}", !toggler->m_toggled);
            break;
        case ProcessingOption::FixWhite:
            m_processing_options.fix_white = !toggler->m_toggled;
            geode::log::debug("fix_white {}", !toggler->m_toggled);
            break;
        case ProcessingOption::FixWavyBlocks:
            m_processing_options.fix_wavy_blocks = !toggler->m_toggled;
            geode::log::debug("fix_wavy_blocks {}", !toggler->m_toggled);
            break;
        case ProcessingOption::UnfixGlow:
            m_processing_options.unfix_glow = !toggler->m_toggled;
            geode::log::debug("unfix_glow {}", !toggler->m_toggled);
            break;
        case ProcessingOption::UnfixUncolored3D:
            m_processing_options.unfix_uncolored_3d = !toggler->m_toggled;
            geode::log::debug("unfix_uncolored_3d {}", !toggler->m_toggled);
            break;
        }
    }
    
    void onOptionsInfo(cocos2d::CCObject* sender) {
        auto node = static_cast<cocos2d::CCNode*>(sender);
        auto option = static_cast<ProcessingOption>(node->getTag());
        
        OptionInfoLayer::create(option)->show();
    }
    
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
