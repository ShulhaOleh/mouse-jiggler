/*
   A mouse jiggler application for Windows and Linux
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

#include "jiggler.h"
#include "localization.h"
#include "command_listener.h"

std::atomic<bool> running(true);
localization locale("locales");

int main() {
    std::cout << locale.get_string("start") << std::endl;
    std::cout << locale.get_string("commands") << std::endl;

    std::thread jiggleThread(mouse_jiggler);
    std::thread commandThread(command_listener);

    commandThread.join();
    running = false;
    jiggleThread.join();

    std::cout << locale.get_string("exiting") << std::endl;

    return 0;
}

