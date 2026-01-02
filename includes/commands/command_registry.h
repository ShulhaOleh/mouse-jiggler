#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include "command.h"

#include <memory>
#include <unordered_map>
#include <string>

class command_registry {
public:
    void register_command(const std::string& name, std::unique_ptr<command> cmd);
    void register_alias(const std::string& alias, const std::string& command_name);
    bool execute(const std::string& name, const std::vector<std::string>& args);

private:
    command* find_command(const std::string& name);
    std::unordered_map<std::string, std::unique_ptr<command>> commands_;
    std::unordered_map<std::string, std::string> aliases_;
};

#endif
