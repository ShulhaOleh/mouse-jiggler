#include "localization.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iterator>

#if _WIN32
    #include <windows.h>
#elif __linux__
    #include <cstdlib>
    #include <unistd.h>
    #include <sys/stat.h>
#endif


// public methods

localization::localization(std::string locales_dir)
: locales_dir_(move(locales_dir)), current_lang_(), default_lang_("en") {
    detect_system_language();
}

void localization::set_language(const std::string& lang){
    std::lock_guard<std::mutex> lk(mutex_);
    current_lang_ = lang;
}

std::string localization::detect_system_language(){
#if _WIN32
    wchar_t name[LOCALE_NAME_MAX_LENGTH]{0};
    std::string lang;
    BOOL have_name = GetUserDefaultLocaleName(name, LOCALE_NAME_MAX_LENGTH);
    
    int n = 0;
    if(have_name) n = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);

    std::string s;
    if(n > 0){
        s.assign(n, '\0');
        WideCharToMultiByte(CP_UTF8, 0, name, -1, &s[0], n, nullptr, nullptr);
    }

    if(!s.empty()) lang = s.substr(0, 2);

    if(lang.empty()) lang = default_lang_;

    transform(lang.begin(), lang.end(), lang.begin(), [](unsigned char c){ return tolower(c); });
    std::lock_guard<std::mutex> lk(mutex_);
    current_lang_ = lang;

    return lang;

#elif __linux__
    const char* env = std::getenv("LC_ALL");

    std::string lang;
    if(env){
        std::string s(env);
        size_t p = s.find_first_of("._");

        if(p != std::string::npos) s = s.substr(0, p);
        if(s.size() >= 2) lang = s.substr(0, 2);
    }

    if(lang.empty()) lang = default_lang_;

    std::transform(lang.begin(), lang.end(), lang.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    std::lock_guard<std::mutex> lk(mutex_);
    current_lang_ = lang;
    
    return lang;
#endif
}


// private methods

static bool file_exists(const std::string& p){
#if _WIN32
    DWORD attr = GetFileAttributesA(p.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
#elif __linux__
    struct stat st;
    return stat(p.c_str(), &st) == 0 && S_ISREG(st.st_mode);
#endif
}

static std::string trim(const std::string& s){
    size_t a = 0;

    while(a < s.size() && isspace((unsigned char)s[a])) a++;

    size_t b = s.size();

    while(b > a && isspace((unsigned char)s[b-1])) b--;

    return s.substr(a, b - a);
}

static std::string unescape(const std::string& s){
    std::string out;
    out.reserve(s.size());

    for(size_t i = 0; i < s.size(); ++i){
        char c = s[i];

        if(c != '\\' || i + 1 >= s.size()){
            out.push_back(c);
            continue;
        }

        char n = s[++i];
        switch(n){
            case 'n': out.push_back('\n'); break;
            case 't': out.push_back('\t'); break;
            case 'r': out.push_back('\r'); break;
            default:  out.push_back(n);    break;
        }
    }

    return out;
}

localization::dict localization::load_dict(const std::string& path) const{
    std::ifstream ifs(path);

    if(!ifs) return {};

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    dict out;
    size_t i = 0;

    while(i < content.size()){
        while(i < content.size() && content[i] != '"') ++i;
        if(i >= content.size()) break;

        size_t kstart = ++i;
        std::string key;

        while(i < content.size()){
            if (content[i] == '\\' && i + 1 < content.size()) {
                i += 2;
                continue; 
            }
            if (content[i] == '"') {
                key = content.substr(kstart, i - kstart);
                ++i;
                break;
            }

            ++i;
        }
        if(key.empty()) continue;

        while(i < content.size() && content[i] != ':') ++i;
        if(i >= content.size()) break;
        ++i;

        while(i < content.size() && isspace((unsigned char)content[i])) ++i;
        if(!(i < content.size() && content[i] == '"')) continue;

        size_t vstart = ++i;
        std::string val;
        while(i < content.size()){
            if(content[i] == '\\' && i + 1 < content.size()) { i += 2; continue; }
            if(content[i] == '"') { val = content.substr(vstart, i - vstart); ++i; break; }
            ++i;
        }
        out.emplace(key, unescape(val));
    }

    return out;
}

void localization::ensure_loaded(const std::string& lang) const{
    std::lock_guard<std::mutex> lk(mutex_);

    if(cache_.count(lang)) return;

    std::string path = locales_dir_ + "\\" + lang + ".json";

    if(!file_exists(path)) {
        cache_.emplace(lang, dict{});
        return;
    }

    cache_.emplace(lang, load_dict(path));
}

std::string localization::get_string(const std::string& key, const std::string& missing) const{
    std::string lang;
    {
        std::lock_guard<std::mutex> lk(mutex_);
        lang = current_lang_.empty() ? default_lang_ : current_lang_;
    }
    ensure_loaded(lang);
    auto find_in_cache = [this, &key](const std::string& l) -> const std::string* {
        std::lock_guard<std::mutex> lk(mutex_);
        auto it = cache_.find(l);

        if(it == cache_.end()) return nullptr;

        auto jt = it->second.find(key);

        if(jt == it->second.end()) return nullptr;

        return &jt->second;
    };

    if(const std::string* s = find_in_cache(lang)) return *s;

    ensure_loaded(default_lang_);
    if(const std::string* s2 = find_in_cache(default_lang_)) return *s2;

    if(!missing.empty()) return missing;

    return key;
}
