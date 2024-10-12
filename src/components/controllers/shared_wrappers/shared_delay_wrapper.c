// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "shared_delay_wrapper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void delay_ms(size_t delay_ms) { vTaskDelay(pdMS_TO_TICKS(delay_ms)); }
