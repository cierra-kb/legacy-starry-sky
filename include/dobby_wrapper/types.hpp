#pragma once

// we cant have a partial template specialization without a primary template
template <typename _> struct method_info;

template <typename R, typename C, typename... A>
struct method_info<R (C::*)(A...)> {
    typedef R return_type;
    typedef C class_t;
    typedef std::tuple<A...> args;
};

struct Hook {
    uintptr_t original_addr;
    uintptr_t hook_addr;
    int32_t prioity;
};
