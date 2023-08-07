#ifndef __CONSTS_HPP__
#define __CONSTS_HPP__

typedef struct {
    double x_velocity;
    double time_mod;
} Speed_info;

typedef enum {
    Slow = 0,
    Normal = 1,
    Medium = 2,
    Fast = 3,
    VeryFast = 4
} Speed;

constexpr Speed_info SPEEDS[5] = {
    Speed_info { 5.98, 0.7 }, Speed_info { 5.77, 0.9 }, Speed_info { 5.87, 1.1 }, Speed_info { 6, 1.3 }, Speed_info { 6, 1.6 }
};

const int BG_TRIGGER_ID = 9;
const uint HALF_BLOCK_UNIT = 15;
const uint BLOCK_UNIT = HALF_BLOCK_UNIT * 2;
/*
const Option MOD_OPTIONS[5] = {
    Option {"object_bypass", "Object bypass"},
    Option {"16k_fix", "16k fix"},
    Option {"unlisted_objects", "Unlisted objects"},
    Option {"level_copying", "Level copying"},
    Option {"object_counter", "Object counter"}
};
*/
#endif
