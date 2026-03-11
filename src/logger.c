#include "logger.h"

#include <stdio.h>
#include <time.h>

static FILE *log_file = NULL;

int logger_init(const char *log_path) {

    log_file = fopen(log_path, "a");

    if (!log_file) {
        printf("Failed to open log file: %s\n", log_path);
        return -1;
    }

    printf("Logging to %s\n", log_path);
    return 0;
}

void logger_log_telemetry(const TelemetryData *data) {

    if (!log_file) {
        return;
    }

    fprintf(
        log_file,
        "%ld,%s,%.2f,%.2f,%d,%s\n",
        data->timestamp,
        data->device_id,
        data->temperature,
        data->humidity,
        data->battery,
        data->status
    );

    fflush(log_file);
}

void logger_close(void) {

    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}