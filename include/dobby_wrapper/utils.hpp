#pragma once

#include <dlfcn.h>

template <typename T>
T member_by_offset(void* base, unsigned int offset)
{
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
}

void* get_symbol_address(const char* lib, const char* sym)
{
    auto handle = dlopen(lib, RTLD_NOW);
    return dlsym(handle, sym);
}

template <typename T>
void* get_function_address(T arg)
{
    if constexpr (std::is_same<T, const char*>::value)
    {
        return get_symbol_address("libgame.so", arg);
    }
    else if constexpr (std::is_member_function_pointer<T>::value)
    {
        // https://stackoverflow.com/a/8122891
        auto addr = reinterpret_cast<void*>((void*&)arg);
        
        Dl_info dli;
        dladdr(addr, &dli);

        using Class = typename method_info<T>::class_t;

        // If the supposed address of our function is smaller than the base address of
        // it's libary of origin, then we are most certainly dealing with a Virtual Table offset.

        if (reinterpret_cast<uintptr_t>(dli.dli_fbase) > reinterpret_cast<uintptr_t>(addr) &&
            std::is_destructible<Class>::value
        ) {
            // Every class with a virtual function has a Virtual Table.
            // It's address is being held by a class member located at exactly 0x0.
            //
            // We want to get the starting address of our Virtual Table
            // and then add the offset, which should put us at the location of
            // the pointer to our function.

            Class instance = Class();

            auto vtable_ptr = member_by_offset<void*>(&instance, 0);
            auto fn_ptr = reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(vtable_ptr) + reinterpret_cast<uintptr_t>(addr));
            
            return *fn_ptr;
        }

        return addr;
    }
    else
    {
        return (void*)arg;
    }
}
