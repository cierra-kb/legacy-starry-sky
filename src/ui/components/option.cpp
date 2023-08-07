#include "ui/components/options.hpp"

bool drawUIRect = false;

CCSize OptionColumn::CONTENT_SIZE = { 170, 30 };
float OptionColumn::SPACING = 7.5;

float OptionRow::SPACING = 10.5;

float OptionTable::SPACING = 5.f;


void OptionCallback::toggle_option(CCObject* obj)
{
    CCMenuItemToggler* toggler = static_cast<CCMenuItemToggler*>(obj);
    bool toggle = member_by_offset<bool>(toggler, 324);
    Option* option = static_cast<Option*>(toggler->getUserData());
    
    //toggle ^= 1;

    if ((toggle ^ 1) == true)
        LOG("%s enabled", option->key);
    else
        LOG("%s disabled", option->key);
    
    Settings::instance()->set_option_enabled(option->key, toggle ^ 1);
}


// Column

OptionColumn* OptionColumn::create(Option option)
{
    auto ret = new OptionColumn();
    ret->init(option);
    ret->autorelease();
    return ret;
}

const char* OptionColumn::get_label_for_option(const Option& option)
{
    const std::unordered_map<const char* /*key*/, const char* /*label*/> DICT = {
        std::make_pair("16k_fix", "16k Fix"),
        std::make_pair("object_bypass", "Object bypass"),
        std::make_pair("unlisted_objects", "Unlisted objects"),
        std::make_pair("level_copying", "Level copying"),
        std::make_pair("object_counter", "Object counter")
    };

    auto it = DICT.find(option.key);

    if (it == DICT.end())
        return option.key;
    
    return DICT.at(option.key);
}

bool OptionColumn::init(Option option)
{
    _option = option;

    CCMenu* menu = CCMenu::create();
    menu->setPosition({0,0});
    this->addChild(menu);
    
    menu->setContentSize(CONTENT_SIZE);
    this->setContentSize(CONTENT_SIZE);

    CCSprite* off_sprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    CCSprite* on_sprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    CCNode* callback_object = CCNode::create();
    CCMenuItemToggler* toggle = CCMenuItemToggler::create(off_sprite, on_sprite,
        callback_object, menu_selector(OptionCallback::toggle_option));
    toggle->setUserData(&_option);
    callback_object->setUserData(toggle);
    _toggle = toggle;

    toggle->setPosition({toggle->getContentSize().width / 2, CONTENT_SIZE.height / 2});
    menu->addChild(toggle);

    CCLabelBMFont* label = CCLabelBMFont::create(get_label_for_option(_option), "bigFont.fnt");
    float max_label_width = (CONTENT_SIZE.width - toggle->getContentSize().width) - SPACING;
    float shrinked_label_scale = max_label_width / label->getContentSize().width;
    

    __android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "%f %f %f", max_label_width, label->getContentSize().width, shrinked_label_scale);
    if (label->getContentSize().width >= max_label_width)
        label->setScale(shrinked_label_scale);
    
    if (label->getScale() >= 0.7)
        label->setScale(0.7);
    
    label->setPosition(toggle->getContentSize().width + SPACING, CONTENT_SIZE.height / 2);
    label->setAnchorPoint({0, 0.5});

    this->addChild(label);

    return true;
}

void OptionColumn::draw()
{
    if (!drawUIRect)
        return;
    
    ccDrawInit();
    ccDrawColor4B(255,255,255,255);
    ccDrawRect({0,0}, {this->m_tContentSize.width, this->m_tContentSize.height});
}


// row
OptionRow* OptionRow::create(std::vector<OptionColumn*> columns)
{
    auto ret = new OptionRow();

    ret->init(columns);
    ret->autorelease();

    return ret;
}

bool OptionRow::init(std::vector<OptionColumn*> columns)
{
    this->setContentSize({OptionColumn::CONTENT_SIZE.width * columns.size() + (SPACING * (columns.size() - 1)), 30});

    int count = 0;

    
    for (OptionColumn* column : columns)
    {
        this->addChild(column);
        _columns[column->_option.key] = column;

        column->setPosition({(OptionColumn::CONTENT_SIZE.width + SPACING) * count, 0});
        count += 1;
    }
    

    return true;
}

void OptionRow::draw()
{
    if (!drawUIRect)
        return;
    
    ccDrawInit();
    ccDrawColor4B(255,255,255,255);
    ccDrawRect({0,0}, {this->m_tContentSize.width, this->m_tContentSize.height});
}


// table

OptionTable* OptionTable::create(std::vector<Option> options)
{
    auto ret = new OptionTable();
    ret->init(options);
    ret->autorelease();
    return ret;
}

OptionTable* OptionTable::create(
    std::vector<Option> options,
    std::vector<const char*> immediate_toggle
) {
    auto ret = new OptionTable();
    ret->_immediate_toggle = immediate_toggle;

    ret->init(options);
    ret->autorelease();
    return ret;
}

bool OptionTable::init(std::vector<Option> options)
{
    const int ITEM_PER_ROW = 2;

    int row_count = std::ceil((double) options.size() / (double) ITEM_PER_ROW);
    
    CCSize content_size;
    content_size.width = OptionColumn::CONTENT_SIZE.width * ITEM_PER_ROW + (OptionRow::SPACING * (ITEM_PER_ROW - 1));
    content_size.height = OptionColumn::CONTENT_SIZE.height * row_count + (SPACING * (row_count - 1));
    this->setContentSize(content_size);

    std::vector<OptionColumn*> columns;
    float y_pos = content_size.height; //- OptionColumn::CONTENT_SIZE.height;

    for (Option option : options)
    {
        if (columns.size() == ITEM_PER_ROW)
        {
            OptionRow* row = OptionRow::create(columns);

            for (const char* key : _immediate_toggle)
            {
                auto it = row->_columns.find(key);

                if (it == row->_columns.end())
                    continue;
                
                row->_columns[key]->_toggle->toggle(true);
            }

            row->setAnchorPoint({0,1});
            row->setPosition({ 0, y_pos });
            this->addChild(row);

            y_pos -= OptionColumn::CONTENT_SIZE.height + SPACING;
            columns.clear();
        }

        columns.push_back(OptionColumn::create(option));
    }

    if (columns.size())
    {
        OptionRow* row = OptionRow::create(columns);

        for (const char* key : _immediate_toggle)
        {
            auto it = row->_columns.find(key);

            if (it == row->_columns.end())
                continue;
            
            row->_columns[key]->_toggle->toggle(true);
        }
        
        row->setAnchorPoint({0, 1});
        row->setPosition({0, y_pos});
        this->addChild(row);
    }

    return true;
}

void OptionTable::draw()
{
    if (!drawUIRect)
        return;
    
    ccDrawInit();
    ccDrawColor4B(255,255,0,255);
    ccDrawRect({0,0}, {this->m_tContentSize.width, this->m_tContentSize.height});
}