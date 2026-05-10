#include "updater.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <print>
#include <string>

#include "logger.h"
#include "version.h"

#if _WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
#endif

namespace fs = std::filesystem;

static std::string exec_capture(const std::string& cmd) {
    std::array<char, 256> buf{};
    std::string result;
#if _WIN32
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe) return {};
    while (fgets(buf.data(), buf.size(), pipe))
        result += buf.data();
#if _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' '))
        result.pop_back();
    return result;
}

static std::string get_current_path() {
#if _WIN32
    char buf[MAX_PATH]{};
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    return buf;
#elif __linux__
    char buf[4096]{};
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len < 0) return {};
    return buf;
#endif
}

static std::string get_latest_version() {
    std::string json = exec_capture(
        "curl -sSL \"https://api.github.com/repos/ShulhaOleh/mouse-jiggler/releases/latest\""
    );

    size_t pos = json.find("\"tag_name\"");
    if (pos == std::string::npos) return {};

    pos = json.find('"', json.find(':', pos) + 1);
    if (pos == std::string::npos) return {};

    ++pos;
    size_t end = json.find('"', pos);
    if (end == std::string::npos) return {};

    return json.substr(pos, end - pos);
}

static std::string get_download_url(const std::string& version) {
    std::string base = "https://github.com/ShulhaOleh/mouse-jiggler/releases/download/"
                       + version + "/mouse-jiggler-x64";
#if _WIN32
    return base + ".exe";
#else
    return base;
#endif
}

void run_update() {
    std::println("Checking for updates...");

    std::string latest = get_latest_version();
    if (latest.empty()) {
        std::println(std::cerr, "Failed to check for updates. Is curl installed?");
        LOG_ERROR("Failed to retrieve latest version from GitHub");
        return;
    }

    if (latest == PROJECT_VERSION) {
        std::println("Already up to date ({}).", PROJECT_VERSION);
        return;
    }

    std::println("Updating {} -> {}...", PROJECT_VERSION, latest);

    std::string current = get_current_path();
    if (current.empty()) {
        std::println(std::cerr, "Failed to determine current binary path.");
        LOG_ERROR("Failed to determine current binary path");
        return;
    }

    std::string url = get_download_url(latest);
    std::string tmp = current + ".new";

    if (std::system(("curl -sSL \"" + url + "\" -o \"" + tmp + "\"").c_str()) != 0) {
        std::println(std::cerr, "Download failed.");
        LOG_ERROR("Download failed: " + url);
        fs::remove(tmp);
        return;
    }

#if _WIN32
    std::string old_path = current + ".old";
    if (!MoveFileExA(current.c_str(), old_path.c_str(), MOVEFILE_REPLACE_EXISTING) ||
        !MoveFileExA(tmp.c_str(), current.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        std::println(std::cerr, "Failed to replace binary. Try running as administrator.");
        LOG_ERROR("Failed to replace binary during update");
        fs::remove(tmp);
        return;
    }
    MoveFileExA(old_path.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
#elif __linux__
    std::error_code ec;
    fs::permissions(tmp,
        fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
        fs::perm_options::add, ec);
    fs::rename(tmp, current, ec);
    if (ec) {
        std::println(std::cerr, "Failed to replace binary: {}. Try running as root.", ec.message());
        LOG_ERROR("Failed to replace binary: " + ec.message());
        fs::remove(tmp);
        return;
    }
#endif

    std::println("Updated to {}. Restart the application to use the new version.", latest);
}
