/*
   A mouse jiggler application for Windows and Linux
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

#include "version.h"
#include "jiggler.h"
#include "localization.h"
#include "commands/command_listener.h"

std::atomic<bool> running(true);

int main(int argc, char* argv[]) {
    // CLI flags handling
    if (argc > 1) {
        if (argv[1] == "--version" || argv[1] == "-v") {
            std::cout << PROJECT_NAME << " " << PROJECT_VERSION << std::endl;

            return 0;
        }
        
        if (argv[1] == "--help" || argv[1] == "-h") {
            std::cout << "Options:"                                               << std::endl
                      << "  --version, -v     Show version information"           << std::endl
                      << "  --help, -h        Show this help message"             << std::endl
                      << "\nInteractive commands:"                                << std::endl
                      << "  quit, q           Stop the application"               << std::endl
                      << "  lang <code>       Change language (e.g., en, uk, ru)" << std::endl
                      << "  help              Show available commands"            << std::endl
                      << "  version, v        Show version information"           << std::endl;

            return 0;
        }
        
        std::cerr << "Unknown option: " << argv[1] << std::endl;
        std::cerr << "Try 'mouse-jiggler --help' for more information." << std::endl;

        return 1;
    }

    std::cout << locale.get_string("start") << std::endl;
    std::cout << locale.get_string("commands") << std::endl;

    std::thread jiggle_thread(mouse_jiggler);
    std::thread command_thread(command_listener);

    command_thread.join();
    running = false;
    jiggle_thread.join();

    std::cout << locale.get_string("exiting") << std::endl;

    return 0;
}

