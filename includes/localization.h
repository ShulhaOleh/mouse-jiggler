#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <string>
#include <unordered_map>
#include <mutex>

class localization {
public:
    localization(std::string locales_dir = "locales");

    void set_language(const std::string& lang);
    std::string detect_system_language();
    std::string get_string(const std::string& key, const std::string& missing = "") const;
private:
    using dict = std::unordered_map<std::string, std::string>;
    dict load_dict(const std::string& path) const;
    void ensure_loaded(const std::string& lang) const;
    std::string locales_dir_;
    std::string current_lang_;
    std::string default_lang_;
    mutable std::unordered_map<std::string, dict> cache_;
    mutable std::mutex mutex_;
};

extern localization locale;

#endif
