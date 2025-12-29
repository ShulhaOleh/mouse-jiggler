// TODO: Extend to a subprogram (class). Possible ways to extend: command hash table, command pattern
// Note to self, as I have no idea what to do with this yet

#include "command_listener.h"

#include <iostream>
#include <atomic>

extern std::atomic<bool> running;

void command_listener() {
    while (running) {
        std::string cmd;
        std::cin >> cmd;

        if (cmd == "q" || cmd == "quit") {
            running = false;
            return;
        }
    }
}
