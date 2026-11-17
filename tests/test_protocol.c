#include "protocol.h"
#include "config.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    TelemetryData original = {"BAT_001", 1700000000L, 26.5f, 62.0f, 90, "OK"};
    TelemetryData parsed = {0};
    char buf[MAX_MESSAGE_LEN];
    assert(build_telemetry_message(&original, buf, sizeof(buf)) > 0);
    assert(parse_telemetry_message(buf, &parsed) == 0);
    assert(validate_telemetry(&parsed) == 0);
    assert(parsed.battery == 90 && parsed.timestamp == original.timestamp);
    assert(parse_telemetry_message("device_id=A;timestamp=1;temp=2;humidity=3;battery=4;status=OK;TRAILING", &parsed) == -1);
    assert(parse_telemetry_message("not telemetry", &parsed) == -1);
    assert(validate_telemetry(&parsed) == -1);
    DeviceConfig config;
    assert(load_device_config("config/device.conf", &config) == 0);
    assert(config.server_port == 8080 && config.interval_sec == 3);
    puts("PASS: telemetry round-trip, validation, malformed input, configuration");
    return 0;
}
