#include "sensor.h"

static float current_temp = 25.0f;
static float current_humidity = 60.0f;
static int battery_level = 100;

void init_sensor_state(void) {
    srand((unsigned int)time(NULL));
}

float generate_temperature(void) {
    current_temp += ((rand() % 5) - 2) * 0.1f;

    if (current_temp < 15.0f) current_temp = 15.0f;
    if (current_temp > 45.0f) current_temp = 45.0f;

    return current_temp;
}

float generate_humidity(void) {
    current_humidity += ((rand() % 5) - 2) * 0.2f;

    if (current_humidity < 20.0f) current_humidity = 20.0f;
    if (current_humidity > 90.0f) current_humidity = 90.0f;

    return current_humidity;
}

int generate_battery(void) {
    if (battery_level > 0) {
        battery_level -= rand() % 2;
    }

    if (battery_level < 0) {
        battery_level = 0;
    }

    return battery_level;
}

const char *generate_status(float temp, int battery) {
    if (battery < 20) {
        return "LOW_BATTERY";
    }

    if (temp > 40.0f) {
        return "OVERHEAT";
    }

    return "OK";
}

void generate_telemetry(const char *device_id, TelemetryData *data) {
    strncpy(data->device_id, device_id, sizeof(data->device_id) - 1);
    data->device_id[sizeof(data->device_id) - 1] = '\0';

    data->timestamp = time(NULL);
    data->temperature = generate_temperature();
    data->humidity = generate_humidity();
    data->battery = generate_battery();

    strncpy(
        data->status,
        generate_status(data->temperature, data->battery),
        sizeof(data->status) - 1
    );
    data->status[sizeof(data->status) - 1] = '\0';
}