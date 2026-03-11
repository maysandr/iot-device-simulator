#include "common.h"
#include "config.h"
#include "sensor.h"
#include "protocol.h"
#include "socket_utils.h"

#include <signal.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define sleep_seconds(x) Sleep((x) * 1000)
#else
#include <unistd.h>
#define sleep_seconds(x) sleep(x)
#endif

static volatile int running = 1;

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

int main(void) {

    DeviceConfig config;
    int sockfd;
    char message[MAX_MESSAGE_LEN];

    signal(SIGINT, handle_signal);

    if (load_device_config("config/device.conf", &config) != 0) {
        fprintf(stderr, "Failed to load configuration\n");
        return 1;
    }

    printf("Loaded configuration:\n");
    printf("  device_id: %s\n", config.device_id);
    printf("  server_ip: %s\n", config.server_ip);
    printf("  server_port: %d\n", config.server_port);
    printf("  interval: %d seconds\n\n", config.interval_sec);

    init_sensor_state();

    sockfd = create_client_socket(config.server_ip, config.server_port);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        cleanup_winsock();
        return 1;
    }

    printf("Device connected to server\n\n");

    while (running) {

        TelemetryData data;

        generate_telemetry(config.device_id, &data);

        if (build_telemetry_message(&data, message, sizeof(message)) < 0) {
            fprintf(stderr, "Failed to build telemetry message\n");
            break;
        }

        if (send_message(sockfd, message) != 0) {
            fprintf(stderr, "Failed to send telemetry\n");
            break;
        }

        printf("Sent: %s\n", message);

        sleep_seconds(config.interval_sec);
    }

    close_socket(sockfd);
    cleanup_winsock();

    printf("Device stopped\n");

    return 0;
}