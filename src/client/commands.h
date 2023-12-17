#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <unordered_map>
#include "constants.h"

void Client(std::string AUCIP, std::string AUCport);
void command_log(std::string &loguid, std::string &logpass,std::string uid, std::string pass, int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_logout(std::string &loguid, std::string &logpass, int &client, struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_unreg(std::string &loguid, std::string &logpass, int &client, struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_exit(std::string loguid, int &client, struct addrinfo* &resClient, int &err);
void command_open(std::string loguid, std::string logpass, std::string name, std::string fname,std::string start_value,std::string timeactive);
void command_close(std::string loguid, std::string logpass, std::string auction);
void command_ma(std::string loguid, int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_mb(std::string loguid, int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_lst(int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_sa(std::string auction);
void command_bid(std::string loguid, std::string logpass, std::string auction,std::string value);
void command_sr(std::string auction,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);

#endif //commands.h