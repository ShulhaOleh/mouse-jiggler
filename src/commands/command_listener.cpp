#include "commands/command_listener.h"

#include <iostream>
#include <atomic>
#include <sstream>
#include <memory>

#include "commands/command_registry.h"
#include "commands/commands.h"
#include "localization.h"

extern std::atomic<bool> running;
extern localization locale;

static command_registry create_registry(std::atomic<bool>& running_flag) {
    command_registry registry;
    
    registry.register_command("quit", std::make_unique<quit_command>(running_flag));
    registry.register_command("lang", std::make_unique<lang_command>());
    registry.register_command("help", std::make_unique<help_command>());
    registry.register_command("version", std::make_unique<version_command>());
    
    registry.register_alias("q", "quit");
    registry.register_alias("v", "version");
    
    return registry;
}

static std::vector<std::string> parse_arguments(std::istringstream& stream) {
    std::vector<std::string> args;
    std::string arg;
    
    while (stream >> arg) args.push_back(arg);
    
    return args;
}

void command_listener() {
    command_registry registry = create_registry(running);
    
    while (running) {
        std::cout << "> " << std::flush;
        std::string line;
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string command_name;
        iss >> command_name;
        
        std::vector<std::string> args = parse_arguments(iss);
        
        if (!registry.execute(command_name, args)) {
            std::cout << locale.get_string("unknown_command") 
                      << command_name << std::endl;
            std::cout << locale.get_string("commands") << std::endl;
        }
    }
}
