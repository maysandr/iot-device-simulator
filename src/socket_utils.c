#include "socket_utils.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

static int winsock_initialized = 0;

static int initialize_winsock(void) {
    WSADATA wsa;

    if (winsock_initialized) {
        return 0;
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return -1;
    }

    winsock_initialized = 1;
    return 0;
}

int create_client_socket(const char *server_ip, int port) {
    SOCKET sockfd;
    struct sockaddr_in server_addr;

    if (initialize_winsock() != 0) {
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("socket creation failed\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((unsigned short)port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("inet_pton failed\n");
        closesocket(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("connect failed\n");
        closesocket(sockfd);
        return -1;
    }

    return (int)sockfd;
}

int create_server_socket(int port) {
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char opt = 1;

    if (initialize_winsock() != 0) {
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("socket creation failed\n");
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("setsockopt failed\n");
        closesocket(sockfd);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons((unsigned short)port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("bind failed\n");
        closesocket(sockfd);
        return -1;
    }

    if (listen(sockfd, 5) == SOCKET_ERROR) {
        printf("listen failed\n");
        closesocket(sockfd);
        return -1;
    }

    return (int)sockfd;
}

int accept_client(int server_fd) {
    SOCKET client_fd;

    client_fd = accept((SOCKET)server_fd, NULL, NULL);
    if (client_fd == INVALID_SOCKET) {
        printf("accept failed\n");
        return -1;
    }

    return (int)client_fd;
}

int send_message(int sockfd, const char *message) {
    size_t len;
    int sent;

    len = strlen(message);
    sent = send((SOCKET)sockfd, message, (int)len, 0);

    if (sent == SOCKET_ERROR) {
        printf("send failed\n");
        return -1;
    }

    if (sent != (int)len) {
        printf("partial send occurred\n");
        return -1;
    }

    return 0;
}

int receive_message(int sockfd, char *buffer, size_t buffer_size) {
    int received;

    received = recv((SOCKET)sockfd, buffer, (int)(buffer_size - 1), 0);

    if (received == SOCKET_ERROR) {
        printf("recv failed\n");
        return -1;
    }

    if (received == 0) {
        return 0;
    }

    buffer[received] = '\0';
    return received;
}

void close_socket(int sockfd) {
    if (sockfd >= 0) {
        closesocket((SOCKET)sockfd);
    }
}

void cleanup_winsock(void) {
    if (winsock_initialized) {
        WSACleanup();
        winsock_initialized = 0;
    }
}