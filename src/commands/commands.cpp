#include "commands/commands.h"

#include "localization.h"
#include <iostream>

extern localization locale;

quit_command::quit_command(std::atomic<bool>& running_flag) : running_(running_flag) {}

void quit_command::execute(const std::vector<std::string>& args) {
    running_ = false;
}

std::string quit_command::get_name() const {
    return "quit";
}

void lang_command::execute(const std::vector<std::string>& args) {
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
    std::cout << locale.get_string("current_language") 
              << locale.detect_system_language() << std::endl;
    std::cout << locale.get_string("language_usage") 
              << std::endl;
}

void lang_command::change_language(const std::string& lang) const {
    locale.set_language(lang);
    std::cout << locale.get_string("language_changed") 
              << lang << std::endl;
    std::cout << locale.get_string("commands") << std::endl;
}

void help_command::execute(const std::vector<std::string>& args) {
    std::cout << locale.get_string("commands") << std::endl;
}

std::string help_command::get_name() const {
    return "help";
}
