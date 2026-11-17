#include "config.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static int parse_int(const char *text, int min, int max, int *out) {
    errno = 0;
    char *end = NULL;
    long v = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || v < min || v > max) return -1;
    *out = (int)v;
    return 0;
}

int load_device_config(const char *filename, DeviceConfig *config) {
    if (filename == NULL || config == NULL) return -1;
    FILE *file = fopen(filename, "r");
    if (file == NULL) return -1;

    memset(config, 0, sizeof(*config));
    char line[MAX_LINE_LEN];
    unsigned flags = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        size_t length = strlen(line);
        if (length > 0 && line[length - 1] != '\n' && !feof(file)) {
            fclose(file); return -1; /* line too long */
        }
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '#' || line[0] == '\0') continue;
        char *equals = strchr(line, '=');
        if (equals == NULL) { fclose(file); return -1; }
        *equals++ = '\0';
        if (strcmp(line, "device_id") == 0) {
            if (strlen(equals) == 0 || strlen(equals) >= sizeof(config->device_id)) {
                fclose(file); return -1;
            }
            strcpy(config->device_id, equals); flags |= 1u;
        } else if (strcmp(line, "server_ip") == 0) {
            if (strlen(equals) == 0 || strlen(equals) >= sizeof(config->server_ip)) {
                fclose(file); return -1;
            }
            strcpy(config->server_ip, equals); flags |= 2u;
        } else if (strcmp(line, "server_port") == 0) {
            if (parse_int(equals, 1, 65535, &config->server_port) != 0) {
                fclose(file); return -1;
            }
            flags |= 4u;
        } else if (strcmp(line, "interval_sec") == 0) {
            if (parse_int(equals, 1, 3600, &config->interval_sec) != 0) {
                fclose(file); return -1;
            }
            flags |= 8u;
        } else { fclose(file); return -1; }
    }
    fclose(file);
    return flags == 15u ? 0 : -1;
}
