#include "common.h"
#include "protocol.h"
#include "socket_utils.h"
#include "logger.h"

#include <signal.h>
#include <stdio.h>

static volatile int running = 1;

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

int main(void) {

    int server_fd;
    int client_fd;
    char buffer[MAX_MESSAGE_LEN];

    signal(SIGINT, handle_signal);

    if (logger_init(DEFAULT_LOG_PATH) != 0) {
        return 1;
    }

    server_fd = create_server_socket(8080);
    if (server_fd < 0) {
        logger_close();
        cleanup_winsock();
        return 1;
    }

    printf("Server listening on port 8080...\n");

    client_fd = accept_client(server_fd);
    if (client_fd < 0) {
        close_socket(server_fd);
        logger_close();
        cleanup_winsock();
        return 1;
    }

    printf("Client connected\n\n");

    while (running) {

        int bytes = receive_message(client_fd, buffer, sizeof(buffer));
        TelemetryData data;

        if (bytes < 0) {
            printf("Receive error\n");
            break;
        }

        if (bytes == 0) {
            printf("Client disconnected\n");
            break;
        }

        if (parse_telemetry_message(buffer, &data) != 0) {
            printf("Invalid message: %s\n", buffer);
            continue;
        }

        if (validate_telemetry(&data) != 0) {
            printf("Invalid telemetry values\n");
            continue;
        }

        printf("Telemetry received\n");
        printf("  device_id: %s\n", data.device_id);
        printf("  temperature: %.2f\n", data.temperature);
        printf("  humidity: %.2f\n", data.humidity);
        printf("  battery: %d\n", data.battery);
        printf("  status: %s\n\n", data.status);

        logger_log_telemetry(&data);
    }

    close_socket(client_fd);
    close_socket(server_fd);

    logger_close();
    cleanup_winsock();

    printf("Server stopped\n");

    return 0;
}