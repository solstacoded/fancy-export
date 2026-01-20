#include "OptionInfoLayer.hpp"

#include <Geode/Geode.hpp>

#include "../layers/FancyExportLayer.hpp"
#include "../classes/TextArea.hpp"

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
    
    string info_key;
    char const* image_path;
    switch (option) {
    case ProcessingOption::FixLayers:
        info_key = "fix_layers";
        image_path = "fix_layers.png"_spr;
        break;
    case ProcessingOption::FixWhite:
        info_key = "fix_white";
        image_path = "fix_white.png"_spr;
        break;
    case ProcessingOption::FixWavyBlocks:
        info_key = "fix_wavy_blocks";
        image_path = "fix_wavy_blocks.png"_spr;
        break;
    case ProcessingOption::UnfixGlow:
        info_key = "unfix_glow";
        image_path = "unfix_glow.png"_spr;
        break;
    case ProcessingOption::UnfixUncolored3D:
        info_key = "unfix_uncolored_3d";
        image_path = "unfix_uncolored_3d.png"_spr;
        break;
    default:
        info_key = "";
        image_path = "jam.png"_spr;
        break;
    }
    
    auto image = cocos2d::CCSprite::createWithSpriteFrameName(image_path);
    
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
    
    auto text = SolstaTextArea::create(info_string, "chatFont.fnt", TEXT_SCALE, TEXTBOX_WIDTH);
    text->setIsJustified(true);
    text->setParagraphPadding(4.0f);
    
    auto max_height = WINDOW_HEIGHT - textbox_offset - BOTTOM_PADDING;
    
    text->limitLabelHeight(max_height, TEXT_SCALE, 0.1f);
    
    m_mainLayer->addChildAtPosition(
        text, geode::Anchor::Top,
        ccp(0.0f, -(textbox_offset + text->getContentSize().height*0.5f))
    );
    
    auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("close-button-dusk.png"_spr);
    setCloseButtonSpr(close_sprite, 0.8f);
    
    return true;
}
