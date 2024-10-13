// Copyright 2024 MACKI, Krzysztof Gliwinski
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

float pressure_sensor_volt_to_bar(float voltage);

float load_cell_millivolt_to_newtons(int16_t milivolt);