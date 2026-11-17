#include "common.h"
#include "protocol.h"
#include "socket_utils.h"
#include "logger.h"

#include <signal.h>
#include <stdio.h>

static volatile sig_atomic_t running = 1;
static void handle_signal(int sig) { (void)sig; running = 0; }

int main(int argc, char **argv) {
    signal(SIGINT, handle_signal);
    int port = 8080;
    if (argc == 2) {
        char *end = NULL;
        long requested = strtol(argv[1], &end, 10);
        if (*argv[1] == '\0' || *end != '\0' || requested < 1 || requested > 65535) {
            fprintf(stderr, "Invalid listen port\n"); return 1;
        }
        port = (int)requested;
    } else if (argc > 2) {
        fprintf(stderr, "Usage: server [port]\n"); return 1;
    }
    if (logger_init(DEFAULT_LOG_PATH) != 0) return 1;

    socket_handle_t server_fd = create_server_socket(port);
    if (server_fd == INVALID_SOCKET_HANDLE) {
        fprintf(stderr, "Unable to listen on TCP port %d\n", port);
        logger_close(); cleanup_sockets(); return 1;
    }
    printf("Server listening on port %d...\n", port);

    socket_handle_t client_fd = accept_client(server_fd);
    if (client_fd == INVALID_SOCKET_HANDLE) {
        fprintf(stderr, "Unable to accept client\n");
        close_socket(server_fd); logger_close(); cleanup_sockets(); return 1;
    }
    puts("Client connected");

    char buffer[MAX_MESSAGE_LEN];
    while (running) {
        int bytes = receive_message(client_fd, buffer, sizeof(buffer));
        if (bytes < 0) { fputs("Invalid or incomplete TCP message\n", stderr); break; }
        if (bytes == 0) { puts("Client disconnected"); break; }
        TelemetryData data;
        if (parse_telemetry_message(buffer, &data) != 0 || validate_telemetry(&data) != 0) {
            fprintf(stderr, "Rejected telemetry message: %s\n", buffer);
            continue;
        }
        printf("Telemetry received: %s temp=%.2fC humidity=%.2f battery=%d%% status=%s\n",
               data.device_id, data.temperature, data.humidity, data.battery, data.status);
        logger_log_telemetry(&data);
    }
    close_socket(client_fd); close_socket(server_fd);
    logger_close(); cleanup_sockets();
    puts("Server stopped");
    return 0;
}
