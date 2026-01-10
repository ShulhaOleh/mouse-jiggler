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
    if (argc == 2) {
        std::string arg = argv[1];

        if (arg == "--version" || arg == "-v") {
            std::cout << PROJECT_NAME << " "
                      << PROJECT_VERSION << " © "
                      << PROJECT_YEAR << " "
                      << PROJECT_AUTHOR << " "
                      << std::endl;

            return 0;
        }
        
        else if (arg == "--help" || arg == "-h") {
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

        else if (arg == "--update" || arg == "-u") {
            // Update from github releases   

            return 0;
        }
        
        std::cerr << "Unknown option: " << arg << std::endl;
        std::cerr << "Try 'mouse-jiggler --help' for more information." << std::endl;

        return 1;
    }

    // Too many arguments
    if (argc > 2) {
        std::cerr << "Error: Too many arguments" << std::endl;
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

