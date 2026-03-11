#ifndef SENSOR_H
#define SENSOR_H

#include "common.h"

void init_sensor_state(void);
float generate_temperature(void);
float generate_humidity(void);
int generate_battery(void);
const char *generate_status(float temp, int battery);
void generate_telemetry(const char *device_id, TelemetryData *data);

#endif