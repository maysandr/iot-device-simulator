#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "common.h"

int build_telemetry_message(
    const TelemetryData *data,
    char *buffer,
    size_t buffer_size
);

int parse_telemetry_message(
    const char *message,
    TelemetryData *data
);

int validate_telemetry(const TelemetryData *data);

#endif