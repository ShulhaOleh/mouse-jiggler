/*
   A mouse jiggler application for Windows and Linux
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <filesystem>
#include <print>

#include "version.h"
#include "jiggler.h"
#include "localization.h"
#include "commands/command_listener.h"
#include "logger.h"
#include "updater.h"

#if __linux__
    #include <unistd.h>
#endif

std::atomic<bool> running(true);

int main(int argc, char* argv[]) {
    // CLI flags handling
    if (argc == 2) {
        std::string arg = argv[1];

        if (arg == "--version" || arg == "-v") {
            std::println("{}", PROJECT_VERSION);

            return 0;
        }
        
        else if (arg == "--help" || arg == "-h") {
            std::string help_text = "Options:\n"
                                    "  --version, -v     Show version information\n"
                                    "  --help, -h        Show this help message\n"
                                    "  --uninstall       Remove binary and udev rule (Linux, requires sudo)\n"
                                    "\nInteractive commands:\n"
                                    "  quit, q           Stop the application\n"
                                    "  lang <code>       Change language (e.g., en, uk, ru)\n"
                                    "  help              Show available commands\n"
                                    "  version, v        Show version information";
            std::println("{}", help_text);

            return 0;
        }

        else if (arg == "--update" || arg == "-u") {
            run_update();
            return 0;
        }

        else if (arg == "--uninstall") {
#if __linux__
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
#else
            std::println(std::cerr, "Uninstall is not supported on this platform.");
            return 1;
#endif
        }
        
        std::println(std::cerr, "Unknown option: {}", arg);
        std::println(std::cerr, "Try 'mouse-jiggler --help' for more information.");
        LOG_ERROR("Unknown command line option: " + arg);

        return 1;
    }

    // Too many arguments
    if (argc > 2) {
        std::println(std::cerr, "Error: Too many arguments");
        std::println(std::cerr, "Try 'mouse-jiggler --help' for more information.");
        LOG_ERROR("Too many command line arguments provided");

        return 1;
    }

    std::println("{}", locale.get_string("start"));
    std::println("{}", locale.get_string("commands"));

    std::thread jiggle_thread(mouse_jiggler);
    std::thread command_thread(command_listener);

    command_thread.join();
    running = false;
    jiggle_thread.join();

    std::println("{}", locale.get_string("exiting"));

    return 0;
}

