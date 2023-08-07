#include "starry_sky.hpp"
#include "modules/editor.hpp"
#include "modules/misc.hpp"
#include "settings.hpp"

#include <memory>

__attribute__((constructor))
void stellar_sky()
{
    Settings::instance();

    DobbyWrapper* hook_manager = starry_sky::get_hook_manager();

    hook_manager
        ->add_module(std::make_shared<EditorModule>().get())
        ->add_module(std::make_shared<MiscModule>().get())
        ->install_hooks();
}
