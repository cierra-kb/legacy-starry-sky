#include "modules/misc.hpp"

bool LevelInfoLayer_init(LevelInfoLayer* self, GJGameLevel* lvl)
{
    if (!starry_sky::get_hook_manager()->orig<&LevelInfoLayer_init>(self, lvl))
        return false;
    
    if (Settings::instance()->is_option_enabled("level_copying"))
    {
        CCMenu* menu = CCMenu::create();

        auto copy_btn = CCMenuItemSpriteExtra::create(CCSprite::create("GJ_button_04.png"), nullptr, (CCObject*)self, menu_selector(LevelInfoLayer::onClone));
        menu->addChild(copy_btn);
        copy_btn->setPosition(menu->convertToNodeSpace(ccp(30, 0)).x, 0);

        self->addChild(menu);
    }

    return true;
}

bool pugi__xml_document__save_file(void* self, const char* a2, const char* a3, uint a4, int a5)
{
    rename(a2, CCString::createWithFormat("%s.bak", a2)->getCString());
    return starry_sky::get_hook_manager()->orig<&pugi__xml_document__save_file>(self, a2, a3, a4, a5);
}

bool ds_dictionary__loadRootSubDictFromFile(void* self, const char* a2)
{
    if (starry_sky::get_hook_manager()->orig<&ds_dictionary__loadRootSubDictFromFile>(self, a2))
        return true;

    a2 = CCString::createWithFormat("%s.bak", a2)->getCString();

    return starry_sky::get_hook_manager()->orig<&ds_dictionary__loadRootSubDictFromFile>(self, a2);
}

class _callback : CCLayer
{
public:
    void open_mod_menu()
    {
        CCDirector::sharedDirector()->getRunningScene()->addChild(MenuSettings::create());
    }
};

void OptionsLayer_customSetup(void* self)
{
    starry_sky::get_hook_manager()->orig<&OptionsLayer_customSetup>(self);

    CCSize win_size = CCDirector::sharedDirector()->getWinSize();
    CCSprite* settings_spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");

    auto settings_btn = CCMenuItemSpriteExtra::create(settings_spr,
        nullptr, (CCObject*)self, menu_selector(_callback::open_mod_menu));
    settings_btn->setScale(0.75);
    settings_btn->setOriginalScale(0.75);

    CCMenu* menu = CCMenu::create();
    menu->addChild(settings_btn);
    
    menu->setPosition({win_size.width - 30,
        win_size.height - 30});
    
    ((CCNode*)self)->addChild(menu);
}

void d(void* self, float s)
{
    starry_sky::get_hook_manager()->orig<&d>(self, 1.0);
}

void MiscModule::init(DobbyWrapper* hook_manager)
{
    hook_manager
        ->add_hook(&LevelInfoLayer::init, &LevelInfoLayer_init)
        //->add_hook("_ZN13DS_Dictionary23loadRootSubDictFromFileEPKc", &ds_dictionary__loadRootSubDictFromFile)
        //->add_hook("_ZNK4pugi12xml_document9save_fileEPKcS2_jNS_12xml_encodingE", &pugi__xml_document__save_file)
        ->add_hook("_ZN12OptionsLayer11customSetupEv", &OptionsLayer_customSetup)
        ->add_hook("_ZN7cocos2d10CCDirector21setContentScaleFactorEf", &d);
}
