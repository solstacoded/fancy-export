#include "OptionInfoLayer.hpp"

#include <Geode/Geode.hpp>

#include "../layers/FancyExportLayer.hpp"

#define WINDOW_WIDTH 300.0f
#define WINDOW_HEIGHT 200.0f
#define SIDE_PADDING 10.0f
#define TOP_PADDING 12.0f
#define BOTTOM_PADDING 10.0f

#define TEXTBOX_WIDTH 280.0f
#define TEXTBOX_GAP 3.0f
#define TEXT_SCALE 0.9f

#define TEST_STRING "I am a test string. I am here to test the appearance of information textboxes, to make sure I have enough room to fit whatever information I want to. The target size is around two to three sentences, but I'm not expecting to need that much space. I guess we will see. Buffalo buffalo buffalo buffalo buffalo buffalo buffalo buffalo."

bool OptionInfoLayer::setup(ProcessingOption option) {
    auto image = cocos2d::CCSprite::createWithSpriteFrameName("jam.png"_spr);
    
    m_mainLayer->addChildAtPosition(
        image, geode::Anchor::Top,
        ccp(0.0f, -(image->getContentSize().height*0.5f + TOP_PADDING))
    );
    
    auto textbox_offset = TOP_PADDING + image->getContentSize().height + TEXTBOX_GAP;
    
    auto text = geode::SimpleTextArea::create(TEST_STRING, "chatFont.fnt", TEXT_SCALE, TEXTBOX_WIDTH);
    
    auto max_height = WINDOW_HEIGHT - textbox_offset - BOTTOM_PADDING;
    
    // doesn't have an auto size, so we just do it the stupid way
    if (text->getContentSize().height > max_height) {
        auto is_outside = true;
        auto base = TEXT_SCALE;
        auto offset = TEXT_SCALE * 0.5f;
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
    
    return true;
}
