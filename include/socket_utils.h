#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stddef.h>

int create_client_socket(const char *server_ip, int port);
int create_server_socket(int port);
int accept_client(int server_fd);

int send_message(int sockfd, const char *message);
int receive_message(int sockfd, char *buffer, size_t buffer_size);

void close_socket(int sockfd);
void cleanup_winsock(void);

#endif