#pragma once

#include <sqlite3.h>
#include <map>

#include "starry_sky.hpp"
#include "consts.hpp"

typedef struct _option
{
    const char* key;
} Option;

class Settings
{
private:
    sqlite3* _db_handle;
    bool _initialized;

    std::vector<Option> _options;
    std::vector<Option*> _enabled_options;
    std::unordered_map<std::string /* key */, Option*> _hashmap_options;

    // This is an sqlite3_exec callback function.
    // It adds enabled options to _enabled_options.
    static int _parse_enabled_option_row(void*, int, char**, char**);

public:
    inline bool is_option_enabled(const std::string& key)
    {
        return std::find(
            _enabled_options.begin(), _enabled_options.end(), get_option(key)
        ) != _enabled_options.end();
    }

    inline Option* get_option(const std::string& key)
    {
        return _hashmap_options[key];
    }
    
    inline const std::vector<Option>& get_options()
    {
        return _options;
    }

    inline const std::vector<Option*>& get_enabled_options()
    {
        return _enabled_options;
    }

    static Settings* instance();
    bool init();
    void set_option_enabled(std::string key, bool enable);
};
