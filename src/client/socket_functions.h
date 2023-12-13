#ifndef SOCKET_FUNCTS_H
#define SOCKET_FUNCTS_H

#include "constants.h"
#include <string>

void ThrowError(const char* message);
void setsocketTCP(int &client_tcp,struct addrinfo &hintsClientTCP); //used in client
void setsocketUDP(int &client,struct addrinfo &hintsClient); //used in client
int TimerON(int sd);
int TimerOFF(int sd);
void sendmsgUDP(char *message,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr,
                std::string &res, std::string &status, std::string &rest);

//void send_file(std::string filename, int client);
void sendmsgTCP(char *message, int &client,struct addrinfo* &resClient);


#endif