#pragma once

#include <android/log.h>
#include <algorithm>
#include <dlfcn.h>
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

        if constexpr (std::is_member_function_pointer<decltype(T)>::value)
        {
            Dl_info dli = get_dlinfo_from_addr(reinterpret_cast<void*>(hook_addr));

            using Class = typename method_info<decltype(T)>::class_t;

            if ((reinterpret_cast<uintptr_t>(dli.dli_fbase) > reinterpret_cast<uintptr_t>(hook_addr) || dli.dli_fbase == nullptr) &&
                std::is_destructible<Class>::value
            ) {
                hook_addr = reinterpret_cast<uintptr_t>(get_fn_addr_from_vftable<Class>(reinterpret_cast<uintptr_t>(hook_addr)));
            }
        }

        union {
            uintptr_t in;
            decltype(T) out;
        } u = {
            _trampoline_addrs[hook_addr]
        };
        
        return std::bind(u.out, args...)();
    }
};
