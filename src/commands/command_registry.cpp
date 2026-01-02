#include "commands/command_registry.h"

void command_registry::register_command(const std::string& name, std::unique_ptr<command> cmd) {
    commands_[name] = std::move(cmd);
}

void command_registry::register_alias(const std::string& alias, const std::string& command_name) {
    aliases_[alias] = command_name;
}

bool command_registry::execute(const std::string& name, const std::vector<std::string>& args) {
    command* cmd = find_command(name);
    
    if (!cmd) {
        return false;
    }
    
    cmd->execute(args);
    return true;
}

command* command_registry::find_command(const std::string& name) {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return it->second.get();
    }
    
    auto alias_it = aliases_.find(name);
    if (alias_it == aliases_.end()) {
        return nullptr;
    }
    
    auto cmd_it = commands_.find(alias_it->second);
    if (cmd_it == commands_.end()) {
        return nullptr;
    }
    
    return cmd_it->second.get();
}
