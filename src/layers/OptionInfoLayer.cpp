#include "OptionInfoLayer.hpp"

#include <Geode/Geode.hpp>

#include "../layers/FancyExportLayer.hpp"

#define WINDOW_WIDTH 300.0f
#define WINDOW_HEIGHT 240.0f
#define SIDE_PADDING 10.0f
#define TOP_PADDING 12.0f
#define BOTTOM_PADDING 16.0f

#define TEXTBOX_WIDTH 270.0f
#define TEXTBOX_GAP 6.0f
#define TEXT_SCALE 0.9f

#define MISSING_INFO "Could not find an info message for this option."

using std::string;

static std::optional<std::unordered_map<string, string>> s_help_strings = std::nullopt;

std::unordered_map<string, string> const& get_help_strings() {
    
    if (!s_help_strings.has_value()) {
        auto unjoined_map = geode::utils::file::readFromJson<std::unordered_map<string, std::vector<string>>>(
            geode::Mod::get()->getResourcesDir() / "options_help.json"
        ).unwrap();
        
        std::unordered_map<string, string> joined_map;
        for (auto it = unjoined_map.begin(); it != unjoined_map.end(); it++) {
            auto pair = *it;
            std::ostringstream os;
            for (int i = 0; i < pair.second.size(); i++) {
                if (i != 0) {
                    os << ' ';
                }
                os << pair.second[i];
            }
            joined_map.insert_or_assign(pair.first, os.str());
        }
        s_help_strings = joined_map;
        geode::log::info("Initialised s_help_strings");
    }
    return *s_help_strings;
}

bool OptionInfoLayer::setup(ProcessingOption option) {
    auto image = cocos2d::CCSprite::createWithSpriteFrameName("jam.png"_spr);
    
    string info_key;
    switch (option) {
    case ProcessingOption::FixLayers:
        info_key = "fix_layers";
        break;
    case ProcessingOption::FixWhite:
        info_key = "fix_white";
        break;
    case ProcessingOption::FixWavyBlocks:
        info_key = "fix_wavy_blocks";
        break;
    case ProcessingOption::UnfixGlow:
        info_key = "unfix_glow";
        break;
    case ProcessingOption::UnfixUncolored3D:
        info_key = "unfix_uncolored_3d";
        break;
    default:
        info_key = "";
        break;
    }
    
    auto help_strings = get_help_strings();
    string info_string;
    try {
        info_string = help_strings.at(info_key);
    }
    catch (std::out_of_range) {
        info_string = MISSING_INFO;
    }
    
    m_mainLayer->addChildAtPosition(
        image, geode::Anchor::Top,
        ccp(0.0f, -(image->getContentSize().height*0.5f + TOP_PADDING))
    );
    
    auto textbox_offset = TOP_PADDING + image->getContentSize().height + TEXTBOX_GAP;
    
    auto text = geode::SimpleTextArea::create(info_string, "chatFont.fnt", TEXT_SCALE, TEXTBOX_WIDTH);  
    text->setWrappingMode(geode::WrappingMode::SPACE_WRAP);
    auto max_height = WINDOW_HEIGHT - textbox_offset - BOTTOM_PADDING;
    
    // doesn't have an auto size, so we just do it the stupid way
    if (text->getContentSize().height > max_height) {
        auto is_outside = true;
        auto base = TEXT_SCALE;
        auto offset = 0.4f;
        for (int i = 0; i < 6; i++) {
            if (is_outside) {
                base -= offset;
                text->setScale(base);
                is_outside = text->getContentSize().height > max_height;
            }
            else {
                text->setScale(base + offset);
                if (text->getContentSize().height <= max_height) {
                    base += offset;
                }
            }
            offset *= 0.5f;
        }
        text->setScale(base);
    }
    
    m_mainLayer->addChildAtPosition(
        text, geode::Anchor::Top,
        ccp(0.0f, -(textbox_offset + text->getContentSize().height*0.5f))
    );
    
    auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("close-button-dusk.png"_spr);
    setCloseButtonSpr(close_sprite, 0.8f);
    
    return true;
}
