#include "protocol.h"

int build_telemetry_message(
    const TelemetryData *data,
    char *buffer,
    size_t buffer_size
) {
    int written = snprintf(
        buffer,
        buffer_size,
        "device_id=%s;timestamp=%ld;temp=%.2f;humidity=%.2f;battery=%d;status=%s",
        data->device_id,
        data->timestamp,
        data->temperature,
        data->humidity,
        data->battery,
        data->status
    );

    if (written < 0 || (size_t)written >= buffer_size) {
        return -1;
    }

    return written;
}

int parse_telemetry_message(
    const char *message,
    TelemetryData *data
) {
    int parsed = sscanf(
        message,
        "device_id=%31[^;];timestamp=%ld;temp=%f;humidity=%f;battery=%d;status=%15s",
        data->device_id,
        &data->timestamp,
        &data->temperature,
        &data->humidity,
        &data->battery,
        data->status
    );

    return (parsed == 6) ? 0 : -1;
}

int validate_telemetry(const TelemetryData *data) {
    if (strlen(data->device_id) == 0) return -1;
    if (data->timestamp <= 0) return -1;
    if (data->temperature < -40.0f || data->temperature > 125.0f) return -1;
    if (data->humidity < 0.0f || data->humidity > 100.0f) return -1;
    if (data->battery < 0 || data->battery > 100) return -1;

    if (
        strcmp(data->status, "OK") != 0 &&
        strcmp(data->status, "LOW_BATTERY") != 0 &&
        strcmp(data->status, "OVERHEAT") != 0
    ) {
        return -1;
    }

    return 0;
}