#include "cocos2dcpp.h"
#include "dobby_wrapper/dobby_wrapper.hpp"

class LoadingLayer
{
public:
    const char* getLoadingString();
};

const char* ll(LoadingLayer* self)
{
    return "hook test";
}

#include <unistd.h>

__attribute__((constructor))
void starry_sky()
{
    static DobbyWrapper hook_manager;
    hook_manager.add_hook(&LoadingLayer::getLoadingString, &ll).install_hooks();
}
