#ifndef __STARRY_SKY_HPP__
#define __STARRY_SKY_HPP__

#include "dobby_wrapper/dobby_wrapper.hpp"

namespace starry_sky
{
    inline DobbyWrapper* get_hook_manager()
    {
        static DobbyWrapper dw;
        return &dw;
    }
}

#endif
