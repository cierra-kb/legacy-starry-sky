#ifndef __EDITOR_HPP__
#define __EDITOR_HPP__

#include <cocos2dcpp.h>
#include "starry_sky.hpp"

const int BG_TRIGGER_ID = 9;
const uint HALF_BLOCK_UNIT = 15;
const uint BLOCK_UNIT = HALF_BLOCK_UNIT * 2;

struct OrderingData
{
    int order_of_arrival;
    int z_order;
};

class EditorModule : public Module
{
public:
    virtual void init(DobbyWrapper*);
};

#endif
