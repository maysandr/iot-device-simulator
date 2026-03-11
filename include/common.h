#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGE_LEN 256
#define MAX_LINE_LEN 256

#define DEFAULT_CONFIG_PATH "config/device.conf"
#define DEFAULT_LOG_PATH "logs/server.log"

typedef struct {
    char device_id[32];
    char server_ip[64];
    int server_port;
    int interval_sec;
} DeviceConfig;

typedef struct {
    char device_id[32];
    long timestamp;
    float temperature;
    float humidity;
    int battery;
    char status[16];
} TelemetryData;

#endif