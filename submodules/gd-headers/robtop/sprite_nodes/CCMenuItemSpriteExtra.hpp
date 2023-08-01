#pragma once

#include <cocos2d.h>

using namespace cocos2d;

class CCMenuItemSpriteExtra : public CCMenuItemSprite
{
public:
    static CCMenuItemSpriteExtra* create(cocos2d::CCNode* sprite, cocos2d::CCNode* unk, cocos2d::CCObject* target, SEL_MenuHandler callback);
};
