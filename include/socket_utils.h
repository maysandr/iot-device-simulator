#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stddef.h>
#ifdef _WIN32
# include <winsock2.h>
typedef SOCKET socket_handle_t;
# define INVALID_SOCKET_HANDLE INVALID_SOCKET
#else
# include <sys/types.h>
# include <sys/socket.h>
typedef int socket_handle_t;
# define INVALID_SOCKET_HANDLE (-1)
#endif

socket_handle_t create_client_socket(const char *server_ip, int port);
socket_handle_t create_server_socket(int port);
socket_handle_t accept_client(socket_handle_t server_fd);

/* Newline-delimited telemetry framing; send/receive one complete message. */
int send_message(socket_handle_t sockfd, const char *message);
/* >0 for a complete message, 0 for clean EOF, -1 on error/overflow. */
int receive_message(socket_handle_t sockfd, char *buffer, size_t buffer_size);

void close_socket(socket_handle_t sockfd);
void cleanup_sockets(void);

#endif
