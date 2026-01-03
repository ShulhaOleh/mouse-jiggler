#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

class command {
public:
    virtual void execute(const std::vector<std::string>& args) = 0;
    virtual std::string get_name() const = 0;

    virtual ~command() = default;
};

#endif
