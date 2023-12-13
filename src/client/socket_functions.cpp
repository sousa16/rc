#include "parser.h"
#include "socket_functions.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <fcntl.h>


//Parses error with a specific message
void ThrowError(const char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void setsocketUDP(int &client,struct addrinfo &hintsClient)
{
    //creates UDP socket
    client = socket(AF_INET, SOCK_DGRAM, 0);
    if (client == -1){
        ThrowError("Error: in creating UDP client socket\n");
    }

    //clears the bytes of the hintsClient address and assigns its family and socket type (UDP)
    memset(&hintsClient, 0, sizeof(hintsClient));
    hintsClient.ai_family = AF_INET;
    hintsClient.ai_socktype = SOCK_DGRAM;
}

//Sets up the client TCP socket
void setsocketTCP(int &client_tcp,struct addrinfo &hintsClientTCP)
{
    //creates TCP socket
    client_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (client_tcp == -1){
        ThrowError("Error: in creating TCP client socket\n");
    }
    //clears the bytes of the hintsClient address and assigns its family and socket type (TCP)
    memset(&hintsClientTCP, 0, sizeof(hintsClientTCP));
    hintsClientTCP.ai_family = AF_INET;
    hintsClientTCP.ai_socktype = SOCK_STREAM;
}

int TimerON(int fd)
{
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=2; /* Wait for 15 sec for a reply from server. */
    return(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

int TimerOFF(int fd)
{
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

//sends messages from client to server using UDP
void sendmsgUDP(char *message,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr,
                std::string &res, std::string &status, std::string &rest)
{
    ssize_t n;
    socklen_t addrlen;
    char buffer[MAX_BUFFER];
    bzero(buffer,MAX_BUFFER*sizeof(char));
    //sends a message to server
    std::cout<<message<<std::endl;
    n = sendto(client, message, strlen(message), 0, resClient->ai_addr, resClient->ai_addrlen);
    if (n == -1){
        ThrowError("Error: couldn't send message to server\n");
    }
    addrlen = sizeof(addr);

    n=TimerON(client);
    if (n == -1)
        ThrowError("Error: unable to activate timer\n");
        
    //receives message from server
    n = recvfrom(client, buffer,MAX_BUFFER, 0, (struct sockaddr *) &addr, &addrlen);
    while (n == -1) /*error*/
    {
        //if a packet is lost, it retransmits the message from the client to server
        //after a timeout of 15 seconds
        std::cout << "Error: message lost, retransmiting message\n";
        n = sendto(client, message, strlen(message), 0, resClient->ai_addr, resClient->ai_addrlen);
        if (n == -1){
            ThrowError("Error: couldn't send message to server\n");
        }
        std::cout << "Retransmited message has been sent\n";
        n = recvfrom(client, buffer,MAX_BUFFER, 0, (struct sockaddr *) &addr, &addrlen);
    }
    n=TimerOFF(client);
    if (n == -1){
        ThrowError("Error: unable to deactivate timer\n");
    }
    std::string response = std::string (buffer);
    std::cout<<buffer<<std::endl;
    if(response.length()>=7){
        res = response.substr(0,3);
        status = response.substr(4,3);
        if (response.length()>8){
            rest=response.substr(8);
        }
        else{
            rest=std::string("");
        }
    }
    else{
        std::cout << "Error in server's message" << std::endl;
    }
    bzero(buffer,MAX_BUFFER*sizeof(char));
}

void sendmsgTCP(char *message, int &client,struct addrinfo* &resClient)
{
    int r;
    ssize_t n, n_written=0;
    r=connect(client,resClient->ai_addr,resClient->ai_addrlen);
    if (r == -1){
        ThrowError("Error: in connecting TCP client socket\n");
    }
    while (n_written<strlen(message)){
        n=write(client,message,strlen(message));
        if (n==-1){
            ThrowError("Error: in sending a message to the server\n");
        }
        n_written+=n;
    }
}

/*void send_file(std::string filename, int client)
{
    std::string message;
    ssize_t size;
    std::fstream file;
    char buffer[MAX_BUFFER];
    file.open(filename,ios::in);
    if(!file)  {
        ThrowError("Error opening file");
    }
    size=(ssize_t)std::filesystem::file_size(filename);
    if(size<0){
        ThrowError("Error: reading size of file\n");
    }
    message=filename + ' ' + std::string(size) + ' '; 
    if((write(client,(char *)message.c_str(),message.length()))!=message.length()){
        ThrowError("Error: in sending a message to the server\n");
    }
    while (file) {
        file.read(buffer, MAX_BUFFER);
        ssize_t bytes_read = (ssize_t)file.gcount();
        ssize_t bytes_sent = 0;
        while (bytes_sent < bytes_read) {
            ssize_t sent = write(client, buffer + bytes_sent, (size_t)(bytes_read - bytes_sent));
            if (sent < 0) {
                ThrowError("Error: in sending file to server\n");
            }
            bytes_sent += sent;
        }
    }

}*/

