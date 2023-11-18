#ifndef USER_H
#define USER_H

#include <csignal>

#include "commands.hpp"
#include "../common/constants.hpp"
// #include "common/auction.hpp"
#include "../common/protocol.hpp"
// #include "user_state.hpp"

class ClientConfig
{
public:
  char *program_path;
  std::string host = DEFAULT_HOSTNAME;
  std::string port = DEFAULT_PORT;
  bool help = false;

  ClientConfig(int argc, char *argv[]);
  void printHelp(std::ostream &stream);
};

// void registerCommands(CommandManager& manager);

#endif