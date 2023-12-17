#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>

#include "commands.h"
#include "parser.h"
#include "socket_functions.h"

//comunicacoes tcp - verificar o socket

void command_log(std::string &loguid, std::string &logpass,std::string uid, std::string pass, int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;

    if (!loguid.compare("")){
        message = "LIN " + uid + ' ' + pass + '\n';
        sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status,rest);
        int i = ParseResponse(res,status);
        if (i){
            loguid = uid;
            logpass = pass;
        }
    }
    else{
        std::cout << "User " << loguid << " already logged in" << std::endl;
    }
}

void command_unreg(std::string &loguid, std::string &logpass, int &client,
                    struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;

    message = "UNR " + loguid + ' ' + logpass + '\n';
    sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status,rest);
    int i = ParseResponse(res,status);
    if (i){
        loguid="";
        logpass ="";
    }
}

void command_logout(std::string &loguid, std::string &logpass, int &client,
                    struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;
    
    message = "LOU " + loguid + ' ' + logpass + '\n';
    sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status,rest);
    int i = ParseResponse(res,status);
    if (i){
        loguid="";
        logpass ="";
    }
}

void command_exit(std::string loguid, int &client, struct addrinfo* &resClient, int &err)
{
    if(loguid.compare("")){
        std::cout << "User must first logout" << std::endl;
    }
    else{
        freeaddrinfo(resClient);
        close(client);
        close(err);
        exit(EXIT_SUCCESS);
    }
}


void command_open(std::string AUCIP,std::string AUCport,std::string loguid, std::string logpass, std::string name, std::string fname,
                 std::string start_value,std::string timeactive)
{
    std::string message;
    std::string rest;
    struct addrinfo hintsClientTCP;
    struct addrinfo *resClientTCP;
    int clientTCP, err;;

    setsocketTCP(clientTCP,hintsClientTCP);

    err=getaddrinfo(AUCIP.c_str(), AUCport.c_str(), &hintsClientTCP, &resClientTCP);
    if(err!=0){
        ThrowError("Error: getting address info of AS server\n");
    }

    message = "OPA " + loguid + ' ' + logpass + ' ' + name + ' ' + start_value + ' ' + timeactive + ' ';
    sendmsgTCP((char *) message.c_str(),clientTCP,resClientTCP);
    send_file(fname,clientTCP);
    int i=receive_msgTCP(clientTCP,rest);
    if(i){
        std::cout << "The auction " << rest << " has started" << std::endl;
    }
    freeaddrinfo(resClientTCP);
    close(clientTCP);
}

void command_close(std::string AUCIP,std::string AUCport,std::string loguid, std::string logpass, std::string auction)
{
    std::string message;
    std::string rest;
    struct addrinfo hintsClientTCP;
    struct addrinfo *resClientTCP;
    int clientTCP, err;

    setsocketTCP(clientTCP,hintsClientTCP);

    err=getaddrinfo(AUCIP.c_str(), AUCport.c_str(), &hintsClientTCP, &resClientTCP);
    if(err!=0){
        ThrowError("Error: getting address info of AS server\n");
    }

    message = "CLS " + loguid + ' ' + logpass + ' ' + auction + '\n';
    sendmsgTCP((char *) message.c_str(),clientTCP,resClientTCP);
    int i=receive_msgTCP(clientTCP,rest);
    if(i){
        std::cout << "The auction "<< auction << " was closed successfully" << std::endl;
    }
    freeaddrinfo(resClientTCP);
    close(clientTCP);
}

void command_ma(std::string loguid, int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;

    message = "LMA " + loguid+ '\n';
    sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status,rest);
    int i = ParseResponse(res,status);
    if(i){
        show_list_auctions(rest);
    }
}

void command_mb(std::string loguid, int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;

    message = "LMB " + loguid +'\n';
    sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status, rest);
    int i = ParseResponse(res,status);
    if(i){
        show_list_auctions(rest);
    }
}

void command_lst(int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;

    message = "LST\n";
    sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status,rest);
    int i = ParseResponse(res,status);
    if(i){
        show_list_auctions(rest);
    }
}

void command_sa(std::string AUCIP,std::string AUCport,std::string auction)
{
    std::string message;
    struct addrinfo hintsClientTCP;
    struct addrinfo *resClientTCP;
    int clientTCP, err;

    setsocketTCP(clientTCP,hintsClientTCP);

    err=getaddrinfo(AUCIP.c_str(), AUCport.c_str(), &hintsClientTCP, &resClientTCP);
    if(err!=0){
        ThrowError("Error: getting address info of AS server\n");
    }

    message = "SAS " + auction + '\n';
    sendmsgTCP((char *) message.c_str(),clientTCP,resClientTCP);
    int i=receive_file(clientTCP);
    if(i){
        std::cout << "The file has been transfered" << std::endl;
    }
    freeaddrinfo(resClientTCP);
    close(clientTCP);
}

void command_bid(std::string AUCIP,std::string AUCport, std::string loguid, 
                std::string logpass, std::string auction,std::string value)
{
    std::string message;
    std::string rest;
    struct addrinfo hintsClientTCP;
    struct addrinfo *resClientTCP;
    int clientTCP, err;

    setsocketTCP(clientTCP,hintsClientTCP);

    err=getaddrinfo(AUCIP.c_str(), AUCport.c_str(), &hintsClientTCP, &resClientTCP);
    if(err!=0){
        ThrowError("Error: getting address info of AS server\n");
    }

    message = "BID " + loguid + ' ' + logpass + ' ' + auction + ' ' + value + '\n';
    sendmsgTCP((char *) message.c_str(),clientTCP,resClientTCP);
    int i = receive_msgTCP(clientTCP,rest);
    if(i){
        std::cout << "The bid was accepted" << std::endl;
    }
    freeaddrinfo(resClientTCP);
    close(clientTCP);
}

void command_sr(std::string auction,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string res;
    std::string status;
    std::string rest;

    message = "SRC " + auction + '\n';
    sendmsgUDP((char *) message.c_str(),client,resClient,addr,res,status,rest);
    int i = ParseResponse(res,status);
    if(i){
        show_record(auction,rest);
    }
}