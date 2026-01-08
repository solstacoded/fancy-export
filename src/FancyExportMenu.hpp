#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>

class FancyExportMenu : public geode::Popup<GJGameLevel const*> {
protected:
    GJGameLevel const* m_level;
    
    bool setup(GJGameLevel const* level) override {
        if (level == nullptr) {
            return false;
        }
        m_level = level;
        this->setTitle("Empty Window !!!");
        
        auto label = cocos2d::CCLabelBMFont::create(m_level->m_levelName.c_str(), "bigFont.fnt");
        m_mainLayer->addChildAtPosition(label, geode::Anchor::Center);
        
        return true;
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
