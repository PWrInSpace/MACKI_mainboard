// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "adc_wrapper.h"

void sensor_task(void* pvParameters);

void sensor_save_task(void* pvParameters);

void macus_task(void* pvParameters);