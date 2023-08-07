#ifndef __STARRY_SKY_HPP__
#define __STARRY_SKY_HPP__

#include "dobby_wrapper/dobby_wrapper.hpp"
#include "sqlite3.h"
#include <android/log.h>

#define LOG(msg, ...) __android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", msg, __VA_ARGS__)

namespace starry_sky
{
    inline DobbyWrapper* get_hook_manager()
    {
        static DobbyWrapper dw;
        return &dw;
    }
}

#endif
