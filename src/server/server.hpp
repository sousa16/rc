#ifndef SERVER_H
#define SERVER_H
#define PORT "58071"

#include <csignal>

#include "commands.hpp"
#include "../common/constants.hpp"
// #include "common/auction.hpp"
#include "../common/protocol.hpp"
// #include "server_state.hpp"

class ServerConfig
{
public:
  char *program_path;
  std::string host = DEFAULT_HOSTNAME;
  std::string port = DEFAULT_PORT;
  bool help = false;

  ServerConfig(int argc, char *argv[]);
  void printHelp(std::ostream &stream);
};

// void registerCommands(CommandManager& manager);

#endif