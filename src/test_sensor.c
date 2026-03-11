#include "common.h"
#include "sensor.h"

int main(void) {
    TelemetryData data;

    init_sensor_state();

    for (int i = 0; i < 5; i++) {
        generate_telemetry("BAT_001", &data);

        printf("Telemetry %d:\n", i + 1);
        printf("  device_id: %s\n", data.device_id);
        printf("  timestamp: %ld\n", data.timestamp);
        printf("  temperature: %.2f\n", data.temperature);
        printf("  humidity: %.2f\n", data.humidity);
        printf("  battery: %d\n", data.battery);
        printf("  status: %s\n\n", data.status);
    }

    return 0;
}