#include "common.hpp"

#include <csignal>

bool is_shutting_down = false;

// Check if argument is a number between 1 and 65535
void validate_port_number(std::string &port)
{
    for (char c : port)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw UnrecoverableError("Invalid port: not a number");
        }
    }

    try
    {
        int32_t parsed_port = std::stoi(port);
        if (parsed_port <= 0 || parsed_port > ((1 << 16) - 1))
        {
            throw std::runtime_error("");
        }
    }
    catch (...)
    {
        throw UnrecoverableError(
            "Invalid port: it must be a number between 1 and 65535");
    }
}