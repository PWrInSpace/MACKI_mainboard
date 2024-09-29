// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "adc_wrapper.h"
#include "data_queues.h"


void adc_sensor_task(void* pvParameters);

void i2c_sensors_task(void* pvParameters);
