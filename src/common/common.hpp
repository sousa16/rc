#ifndef COMMON_H
#define COMMON_H

#include <cstring>
#include <stdexcept>

class UnrecoverableError : public std::runtime_error
{
public:
    UnrecoverableError(const std::string &__error) : std::runtime_error(__error) {}
    UnrecoverableError(const std::string &__error, const int __errno)
        : std::runtime_error(__error + ": " + strerror(__errno)) {}
};

void validate_port_number(std::string &port);

#endif