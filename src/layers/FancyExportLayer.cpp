#include "FancyExportLayer.hpp"

#include <Geode/Geode.hpp>



#define WINDOW_WIDTH 400.0f
#define WINDOW_HEIGHT 240.0f
#define SIDE_PADDING 10.0f
#define TOP_PADDING 36.0f
#define BOTTOM_PADDING 12.0f

#define LAYER_HEIGHT (WINDOW_HEIGHT - BOTTOM_PADDING - TOP_PADDING)

#define EXPORT_LAYER_WIDTH 180.0f
#define OPTIONS_LAYER_WIDTH (WINDOW_WIDTH - EXPORT_LAYER_WIDTH - 3*SIDE_PADDING)

#define OPTIONS_MEMBER_GAP 8.0f
#define OPTIONS_GAP 10.0f

void add_option(
    FancyExportLayer* window_layer,
    cocos2d::CCLayer* options_layer, char const* label_string,
    cocos2d::SEL_MenuHandler toggle_callback,
    cocos2d::SEL_MenuHandler info_callback
) {
    auto checkbox = CCMenuItemToggler::createWithStandardSprites(
        window_layer, toggle_callback, 0.6f
    );
    auto info_button_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    info_button_sprite->setScale(0.5f);
    auto info_button = CCMenuItemSpriteExtra::create(
        info_button_sprite, window_layer, info_callback
    );
    
    auto used_space = checkbox->getContentSize().width
        + info_button->getContentSize().width
        + 2*OPTIONS_MEMBER_GAP;
    
    auto label = cocos2d::CCLabelBMFont::create(label_string, "bigFont.fnt");
    label->limitLabelWidth(OPTIONS_LAYER_WIDTH - used_space, 0.5f, 0.1f);
    
    auto menu = cocos2d::CCMenu::create();
    menu->setLayout(
        geode::RowLayout::create()
        ->setAxisAlignment(geode::AxisAlignment::Start)
        ->setGap(OPTIONS_MEMBER_GAP)
        ->setAutoScale(false)
        ->setCrossAxisOverflow(true)
        ->setAutoGrowAxis(0.0f)
    );
    
    menu->addChild(checkbox);
    menu->addChild(info_button);
    menu->addChild(label);
    menu->updateLayout();
    options_layer->addChild(menu);
}


bool FancyExportLayer::setup(GJGameLevel const* level) {
    if (level == nullptr) {
        return false;
    }
    m_level = level;
    this->setTitle("Fancy Export");
    
    // == left hand side - options such as layer fixing
    m_options_layer = cocos2d::CCLayer::create();
    m_options_layer->setLayout(
        geode::ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setCrossAxisAlignment(geode::AxisAlignment::Start)
            ->setCrossAxisLineAlignment(geode::AxisAlignment::Start)
            ->setAutoScale(false)
            ->setCrossAxisOverflow(false)
            ->setGap(OPTIONS_GAP)
    );
    m_options_layer->setContentSize(ccp(OPTIONS_LAYER_WIDTH, LAYER_HEIGHT));
    
    add_option(
        this, m_options_layer, "Fix layers",
        menu_selector(FancyExportLayer::onFixLayersToggle),
        menu_selector(FancyExportLayer::onFixLayersInfo)
    );
    add_option(
        this, m_options_layer, "Fix white channel",
        menu_selector(FancyExportLayer::onFixWhiteToggle),
        menu_selector(FancyExportLayer::onFixWhiteInfo)
    );
    add_option(
        this, m_options_layer, "Fix wavy blocks",
        menu_selector(FancyExportLayer::onFixWavyBlocksToggle),
        menu_selector(FancyExportLayer::onFixWavyBlocksInfo)
    );
    add_option(
        this, m_options_layer, "Unfix object glow",
        menu_selector(FancyExportLayer::onUnfixGlowToggle),
        menu_selector(FancyExportLayer::onUnfixGlowInfo)
    );
    add_option(
        this, m_options_layer, "Unfix uncolored 3D",
        menu_selector(FancyExportLayer::onUnfixUncolored3DToggle),
        menu_selector(FancyExportLayer::onUnfixUncolored3DInfo)
    );
    
    m_options_layer->updateLayout();
    m_mainLayer->addChildAtPosition(
        m_options_layer, geode::Anchor::BottomLeft,
        ccp(OPTIONS_LAYER_WIDTH*0.5f + SIDE_PADDING, LAYER_HEIGHT*0.5f + BOTTOM_PADDING)
    );
    
    // == right hand side - logins and (future) gmd export
    m_export_layer = cocos2d::CCLayer::create();
    m_export_layer->setLayout(
        geode::ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(geode::AxisAlignment::Between)
            ->setAutoScale(false)
    );
    m_export_layer->setContentSize(ccp(EXPORT_LAYER_WIDTH, LAYER_HEIGHT));
    
    
    
    m_export_layer->updateLayout();
    m_mainLayer->addChildAtPosition(
        m_export_layer, geode::Anchor::BottomRight,
        ccp(-(EXPORT_LAYER_WIDTH*0.5f + SIDE_PADDING), LAYER_HEIGHT*0.5f + BOTTOM_PADDING)
    );
    
    return true;
}
