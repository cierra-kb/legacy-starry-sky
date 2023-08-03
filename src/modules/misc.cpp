#include "modules/misc.hpp"

bool LevelInfoLayer_init(LevelInfoLayer* self, GJGameLevel* lvl)
{
    if (!starry_sky::get_hook_manager()->orig<&LevelInfoLayer_init>(self, lvl))
        return false;
    
    CCMenu* menu = CCMenu::create();

    auto copy_btn = CCMenuItemSpriteExtra::create(CCSprite::create("GJ_button_04.png"), nullptr, (CCObject*)self, menu_selector(LevelInfoLayer::onClone));
    menu->addChild(copy_btn);
    copy_btn->setPosition(menu->convertToNodeSpace(ccp(30, 0)).x, 0);

    self->addChild(menu);

    return true;
}

void MiscModule::init(DobbyWrapper* hook_manager)
{
    hook_manager
        ->add_hook(&LevelInfoLayer::init, &LevelInfoLayer_init);
}
