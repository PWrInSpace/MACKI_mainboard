// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "adc_driver.h"

// TODO(Glibus): change this to single-shot

typedef bool (*_adc_wrapper_conversion_done_cb)(
    adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata,
    void *user_data);

bool adc_wrapper_init();

bool adc_wrapper_register_callback(_adc_wrapper_conversion_done_cb cbs);

bool adc_wrapper_start();

bool adc_wrapper_get_readings(uint32_t *readings, size_t readings_num,
                              uint32_t *readings_done);

adc_driver_t *adc_wrapper_get_instance();

bool adc_wrapper_deinit();
