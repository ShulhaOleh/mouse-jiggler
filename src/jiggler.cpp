#include "jiggler.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <print>

#include "localization.h"
#include "logger.h"

#if _WIN32
    #include <windows.h>
#elif __linux__
    #include <cerrno>
    #include <cstring>
    #include <fcntl.h>
    #include <unistd.h>
    #include <linux/uinput.h>
#endif

extern std::atomic<bool> running;

#if __linux__
static bool emit(int fd, int type, int code, int value) {
    struct input_event ev{};
    ev.type  = static_cast<__u16>(type);
    ev.code  = static_cast<__u16>(code);
    ev.value = value;
    return write(fd, &ev, sizeof(ev)) == sizeof(ev);
}

static bool setup_uinput(int fd) {
    if (ioctl(fd, UI_SET_EVBIT, EV_REL) < 0) {
        std::println(std::cerr, "uinput: UI_SET_EVBIT failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_SET_EVBIT failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0) {
        std::println(std::cerr, "uinput: UI_SET_RELBIT failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_SET_RELBIT failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0) {
        std::println(std::cerr, "uinput: UI_SET_RELBIT (Y) failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_SET_RELBIT (Y) failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0) {
        std::println(std::cerr, "uinput: UI_SET_EVBIT (KEY) failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_SET_EVBIT (KEY) failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0) {
        std::println(std::cerr, "uinput: UI_SET_KEYBIT failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_SET_KEYBIT failed: " + std::string(std::strerror(errno)));
        return false;
    }

    struct uinput_setup usetup{};
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234;
    usetup.id.product = 0x5678;
    std::strncpy(usetup.name, "mouse-jiggler", UINPUT_MAX_NAME_SIZE - 1);

    if (ioctl(fd, UI_DEV_SETUP, &usetup) < 0) {
        std::println(std::cerr, "uinput: UI_DEV_SETUP failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_DEV_SETUP failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (ioctl(fd, UI_DEV_CREATE) < 0) {
        std::println(std::cerr, "uinput: UI_DEV_CREATE failed: {}", std::strerror(errno));
        LOG_ERROR("uinput UI_DEV_CREATE failed: " + std::string(std::strerror(errno)));
        return false;
    }

    return true;
}
#endif

void mouse_jiggler() {
#if _WIN32
    while (running) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x + 1, pos.y);
        Sleep(1);
        SetCursorPos(pos.x, pos.y);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#elif __linux__
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::println(std::cerr, "{}", locale.get_string("uinput_error"));
        LOG_ERROR("Failed to open /dev/uinput");
        return;
    }

    if (!setup_uinput(fd)) {
        close(fd);
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (running) {
        if (!emit(fd, EV_REL, REL_X, 5) || !emit(fd, EV_SYN, SYN_REPORT, 0)) {
            std::println(std::cerr, "uinput: write failed: {}", std::strerror(errno));
            LOG_ERROR("uinput write failed: " + std::string(std::strerror(errno)));
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        emit(fd, EV_REL, REL_X, -5);
        emit(fd, EV_SYN, SYN_REPORT, 0);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
#endif
}
