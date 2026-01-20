#pragma once

#include <Geode/Geode.hpp>


class SolstaTextArea : public cocos2d::CCNodeRGBA {
public:
    static SolstaTextArea* create(std::string const& text, std::string const& font = "chatFont.fnt", float scale = 1.0f);
    static SolstaTextArea* create(std::string const& text, std::string const& font, float scale, float width);
    
    std::string const& getFont();
    void setFont(std::string const&);
    std::string const& getText();
    void setText(std::string const&);
    float getFontScale();
    void setFontScale(float);
    float getLinePadding();
    void setLinePadding(float);
    float getParagraphPadding();
    void setParagraphPadding(float);
    bool getIsJustified();
    void setIsJustified(bool);
    cocos2d::CCTextAlignment getAlignment();
    void setAlignment(cocos2d::CCTextAlignment);
    
    void updateTextDisplay();
    
    void limitLabelHeight(float height, float defaultScale, float minScale);
protected:
    std::string m_font;
    std::string m_text;
    float m_fontScale = 1.0f;
    float m_linePadding = 0.0f;
    float m_paragraphPadding = 0.0f;
    bool m_isJustified = false;
    cocos2d::CCTextAlignment m_alignment = cocos2d::kCCTextAlignmentLeft;
    
    cocos2d::CCNodeRGBA* m_container = nullptr;
    
    bool init(const std::string& text, const std::string& font, float scale, float width);
    
    bool updateTextDisplay(std::optional<float> maxHeight);
};

