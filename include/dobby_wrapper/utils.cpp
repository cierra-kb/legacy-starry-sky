#include "utils.hpp"

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
