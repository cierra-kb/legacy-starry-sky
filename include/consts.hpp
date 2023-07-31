#pragma once

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
