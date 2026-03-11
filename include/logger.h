#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"

int logger_init(const char *log_path);
void logger_log_telemetry(const TelemetryData *data);
void logger_close(void);

#endif