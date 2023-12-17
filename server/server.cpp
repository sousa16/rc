#include "server.hpp"

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
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

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

void parseDateTime(const std::string &dateTimeStr, std::time_t &dateTime)
{
    std::tm tm = {};
    std::istringstream ss(dateTimeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    dateTime = std::mktime(&tm);
}

void readAuctionInfo(const std::string &fileName, int &UID, std::string &name, std::string &asset_fname,
                     double &start_value, int &timeactive, std::time_t &start_datetime, std::time_t &start_fulltime)
{
    std::ifstream file(fileName);
    if (file.is_open())
    {
        file >> UID >> name >> asset_fname >> start_value >> timeactive;

        std::string startDateTimeStr;
        file >> startDateTimeStr;
        parseDateTime(startDateTimeStr, start_datetime);

        file >> start_fulltime;
        file.close();
    }
}

void readBidInfo(const std::string &fileName, std::vector<std::tuple<int, double, std::time_t, int>> &bids)
{
    std::ifstream file(fileName);
    if (file.is_open())
    {
        int UID;
        double bid_value;
        std::time_t bid_datetime;
        int bid_sec_time;

        while (file >> UID >> bid_value)
        {
            std::string bidDateTimeStr;
            file >> bidDateTimeStr;
            parseDateTime(bidDateTimeStr, bid_datetime);
            file >> bid_sec_time;
            bids.emplace_back(UID, bid_value, bid_datetime, bid_sec_time);
        }
        file.close();
    }
}

void listAuctionRecord(const std::string &AID)
{
    std::string auctionDir = AUCTION_DIR + AID;
    std::string endFile = auctionDir + "/END.txt";
    int UID;
    std::string name, asset_fname;
    double start_value;
    int timeactive;
    std::time_t start_datetime, start_fulltime;
    std::vector<std::tuple<int, double, std::time_t, int>> bids;

    std::string startFile = auctionDir + "/START.txt";
    readAuctionInfo(startFile, UID, name, asset_fname, start_value, timeactive, start_datetime, start_fulltime);

    std::string bidsFile = auctionDir + "/BIDS.txt";
    readBidInfo(bidsFile, bids);

    std::cout << UID << name << asset_fname << start_value << start_datetime << timeactive;

    if (!bids.empty())
    {
        for (const auto &bid : bids)
        {
            std::cout << "\nB " << std::get<0>(bid) << std::get<1>(bid) << std::put_time(std::localtime(&std::get<2>(bid)), "%Y-%m-%d %H:%M:%S") << std::get<3>(bid);
        }
    }

    if (std::filesystem::exists(endFile))
    {
        std::ifstream endFileStream(endFile);
        std::time_t end_datetime;
        int end_sec_time;

        endFileStream >> std::get_time(std::localtime(&end_datetime), "%Y-%m-%d %H:%M:%S");
        endFileStream >> end_sec_time;

        std::cout << "\nE " << std::put_time(std::localtime(&end_datetime), "%Y-%m-%d %H:%M:%S") << end_sec_time;
    }
}

// Function to generate the next AID
std::string generateNextAID(int &baseAID)
{
    // Increment the baseAID for the next auction
    ++baseAID;

    // Convert the incremented baseAID to a three-digit string
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << baseAID;

    return oss.str();
}

std::time_t getAuctionStart(std::string AID)
{
    // Read the content from the "START_AID.txt" file
    std::ifstream startFile(AUCTION_DIR + AID + "/START_" + AID + ".txt");
    if (startFile.is_open())
    {
        std::string line;
        if (std::getline(startFile, line))
        {
            std::istringstream iss(line);

            // Extracting values from the line
            std::string UID, name, asset, fname, start, value, timeactive, startDatetime, startFulltime;

            if (iss >> UID >> name >> asset >> fname >> start >> value >> timeactive >> startDatetime >> startFulltime)
            {
                std::tm auctionStartTime = {};
                std::istringstream dateStream(startDatetime);
                dateStream >> std::get_time(&auctionStartTime, "%F %T");

                // Convert 'auctionStartTime' to time_t
                return std::mktime(&auctionStartTime);
            }
        }

        // Close the file
        startFile.close();
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int baseAID = 0;

    // Input Parsing
    int verbose = 0;
    char ASport[10] = "58071";

    if (argc == 4)
    {
        if (strcmp("-p", argv[1]) == 0)
            strcpy(ASport, argv[2]);
        if (strcmp("-v", argv[3]) == 0)
            verbose = 1;
    }
    else if (argc == 3)
    {
        if (strcmp("-p", argv[1]) == 0)
            strcpy(ASport, argv[2]);
    }
    else if (argc == 2)
    {
        if (strcmp("-v", argv[3]) == 0)
            verbose = 1;
    }

    std::cout << "Verbose: " << verbose << " ASport: " << ASport << std::endl;

    // UDP
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
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

    // Set up hints for getaddrinfo
    memset(&hints_udp, 0, sizeof hints_udp);
    hints_udp.ai_family = AF_INET;      // IPv4
    hints_udp.ai_socktype = SOCK_DGRAM; // UDP Socket
    hints_udp.ai_flags = AI_PASSIVE;

    // Get address information
    errcode = getaddrinfo(nullptr, PORT, &hints_udp, &res_udp);
    if (errcode != 0)
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

    // Bind the socket
    n = bind(udp_fd, res_udp->ai_addr, res_udp->ai_addrlen);
    if (n == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

    // Release allocated memory
    freeaddrinfo(res_udp);

    // Create a TCP socket
    if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

    // Set up hints for getaddrinfo
    memset(&hints_tcp, 0, sizeof hints_tcp);
    hints_tcp.ai_family = AF_INET;       // IPv4
    hints_tcp.ai_socktype = SOCK_STREAM; // TCP socket
    hints_tcp.ai_flags = AI_PASSIVE;

    // Get address information
    if ((getaddrinfo(nullptr, PORT, &hints_tcp, &res_tcp)) != 0)
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

    // Bind the socket
    if (bind(tcp_fd, res_tcp->ai_addr, res_tcp->ai_addrlen) == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

    // Listen on the socket
    if (listen(tcp_fd, 5) == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        exit(1); // error
    }

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
            {
                fprintf(stderr, "%s", strerror(errno));
                exit(1);
            }

            write(1, "received: ", 10);
            write(1, buffer, n);

            char response[30];
            memset(response, 0, strlen(response));

            char action[50], value1[50], value2[50], value3[50], value4[50], value5[50], value6[50], value7[50], value8[50];
            memset(action, 0, sizeof(action));
            memset(value1, 0, sizeof(value1));
            memset(value2, 0, sizeof(value2));
            memset(value3, 0, sizeof(value3));
            memset(value4, 0, sizeof(value4));
            memset(value5, 0, sizeof(value5));
            memset(value6, 0, sizeof(value6));
            memset(value7, 0, sizeof(value7));
            memset(value8, 0, sizeof(value8));

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
                        token = strtok(NULL, " ");

                        if (token)
                        {
                            strcpy(value3, token);
                            token = strtok(NULL, " ");

                            if (token)
                            {
                                strcpy(value4, token);
                                token = strtok(NULL, " ");

                                if (token)
                                {
                                    strcpy(value5, token);
                                    token = strtok(NULL, " ");

                                    if (token)
                                    {
                                        strcpy(value6, token);
                                        token = strtok(NULL, " ");

                                        if (token)
                                        {
                                            strcpy(value7, token);
                                            token = strtok(NULL, " ");

                                            if (token)
                                            {
                                                strcpy(value8, token);
                                                token = strtok(NULL, " ");
                                            }
                                        }
                                    }
                                }
                            }
                        }
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

                std::string status;
                std::string userFilePath = USER_DIR + std::string(value1);

                struct stat st = {};
                if (stat(userFilePath.c_str(), &st) == -1)
                {
                    // User doesn't exist, set status=REG and register.
                    status = "REG";

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
                        status = "OK";
                    }
                    else
                    {
                        status = "NOK";
                    }
                }

                // Print status at the end
                std::cout << "RLI " << status << std::endl;
            }

            // LOU UID password >>> RLO status
            if (strcasecmp(action, "LOU") == 0)
            {

                /* Lógica do LOU
                Verificar se o user UID já existe, se não existir status=UNR
                Verificar se user está logged in, se sim dar logout e status=OK
                if not logged in status=NOK
                */

                std::string status;
                std::string userFilePath = USER_DIR + std::string(value1);

                struct stat st = {};
                if (stat(userFilePath.c_str(), &st) == -1)
                {
                    // User doesn't exist, set status=UNR.
                    status = "UNR";
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
                            status = "OK";
                        }
                        else
                        {
                            status = "NOK";
                        }
                    }

                    // Print status at the end
                    std::cout << "RLO " << status << std::endl;
                }

                // UNR UID password >>> RUR status
                if (strcasecmp(action, "UNR") == 0)
                {

                    /* Lógica do UNR
                    Verificar se UID está registado e logged in, se sim dar unregister e status=OK
                    Se não estiver logged in status=NOK
                    Se não estiver registado status=UNR
                    */

                    std::string status;
                    std::string userFilePath = USER_DIR + std::string(value1);

                    struct stat st = {};
                    if (stat(userFilePath.c_str(), &st) == -1)
                    {
                        // User doesn't exist, set status=UNR.
                        status = "UNR";
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
                                status = "OK";
                            }
                        }
                        else
                        {
                            status = "NOK";
                        }
                    }

                    // Print status at the end
                    std::cout << "RUR " << status << std::endl;
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

                    std::string status;
                    std::string userFilePath = USER_DIR + std::string(value1);

                    std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);
                    if (loginFile)
                    {
                        // Check if HOSTED dir is empty
                        std::string hostedPath = userFilePath + "/HOSTED";

                        if (std::filesystem::is_empty(hostedPath))
                        {
                            // No ongoing auctions
                            status = "NOK";
                            std::cout << "RMA " << status << std::endl;
                        }
                        else
                        {
                            // User has ongoing auctions
                            status = "OK";

                            // Print status at the end
                            std::cout << "RMA " << status << " ";

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
                        status = "NLG";
                    }

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

                        std::string status;
                        std::string userFilePath = USER_DIR + std::string(value1);

                        std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);
                        if (loginFile)
                        {
                            // Check if BIDDED dir is empty
                            std::string biddedPath = userFilePath + "/BIDDED";

                            if (std::filesystem::is_empty(biddedPath))
                            {
                                // No ongoing bids
                                status = "NOK";
                                std::cout << "RMB " << status << std::endl;
                            }
                            else
                            {
                                // User has ongoing bids
                                status = "OK";

                                std::cout << "RMB " << status << " ";

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
                            status = "NLG";
                        }
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

                        std::string status;

                        // if auction dir empty
                        if (std::filesystem::is_empty(AUCTION_DIR))
                        {
                            // No auctions, status = NOK
                            status = "NOK";
                            std::cout << "RLS " << status << std::endl;
                        }
                        else
                        {
                            status = "OK";
                            std::cout << "RLS " << status << " ";

                            listAllAuctions();
                        }
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

                        std::string status;

                        // if auction dir empty
                        if (std::filesystem::is_empty(AUCTION_DIR))
                        {
                            // No auctions, status = NOK
                            status = "NOK";
                            std::cout << "RRC " << status << std::endl;
                        }
                        else
                        {
                            status = "OK";
                            std::cout << "RRC " << status << " ";
                            listAuctionRecord(value1);
                        }

                        // Print status at the end
                        std::cout << status << std::endl;
                    }
                }
            }
            else // Processo pai
            {
                // OPA UID password name start_value timeactive Fname Fsize Fdata >>> ROA status [AID]
                if (strcasecmp(action, "OPA") == 0)
                {

                    /* Lógica do OPA
                    user opens tcp conn with AS
                    Verificar se é possível criar auction e status=OK
                    Se não estiver logged in status=NLG
                    Se não for possivel criar auction status=NOK
                    */

                    std::string status;
                    std::string userFilePath = USER_DIR + std::string(value1);
                    std::ifstream passwordFile(userFilePath + "/" + value1 + "_pass.txt");
                    std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);

                    if (loginFile && (passwordFile >> value2))
                    {
                        std::string AID = generateNextAID(baseAID);
                        std::string auctionPath = AUCTION_DIR + AID;

                        // Create the directory
                        std::filesystem::create_directory(auctionPath);

                        // Create and write to "START_AID.txt"
                        std::ofstream startFile(auctionPath + "/START_" + AID + ".txt");
                        if (startFile.is_open())
                        {
                            status = "OK";
                            // Get current date and time
                            std::time_t currentTime = std::time(nullptr);
                            std::tm *localTime = std::localtime(&currentTime);
                            std::ostringstream oss;
                            oss << std::put_time(localTime, "%F %T");

                            // Write content to the file
                            startFile << value1 << " " << value3 << " " << value6 << " " << value4 << " "
                                      << value5 << " " << oss.str() << " " << currentTime;
                            startFile.close();

                            // STORE FILE COPY???

                            std::cout << "ROA " << status << " " << AID << std::endl;
                        }
                        else
                        {
                            status = "NOK";
                            std::cout << "ROA " << status << std::endl;
                        }
                    }
                    else
                    {
                        // User not logged in, set status=NLG.
                        status = "NLG";
                        std::cout << "ROA " << status << std::endl;
                    }
                }

                // CLS UID password AID >>> RCL status
                if (strcasecmp(action, "CLS") == 0)
                {

                    /* Lógica do CLS
                    user opens tcp conn with AS
                    status=OK se auction AID ongoing, was started by user UID and could be closed by AS
                    Se não estiver logged in status=NLG X
                    Se auction não existir status=EAU
                    Se user not owner status=EOW
                    Se auction already finished status=END
                    */

                    std::string status;
                    std::string userFilePath = USER_DIR + std::string(value1);
                    std::ifstream passwordFile(userFilePath + "/" + value1 + "_pass.txt");
                    std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);

                    if (loginFile && (passwordFile >> value2))
                    {
                        // Check if auction exists
                        std::string auctionPath = AUCTION_DIR + std::string(value3);

                        if (std::filesystem::exists(auctionPath) && std::filesystem::is_directory(auctionPath))
                        {
                            // Auction exists, check if UID is owner
                            std::ifstream userAuctionFile(userFilePath + "/HOSTED/" + std::string(value3) + ".txt", std::ios::binary);

                            if (userAuctionFile)
                            {
                                // User is owner, check if finished
                                std::ifstream auctionEndFile(auctionPath + "/END_" + value3 + ".txt", std::ios::binary);
                                if (auctionEndFile)
                                {
                                    // Auction finished
                                    status = "END";
                                }
                                else
                                {
                                    // Auction ongoing, close
                                    status = "OK";

                                    // Create and write to "END_AID.txt"
                                    std::ofstream endFile(auctionPath + "/END_" + value3 + ".txt");
                                    if (endFile.is_open())
                                    {
                                        // Get current date and time
                                        std::time_t currentTime = std::time(nullptr);
                                        std::tm *localTime = std::localtime(&currentTime);
                                        std::ostringstream oss;
                                        oss << std::put_time(localTime, "%F %T");

                                        // Write content to the file
                                        endFile << oss.str() << " " << currentTime;
                                        endFile.close();
                                    }
                                }
                            }
                            else
                            {
                                // User not owner
                                status = "EOW";
                            }
                        }
                        else
                        {
                            // auction doesnt exist
                            status = "EAU";
                        }
                    }
                    else
                    {
                        // User not logged in, set status=NLG.
                        status = "NLG";
                    }

                    // Print status at the end
                    std::cout << "RCL " << status << std::endl;
                }

                // SAS AID >>> RSA status [Fname Fsize Fdata]
                if (strcasecmp(action, "SAS") == 0)
                {

                    /* Lógica do SAS
                    status=OK e show asset
                    If there is no file to be sent, or some other problem, the AS replies with status = NOK.
                    */

                    std::string status;
                    std::string assetData;
                    std::string userFilePath = USER_DIR + std::string(value1);

                    if (true)
                    {
                        status = "OK";
                        std::cout << "RSA " << status << " " << assetData << std::endl;
                    }
                    else
                    {
                        status = "NOK",
                        std::cout << "RSA " << status << std::endl;
                    }
                }

                // BID UID password AID value >>> RBD status
                if (strcasecmp(action, "BID") == 0)
                {

                    /* Lógica do BID
                    status=NOK if auction AID is not active.
                    Se não estiver logged in status=NLG
                    If auction AID is ongoing status=ACC if the bid was accepted
                    status=REF if the bid was refused because a larger bid has already been placed previously
                    status=ILG if the user tries to make a bid in an auction hosted by himself.
                    */

                    std::string status;
                    std::string userFilePath = USER_DIR + std::string(value1);
                    std::ifstream passwordFile(userFilePath + "/" + value1 + "_pass.txt");
                    std::ifstream loginFile(userFilePath + "/" + value1 + "_login.txt", std::ios::binary);

                    if (loginFile && (passwordFile >> value2))
                    {
                        // Check if auction exists and is active
                        std::string auctionPath = AUCTION_DIR + std::string(value3);
                        std::ifstream startFile(auctionPath + "/START_" + value3 + ".txt", std::ios::binary);

                        if (std::filesystem::exists(auctionPath) && std::filesystem::is_directory(auctionPath) && startFile)
                        {
                            // Auction exists and is active, check if it is hosted by himself
                            std::ifstream userAuctionFile(userFilePath + "/HOSTED/" + std::string(value3) + ".txt", std::ios::binary);

                            if (userAuctionFile)
                            {
                                // User is owner
                                status = "ILG";
                            }
                            else
                            {
                                // User not owner, check if bid is accepted
                                std::string bidPath = auctionPath + "/BIDS/";
                                int intValue = std::atoi(value4);

                                // Iterate through existing bid files
                                bool largerBidExists = false;
                                for (const auto &entry : std::filesystem::directory_iterator(bidPath))
                                {
                                    if (entry.is_regular_file())
                                    {
                                        // Extract the bid value from the file name
                                        std::string fileName = entry.path().filename().stem();
                                        int existingBidValue = std::stoi(fileName);

                                        // Compare with the new bid value
                                        if (existingBidValue > intValue)
                                        {
                                            largerBidExists = true;
                                            break;
                                        }
                                    }
                                }
                                if (largerBidExists)
                                {
                                    // Larger bid already placed
                                    status = "REF";
                                }
                                else
                                {
                                    // Bid accepted
                                    status = "ACC";

                                    // Create bid txt file
                                    std::ostringstream bidFileName;
                                    bidFileName << std::setw(6) << std::setfill('0') << intValue << ".txt";
                                    std::ofstream bidFile(bidPath + bidFileName.str());

                                    // Create bid datetime and bid sec time
                                    std::time_t currentTime = std::time(nullptr);
                                    std::tm *localTime = std::localtime(&currentTime);
                                    std::ostringstream bidDatetime;
                                    bidDatetime << std::put_time(localTime, "%F %T");

                                    std::time_t auctionStartTime = getAuctionStart(value3);

                                    long bidSecTime = static_cast<long>(currentTime - auctionStartTime);

                                    bidFile << value1 << " " << value4 << " " << bidDatetime.str() << " " << bidSecTime << std::endl;
                                }
                            }
                        }
                        else
                        {
                            // auction doesnt exist or is not active
                            status = "NOK";
                        }
                    }
                    else
                    {
                        // User not logged in, set status=NLG.
                        status = "NLG";
                    }

                    // Print status at the end
                    std::cout << "RBD " << status << std::endl;
                }
            }

            close(udp_fd); // Close the socket when done
            close(tcp_fd); // Close the socket when done
        }
    }
}