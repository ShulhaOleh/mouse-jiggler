#include "commands/commands.h"

#include <iostream>
#include <print>

#include "localization.h"
#include "version.h"

quit_command::quit_command(std::atomic<bool>& running_flag) : running_(running_flag) {}

void quit_command::execute(const std::vector<std::string>& args) {
    running_ = false;
}

std::string quit_command::get_name() const {
    return "quit";
}

void lang_command::execute (const std::vector<std::string>& args) {
    if (args.empty()) {
        show_current_language();
        return;
    }
    
    change_language(args[0]);
}

std::string lang_command::get_name() const {
    return "lang";
}

void lang_command::show_current_language() const {
    std::println("{}", locale.get_string("current_language") + locale.detect_system_language());
    std::println("{}", locale.get_string("language_usage"));
}

void lang_command::change_language(const std::string& lang) const {
    locale.set_language(lang);
    std::println("{}{}", locale.get_string("language_changed"), lang);
    std::println("{}", locale.get_string("commands"));
}

void help_command::execute(const std::vector<std::string>& args) {
    std::println("{}", locale.get_string("commands"));
}

std::string help_command::get_name() const {
    return "help";
}

void version_command::execute(const std::vector<std::string>& args) {
    std::println("{} {} © {} {}", PROJECT_NAME, PROJECT_VERSION, PROJECT_YEAR, PROJECT_AUTHOR);
}

std::string version_command::get_name() const {
    return "version";
}
