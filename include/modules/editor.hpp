#ifndef __EDITOR_HPP__
#define __EDITOR_HPP__

#include <cocos2dcpp.h>
#include "consts.hpp"
#include "starry_sky.hpp"
#include "ui/menu_settings.hpp"

struct OrderingData
{
    int order_of_arrival;
    int z_order;
};

class UpdatableLevelEditorLayer : public LevelEditorLayer
{
public:
    static LevelEditorLayer* create(GJGameLevel* lvl)
    {
        auto ulel = new UpdatableLevelEditorLayer();

        if (ulel->init(lvl))
        {
            ulel->autorelease();
            return ulel;
        }

        delete ulel;
        return nullptr;
    }

    virtual void update(float dt) override;
};

class EditorModule : public Module
{
public:
    virtual void init(DobbyWrapper*);
};

#endif
