// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include "freertos/FreeRTOS.h"

void motor_task(void *pvParameters);

void limit_switch_task(void *pvParameters);