// Copyright 2024 MACKI

#include "usb_cdc_driver.h"

bool usb_cdc_init(usb_cdc_driver_t *usb_cdc_driver) {
  esp_err_t ret = ESP_OK;

  ret |= tinyusb_driver_install(&usb_cdc_driver->tusb_cfg);
  ret |= tusb_cdc_acm_init(&usb_cdc_driver->acm_cfg);

  usb_cdc_driver->cdc_port = usb_cdc_driver->acm_cfg.cdc_port;
  return ret == ESP_OK;
}

bool usb_cdc_send(usb_cdc_driver_t *usb_cdc_driver, uint8_t *data,
                  size_t length) {
  esp_err_t ret = ESP_OK;
  tinyusb_cdcacm_write_queue(usb_cdc_driver->cdc_port, data, length);
  ret = tinyusb_cdcacm_write_flush(usb_cdc_driver->cdc_port,
                                   USB_CDC_TRANSFER_TIMEOUT);

  return ret != ESP_ERR_TIMEOUT;
}

bool usb_cdc_receive(usb_cdc_driver_t *usb_cdc_driver, uint8_t *data,
                     size_t length, size_t *rx_size) {
  esp_err_t ret = ESP_OK;
  ret |= tinyusb_cdcacm_read(usb_cdc_driver->cdc_port, data, length, rx_size);
  return ret == ESP_OK;
}
