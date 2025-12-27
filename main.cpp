/*
   A mouse jiggler application for Windows and Linux
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

#if _WIN32
    #include <windows.h>
#elif __linux__
    #include <X11/Xlib.h>
#endif

std::atomic<bool> running(true);

void mouseJiggler() {
#if __linux__
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display\n";
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


void commandListener() {
    while (running) {
        std::string cmd;
        std::cin >> cmd;

        if (cmd == "q" || cmd == "quit") {
            running = false;
            return;
        }
    }
}

int main() {
    std::cout << "Mouse Jiggler started.\n";
    std::cout << "Type 'q' to quit.\n";

    std::thread jiggleThread(mouseJiggler);
    std::thread commandThread(commandListener);

    commandThread.join();
    running = false;
    jiggleThread.join();

    std::cout << "Exiting...\n";

    return 0;
}

