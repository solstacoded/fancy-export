#include <Geode/Geode.hpp>

#include "layers/FancyExportLayer.hpp"

static bool s_spin = false;

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(FancyEditLevelLayer, EditLevelLayer) {
    void onFancy(cocos2d::CCObject*) {
        FancyExportLayer::create(m_level)->show();
    }
    
    bool init(GJGameLevel* level) {
		if (!EditLevelLayer::init(level)) {
            return false;
        }
        auto button_sprite = geode::CircleButtonSprite::createWithSpriteFrameName(
            "export-icon.png"_spr,
            0.8f,
            geode::CircleBaseColor::Green,
            geode::CircleBaseSize::Medium
        );
        if (!button_sprite) {
            geode::log::warn("failed to load button sprite for whatever reason");
            return true;
        }
        
        // we only have professional code here
        auto inner_sprite = button_sprite->getTopNode();
        
        auto angle = 360.0f * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        inner_sprite->setRotation(angle);
        
        auto spin_mask = s_spin ? 0x0f : 0x7f;
        if ((std::rand() & spin_mask) == 0) {
            auto rotate_action = cocos2d::CCRepeatForever::create(
                cocos2d::CCRotateBy::create(1.0, 10.0)
            );
            inner_sprite->runAction(rotate_action);
            s_spin = true;
        }
        
        auto button = CCMenuItemSpriteExtra::create(
            button_sprite,
            this,
            menu_selector(FancyEditLevelLayer::onFancy)
        );
        auto menu = static_cast<cocos2d::CCMenu*>(this->getChildByID("level-actions-menu"));
        if (menu) {
            menu->addChild(button);
            button->setID("fancy-export-button"_spr);
            menu->updateLayout();
        } else {
            geode::log::warn("couldn't add button");
        }
        return true;
    }
};
