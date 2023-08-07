#pragma once

#include <cocos2dcpp.h>
#include <cmath>

#include "dobby_wrapper/utils.hpp"
#include "ui/components/options.hpp"
#include "starry_sky.hpp"
#include "consts.hpp"

class MenuSettings : public cocos2d::CCLayerColor
{
private:
    CCMenuItemToggler* m_p16KToggle;
    CCLayer* m_pLayer;
    
public:
    LAYER_CREATE_FUNC(MenuSettings);

    virtual bool init();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(cocos2d::CCTouch*, cocos2d::CCEvent*);
    virtual void registerWithTouchDispatcher();
};
