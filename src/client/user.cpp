#include "user.hpp"
#include "../common/common.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        // Use ClientConfig to process command line arguments
        ClientConfig config(argc, argv);
        if (config.help)
        {
            config.printHelp(std::cout);
            return EXIT_SUCCESS;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "The application has faced a critical error that cannot be resolved during its execution. Shutting down..."
                  << std::endl
                  << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "The application has faced a critical error that cannot be resolved during its execution. Shutting down..."
                  << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

ClientConfig::ClientConfig(int argc, char *argv[])
{
    program_path = argv[0];
    int opt;

    // Process command line arguments
    while ((opt = getopt(argc, argv, "n:p:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            host = std::string(optarg);
            break;
        case 'p':
            port = std::string(optarg);
            break;
        case 'h':
            help = true;
            break;
        default:
            std::cerr << std::endl;
            printHelp(std::cerr);
            exit(EXIT_FAILURE);
        }
    }

    // Check if argument is a number between 1 and 65535
    validate_port_number(port);
}

// -h argument used to help the user
void ClientConfig::printHelp(std::ostream &stream)
{
    stream << "How to use: " << program_path << " [-n ASIP] [-p ASport] [-h]"
           << std::endl;
    stream << "Available options:" << std::endl;
    stream << "-n ASIP\t\tSet hostname of Auction Server. Default: "
           << DEFAULT_HOSTNAME << std::endl;
    stream << "-p ASport\tSet port of Auction Server. Default: " << DEFAULT_PORT
           << std::endl;
    stream << "-h\t\tPrint this help menu." << std::endl;
}
