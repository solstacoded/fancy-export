#pragma once

#include <Geode/Geode.hpp>

#include "../classes/Popup.hpp"
#include "../classes/UnpackedLevel.hpp"

class OptionInfoLayer : public not_geode::Popup<ProcessingOption> {
protected:
    bool setup(ProcessingOption option) override;

public:
    static OptionInfoLayer* create(ProcessingOption option) {
        auto ret = new OptionInfoLayer();
        if (ret->initAnchored(300.f, 240.f, option, "square-sunset-darker.png"_spr)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
