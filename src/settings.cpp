#include "settings.hpp"

int Settings::_parse_enabled_option_row(
    void* obj_, int column_count, char** column_values, char** column_names
) {
    auto obj = static_cast<Settings*>(obj_);

    std::string key(column_values[0]);
    Option* option = obj->_hashmap_options[key];

    if (!option)
    {
        LOG("Settings (_parse_enabled_option_row) - Unknown enabled option (%s), ignoring.", key.c_str());
        return 0;
    }

    obj->_enabled_options.push_back(option);

    return 0;
}

Settings* Settings::instance()
{
    static Settings inst;

    if (!inst._initialized)
        inst._initialized = inst.init();
    
    return &inst;
}

void Settings::set_option_enabled(std::string key, bool enable)
{
    Option* option = this->_hashmap_options[key];

    if (auto it = std::find(this->_enabled_options.begin(), this->_enabled_options.end(), option); it != this->_enabled_options.end())
    {
        if (!enable)
            this->_enabled_options.erase(it);
    }
    else if (enable)
    {
        this->_enabled_options.push_back(option);
    }

    sqlite3_stmt* stmt;
    const char* pzTail;

    sqlite3_prepare_v2(this->_db_handle, "UPDATE settings SET enabled = ? WHERE key = ?", -1, &stmt, &pzTail);
    sqlite3_bind_int(stmt, 1, enable);
    sqlite3_bind_text(stmt, 2, option->key, -1, SQLITE_STATIC);
    
    if (int res = sqlite3_step(stmt); res != SQLITE_DONE)
    {
        LOG("Settings (set_option_enabled) - Failed to set %s to %i: %i", option->key, enable, res);
    }
}

bool Settings::init()
{
    _options = {
        Option {"object_bypass"},
        Option {"16k_fix"},
        Option {"unlisted_objects"},
        Option {"level_copying"},
        Option {"object_counter"}
    };

    for (Option& option : _options)
    {
        this->_hashmap_options.insert(std::make_pair(option.key, &option));
    }

    //LOG("Option 16k_fix addr: %s", this->_hashmap_options.at("16k_fix").key);

    if (int res = sqlite3_open("/sdcard/13gdps/userdata/starry_sky.db", &this->_db_handle); res != SQLITE_OK)
    {
        LOG("Settings - sqlite3_open failed: %i", res);
        return false;
    }
    
    /* CREATE SETTINGS TABLE IF IT DOESN'T EXIST */

    const char* settings_create_sql = R"(CREATE TABLE IF NOT EXISTS settings (
    key VARCHAR PRIMARY KEY NOT NULL,
    enabled BOOL NOT NULL DEFAULT false
);)";

    char* err;
    if (int res = sqlite3_exec(this->_db_handle, settings_create_sql,
        [](void*, int, char**, char**)->int { return 0; }, nullptr, &err); res != SQLITE_OK)
    {
        LOG("Settings - creating settings table failed: %i", res);
        return false;
    }


    /* CREATE KEYS IF IT DOESN'T EXIST */

    const char* setting_create_sql = "INSERT OR IGNORE INTO settings(key) VALUES(?);";
    sqlite3_stmt* stmt;
    const char* f;

    sqlite3_prepare_v2(this->_db_handle, setting_create_sql, -1, &stmt, &f);

    for (const Option& option : this->_options)
    {
        sqlite3_bind_text(stmt, 1, option.key, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    /* GET ENABLED OPTIONS */
    char* r;

    sqlite3_prepare_v2(this->_db_handle, "SELECT key FROM settings WHERE enabled = 1", -1, &stmt, &f);
    if (int res = sqlite3_exec(
        this->_db_handle, sqlite3_sql(stmt), &this->_parse_enabled_option_row, static_cast<void*>(this), &r);
        res != SQLITE_OK
    ) {
        LOG("Settings - Failed to get enabled options: %i", res);
    }


    return true;
}
