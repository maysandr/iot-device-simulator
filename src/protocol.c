#include "protocol.h"

int build_telemetry_message(const TelemetryData *data, char *buffer, size_t buffer_size) {
    if (!data || !buffer || buffer_size == 0) return -1;
    int written = snprintf(buffer, buffer_size,
        "device_id=%s;timestamp=%ld;temp=%.2f;humidity=%.2f;battery=%d;status=%s",
        data->device_id, data->timestamp, data->temperature,
        data->humidity, data->battery, data->status);
    return written < 0 || (size_t)written >= buffer_size ? -1 : written;
}

int parse_telemetry_message(const char *message, TelemetryData *data) {
    if (!message || !data) return -1;
    memset(data, 0, sizeof(*data));
    int used = 0;
    int parsed = sscanf(message,
        "device_id=%31[^;];timestamp=%ld;temp=%f;humidity=%f;battery=%d;status=%15[^;\r\n]%n",
        data->device_id, &data->timestamp, &data->temperature,
        &data->humidity, &data->battery, data->status, &used);
    return parsed == 6 && used > 0 && message[used] == '\0' ? 0 : -1;
}

int validate_telemetry(const TelemetryData *data) {
    if (!data) return -1;
    if (strlen(data->device_id) == 0 || data->timestamp <= 0) return -1;
    if (!(data->temperature >= -40.0f && data->temperature <= 125.0f)) return -1;
    if (!(data->humidity >= 0.0f && data->humidity <= 100.0f)) return -1;
    if (data->battery < 0 || data->battery > 100) return -1;
    if (strcmp(data->status, "OK") && strcmp(data->status, "LOW_BATTERY") &&
        strcmp(data->status, "OVERHEAT")) return -1;
    return 0;
}
