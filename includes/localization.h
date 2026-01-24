#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <span>

class localization {
public:
    localization();

    void set_language(const std::string& lang);
    std::string detect_system_language();
    std::string get_string(const std::string& key, const std::string& missing = "") const;
private:
    using dict = std::unordered_map<std::string, std::string>;
    dict load_dict_from_memory(std::span<const unsigned char> data) const;
    void ensure_loaded(const std::string& lang) const;
    std::string current_lang_;
    std::string default_lang_;
    mutable std::unordered_map<std::string, dict> cache_;
    mutable std::mutex mutex_;
};

extern localization locale;
