#include "server.hpp"
#include "../common/common.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <dirent.h> // For opendir, readdir, closedir
#include <vector>

bool removeDirectory(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        return false; // Failed to open directory
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string entryName = entry->d_name;

        // Skip "." and ".." entries
        if (entryName != "." && entryName != "..")
        {
            std::string fullPath = path + "/" + entryName;

            if (remove(fullPath.c_str()) != 0)
            {
                closedir(dir);
                return false; // Failed to remove file
            }
        }
    }

    closedir(dir);

    if (rmdir(path.c_str()) != 0)
    {
        return false; // Failed to remove directory
    }

    return true; // Successfully removed directory and its contents
}

void listAuctions(std::vector<std::string> ongoingAuctions)
{
    for (const auto &AID : ongoingAuctions)
    {
        std::string endFilePath = AUCTION_DIR + AID + "/END_" + AID + ".txt";

        if (std::filesystem::exists(endFilePath))
        {
            std::cout << AID << " 0" << std::endl; // Auction ended
        }
        else
        {
            std::cout << AID << " 1" << std::endl; // Auction hasn't ended
        }
    }
}

void listBids(std::vector<std::string> ongoingBids)
{
    for (const auto &AID : ongoingBids)
    {
        std::string endFilePath = AUCTION_DIR + AID + "/END_" + AID + ".txt";

        if (std::filesystem::exists(endFilePath))
        {
            std::cout << AID << " 0" << std::endl; // Auction ended
        }
        else
        {
            std::cout << AID << " 1" << std::endl; // Auction hasn't ended
        }
    }
}

void listAllAuctions()
{
    for (const auto &entry : std::filesystem::directory_iterator(AUCTION_DIR))
    {
        // Process each entry in the auction directory
        if (entry.is_directory())
        {
            std::string AID = entry.path().filename().string();

            // Check if the "END" file exists for the current auction
            std::string endFilePath = AUCTION_DIR + AID + "/END_" + AID + ".txt";

            if (std::filesystem::exists(endFilePath))
            {
                std::cout << AID << " 0" << std::endl; // Auction ended
            }
            else
            {
                std::cout << AID << " 1" << std::endl; // Auction hasn't ended
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Input Parsing
    int verbose = 0;
    char ASport[10] = "58071";

    if (argc == 3)
    {
        if (strcmp("-p", argv[1]) == 0)
            strcpy(ASport, argv[2]);
        if (strcmp("-v", argv[3]) == 0)
            verbose = 1;
    }
    else if (argc == 2)
    {
        if (strcmp("-p", argv[1]) == 0)
            strcpy(ASport, argv[2]);
    }
    else if (argc == 1)
    {
        if (strcmp("-v", argv[3]) == 0)
            verbose = 1;
    }

    std::cout << "Verbose: " << verbose << " ASport: " << ASport << std::endl;

    // UDP and TCP Servers
    int udp_fd, tcp_fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints_udp, *res_udp;
    struct addrinfo hints_tcp, *res_tcp;
    struct sockaddr_storage addr; // sockaddr_storage is used to store any type of address
    char buffer[128];
    pid_t pid;

    // Create a UDP socket
    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        exit(1); // error

    // Set up hints for getaddrinfo
    memset(&hints_udp, 0, sizeof hints_udp);
    hints_udp.ai_family = AF_INET;      // IPv4
    hints_udp.ai_socktype = SOCK_DGRAM; // UDP Socket
    hints_udp.ai_flags = AI_PASSIVE;

    // Get address information
    errcode = getaddrinfo(nullptr, PORT, &hints_udp, &res_udp);
    if (errcode != 0)
        exit(1);

    // Bind the socket
    n = bind(udp_fd, res_udp->ai_addr, res_udp->ai_addrlen);
    if (n == -1)
        exit(1);

    // Release allocated memory
    freeaddrinfo(res_udp);

    // Create a TCP socket
    if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(1);

    // Set up hints for getaddrinfo
    memset(&hints_tcp, 0, sizeof hints_tcp);
    hints_tcp.ai_family = AF_INET;       // IPv4
    hints_tcp.ai_socktype = SOCK_STREAM; // TCP socket
    hints_tcp.ai_flags = AI_PASSIVE;

    // Get address information
    if ((getaddrinfo(nullptr, PORT, &hints_tcp, &res_tcp)) != 0)
        exit(1);

    // Bind the socket
    if (bind(tcp_fd, res_tcp->ai_addr, res_tcp->ai_addrlen) == -1)
        exit(1);

    // Listen on the socket
    if (listen(tcp_fd, 5) == -1)
        exit(1);

    // Release allocated memory
    freeaddrinfo(res_tcp);

    // Processo pai recebe pedidos tcp, processo filho recebe pedidos UDP
    if ((pid = fork()) == -1)
    {
        perror("Pid valor -1");
        exit(1);
    }

    else if (pid == 0) // Processo filho
    {
        while (true)
        {
            // Receive data
            n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);

            if (n == -1)
                exit(1);

            write(1, "received: ", 10);
            write(1, buffer, n);

            char response[30];
            memset(response, 0, strlen(response));

            char action[50], value1[50], value2[50];
            memset(action, 0, sizeof(action));
            memset(value1, 0, sizeof(value1));
            memset(value2, 0, sizeof(value2));

            char *token;
            token = strtok(buffer, " ");

            if (token)
            {
                strcpy(action, token);
                token = strtok(NULL, " ");

                // Check if there is a second token
                if (token)
                {
                    strcpy(value1, token);
                    token = strtok(NULL, " ");

                    // Check if there is a third token
                    if (token)
                    {
                        strcpy(value2, token);
                    }
                }
            }

            // LIN UID password >>> RLI status
            if (strcasecmp(action, "LIN") == 0)
            {

                /* Lógica do LIN
                Verificar se o user UID já existe, se não existir regista-o e status=REG
                Verificar a password, se sim status=OK se não status=NOK
                */

                char status;
                std::string userFilePath = USER_DIR + std::string(value1);

                struct stat st = {0};
                if (stat(userFilePath.c_str(), &st) == -1)
                {
                    // User doesn't exist, set status=REG and register.
                    status = 'REG';

                    // Create the directory (UID) under USERS
                    if (mkdir(userFilePath.c_str(), 0700) != 0)
                    {
                        perror("mkdir");
                        exit(1);
                    }

                    // Create (UID)_pass.txt with value2
                    std::ofstream passFile(userFilePath + "/" + value1 + "_pass.txt");
                    passFile << value2;
                    passFile.close();

                    // Create UID_login.txt
                    std::ofstream loginFile(userFilePath + "/" + value1 + "_login.txt");
                    loginFile.close();
                }
                else
                {
                    // User exists, check the password file.
                    std::ifstream passwordFile(userFilePath + "/" + value1 + "_pass.txt");
                    std::string storedPassword;

                    if (passwordFile >> storedPassword && storedPassword == value2)
                    {
                        status = 'OK';
                    }
                    else
                    {
                        status = 'NOK';
                    }
                }

                // Print status at the end
                std::cout << status << std::endl;
            }

            // LOU UID password >>> RLO status
            if (strcasecmp(action, "LOU") == 0)
            {

                /* Lógica do LOU
                Verificar se o user UID já existe, se não existir status=UNR
                Verificar se user está logged in, se sim dar logout e status=OK
                if not logged in status=NOK
                */

                char status;
                std::string userFilePath = USER_DIR + std::string(value1);

                struct stat st = {0};
                if (stat(userFilePath.c_str(), &st) == -1)
                {
                    // User doesn't exist, set status=UNR.
                    status = 'UNR';
                }
                else
                {
                    // User exists, check the password file.
                    std::ifstream passwordFile(userFilePath + "/" + value1 + "_pass.txt");
                    std::string storedPassword;

                    if (passwordFile >> storedPassword && storedPassword == value2)
                    {
                        std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);
                        if (loginFile)
                        {
                            // User is logged in, perform logout by removing login file
                            if (remove((userFilePath).c_str()) != 0)
                            {
                                perror("remove");
                                exit(1);
                            }
                            status = 'OK';
                        }
                        else
                        {
                            status = 'NOK';
                        }
                    }

                    // Print status at the end
                    std::cout << status << std::endl;
                }

                // UNR UID password >>> RUR status
                if (strcasecmp(action, "UNR") == 0)
                {

                    /* Lógica do UNR
                    Verificar se UID está registado e logged in, se sim dar unregister e status=OK
                    Se não estiver logged in status=NOK
                    Se não estiver registado status=UNR
                    */

                    char status;
                    std::string userFilePath = USER_DIR + std::string(value1);

                    struct stat st = {0};
                    if (stat(userFilePath.c_str(), &st) == -1)
                    {
                        // User doesn't exist, set status=UNR.
                        status = 'UNR';
                    }
                    else
                    {
                        // User exists, check the password file.
                        std::ifstream passwordFile(userFilePath + "/" + value1 + "_pass.txt");
                        std::string storedPassword;

                        if (passwordFile >> storedPassword && storedPassword == value2)
                        {
                            std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);
                            if (loginFile)
                            {
                                // User is logged in, perform unregister by removing user directory
                                if (removeDirectory((userFilePath + "/" + value1).c_str()) != 0)
                                {
                                    perror("remove");
                                    exit(1);
                                }
                                status = 'OK';
                            }
                        }
                        else
                        {
                            status = 'NOK';
                        }
                    }

                    // Print status at the end
                    std::cout << status << std::endl;
                }

                // LMA UID >>> RMA status[ AID state]*
                if (strcasecmp(action, "LMA") == 0)
                {

                    /* Lógica do LMA - List My Auctions
                    Verificar se user está logged in, se não status = NLG
                    Se não tiver ongoing auctions, status = NOK
                    Se tiver, status = OK e list of the
                    identifiers AID and state for all ongoing auctions started by this user, separated by single spaces,
                    is sent by the AS. state takes value 1 if the auction is active, or 0 otherwise.
                    */

                    char status;
                    std::string userFilePath = USER_DIR + std::string(value1);

                    std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);
                    if (loginFile)
                    {
                        // Check if HOSTED dir is empty
                        std::string hostedPath = userFilePath + "/HOSTED";

                        if (std::filesystem::is_empty(hostedPath))
                        {
                            // No ongoing auctions
                            status = 'NOK';
                        }
                        else
                        {
                            // User has ongoing auctions
                            status = 'OK';

                            std::vector<std::string> ongoingAuctions;

                            for (const auto &entry : std::filesystem::directory_iterator(hostedPath))
                            {
                                // Process each file in the HOSTED directory
                                std::string fileName = entry.path().filename().string();
                                // Extract AID information
                                std::string AID = fileName.substr(0, fileName.find('.'));

                                ongoingAuctions.push_back(AID);
                            }

                            listAuctions(ongoingAuctions);
                        }
                    }
                    else
                    {
                        // User not logged in, set status=NLG.
                        status = 'NLG';
                    }

                    // Print status at the end
                    std::cout << status << std::endl;
                    // Print auctions

                    // LMB UID >>> RMB status[ AID state]*
                    if (strcasecmp(action, "LMB") == 0)
                    {

                        /* Lógica do LMB - List My Bids
                        Verificar se user está logged in, se não status = NLG
                        Se não tiver ongoing bids, status = NOK
                        Se tiver, status = OK e list of the identifiers AID and
                        state for all ongoing auctions for which this user has placed bids,
                        separated by single spaces, is sent by the AS. state takes value 1 if the auction
                        is active, or 0 otherwise.
                        */

                        char status;
                        std::string userFilePath = USER_DIR + std::string(value1);

                        std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);
                        if (loginFile)
                        {
                            // Check if BIDDED dir is empty
                            std::string biddedPath = userFilePath + "/BIDDED";

                            if (std::filesystem::is_empty(biddedPath))
                            {
                                // No ongoing bids
                                status = 'NOK';
                            }
                            else
                            {
                                // User has ongoing bids
                                status = 'OK';

                                std::vector<std::string> ongoingBids;

                                for (const auto &entry : std::filesystem::directory_iterator(biddedPath))
                                {
                                    // Process each file in the BIDDED directory
                                    std::string fileName = entry.path().filename().string();
                                    // Extract AID information
                                    std::string AID = fileName.substr(0, fileName.find('.'));

                                    ongoingBids.push_back(AID);
                                }

                                listBids(ongoingBids);
                            }
                        }
                        else
                        {
                            // User not logged in, set status=NLG.
                            status = 'NLG';
                        }

                        // Print status at the end
                        std::cout << status << std::endl;
                        // Print bids
                    }

                    // LST >>> RLS status[ AID state]*
                    if (strcasecmp(action, "LST") == 0)
                    {

                        /* Lógica do LST - List Auctions
                        If no auction has started, status = NOK
                        If there are ongoing auctions, status = OK and list of the
                        identifiers AID and state for all auctions, separated by single spaces, is sent
                        by the AS. state takes value 1 if the auction is active, or 0 otherwise.
                        */

                        char status;

                        // if auction dir empty
                        if (std::filesystem::is_empty(AUCTION_DIR))
                        {
                            // No auctions, status = NOK
                            status = 'NOK';
                        }
                        else
                        {
                            status = 'OK';
                            listAllAuctions();
                        }

                        // Print status at the end
                        std::cout << status << std::endl;
                    }

                    // SRC AID >> RRC status [host_UID auction_name asset_fname start_value start_date-time timeactive]
                    // [\nB bidder_UID bid_value bid_date-time bid_sec_time]*
                    // [\nE end_date-time end_sec_time]

                    if (strcasecmp(action, "SRC") == 0)
                    {

                        /* Lógica do SRC - Show Auction Record
                        If auction does not exist, status = NOK
                        Otherwise the status = OK followed by information about the
                        ID host_UID of the user that started the auction, the auction name
                        auction_name and the name of the file asset_fname with information
                        about the item being sold, the minimum bid value start_value, and the start
                        date and time start_date-time of the auction in the format YYYY-MMDD HH:MM:SS (19 bytes), as well as the duration of the auction timeactive
                        in seconds (represented using 6 digits).
                        If this auction has received bids then a description of each bid is presented in a
                        separate line starting with B and including: the ID of the user that place this bid
                        bidder_UID, the bid value bid_value, the bid date and time
                        bid_date-time in the format YYYY-MM-DD HH:MM:SS (19 bytes), as
                        well as the number of seconds elapsed since the beginning of the auction until
                        the bid was made bid_sec_time (represented using 6 digits).
                        In case the auction is already closed there is one last line added to the reply
                        including the date and time of the auction closing end_date-time in the
                        format YYYY-MM-DD HH:MM:SS (19 bytes), as well as the number of
                        seconds elapsed since the beginning of the auction until the bid was made
                        end_sec_time.
                        */

                        char status;

                        // if auction doesnt exist
                        if (true)
                        {
                            status = 'NOK';
                        }
                        else
                        {
                            status = 'OK';
                        }

                        // Print status at the end
                        std::cout << status << std::endl;
                        // Print auction info
                    }
                }
            }
            else
            { // Processo pai
                return 0;
            }

            close(udp_fd); // Close the socket when done
            close(tcp_fd); // Close the socket when done
        }
    }
}