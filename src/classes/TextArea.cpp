#include "TextArea.hpp"

#include <Geode/Geode.hpp>

SolstaTextArea* SolstaTextArea::create(std::string const& text, std::string const& font, float scale) {
    return SolstaTextArea::create(text, font, scale, cocos2d::CCDirector::sharedDirector()->getWinSize().width * 0.5f);
}

SolstaTextArea* SolstaTextArea::create(std::string const& text, std::string const& font, float scale, float width) {
    SolstaTextArea* instance = new SolstaTextArea();
    
    if (instance->init(text, font, scale, width)) {
        instance->autorelease();
        return instance;
    }

    delete instance;
    return nullptr;
}


bool SolstaTextArea::init(const std::string& text, const std::string& font, float scale, float width) {
    m_container = cocos2d::CCNodeRGBA::create();
    if (!m_container) { return true; }
    
    m_text = text;
    m_font = font;
    m_fontScale = scale;
    this->setContentWidth(width);
    this->setAnchorPoint({ 0.5f, 0.5f });
    m_container->setPosition({ 0, 0 });
    this->addChild(m_container);
    
    this->updateTextDisplay();
    
    return true;
}


void SolstaTextArea::updateTextDisplay() {
    this->updateTextDisplay(std::nullopt);
    
}


bool SolstaTextArea::updateTextDisplay(std::optional<float> maxHeight) {
    m_container->removeAllChildren();
    
    // could be a better way to do this but whatever
    float line_height;
    float space_width;
    {
        auto test_label = cocos2d::CCLabelBMFont::create(" ", m_font.c_str());
        auto size = test_label->getContentSize();
        line_height = size.height;
        space_width = size.width;
        delete test_label;
    }
    
    /* todo:
        - split text into words and line breaks
        - make the lines
    */
    return false;
}


std::string const& SolstaTextArea::getFont() {
    return m_font;
}
void SolstaTextArea::setFont(std::string const& font) {
    m_font = font;
}

std::string const& SolstaTextArea::getText() {
    return m_text;
}
void SolstaTextArea::setText(std::string const& text) {
    m_text = text;
}

float SolstaTextArea::getFontScale() {
    return m_fontScale;
}
void SolstaTextArea::setFontScale(float fontScale) {
    m_fontScale = fontScale;
}

float SolstaTextArea::getLinePadding() {
    return m_linePadding;
}
void SolstaTextArea::setLinePadding(float linePadding) {
    m_linePadding = linePadding;
}

float SolstaTextArea::getParagraphPadding() {
    return m_paragraphPadding;
}
void SolstaTextArea::setParagraphPadding(float paragraphPadding) {
    m_paragraphPadding = paragraphPadding;
}

bool SolstaTextArea::getIsJustified() {
    return m_isJustified;
}
void SolstaTextArea::setIsJustified(bool isJustified) {
    m_isJustified = isJustified;
}

cocos2d::CCTextAlignment SolstaTextArea::getAlignment() {
    return m_alignment;
}
void SolstaTextArea::setAlignment(cocos2d::CCTextAlignment alignment) {
    m_alignment = alignment;
}
