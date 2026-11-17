#include "socket_utils.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <ws2tcpip.h>
static int sockets_initialized = 0;
static int initialize_sockets(void) {
    if (sockets_initialized) return 0;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;
    sockets_initialized = 1;
    return 0;
}
#define socket_close closesocket
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
static int initialize_sockets(void) { return 0; }
#define socket_close close
#endif

#ifdef _WIN32
typedef int address_len_t;
#else
typedef socklen_t address_len_t;
#endif

static int socket_failed(socket_handle_t sock) {
    return sock == INVALID_SOCKET_HANDLE;
}

socket_handle_t create_client_socket(const char *server_ip, int port) {
    if (server_ip == NULL || port < 1 || port > 65535 || initialize_sockets() != 0)
        return INVALID_SOCKET_HANDLE;
    socket_handle_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_failed(sock)) return INVALID_SOCKET_HANDLE;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) != 1 ||
        connect(sock, (struct sockaddr *)&addr, (address_len_t)sizeof(addr)) != 0) {
        socket_close(sock);
        return INVALID_SOCKET_HANDLE;
    }
    return sock;
}

socket_handle_t create_server_socket(int port) {
    if (port < 1 || port > 65535 || initialize_sockets() != 0)
        return INVALID_SOCKET_HANDLE;
    socket_handle_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_failed(sock)) return INVALID_SOCKET_HANDLE;

    int reuse = 1;
#ifdef _WIN32
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) != 0)
#else
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
#endif
    { socket_close(sock); return INVALID_SOCKET_HANDLE; }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)port);
    if (bind(sock, (struct sockaddr *)&addr, (address_len_t)sizeof(addr)) != 0 ||
        listen(sock, 5) != 0) {
        socket_close(sock);
        return INVALID_SOCKET_HANDLE;
    }
    return sock;
}

socket_handle_t accept_client(socket_handle_t server_fd) {
    return accept(server_fd, NULL, NULL);
}

static int send_all(socket_handle_t sock, const char *bytes, size_t length) {
    size_t sent_total = 0;
    while (sent_total < length) {
        int sent = send(sock, bytes + sent_total, (int)(length - sent_total), 0);
        if (sent <= 0) return -1;
        sent_total += (size_t)sent;
    }
    return 0;
}

int send_message(socket_handle_t sock, const char *message) {
    if (message == NULL || strchr(message, '\n') != NULL) return -1;
    size_t len = strlen(message);
    if (send_all(sock, message, len) != 0) return -1;
    return send_all(sock, "\n", 1);
}

int receive_message(socket_handle_t sock, char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size < 2) return -1;
    size_t used = 0;
    int overlong = 0;
    for (;;) {
        char c;
        int got = recv(sock, &c, 1, 0);
        if (got < 0) return -1;
        if (got == 0) return used == 0 && !overlong ? 0 : -1;
        if (c == '\n') {
            if (overlong || used == 0) return -1;
            if (buffer[used - 1] == '\r') --used;
            if (used == 0) return -1;
            buffer[used] = '\0';
            return (int)used;
        }
        if (used < buffer_size - 1 && !overlong) buffer[used++] = c;
        else overlong = 1; /* Consume to newline to preserve stream alignment. */
    }
}

void close_socket(socket_handle_t sock) {
    if (!socket_failed(sock)) socket_close(sock);
}

void cleanup_sockets(void) {
#ifdef _WIN32
    if (sockets_initialized) { WSACleanup(); sockets_initialized = 0; }
#endif
}
