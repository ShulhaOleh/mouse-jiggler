#ifndef COMMANDS_H
#define COMMANDS_H

#include "command.h"
#include <atomic>

class quit_command : public command {
public:
    quit_command(std::atomic<bool>& running_flag);
    void execute(const std::vector<std::string>& args) override;
    std::string get_name() const override;

private:
    std::atomic<bool>& running_;
};

class lang_command : public command {
public:
    void execute(const std::vector<std::string>& args) override;
    std::string get_name() const override;

private:
    void show_current_language() const;
    void change_language(const std::string& lang) const;
};

class help_command : public command {
public:
    void execute(const std::vector<std::string>& args) override;
    std::string get_name() const override;
};

#endif
