#include "parser.h"
#include "constants.h"
#include "socket_functions.h"
#include <unistd.h>
#include <sys/stat.h>
#include <bits/stdc++.h> 
#include <fcntl.h>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

int ParseResponse(std::string com, std::string status)
{
    if(!status.compare("ERR")){
        std::cout << "Syntax of request is incorrect" << std::endl; 
        return 0;
    }
    else if (!com.compare("RLI")){
        if(!status.compare("OK")){
            std::cout << "User successfuly logged in" << std::endl;
            return 1;
        }
        else if(!status.compare("REG")){
            std::cout << "New user registered and successfully logged in" << std::endl;
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "incorrect loggin attempt" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RLO")){
        if(!status.compare("OK")){
            std::cout << "User successfuly logged out" << std::endl;
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "User not logged in" << std::endl;
            return 0;
        }
        else if(!status.compare("UNR")){
            std::cout << "Unknown user" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RUR")){
        if(!status.compare("OK")){
            std::cout << "User successfuly logged out" << std::endl;
            std::cout << "User successfuly unregistered" << std::endl;
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "incorrect unregister attempt" << std::endl;
            return 0;
        }
        else if(!status.compare("UNR")){
            std::cout << "Unknown user" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RMA")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "Currently the user has no active auctions" << std::endl;
            return 0;
        } 
        else if(!status.compare("NLG")){
            std::cout << "User not logged in" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RMB")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "User has no active auctions bids" << std::endl;
            return 0;
        }
        else if(!status.compare("NLG")){
            std::cout << "User not logged in" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RLS")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "there are no active auctions" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RRC")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "The auction doesn't exist" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("ROA")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "The auction could not be started" << std::endl;
            return 0;
        }
        else if(!status.compare("NLG")){
            std::cout << "User not logged in" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RCL")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("END")){
            std::cout << "The auction has already ended" << std::endl;
            return 0;
        }
        else if(!status.compare("NLG")){
            std::cout << "User not logged in" << std::endl;
            return 0;
        }
        else if(!status.compare("EAU")){
            std::cout << "The auction doesn't exist" << std::endl;
            return 0;
        }
        else if(!status.compare("EOW")){
            std::cout << "The auction was not started by user" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RSA")){
        if(!status.compare("OK")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "There is no file to be sent or an error has occurred" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    else if(!com.compare("RBD")){
        if(!status.compare("ACC")){
            return 1;
        }
        else if(!status.compare("NOK")){
            std::cout << "The auction is not active" << std::endl;
            return 0;
        }
        else if(!status.compare("NLG")){
            std::cout << "User not logged in" << std::endl;
            return 0;
        }
        else if(!status.compare("REF")){
            std::cout << "The bid was refused because there was already a higher one" << std::endl;
            return 0;
        }
        else if(!status.compare("ILG")){
            std::cout << "The bid was refused since the auction belongs to user" << std::endl;
            return 0;
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    //com==ERR
    std::cout << "Unexpected protocol message" << std::endl;
    return 0;   
}

int is_number(char c)
{
    if (c>='0' && c<='9'){
        return 1;
    }
    return 0;
}

int is_letter(char c)
{
    if ((c>='a' && c<='z') || (c>='A' && c<='Z')){
        return 1;
    }
    return 0;
}

std::string read_string(std::string buffer, ssize_t &i)
{
    char c;
    std::string str=std::string("");
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(is_number(c) || is_letter(c)){
            str.append(&c);
            i++;
        }
        else{
            return std::string("");
        }
    }
    return str;
}

std::string read_auc_name(std::string buffer, ssize_t &i)
{
    char c;
    ssize_t max=10;
    std::string name;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(is_number(c) || is_letter(c) || c=='-' || c=='_'){
            name.append(&c);
            i++;
        }
        else{
            return std::string("");
        }
    }
    if (name.size()>max){
        return std::string("");
    }
    return name;
}

std::string read_user(std::string buffer, ssize_t &i)
{
    char c;
    std::string user;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(!is_number(c)){
            return std::string("");
        }
        user.append(&c);
        i++;
    }
    if (user.length()!=6){
        return std::string("");
    }
    return user;
}

std::string read_filename(std::string buffer, ssize_t &i)
{
    char c;
    ssize_t max=24;
    std::string file;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(!is_number(c) && !is_letter(c) && c!='-' && c!='_' && c!='.'){
            return std::string("");
        }
        file.append(&c);
        i++;
    }
    if (file.length()>24){
        return std::string("");
    }
    if((c=file.at(file.length()-4))!='.'){
        return std::string("");
    }
    return file;
}

std::string read_value(std::string buffer, ssize_t &i)
{
    char c;
    std::string value;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(!is_number(c)){
            return std::string("");
        }
        value.append(&c);
        i++;
    }

    if(value.length()>6){
        return std::string("");
    }
    return value;
}

std::string read_fsize(std::string buffer, ssize_t &i)
{
    char c;
    std::string value;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(!is_number(c)){
            return std::string("");
        }
        value.append(&c);
        i++;
    }

    if(value.length()>8){
        return std::string("");
    }
    return value;
}

std::string read_date(std::string buffer, ssize_t &i)
{
    char c;
    std::string date;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(c=='-' || is_number(c)){
            i++;
            date.append(&c);
        }
        else{
            return std::string("");
        }
    }
    if(date.size()!=10){
        return std::string("");
    }
    return date;
}

std::string read_hour(std::string buffer, ssize_t &i)
{
    char c;
    std::string hour;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(c==':' || is_number(c)){
            i++;
            hour.append(&c);
        }
        else{
            return std::string("");
        }
    }
    if(hour.size()!=8){
        return std::string("");
    }
    return hour;
}

std::string read_time(std::string buffer, ssize_t &i)
{
    char c;
    std::string time;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(!is_number(c)){
            return std::string("");
        }
        time.append(&c);
        i++;
    }
    if (time.length()>6){
        return std::string("");
    }
    return time;
}

std::string read_auction(std::string buffer, ssize_t &i)
{
    char c;
    std::string auc;
    if(i>=buffer.size()){
        return std::string("");
    }
    while((c=buffer.at(i))!=' ' && c!='\n'){
        if(!is_number(c)){
            return std::string("");
        }
        auc.append(&c);
        i++;
    }

    if(auc.length()>3){
        return std::string("");
    }
    return auc;
}

void show_list_auctions(std::string list)
{
    std::string show, aux;
    char c;
    ssize_t i=0, spaces=0;

    while((c=list.at(i))!='\n'){
        aux=read_auction(list,i);
        if(!aux.compare("")){
            std::cout << "Error in server's message" << std::endl;
            return;
        }
        show.append(aux);
        i++;
        if((c=list.at(i))=='0'){
            show.append(": ended\n");
        }
        else if(c=='1'){
            show.append(": active\n");
        }
        else{
            std::cout << "Error in server's message" << std::endl;
            return;
        }
        i++;
        if((c=list.at(i))==' '){
            i++;
        }
    }
    std::cout << show << std::endl;
}

void show_record(std::string auction, std::string record)
{
    std::string inf, aux;
    char c;
    ssize_t i=0;
    //information about the auction
    inf.append(std::string("show record "));
    inf.append(auction);
    inf.append(std::string("\n"));
    aux=read_user(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    inf.append(" ");
    i++;
    aux=read_auc_name(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    inf.append(" ");
    i++;
    aux=read_filename(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    inf.append(" ");
    i++;
    aux=read_value(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    inf.append(" ");
    i++;
    aux=read_date(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    inf.append(" ");
    i++;
    aux=read_hour(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    inf.append(" ");
    i++;
    aux=read_time(record,i);
    if(!aux.compare("")){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    inf.append(aux);
    if ((c=record.at(i))==' '){
        i++;
        while((c=record.at(i-1))!='\n' && c=='B' && i<record.size()){ //information of bids
            i+=2;
            inf.append(std::string("\nbid: "));
            aux=read_user(record,i);

            if(!aux.compare("")){
                std::cout << "Error in server's message" << std::endl;
                return;
            }
            inf.append(aux);
            inf.append(" ");
            i++;
            aux=read_value(record,i);
            if(!aux.compare("")){
                std::cout << "Error in server's message" << std::endl;
                return;
            }
            inf.append(aux);
            inf.append(" ");
            i++;
            aux=read_date(record,i);
            if(!aux.compare("")){
                std::cout << "Error in server's message" << std::endl;
                return;
            }
            inf.append(aux);
            inf.append(" ");
            i++;
            aux=read_hour(record,i);
            if(!aux.compare("")){
                std::cout << "Error in server's message" << std::endl;
                return;
            }
            inf.append(aux);
            inf.append(" ");
            i++;
            aux=read_time(record,i);
            if(!aux.compare("")){
                std::cout << "Error in server's message" << std::endl;
                return;
            }
            inf.append(aux);
            inf.append(" ");
            i++;
        }
    }
    if ((c=record.at(i-1))!='\n'){
        if(i<record.size()){ //information of when the auction was closed
            if((c=record.at(i))=='E'){
                inf.append(std::string("\nend: "));
                i+=2;
                aux=read_date(record,i);
                if(!aux.compare("")){
                    std::cout << "Error in server's message" << std::endl;
                    return;
                }
                inf.append(aux);
                inf.append(" ");
                i++;
                aux=read_hour(record,i);
                if(!aux.compare("")){
                    std::cout << "Error in server's message" << std::endl;
                    return;
                }
                inf.append(aux);
                inf.append(" ");
                i++;
                aux=read_time(record,i);
                if(!aux.compare("")){
                    std::cout << "Error in server's message" << std::endl;
                    return;
                }
                inf.append(aux);
                i++;
            }
        }
    }
    
    std::cout << inf << std::endl;
}

int get_number(std::string number)
{
    ssize_t n;
    sscanf(number.c_str(), "%zu", &n);
    return n;
}

int receive_msgTCP(int &client, std::string &rest)
{
    char c;
    int r;
    ssize_t i=0;
    std::string com, status, response;
    response=std::string("");
    while((r=read(client, &c, 1))!=0){ 
        if(r==-1){
            ThrowError("Error: in reading the buffer from the server\n");
        }
        response+=c;
    }

    if(response.length()>=7){
        com = read_string(response,i);
        i++;
        status = read_string(response,i);
        if(response.at(i)!='\n'){
            i++;
            rest= read_string(response,i); //para a resposta de OPA
        }
    }
    else{
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    return ParseResponse(com, status);
}

int receive_file(int client)
{
    int  r, spaces=0;
    ssize_t n_written=0, size, i=0;
    char c;
    std::string com, status, response, filename, f_size;
    response=std::string("");
    while((r=read(client, &c, 1))!=0 && spaces<4){ 
        if(r==-1){
            ThrowError("Error: in reading the buffer from the server\n");
        }
        response+=c;
        if(c==' '){
            spaces++;
        }
    }

    std::cout<<response<<std::endl; //testing
    
    if(response.length()>=7){
        com = read_string(response,i);
        i++;
        status = read_string(response,i);
        i++;
        filename=read_filename(response,i);
        i++;
        f_size=read_fsize(response,i);
        size=get_number(f_size);
    }
    else{
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    if(ParseResponse(com,status)){
        if(!filename.compare("") || !f_size.compare("")){
            std::cout << "Error in server's message" << std::endl;
            return 0;
        }
        return save_to_file(client, filename.c_str(),size);
    }
    return 0;
}

int save_to_file(int client, std::string file, ssize_t size)
{
    mode_t filePerms = S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;  /*xrw-rw-rw*/
    int fd = open((char *)file.c_str(), O_CREAT | O_RDWR | O_TRUNC, filePerms);
    if(fd==-1){
        ThrowError("Error: in downloading in file\n"); //couldn't create file
    }
    ssize_t n_read=0, to_read, written, to_write, n;
    char buffer[MAX_BUFFER];
    bzero(buffer,MAX_BUFFER*sizeof(char));
    while((to_write=read(client,buffer,MAX_BUFFER))>0){ // read file data from server
        if(to_write==-1){
            ThrowError("Error: in reading file from server\n");
        }
        char c=buffer[to_write];
        n_read+=to_write;
        written=0;
        while(written<to_write){   //writes file data in a local copy
            if(n_read>=size+1 && (c=='\n' || c==' ')){
                n_read--;
                buffer[to_write]='\0';
                to_write--;
            }
            n=write(fd,buffer,to_write-written);
            if(n==-1){
                ThrowError("Error: in downloading in file\n");
            }
            written+=n;
        }
    }
    close(fd);
    if(n_read!=size){
        int i=unlink((char *)file.c_str());
        if(i==-1){
            ThrowError("Error: in deleting file\n");
        }
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    return 1;
}