// Copyright 2024 MACKI

#pragma once

#include "usb_cdc_driver.h"
// TODO(Gliwus): To be removed:
#include "esp_log.h"

void usb_cdc_interface_init();

void usb_cdc_register_rx_callback(tinyusb_cdcacm_rx_callback_t callback);

void on_log_received(uint8_t *data, size_t length);

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
