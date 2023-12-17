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
    /*else if(!com.compare("ROA")){
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
    }*/
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
    if (c>='a' && c<='z' || c>='A' && c<='Z'){
        return 1;
    }
    return 0;
}

std::string read_auc_name(std::string buffer, ssize_t &i)
{
    char c;
    int max=10;
    std::string name;

    while((c=buffer.at(i))!=' '){
        if(!is_number(c) || !is_letter(c) || c!='-' || c!='_'){
            return std::string("");
        }
        name+=c;
        i++;
    }
    if (name.length()>max){
        return std::string("");
    }
    return name;
}

std::string read_user(std::string buffer, ssize_t &i)
{
    char c;
    std::string user;
    while((c=buffer.at(i))!=' '){
        if(!is_number(c)){
            return std::string("");
        }
        user+=c;
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
    int max=24;
    std::string file;

    while((c=buffer.at(i))!=' '){
        if(!is_number(c) || !is_letter(c) || c!='-' || c!='_' || c!='.'){
            return std::string("");
        }
        file+=c;
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
    int v=0;
    std::string value;
    while((c=buffer.at(i))!=' '){
        if(!is_number(c)){
            return std::string("");
        }
        value+=c;
        v=v*10+(c-'0');
        i++;
    }

    if(value.length()>6){
        return std::string("");
    }
    return value;
}

std::string read_date(std::string buffer, ssize_t &i)
{
    char c;
    int mes=0, dia=0, s=0;
    std::string date;
    while((c=buffer.at(i))!=' '){
        if(c=='-'){
            s++;
        }
        else{
            if(!is_number(c)){
                return std::string("");
            }
            if(s==1){
                mes=mes*10+(c-'0');
            }
            if(s==2){
                dia=dia*10+(c-'0');
            }
        }
        date+=c;
        i++;
    }
    if(mes>0 && mes<13 && dia>0 && dia<32){
        return date;
    }
    return std::string("");
}

std::string read_hour(std::string buffer, ssize_t &i)
{
    char c;
    int h=0, min=0, s=0;
    std::string hour;
    while((c=buffer.at(i))!=' '){
        if(c==':'){
            s++;
        }
        else{
            if(!is_number(c)){
                return std::string("");
            }
            if(s==0){
                h=h*10+(c-'0');
            }
            if(s==1){
                min=min*10+(c-'0');
            }
        }
        hour+=c;
        i++;
    }
    if(h>=0 && h<24 && min>=0 && min<60){
        return hour;
    }
    return std::string("");
}

std::string read_time(std::string buffer, ssize_t &i)
{
    char c;
    std::string time;
    while((c=buffer.at(i))!=' '){
        if(!is_number(c)){
            return std::string("");
        }
        time+=c;
        i++;
    }
    if (time.length()>6){
        return std::string("");
    }
    return time;
}

void show_list_auctions(std::string list)
{
    std::string state;
    std::vector <std::string> auctions;
    char c;
    ssize_t i=0, spaces=0;

    while((c=list.at(i))!='\n'){
        if(c==' '){
            spaces++;
        }
        else{
            if(!is_number(c)){
                std::cout << "Error in server's message" << std::endl;
                return;
            }
            if(spaces%2==0){
                auctions[spaces] += c;
            }
            else{
                if(auctions[spaces-1].length()!=3){
                    std::cout << "Error in server's message" << std::endl;
                    return;
                }
                if(c=='0'){
                    auctions[spaces-1] += " - closed;"; 
                }
                else if(c=='1'){
                    auctions[spaces-1] += " - active;"; 
                }
                else{
                    std::cout << "Error in server's message" << std::endl;
                    return;
                }
            }
        }
        i++;
    }
    for(i=0; i<auctions.size();i++){
        std::cout << auctions[i] << std::endl;
    }
}

void show_record(std::string auction, std::string record)
{
    std::vector <std::string> inf;
    char c;
    ssize_t i=0;
    //information about the auction
    inf[0]=std::string("show record ")+auction;
    inf[1]=read_user(record,i);
    inf[2]=read_auc_name(record,i);
    inf[3]=read_filename(record,i);
    inf[4]=read_value(record,i);
    inf[5]=read_date(record,i);
    inf[6]=read_hour(record,i);
    inf[7]=read_time(record,i);
    if ((c=record.at(i))==' '){
        i++;
        int j=1;
        while((c=record.at(i))=='B' && i<record.length()){ //information of bids
            i+=2;
            inf[7+j]=std::string("bid: ");
            j++;
            inf[7+j]=read_user(record,i);
            j++;
            inf[7+j]=read_value(record,i);
            j++;
            inf[7+j]=read_date(record,i);
            j++;
            inf[7+j]=read_hour(record,i);
            j++;
            inf[7+j]=read_time(record,i);
            j++;
            i++;
        }
        if(i<record.length()){ //information of when the auction was closed
            if((c=record.at(i))=='E'){
                inf[7+j]=std::string("end: ");
                j++;
                inf[7+j]=read_date(record,i);
                j++;
                inf[7+j]=read_hour(record,i);
                j++;
                inf[7+j]=read_time(record,i);
                j++;
            }
        }
    }
    if ((c=record.at(i))!='\n'){
        std::cout << "Error in server's message" << std::endl;
        return;
    }
    for(i=0; i<inf.size();i++){
        if(!inf[i].compare("")){
            std::cout << "Error in server's message" << std::endl;
            return;
        }
    }
    for(i=0; i<inf.size();i++){
        if(i==8 || i==2 || !inf[i].compare("end: ") || !inf[i].compare("bid: ")){
            std::cout << std::endl;
        }
        std::cout << inf[i] << ", ";
    }
}

char read_char(int fd)
{
    char c;
    if(read(fd, &c,1)!=1){
        ThrowError("Error: in reading the buffer from the server\n");
    }
    return c;
}

int is_read_end(char c)
{
    if (c=='\n'){
        return 1;
    }
    return 0;
}

int get_number(std::string number)
{
    int n=0;
    char c;
    for(int i=0; i<number.length();i++){
        c=number.at(i);
        if(is_number(c)){
            n=n*10+(c-'0');
        }
        else{
            return 0;
        }
    }
    return n;
}

int receive_msgTCP(int &client, std::string &rest)
{
    char c;
    std::string com, status, response;
    c=read_char(client);
    response=c;
    while(!is_read_end(c)){
        c=read_char(client);
        response+=c;
    }
    std::cout<<response<<std::endl;
    if(response.length()>=7){
        com = response.substr(0,3);
        status = response.substr(4,3);
        if (response.length()>8){
            rest=response.substr(8,3); //para a resposta de OPA
            if(response.at(8+3)!='\n'){
                std::cout << "Error in server's message" << std::endl;
                return 0;
            }
        }
        else{
            rest=std::string("");
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
    int  spaces=0;
    ssize_t n_written=0, size;
    char c;
    std::string com, status, response, filename, f_size;
    c=read_char(client);
    response=c;
    while(!is_read_end(c) || spaces<4){
        c=read_char(client);
        response+=c;
        if(c==' '){
            spaces++;
        }
    }
    if(response.length()>=7){
        com = response.substr(0,3);
        status = response.substr(4,3);
        if (response.length()>8){
            ssize_t i=8;
            filename=read_filename(response,i);
            i++;
            f_size=read_value(response,i);
            size=get_number(f_size);
        }
    }
    else{
        std::cout << "Error in server's message" << std::endl;
        return 0;
    }
    if(ParseResponse(com,status)){
        return save_to_file(client, filename.c_str(),size);
    }
    return 0;
}

int save_to_file(int client, std::string file, ssize_t size)
{
    int fd = open((char *)file.c_str(), O_CREAT | O_RDWR | O_TRUNC);
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
        return 0;
    }
    return 1;
}