#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>

#include "dobby_wrapper/types.hpp"
#include "dobby_wrapper/utils.hpp"

#include "dobby.h"

class DobbyWrapper
{
private:
    std::vector<Hook> _hooks;
    std::unordered_map<uintptr_t /* hook_addr */, uintptr_t /* trampoline_addr */> _trampoline_addrs;

    void _add_hook_to_list(Hook hook)
    {
        _hooks.push_back(hook);
    }

public:

    template <int32_t P = 0, typename T, typename H>
    DobbyWrapper& add_hook(T target, H hook)
    {
        Hook hook_info;

        hook_info.hook_addr = reinterpret_cast<uintptr_t>(get_function_address(hook));

        if constexpr (std::is_same<T, const char*>::value)
            hook_info.original_addr = reinterpret_cast<uintptr_t>(get_symbol_address(target));
        else
            hook_info.original_addr = reinterpret_cast<uintptr_t>(get_function_address(target));
        
        hook_info.prioity = P;

        this->_add_hook_to_list(std::move(hook_info));

        return *this;
    }

    void install_hooks()
    {
        std::sort(_hooks.begin(), _hooks.end(), [](const Hook& a, const Hook& b) {
            return a.prioity < b.prioity;
        });

        for (const Hook& hook_info : this->_hooks)
        {
            void* trampoline = nullptr;

            DobbyHook(
                reinterpret_cast<void*>(hook_info.original_addr),
                reinterpret_cast<void*>(hook_info.hook_addr),
                &trampoline
            );

            _trampoline_addrs[hook_info.hook_addr] = reinterpret_cast<uintptr_t>(trampoline);
        }
    }

    template <auto T, typename... args_t>
    auto orig(args_t&&... args)
    {
        auto hook_addr = reinterpret_cast<uintptr_t>(get_function_address(T));
        auto trampoline_fn = *reinterpret_cast<decltype(T)>(reinterpret_cast<void*>(_trampoline_addrs[hook_addr]));

        return std::bind(trampoline_fn, args...)();
    }
};
