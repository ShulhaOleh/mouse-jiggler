/*
   A mouse jiggler application for Windows and Linux
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

#include "jiggler.h"
#include "localization.h"
#include "commands/command_listener.h"

std::atomic<bool> running(true);

int main() {
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

