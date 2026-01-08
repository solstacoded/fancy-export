#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/cocos.hpp>
#include "FancyExportMenu.hpp"

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(FancyEditLevelLayer, EditLevelLayer) {
    void onFancy(cocos2d::CCObject*) {
        FancyExportMenu::create(m_level)->show();
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
        
        // only professional code here
        // m_onTop is protected, so we get it the sneaky way
        auto inner_sprite = geode::cocos::getChild(button_sprite, 0);
        if (inner_sprite) {
            auto angle = 360.0f * (float)std::rand() / (float)RAND_MAX;
            inner_sprite->setRotation(angle);
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
