#include "common.h"
#include "sensor.h"
#include "protocol.h"

int main(void) {
    TelemetryData original;
    TelemetryData parsed;
    char buffer[MAX_MESSAGE_LEN];

    init_sensor_state();
    generate_telemetry("BAT_001", &original);

    if (build_telemetry_message(&original, buffer, sizeof(buffer)) < 0) {
        printf("Failed to build telemetry message\n");
        return 1;
    }

    printf("Built message:\n%s\n\n", buffer);

    if (parse_telemetry_message(buffer, &parsed) != 0) {
        printf("Failed to parse telemetry message\n");
        return 1;
    }

    if (validate_telemetry(&parsed) != 0) {
        printf("Parsed telemetry is invalid\n");
        return 1;
    }

    printf("Parsed telemetry:\n");
    printf("  device_id: %s\n", parsed.device_id);
    printf("  timestamp: %ld\n", parsed.timestamp);
    printf("  temperature: %.2f\n", parsed.temperature);
    printf("  humidity: %.2f\n", parsed.humidity);
    printf("  battery: %d\n", parsed.battery);
    printf("  status: %s\n", parsed.status);

    return 0;
}