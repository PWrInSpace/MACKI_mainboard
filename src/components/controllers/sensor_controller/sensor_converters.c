// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "sensor_converters.h"

#define PRESSURE_SENSOR_RANGE_MAX_BAR 10.0f
#define PRESSURE_SENSOR_VOLTAGE_MAX 24.f * 24.f / (56.f + 24.f)

#define LOAD_CELL_MAX_VOLTAGE 3.f // 10.f * 24.f / (24.f + 56.f)
#define LOAD_CELL_MAX_FORCE 200.0f

float pressure_sensor_volt_to_bar(float voltage) {
  return voltage * PRESSURE_SENSOR_RANGE_MAX_BAR / PRESSURE_SENSOR_VOLTAGE_MAX;
}

float load_cell_millivolt_to_newtons(int16_t milivolt) {
  float millivolt_f = (float)milivolt;
  return (float)LOAD_CELL_MAX_FORCE * (millivolt_f / 1000.f) / LOAD_CELL_MAX_VOLTAGE;
}
