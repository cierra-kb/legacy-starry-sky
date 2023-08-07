#pragma once

#include <cocos2dcpp.h>
#include <android/log.h>
#include <cmath>
#include <unordered_map>
#include "settings.hpp"

using namespace cocos2d;

extern bool drawUIRect;

/*
struct Option
{
    const char* key;
    const char* label;
};
*/
class OptionCallback : public CCNode
{
public:
    void toggle_option(CCObject* obj);
};

class OptionColumn : public CCNode
{
public:
    Option _option;
    CCMenuItemToggler* _toggle;

    static CCSize CONTENT_SIZE;
    static float SPACING;

    static OptionColumn* create(Option option);
    static const char* get_label_for_option(const Option& option);

    bool init(Option option);
    virtual void draw();
};

class OptionRow : public CCNode
{
public:
    std::unordered_map<const char* /*key*/, OptionColumn*> _columns;

    static float SPACING;

    static OptionRow* create(std::vector<OptionColumn*> columns);
    bool init(std::vector<OptionColumn*> columns);
    virtual void draw();
};

class OptionTable : public CCNode
{
public:
    std::vector<const char* /*keys*/> _immediate_toggle;
    
    static float SPACING;

    static OptionTable* create(std::vector<Option> options);
    static OptionTable* create(std::vector<Option> options,
        std::vector<const char*> immediate_toggle);
    
    bool init(std::vector<Option> options);
    virtual void draw();
};
