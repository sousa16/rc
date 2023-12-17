#ifndef CLIENT_CONSTANTS_H
#define CLIENT_CONSTANTS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

//Constants
#define IP "tejo.tecnico.ulisboa.pt"
#define PORT "58011"
#define GPPORT "58071"
#define MAX_MESSAGE 1048576
#define MAX_LINE 320
#define MAX_BUFFER 50000


#endif //client_constants.h