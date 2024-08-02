// Copyright 2024 MACKI

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "tinyusb.h"
#include "tusb_cdc_acm.h"

<<<<<<< HEAD
=======
#define USB_CDC_TRANSFER_TIMEOUT 0

>>>>>>> main
typedef struct {
  tinyusb_config_t tusb_cfg;
  tinyusb_config_cdcacm_t acm_cfg;

  tinyusb_cdcacm_itf_t cdc_port;  // DO NOT SET THIS VALUE
} usb_cdc_driver_t;

/*!
 * @brief Initialize USB CDC driver
 *
 * @param usb_cdc_driver USB CDC driver configuration
 */
void usb_cdc_init(usb_cdc_driver_t *usb_cdc_driver);

/*!
 * @brief Send data over USB CDC
 *
 * @param data Data to send
 * @param length Data length
 */
bool usb_cdc_send(usb_cdc_driver_t *usb_cdc_driver, uint8_t *data,
                  size_t length);

/*!
 * @brief Receive data over USB CDC
 *
 * @param data Data buffer
 * @param length Data length
 */
bool usb_cdc_receive(usb_cdc_driver_t *usb_cdc_driver, uint8_t *data,
                     size_t length, size_t *rx_size);
