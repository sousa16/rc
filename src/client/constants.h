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
#define GPPORT "58077"
#define MAX_MESSAGE 1048576
#define MAX_LINE 320
#define MAX_BUFFER 1048576


//Structs

//Structure that represents a message
typedef struct msg{
    char MID[5];
    char UID[6];
    char textsize[11];
    char text[241];
    char fname[26];
    char fsize[11];
    char data[1025];
}msg;


#endif //client_constants.h