#include "config.h"

#include <stdio.h>
#include <string.h>

static void trim_newline(char *str) {
    size_t len = strlen(str);

    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
    }
}

int load_device_config(const char *filename, DeviceConfig *config) {

    FILE *file;
    char line[MAX_LINE_LEN];

    file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open config file: %s\n", filename);
        return -1;
    }

    while (fgets(line, sizeof(line), file)) {

        trim_newline(line);

        if (strncmp(line, "device_id=", 10) == 0) {
            strncpy(config->device_id, line + 10, sizeof(config->device_id) - 1);
        }
        else if (strncmp(line, "server_ip=", 10) == 0) {
            strncpy(config->server_ip, line + 10, sizeof(config->server_ip) - 1);
        }
        else if (strncmp(line, "server_port=", 12) == 0) {
            config->server_port = atoi(line + 12);
        }
        else if (strncmp(line, "interval_sec=", 13) == 0) {
            config->interval_sec = atoi(line + 13);
        }
    }

    fclose(file);
    return 0;
}