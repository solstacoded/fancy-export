#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>

$execute {
    new geode::EventListener<geode::EventFilter<geode::ModLogoUIEvent>>(+[](geode::ModLogoUIEvent* event) {
        
        if (event->getModID() == "solstacoded.fancy-export") {
            if (auto sprite = event->getSprite()) {
                auto angle = 360.0f * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
                sprite->setRotation(angle);
            }
        }

        return geode::ListenerResult::Propagate;
    });
}
