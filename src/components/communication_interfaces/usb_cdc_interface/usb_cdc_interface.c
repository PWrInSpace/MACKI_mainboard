// Copyright 2024 MACKI

#include "usb_cdc_interface.h"
#include "macki_log.h"
// TODO(Gliwus): To be removed:
#define TAG "USB_CDC_INTERFACE"

static usb_cdc_driver_t usb_cdc_driver = {
    .tusb_cfg =
        {
            .device_descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false,
            .configuration_descriptor = NULL,
        },
    .acm_cfg = {.usb_dev = TINYUSB_USBDEV_0,
                .cdc_port = TINYUSB_CDC_ACM_0,
                .rx_unread_buf_sz = 64,
                .callback_rx = &tinyusb_cdc_rx_callback,
                .callback_rx_wanted_char = NULL,
                .callback_line_state_changed = NULL,
                .callback_line_coding_changed = NULL},
};

static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

void usb_cdc_interface_init() { usb_cdc_init(&usb_cdc_driver); }

void usb_cdc_register_rx_callback(tusb_cdcacm_callback_t callback) {
  tinyusb_cdcacm_register_callback(usb_cdc_driver.acm_cfg.cdc_port,
                                   CDC_EVENT_LINE_STATE_CHANGED, callback);
}

void usb_cdc_interface_send_data(uint8_t *data, size_t length) {
  usb_cdc_send(&usb_cdc_driver, data, length);
}

void usb_cdc_on_log_received(char *data, size_t length) {
  usb_cdc_interface_send_data((uint8_t *)data, length);
}

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event) {
  (void)itf;
  bool ret = true;
  size_t rx_size = 0;

  /* read */
  ret = usb_cdc_receive(&usb_cdc_driver, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE,
                        &rx_size);
  if (ret == true) {
    MACKI_LOG_DEBUG(TAG, "Data from channel %d:", itf);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
  } else {
    MACKI_LOG_ERROR(TAG, "Read error");
    return;
  }

  usb_cdc_interface_send_data(buf, rx_size);
}

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event) {
  (void)itf;
  int dtr = event->line_state_changed_data.dtr;
  int rts = event->line_state_changed_data.rts;
  MACKI_LOG_TRACE(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr,
           rts);
}
