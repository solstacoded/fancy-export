#include "FancyExportLayer.hpp"

#include <Geode/Geode.hpp>

#include "../server.hpp"
#include "../classes/LevelObject.hpp"
#include "../classes/ObjectHelper.hpp"

using std::string;

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
#define EXPORT_TEXTBOX_GAP 3.0f
#define EXPORT_UPLOAD_GAP (8.0f - EXPORT_TEXTBOX_GAP)

#define THROBBER_SIZE 12.0f
#define UPLOAD_INFO_GAP 6.0f


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
    checkbox->setCascadeOpacityEnabled(true);
    checkbox->setTag(static_cast<int>(option));
    auto info_button_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    info_button_sprite->setScale(0.5f);
    auto info_button = CCMenuItemSpriteExtra::create(
        info_button_sprite, this, menu_selector(FancyExportLayer::onOptionsInfo)
    );
    info_button->setTag(static_cast<int>(option));
    info_button->setLayoutOptions(
        geode::AxisLayoutOptions::create()
            ->setCrossAxisAlignment(geode::AxisAlignment::End)
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
    
    menu->addChild(info_button);
    menu->addChild(checkbox);
    menu->addChild(label);
    menu->setCascadeOpacityEnabled(true);
    menu->updateLayout();
    m_options_layer->addChild(menu);
}


bool FancyExportLayer::setup(GJGameLevel const* level) {
    if (level == nullptr) {
        return false;
    }
    m_level = level;
    m_level_string = m_level->m_levelString;
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
    m_password_input->setPasswordMode(true);
    
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
            ->setGap(5.0f)
    );
    auto upload_button_sprite = ButtonSprite::create("Upload", "goldFont.fnt", "button-dusk.png"_spr, 0.6f);
    upload_button_sprite->setCascadeOpacityEnabled(true);
    m_upload_button = CCMenuItemSpriteExtra::create(
        upload_button_sprite, this, menu_selector(FancyExportLayer::onUploadButton)
    );
    m_upload_button->setLayoutOptions(
        geode::AxisLayoutOptions::create()
            ->setNextGap(16.0f)
    );
    m_unlisted_toggle = CCMenuItemToggler::createWithStandardSprites(this, nullptr, 0.6f);
    m_unlisted_toggle->setCascadeOpacityEnabled(true);
    
    m_unlisted_label = cocos2d::CCLabelBMFont::create("Unlisted", "bigFont.fnt");
    m_unlisted_label->setScale(0.4f);
    // no
    if (!m_level->m_isVerified) {
        m_unlisted_toggle->toggle(true);
        m_unlisted_toggle->setEnabled(false);
        m_unlisted_toggle->setOpacity(150);
        m_unlisted_label->setOpacity(150);
        m_force_unlisted = true;
    }
    upload_menu->addChild(m_upload_button);
    upload_menu->addChild(m_unlisted_toggle);
    upload_menu->addChild(m_unlisted_label);
    upload_menu->setContentSize(ccp(EXPORT_LAYER_WIDTH, 0.0f));
    upload_menu->updateLayout();
    
    m_export_layer->addChildAtPosition(
        upload_menu, geode::Anchor::Top,
        ccp(0.0f, -(0.5*upload_menu->getContentSize().height + 3*prev_text_inputs + EXPORT_UPLOAD_GAP))
    );
    
    m_upload_info_layer = cocos2d::CCLayer::create();
    m_upload_info_layer->setLayout(
        geode::RowLayout::create()
            ->setAutoScale(false)
            ->setAxisAlignment(geode::AxisAlignment::Start)
            ->setCrossAxisOverflow(true)
            ->setGap(UPLOAD_INFO_GAP)
    );
    m_upload_info_layer->getLayout()->ignoreInvisibleChildren(true);
    m_upload_throbber = geode::LoadingSpinner::create(THROBBER_SIZE);
    m_upload_message = geode::SimpleTextArea::create("Hello there", "bigFont.fnt", 0.25f);
    m_upload_message->setWrappingMode(geode::WrappingMode::SPACE_WRAP);
    
    m_upload_info_layer->addChild(m_upload_throbber);
    m_upload_info_layer->addChild(m_upload_message);
    m_upload_info_layer->setContentSize(ccp(EXPORT_LAYER_WIDTH, 0.0f));
    m_upload_info_layer->updateLayout();
    m_upload_info_layer->ignoreAnchorPointForPosition(false);
    
    auto upload_message_offset = (upload_menu->getContentSize().height + 3*prev_text_inputs + EXPORT_UPLOAD_GAP);
    
    m_export_layer->addChildAtPosition(
        m_upload_info_layer, geode::Anchor::Top,
        ccp(0.0f, -(0.5*m_upload_throbber->getContentSize().height + upload_message_offset + 8.0f))
    );
    
    m_export_layer->updateLayout();
    m_mainLayer->addChildAtPosition(
        m_export_layer, geode::Anchor::BottomRight,
        ccp(-(EXPORT_LAYER_WIDTH*0.5f + SIDE_PADDING), LAYER_HEIGHT*0.5f + BOTTOM_PADDING)
    );
    
    auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("close-button-dusk.png"_spr);
    setCloseButtonSpr(close_sprite, 0.8f);
    
    setUploadMessage("");
    
    return true;
}

void FancyExportLayer::updateLevelString() {
    if (m_options_cache == m_processing_options) { return; }
    
    m_options_cache = m_processing_options;
    
    if (m_processing_options == ALL_OPTIONS_OFF) {
        m_level_string = m_level->m_levelString;
        geode::log::debug("All options off");
        return;
    }
    
    geode::log::debug("fix_layers: {}", m_processing_options.fix_layers);
    geode::log::debug("fix_white: {}", m_processing_options.fix_white);
    geode::log::debug("fix_wavy_blocks: {}", m_processing_options.fix_wavy_blocks);
    geode::log::debug("unfix_glow: {}", m_processing_options.unfix_glow);
    geode::log::debug("unfix_uncolored_3d: {}", m_processing_options.unfix_uncolored_3d);
    
    auto objects = LevelObject::from_level_string(m_level->m_levelString);
    
    auto helper = obj_helper::getSharedHelper();
    
    for (int i = 1; i < objects.size(); i++) {
        if (m_processing_options.fix_layers) {
            objects[i].fix_layers(helper);
        }
        if (m_processing_options.fix_white) {
            objects[i].fix_white(helper);
        }
        if (m_processing_options.fix_wavy_blocks) {
            objects[i].fix_wavy_blocks();
        }
        if (m_processing_options.unfix_glow) {
            objects[i].unfix_glow();
        }
        if (m_processing_options.unfix_uncolored_3d) {
            objects[i].unfix_uncolored_3d();
        }
    }
    
    m_level_string = LevelObject::to_level_string(objects);
}


void FancyExportLayer::setMenusEnabled(bool new_state) {
    if (new_state == m_menus_enabled) { return; }
    
    unsigned char opacity = new_state ? 255 : 150;
    
    if (new_state) {
        // forceprio gets set to false when we open and close an info box.
        // this makes textinputs use a prio of 500 which is not good for us.
        // instead of figuring out how to make info boxes not do that we just
        // ensure forceprio is enabled here. hacky fix but it works
        auto td = cocos2d::CCTouchDispatcher::get();
        td->setForcePrio(true);
    }
    
    m_username_input->setEnabled(new_state);
    m_password_input->setEnabled(new_state);
    m_level_name_input->setEnabled(new_state);
    m_upload_button->setEnabled(new_state);
    static_cast<cocos2d::CCNodeRGBA*>(
        m_upload_button->getNormalImage()
    )->setOpacity(opacity);
    
    for (int i = 0; i < m_options_layer->getChildrenCount(); i++) {
        auto option = m_options_layer->getChildByIndex<cocos2d::CCMenu>(i);
        option->setEnabled(new_state);
        option->setOpacity(opacity);
    }
    
    if (!m_force_unlisted) {
        m_unlisted_toggle->setEnabled(new_state);
        m_unlisted_toggle->setOpacity(opacity);
        m_unlisted_label->setOpacity(opacity);
    }
    
    m_menus_enabled = new_state;
}


void FancyExportLayer::setUploadMessage(string const& message, bool enableThrobber, std::optional<cocos2d::ccColor3B> colorPulse) {
    m_upload_throbber->setVisible(enableThrobber);
    m_upload_message->setText(message);
    auto max_width = enableThrobber ?
        (EXPORT_LAYER_WIDTH - UPLOAD_INFO_GAP - THROBBER_SIZE) : EXPORT_LAYER_WIDTH;
    m_upload_message->setWidth(max_width);
    
    m_upload_info_layer->updateLayout();
    
    if (colorPulse.has_value()) {
        auto lines = m_upload_message->getLines();
        for (auto it = lines.begin(); it != lines.end(); it++) {
            auto line = *it;
            line->setColor(*colorPulse);
            line->runAction(
                cocos2d::CCTintTo::create(0.5f, 255, 255, 255)
            );
        }
    }
}


void FancyExportLayer::onUploadButton(cocos2d::CCObject*) {
    auto username = m_username_input->getString();
    if (username.length() < 3) {
        setUploadMessage("Username must be at least 3 characters", false, cocos2d::ccRED);
        return;
    }
    auto password = m_password_input->getString();
    if (password.length() < 6) {
        setUploadMessage("Password must be at least 6 characters", false, cocos2d::ccRED);
        return;
    }
    
    server::attempt_login(
        username, password,
        m_listener,
        [this](auto res) { this->onLoginResult(res); }
    );
    setUploadMessage("Logging in...", true);
    setMenusEnabled(false);
}


void FancyExportLayer::onLoginResult(geode::Result<server::AccountLogin, string> res) {
    if (res.isOk()) {
        // causes a big lagspike with high object levels. will move to a different thread in a future update
        updateLevelString();
        
        auto level_name = m_level_name_input->getString();
        auto unlisted = m_unlisted_toggle->m_toggled;
        
        server::attempt_upload_level(
            m_level, level_name, m_level_string,
            res.unwrap(), unlisted,
            m_listener,
            [this](auto res) { this->onUploadResult(res); }
        );
        setUploadMessage("Uploading level...", true);
    }
    else {
        setUploadMessage(
            std::format("Login failed: {}", res.unwrapErr()),
            false, cocos2d::ccRED
        );
        setMenusEnabled(true);
    }
}


void FancyExportLayer::onUploadResult(geode::Result<int, string> res) {
    if (res.isOk()) {
        setUploadMessage(
            std::format("Successfully uploaded level to 2.2! ({})", res.unwrap()),
            false, cocos2d::ccGREEN
        );
    }
    else {
        setUploadMessage(
            std::format("Login failed: {}", res.unwrapErr()),
            false, cocos2d::ccRED
        );
    }
    
    setMenusEnabled(true);
}
