// Copyright 2024 MACKI

#include "usb_cdc_interface.h"


static usb_cdc_driver_t usb_cdc_driver = {
    .tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    },
    .acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = NULL,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    },
};

static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];


void usb_cdc_interface_init() {
    usb_cdc_init(&usb_cdc_driver);
}

void usb_cdc_register_rx_callback(tinyusb_cdcacm_rx_callback_t callback) {
    usb_cdc_driver.acm_cfg.callback_rx = callback;
}

void on_log_received(uint8_t *data, size_t length) {
    usb_cdc_send(&usb_cdc_driver, data, length);
}

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{   
    (void) itf;
    bool ret == true;
    /* initialization */
    size_t rx_size = 0;

    /* read */
    ret = usb_cdc_read(&usb_cdc_driver, buf, );
    if (ret == true) {
        ESP_LOGI(TAG, "Data from channel %d:", itf);
        ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
    } else {
        ESP_LOGE(TAG, "Read error");
        return;
    }

    /* write back */
    ret = usb_cdc_send(&usb_cdc_driver, buf, rx_size);
}