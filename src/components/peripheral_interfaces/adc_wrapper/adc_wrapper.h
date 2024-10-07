// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "adc_driver.h"

typedef enum { ADC_STRAIN_GAUGE = 0, ADC_MAX_NUM = 1 } adc_wrapper_instance_t;

bool adc_wrapper_init();

bool adc_wrapper_get_reading(adc_wrapper_instance_t instance, int16_t *reading);

adc_driver_t *adc_wrapper_get_instance(adc_wrapper_instance_t instance);

bool adc_wrapper_deinit();
