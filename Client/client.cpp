#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "constants.h"
#include "commands.h"
#include "socket_functions.h"


ssize_t n;
socklen_t addrlen;
std::string AUCIP, AUCport;
struct addrinfo hintsClient;
struct addrinfo *resClient;
struct sockaddr_in addr;
int client, err;
char buffer[MAX_BUFFER];
char message[MAX_MESSAGE], line[MAX_LINE];
char command[20], var1[20], var2[20], var3[20], var4[20];

void Client(std::string AUCIP, std::string AUCport)
{
    
    std::string logUID, logPass;
    setsocketUDP(client, hintsClient);
    err = getaddrinfo(AUCIP.c_str(), AUCport.c_str(), &hintsClient, &resClient);

    if (err != 0)
        ThrowError("Error: getting address info of AS server\n");

    while (1)
    {
        memset(message, '\0', sizeof(message));

        fgets(line, sizeof(line), stdin);
        sscanf(line, "%s %s %s %s %s", command, var1, var2, var3, var4);
        std::string input1(var1);
        std::string input2(var2);
        std::string input3(var3);
        std::string input4(var4);

        if(!strcmp(command, "login")){
            command_log(logUID, logPass, input1, input2, client, resClient, addr);
        }
        else if(!strcmp(command, "logout")){
            command_logout(logUID, logPass, client, resClient, addr);
        }
        else if(!strcmp(command, "unregister")){
            command_unreg(logUID, logPass, client, resClient, addr);
        }
        else if(!strcmp(command, "exit")){
            command_exit(logUID, client, resClient, err);
        }
        
        else if(!strcmp(command, "open")){
            command_open(AUCIP,AUCport,logUID, logPass,input1, input2, input3, input4);
        }
        else if(!strcmp(command, "close")){
            command_close(AUCIP,AUCport,logUID, logPass,input1);
        }
        else if(!strcmp(command, "myauctions") || !strcmp(command,"ma")){
            command_ma(logUID, client, resClient, addr);
        }
        else if(!strcmp(command, "mybids") || !strcmp(command,"mb")){
            command_mb(logUID, client, resClient, addr);
        }
        else if(!strcmp(command, "list") || !strcmp(command, "l")){
            command_lst(client, resClient, addr);
        }
        else if(!strcmp(command, "show_asset") || !strcmp(command,"sa")){
            command_sa(AUCIP,AUCport,input1);
        }
        else if(!strcmp(command, "bid") || !strcmp(command,"b")){
            command_bid(AUCIP,AUCport,logUID, logPass,input1,input2);
        }
        else if(!strcmp(command, "show_record") || !strcmp(command, "sr")){
            command_sr(input1, client, resClient, addr);
        }
        else{
            std::cout << "Invalid command" << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    int option;
    int i = 0;
    while ((option = getopt(argc, argv, "n:p:")) != -1){
        switch (option)
        {
        case 'n':
            //IP found and assgins it to the Port variable
            i += 2;
            AUCIP = std::string(argv[i]);
            break;
        case 'p':
            //Port found and assgins it to the Port variable
            i += 2; 
            AUCport = std::string(argv[i]);
            break;
        default:
            break;
        }
    }
    //If the IP is omitted
    if (AUCIP.size() == 0)
    {
        char hostclient[256];
        int hostname;
        //It gets the machine IP
        hostname = gethostname(hostclient,sizeof(hostclient));
        if (hostname == -1) {
            perror("Error: in gethostname");
            exit(1);
        }
        AUCIP = std::string(hostclient);
    }

    //If port is omitted it assigns the GGPORT to the port
    if (AUCport.size() == 0){
        AUCport  = std::string(GPPORT);
    }
    Client(AUCIP, AUCport);
}