#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

#define DEFAULT_HOSTNAME "lima" //? not sure about this, check with teacher
#define DEFAULT_PORT "58011"

#define UDP_TIMEOUT_SECONDS (4)
#define UDP_RESEND_TRIES (3)
#define TCP_READ_TIMEOUT_SECONDS (15)
#define TCP_WRITE_TIMEOUT_SECONDS (20 * 60) // 20 min
#define SERVER_RECV_RESTART_TIMEOUT_SECONDS (3)

#define SOCKET_BUFFER_LEN (256)
#define PACKET_ID_LEN (3)

#define FILE_BUFFER_LEN (512)

#endif
