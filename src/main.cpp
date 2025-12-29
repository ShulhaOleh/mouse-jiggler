/*
   A mouse jiggler application for Windows and Linux
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

#include "localization.h"
#include "command_listener.h"

#if _WIN32
    #include <windows.h>
#elif __linux__
    #include <X11/Xlib.h>
#endif

std::atomic<bool> running(true);
localization locale("locales");

void mouseJiggler() {
#if __linux__
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << locale.get_string("display_error") << std::endl;
        return;
    }

    Window root = DefaultRootWindow(display);
#endif

    while (running) {

#if _WIN32
        POINT pos;
        GetCursorPos(&pos);

        SetCursorPos(pos.x + 1, pos.y);
        Sleep(1);
        SetCursorPos(pos.x, pos.y);

#elif __linux__
        Window ret_root, ret_child;
        int root_x, root_y;
        int win_x, win_y;
        unsigned int mask;

        if (XQueryPointer(display, root,
                           &ret_root, &ret_child,
                           &root_x, &root_y,
                           &win_x, &win_y,
                           &mask)) {

            XWarpPointer(display, None, root, 0, 0, 0, 0, 1, 0);
            XFlush(display);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            XWarpPointer(display, None, root, 0, 0, 0, 0, -1, 0);
            XFlush(display);
        }
#endif

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

#if __linux__
    XCloseDisplay(display);
#endif
}

int main() {
    std::cout << locale.get_string("start") << std::endl;
    std::cout << locale.get_string("commands") << std::endl;

    std::thread jiggleThread(mouseJiggler);
    std::thread commandThread(command_listener);

    commandThread.join();
    running = false;
    jiggleThread.join();

    std::cout << locale.get_string("exiting") << std::endl;

    return 0;
}

