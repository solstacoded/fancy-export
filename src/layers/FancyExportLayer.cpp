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
            ->setAxisAlignment(geode::AxisAlignment::Between)
            ->setAutoScale(false)
            ->setCrossAxisOverflow(false)
    );
    m_options_layer->setContentSize(ccp(OPTIONS_LAYER_WIDTH, LAYER_HEIGHT));
    
    auto silly = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(FancyExportLayer::onFixLayersToggle), 1.0f
    );
    m_options_layer->addChild(silly);
    auto silly2 = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(FancyExportLayer::onFixLayersToggle), 1.0f
    );
    m_options_layer->addChild(silly2);
    
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
