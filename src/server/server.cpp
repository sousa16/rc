#include "server.hpp"
#include "../common/common.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

int main(int argc, char *argv[])
{
    // Input Parsing
    int verbose = 0;
    char ASport[10] = "58071";

    if (argc == 3)
    {
        if (strcmp("-p", argv[1]) == 0)
            strcpy(ASport, argv[2]);
        if (strcmp("-v", argv[3]) == 0)
            verbose = 1;
    }
    else if (argc == 2)
    {
        if (strcmp("-p", argv[1]) == 0)
            strcpy(ASport, argv[2]);
    }
    else if (argc == 1)
    {
        if (strcmp("-v", argv[3]) == 0)
            verbose = 1;
    }

    std::cout << "Verbose: " << verbose << " ASport: " << ASport << std::endl;

    // UDP and TCP Servers
    int udp_fd, tcp_fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints_udp, *res_udp;
    struct addrinfo hints_tcp, *res_tcp;
    struct sockaddr_storage addr; // sockaddr_storage is used to store any type of address
    char buffer[128];

    // Create a UDP socket
    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        exit(1); // error

    // Set up hints for getaddrinfo
    memset(&hints_udp, 0, sizeof hints_udp);
    hints_udp.ai_family = AF_INET;      // IPv4
    hints_udp.ai_socktype = SOCK_DGRAM; // UDP Socket
    hints_udp.ai_flags = AI_PASSIVE;

    // Get address information
    errcode = getaddrinfo(nullptr, PORT, &hints_udp, &res_udp);
    if (errcode != 0)
        exit(1);

    // Bind the socket
    n = bind(udp_fd, res_udp->ai_addr, res_udp->ai_addrlen);
    if (n == -1)
        exit(1);

    // Release allocated memory
    freeaddrinfo(res_udp);

    // Create a TCP socket
    if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(1);

    // Set up hints for getaddrinfo
    memset(&hints_tcp, 0, sizeof hints_tcp);
    hints_tcp.ai_family = AF_INET;       // IPv4
    hints_tcp.ai_socktype = SOCK_STREAM; // TCP socket
    hints_tcp.ai_flags = AI_PASSIVE;

    // Get address information
    if ((getaddrinfo(nullptr, PORT, &hints_tcp, &res_tcp)) != 0)
        exit(1);

    // Bind the socket
    if (bind(tcp_fd, res_tcp->ai_addr, res_tcp->ai_addrlen) == -1)
        exit(1);

    // Listen on the socket
    if (listen(tcp_fd, 5) == -1)
        exit(1);

    // Release allocated memory
    freeaddrinfo(res_tcp);

    while (true)
    {
        // Receive data
        n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);

        if (n == -1)
            exit(1);

        write(1, "received: ", 10);
        write(1, buffer, n);

        char response[30];
        memset(response, 0, strlen(response));

        char action[50], value1[50], value2[50];
        memset(action, 0, sizeof(action));
        memset(value1, 0, sizeof(value1));
        memset(value2, 0, sizeof(value2));

        char *token;
        token = strtok(buffer, " ");

        if (token)
        {
            strcpy(action, token);
            token = strtok(NULL, " ");

            // Check if there is a second token
            if (token)
            {
                strcpy(value1, token);
                token = strtok(NULL, " ");

                // Check if there is a third token
                if (token)
                {
                    strcpy(value2, token);
                }
            }
        }
    }
    close(udp_fd); // Close the socket when done
    close(tcp_fd); // Close the socket when done
}