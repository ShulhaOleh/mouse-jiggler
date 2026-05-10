#include "uninstaller.h"

#include <iostream>
#include <print>

#if __linux__

#include <unistd.h>
#include <filesystem>
#include <cstdlib>

int uninstall() {
    if (geteuid() != 0) {
        std::println(std::cerr, "Please run as root: sudo mouse-jiggler --uninstall");
        return 1;
    }

    namespace fs = std::filesystem;
    std::error_code ec;
    bool ok = true;

    if (!fs::remove("/etc/udev/rules.d/99-mouse-jiggler.rules", ec)) {
        std::println(std::cerr, "Failed to remove udev rule: {}", ec.message());
        ok = false;
    }

    std::system("udevadm control --reload-rules");

    if (!fs::remove("/usr/local/bin/mouse-jiggler", ec)) {
        std::println(std::cerr, "Failed to remove binary: {}", ec.message());
        ok = false;
    }

    if (ok) std::println("Uninstalled successfully.");
    return ok ? 0 : 1;
}

#elif _WIN32

#include <windows.h>
#include <string>
#include <vector>

static bool is_elevated() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return false;
    TOKEN_ELEVATION elev{};
    DWORD sz = sizeof(elev);
    GetTokenInformation(token, TokenElevation, &elev, sz, &sz);
    CloseHandle(token);
    return elev.TokenIsElevated;
}

static std::wstring get_install_dir() {
    wchar_t pf[MAX_PATH];
    if (!GetEnvironmentVariableW(L"ProgramFiles", pf, MAX_PATH))
        return {};
    return std::wstring(pf) + L"\\mouse-jiggler";
}

static void remove_from_system_path(const std::wstring& dir) {
    HKEY key;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
            0, KEY_READ | KEY_WRITE, &key) != ERROR_SUCCESS)
        return;

    DWORD type, size = 0;
    RegQueryValueExW(key, L"Path", nullptr, &type, nullptr, &size);
    std::wstring path(size / sizeof(wchar_t), L'\0');
    RegQueryValueExW(key, L"Path", nullptr, &type, reinterpret_cast<LPBYTE>(path.data()), &size);
    while (!path.empty() && path.back() == L'\0') path.pop_back();

    std::wstring norm_dir = dir;
    if (!norm_dir.empty() && norm_dir.back() == L'\\') norm_dir.pop_back();

    std::wstring new_path;
    size_t start = 0;
    while (start <= path.size()) {
        size_t end = path.find(L';', start);
        if (end == std::wstring::npos) end = path.size();
        std::wstring seg = path.substr(start, end - start);
        if (!seg.empty() && seg.back() == L'\\') seg.pop_back();
        if (!seg.empty() && _wcsicmp(seg.c_str(), norm_dir.c_str()) != 0) {
            if (!new_path.empty()) new_path += L';';
            new_path += seg;
        }
        start = end + 1;
    }

    RegSetValueExW(key, L"Path", 0, type,
        reinterpret_cast<const BYTE*>(new_path.c_str()),
        static_cast<DWORD>((new_path.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(key);

    DWORD_PTR result;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
        reinterpret_cast<LPARAM>(L"Environment"), SMTO_ABORTIFHUNG, 5000, &result);
}

static void schedule_deletion(const std::wstring& dir) {
    std::wstring cmd = L"powershell -NoProfile -NonInteractive -WindowStyle Hidden -Command "
        L"\"Start-Sleep 2; Remove-Item -Recurse -Force -Path '" + dir + L"'\"";
    std::vector<wchar_t> buf(cmd.begin(), cmd.end());
    buf.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};
    CreateProcessW(nullptr, buf.data(), nullptr, nullptr,
        FALSE, CREATE_NO_WINDOW | DETACHED_PROCESS, nullptr, nullptr, &si, &pi);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

int uninstall() {
    if (!is_elevated()) {
        std::println(std::cerr, "Please run as Administrator: Right-click > Run as administrator");
        return 1;
    }

    std::wstring install_dir = get_install_dir();
    if (install_dir.empty()) {
        std::println(std::cerr, "Failed to locate Program Files directory.");
        return 1;
    }

    remove_from_system_path(install_dir);
    schedule_deletion(install_dir);

    std::println("Uninstalled successfully.");
    return 0;
}

#else

int uninstall() {
    std::println(std::cerr, "Uninstall is not supported on this platform.");
    return 1;
}

#endif
