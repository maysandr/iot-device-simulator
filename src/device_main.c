#include "common.h"
#include "config.h"
#include "sensor.h"
#include "protocol.h"
#include "socket_utils.h"

#include <signal.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#define sleep_seconds(x) Sleep((DWORD)(x) * 1000u)
#else
#include <unistd.h>
#define sleep_seconds(x) sleep((unsigned)(x))
#endif

static volatile sig_atomic_t running = 1;
static void handle_signal(int sig) { (void)sig; running = 0; }

int main(void) {
    signal(SIGINT, handle_signal);
    DeviceConfig config;
    char message[MAX_MESSAGE_LEN];
    if (load_device_config(DEFAULT_CONFIG_PATH, &config) != 0) {
        fprintf(stderr, "Invalid or missing configuration: %s\n", DEFAULT_CONFIG_PATH);
        return 1;
    }
    printf("Device %s connecting to %s:%d every %d seconds\n",
           config.device_id, config.server_ip, config.server_port, config.interval_sec);
    init_sensor_state();
    socket_handle_t sockfd = create_client_socket(config.server_ip, config.server_port);
    if (sockfd == INVALID_SOCKET_HANDLE) {
        fprintf(stderr, "Failed to connect to server\n");
        cleanup_sockets(); return 1;
    }
    puts("Device connected");
    while (running) {
        TelemetryData data;
        generate_telemetry(config.device_id, &data);
        if (build_telemetry_message(&data, message, sizeof(message)) < 0 ||
            send_message(sockfd, message) != 0) {
            fputs("Failed to prepare or transmit telemetry\n", stderr);
            break;
        }
        printf("Sent: %s\n", message);
        sleep_seconds(config.interval_sec);
    }
    close_socket(sockfd);
    cleanup_sockets();
    puts("Device stopped");
    return 0;
}
