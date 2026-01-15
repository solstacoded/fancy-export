#include "FancyExportLayer.hpp"

#include <Geode/Geode.hpp>

#define WINDOW_WIDTH 420.0f
#define WINDOW_HEIGHT 240.0f
#define SIDE_PADDING 10.0f
#define TOP_PADDING 36.0f
#define BOTTOM_PADDING 12.0f

#define LAYER_HEIGHT (WINDOW_HEIGHT - BOTTOM_PADDING - TOP_PADDING)

#define EXPORT_LAYER_WIDTH 180.0f
#define OPTIONS_LAYER_WIDTH (WINDOW_WIDTH - EXPORT_LAYER_WIDTH - 3*SIDE_PADDING)

#define OPTIONS_MEMBER_GAP 6.0f
#define OPTIONS_GAP 16.0f

#define TEXT_LABEL_HEIGHT 12.0f
#define EXPORT_TEXTBOX_GAP 4.0f
#define EXPORT_UPLOAD_GAP (12.0f - EXPORT_TEXTBOX_GAP)


void FancyExportLayer::addOption(ProcessingOption option) {
    char const* label_string;
    switch (option) {
    case ProcessingOption::FixLayers:
        label_string = "Fix Layers";
        break;
    case ProcessingOption::FixWhite:
        label_string = "Fix White Channel";
        break;
    case ProcessingOption::FixWavyBlocks:
        label_string = "Fix Wavy Blocks";
        break;
    case ProcessingOption::UnfixGlow:
        label_string = "Unfix Object Glow";
        break;
    case ProcessingOption::UnfixUncolored3D:
        label_string = "Unfix Uncolored 3D";
        break;
    }
    auto checkbox = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(FancyExportLayer::onOptionsToggle), 0.6f
    );
    checkbox->setTag(static_cast<int>(option));
    auto info_button_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    info_button_sprite->setScale(0.5f);
    auto info_button = CCMenuItemSpriteExtra::create(
        info_button_sprite, this, menu_selector(FancyExportLayer::onOptionsInfo)
    );
    info_button->setTag(static_cast<int>(option));
    
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
    
    menu->addChild(info_button);
    menu->addChild(checkbox);
    menu->addChild(label);
    menu->updateLayout();
    m_options_layer->addChild(menu);
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
    
    addOption(ProcessingOption::FixLayers);
    addOption(ProcessingOption::FixWhite);
    addOption(ProcessingOption::FixWavyBlocks);
    addOption(ProcessingOption::UnfixGlow);
    addOption(ProcessingOption::UnfixUncolored3D);
    
    m_options_layer->updateLayout();
    m_mainLayer->addChildAtPosition(
        m_options_layer, geode::Anchor::BottomLeft,
        ccp(OPTIONS_LAYER_WIDTH*0.5f + SIDE_PADDING, LAYER_HEIGHT*0.5f + BOTTOM_PADDING)
    );
    
    // == right hand side - logins and (future) gmd export
    m_export_layer = cocos2d::CCLayer::create();
    m_export_layer->setLayout(
        geode::AnchorLayout::create()
    );
    m_export_layer->setContentSize(ccp(EXPORT_LAYER_WIDTH, LAYER_HEIGHT));
    
    m_username_input = geode::TextInput::create(
        EXPORT_LAYER_WIDTH, "Username", "bigFont.fnt"
    );
    m_username_input->setLabel("2.2 Username");
    m_username_input->setCommonFilter(geode::CommonFilter::Name);
    m_username_input->setMaxCharCount(16);
    
    auto text_input_height = m_username_input->getContentSize().height;
    auto prev_text_inputs = TEXT_LABEL_HEIGHT + text_input_height + EXPORT_TEXTBOX_GAP;
    // manually positioning elements because columnlayout was being really annoying
    m_export_layer->addChildAtPosition(
        m_username_input, geode::Anchor::Top,
        ccp(0.0f, -(TEXT_LABEL_HEIGHT + 0.5*text_input_height))
    );
    
    m_password_input = geode::TextInput::create(
        EXPORT_LAYER_WIDTH, "Password", "bigFont.fnt"
    );
    m_password_input->setLabel("2.2 Password");
    m_password_input->setCommonFilter(geode::CommonFilter::Any);
    m_password_input->setMaxCharCount(20);
    
    m_export_layer->addChildAtPosition(
        m_password_input, geode::Anchor::Top,
        ccp(0.0f, -(TEXT_LABEL_HEIGHT + 0.5*text_input_height + prev_text_inputs))
    );
    
    m_level_name_input = geode::TextInput::create(
        EXPORT_LAYER_WIDTH, "Level Name", "bigFont.fnt"
    );
    m_level_name_input->setLabel("Level Name");
    m_level_name_input->setCommonFilter(geode::CommonFilter::Name);
    m_level_name_input->setMaxCharCount(20);
    m_level_name_input->setString(m_level->m_levelName);
    
    m_export_layer->addChildAtPosition(
        m_level_name_input, geode::Anchor::Top,
        ccp(0.0f, -(TEXT_LABEL_HEIGHT + 0.5*text_input_height + 2*prev_text_inputs))
    );
    
    auto upload_menu = cocos2d::CCMenu::create();
    upload_menu->setLayout(
        geode::RowLayout::create()
            ->setAutoScale(false)
            ->setAxisAlignment(geode::AxisAlignment::Start)
            ->setCrossAxisOverflow(true)
            ->setGap(6.0f)
    );
    auto upload_button_sprite = ButtonSprite::create("Upload", "goldFont.fnt", "button-dusk.png"_spr, 0.6f);
    auto upload_button = CCMenuItemSpriteExtra::create(
        upload_button_sprite, this, menu_selector(FancyExportLayer::onUploadButton)
    );
    upload_button->setLayoutOptions(
        geode::AxisLayoutOptions::create()
            ->setNextGap(16.0f)
    );
    m_unlisted_toggle = CCMenuItemToggler::createWithStandardSprites(this, nullptr, 0.6f);
    
    auto unlisted_label = cocos2d::CCLabelBMFont::create("Unlisted", "bigFont.fnt");
    unlisted_label->setScale(0.4f);
    upload_menu->addChild(upload_button);
    upload_menu->addChild(m_unlisted_toggle);
    upload_menu->addChild(unlisted_label);
    upload_menu->setContentSize(ccp(EXPORT_LAYER_WIDTH, 0.0f));
    upload_menu->updateLayout();
    
    m_export_layer->addChildAtPosition(
        upload_menu, geode::Anchor::Top,
        ccp(0.0f, -(0.5*upload_menu->getContentSize().height + 3*prev_text_inputs + EXPORT_UPLOAD_GAP))
    );
    
    m_export_layer->updateLayout();
    m_mainLayer->addChildAtPosition(
        m_export_layer, geode::Anchor::BottomRight,
        ccp(-(EXPORT_LAYER_WIDTH*0.5f + SIDE_PADDING), LAYER_HEIGHT*0.5f + BOTTOM_PADDING)
    );
    
    auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("close-button-dusk.png"_spr);
    setCloseButtonSpr(close_sprite, 0.8f);
    
    return true;
}
