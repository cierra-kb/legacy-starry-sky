#include "ui/menu_settings.hpp"
#include "sqlite3.h"
#include <android/log.h>

using namespace cocos2d;

void MenuSettings::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0x80000004, true);
}

bool MenuSettings::init()
{
    if (!CCLayerColor::initWithColor({0,0,0,125}))
        return false;

    CCSize win_size = CCDirector::sharedDirector()->getWinSize();

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);

    m_pLayer = CCLayer::create();
    
    std::vector<const char*> enabled_option_keys;
    for (const Option* opt : Settings::instance()->get_enabled_options())
    {
        LOG("Immediate toggle: %s", opt->key);

        enabled_option_keys.push_back(opt->key);
    }

    auto column = OptionTable::create(
        Settings::instance()->get_options(),
        enabled_option_keys
    );
    column->setPosition({win_size.width / 2, win_size.height / 2});
    column->setScale(0.8);
    column->setAnchorPoint({0.5, 0.5});

    m_pLayer->addChild(column);

    this->addChild(m_pLayer);

    return true; 
}

void MenuSettings::keyBackClicked()
{
    this->setKeypadEnabled(true);
    this->removeFromParentAndCleanup(true);
}

bool MenuSettings::ccTouchBegan(CCTouch*, CCEvent*)
{
    return true;
}
