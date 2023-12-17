#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H

#include "constants.h"
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

int ParseResponse(std::string com, std::string status);
int save_to_file(int client, std::string file, ssize_t size);
int receive_file(int client);
int receive_msgTCP(int &client, std::string &rest);
int is_read_end(char c);
char read_char(int fd);
void show_record(std::string auction, std::string record);
void show_list_auctions(std::string list);
std::string read_time(std::string buffer, int &i);
std::string read_hour(std::string buffer, ssize_t &i);
std::string read_date(std::string buffer, ssize_t &i);
std::string read_value(std::string buffer, ssize_t &i);
std::string read_filename(std::string buffer, ssize_t &i);
std::string read_user(std::string buffer, ssize_t &i);
std::string read_auc_name(std::string buffer, ssize_t &i);
int is_letter(char c);
int is_number(char c);
int get_number(std::string number);

#endif //parser.h