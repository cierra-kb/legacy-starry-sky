#pragma once

#include <dlfcn.h>

template <typename T>
T member_by_offset(void* base, unsigned int offset)
{
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
}

Dl_info get_dlinfo_from_addr(void* addr)
{
    Dl_info dli;

    dladdr(addr, &dli);
    return dli;
}

void* get_fn_addr_from_symbol(const char* lib, const char* sym)
{
    auto handle = dlopen(lib, RTLD_NOW);
    return dlsym(handle, sym);
}

template <typename T>
void* get_fn_addr_from_vftable(uintptr_t offset)
{
    T instance = T();

    auto vtable_ptr = member_by_offset<void*>(&instance, 0);
    auto fn_ptr = reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(vtable_ptr) + reinterpret_cast<uintptr_t>(offset));
    
    return *fn_ptr;
}

template <typename T>
void* get_function_address(T arg)
{
    if constexpr (std::is_same<T, const char*>::value)
    {
        return get_fn_addr_from_symbol("libgame.so", arg);
    }
    else if constexpr (std::is_member_function_pointer<T>::value)
    {
        auto addr = reinterpret_cast<void*>((void*&)arg); // https://stackoverflow.com/a/8122891
        Dl_info dli = get_dlinfo_from_addr(addr);

        using Class = typename method_info<T>::class_t;

        if ((reinterpret_cast<uintptr_t>(dli.dli_fbase) > reinterpret_cast<uintptr_t>(addr) || dli.dli_fbase == nullptr) &&
            std::is_destructible<Class>::value
        ) {
            return get_fn_addr_from_vftable<Class>(reinterpret_cast<uintptr_t>(addr));
        }

        return addr;
    }
    else
    {
        return (void*)arg;
    }
}
