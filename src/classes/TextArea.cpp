#include "TextArea.hpp"

#include <Geode/Geode.hpp>

using std::string;

SolstaTextArea* SolstaTextArea::create(string const& text, string const& font, float scale) {
    return SolstaTextArea::create(text, font, scale, cocos2d::CCDirector::sharedDirector()->getWinSize().width * 0.5f);
}

SolstaTextArea* SolstaTextArea::create(string const& text, string const& font, float scale, float width) {
    SolstaTextArea* instance = new SolstaTextArea();
    
    if (instance->init(text, font, scale, width)) {
        instance->autorelease();
        return instance;
    }

    delete instance;
    return nullptr;
}


bool SolstaTextArea::init(const string& text, const string& font, float scale, float width) {
    m_container = cocos2d::CCNodeRGBA::create();
    if (!m_container) { return true; }
    
    m_text = text;
    m_font = font;
    m_fontScale = scale;
    this->setContentWidth(width);
    this->setAnchorPoint({ 0.5f, 0.5f });
    m_container->setPosition({ 0, 0 });
    this->addChild(m_container);
    
    m_container->setLayout(geode::AnchorLayout::create());
    
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
        line_height = size.height * m_fontScale;
        space_width = size.width * m_fontScale;
        delete test_label;
    }
    if (maxHeight.has_value() && (*maxHeight < line_height)) {
        return false;
    }
    
    std::vector<string> words;
    if (m_text.size() != 0) {
        string next_word;
        
        for (const char c : m_text) {
            if (c == ' ') {
                // does not check if next_word is empty.
                // this is intentional, i like funny double spaces
                words.push_back(next_word);
                next_word = "";
            }
            else if (c == '\n') {
                if (next_word.size() != 0) {
                    words.push_back(next_word);
                }
                words.push_back("\n");
                next_word = "";
            }
            else {
                next_word.push_back(c);
            }
        }
        if (next_word.size() != 0) {
            words.push_back(next_word);
        }
        else if (words.back() == "\n") {
            words.pop_back();
        }
    }
    
    geode::AxisAlignment last_line_alignment;
    switch (m_alignment) {
    case cocos2d::kCCTextAlignmentLeft:
        last_line_alignment = geode::AxisAlignment::Start;
        break;
    case cocos2d::kCCTextAlignmentRight:
        last_line_alignment = geode::AxisAlignment::End;
        break;
    case cocos2d::kCCTextAlignmentCenter:
        last_line_alignment = geode::AxisAlignment::Center;
        break;
    };
    auto line_alignment = m_isJustified ? geode::AxisAlignment::Between : last_line_alignment;
    
    auto target_line_width = this->getContentSize().width;
    
    auto line_offset = 0.0f;
    auto line_width = 0.0f;
    auto line = cocos2d::CCMenu::create();
    
    auto line_padding = m_linePadding * m_fontScale;
    auto paragraph_padding = m_paragraphPadding * m_fontScale;
    
    bool bailed = false;
    for (auto word : words) {
        if (word == "\n") {
            auto next_line_offset = line_offset + line_height + paragraph_padding;
            if (maxHeight.has_value() && (*maxHeight < (next_line_offset + line_height))) {
                bailed = true;
                break;
            }
            
            line->setLayout(
                geode::RowLayout::create()
                    ->setAutoScale(false)
                    ->setAxisAlignment(last_line_alignment)
                    ->setGap(space_width)
                    ->setGrowCrossAxis(false)
                    ->setAutoGrowAxis(std::nullopt)
            );
            line->setContentSize(ccp(target_line_width, line_height));
            line->updateLayout();
            m_container->addChildAtPosition(line, geode::Anchor::Top, ccp(0.0f, -line_offset - line_height*0.5f), true);
            line = cocos2d::CCMenu::create();
            line_offset = next_line_offset;
            line_width = 0.0f;
        } else {
            auto label = cocos2d::CCLabelBMFont::create(word.c_str(), m_font.c_str());
            label->setScale(m_fontScale);
            auto word_width = label->getScaledContentSize().width;
            if ((line_width + word_width) > target_line_width) {
                auto next_line_offset = line_offset + line_height + line_padding;
                if (maxHeight.has_value() && (*maxHeight < (next_line_offset + line_height))) {
                    bailed = true;
                    break;
                }
                line->setLayout(
                    geode::RowLayout::create()
                        ->setAutoScale(false)
                        ->setAxisAlignment(line_alignment)
                        ->setGap(space_width)
                        ->setGrowCrossAxis(false)
                        ->setAutoGrowAxis(std::nullopt)
                );
                line->setContentSize(ccp(target_line_width, line_height));
                line->updateLayout();
                m_container->addChildAtPosition(line, geode::Anchor::Top, ccp(0.0f, -line_offset - line_height*0.5f), true);
                line = cocos2d::CCMenu::create();
                line_offset = next_line_offset;
                line_width = 0.0f;
            }
            line->addChild(label);
            line_width += word_width + space_width;
        }
    }
    
    if (bailed) {
        delete line;
        m_container->removeAllChildren();
        return false;
    }
    if (line->getChildrenCount() != 0) {
        line->setLayout(
            geode::RowLayout::create()
                ->setAutoScale(false)
                ->setAxisAlignment(last_line_alignment)
                ->setGap(space_width)
                ->setGrowCrossAxis(false)
                ->setAutoGrowAxis(std::nullopt)
        );
        line->setContentSize(ccp(target_line_width, line_height));
        line->updateLayout();
        m_container->addChildAtPosition(line, geode::Anchor::Top, ccp(0.0f, -line_offset - line_height*0.5f), true);
    }
    else {
        delete line;
    }
    auto height = line_offset + line_height;
    m_container->setContentSize(ccp(target_line_width, height));
    this->setContentHeight(height);
    m_container->updateLayout();
    
    return true;
}


string const& SolstaTextArea::getFont() {
    return m_font;
}
void SolstaTextArea::setFont(string const& font) {
    m_font = font;
}

string const& SolstaTextArea::getText() {
    return m_text;
}
void SolstaTextArea::setText(string const& text) {
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
