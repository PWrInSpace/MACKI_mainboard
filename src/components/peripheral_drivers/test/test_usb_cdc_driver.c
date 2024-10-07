// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sdkconfig.h"
#include "unity.h"
#include "usb_cdc_driver.h"

static void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);

#define BUFFER_LENGTH 255
static uint8_t buf[BUFFER_LENGTH + 1];

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

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event) {
  (void)itf;
  size_t rx_size = 0;

  /* read */
  usb_cdc_receive(&usb_cdc_driver, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE,
                  &rx_size);

  usb_cdc_send(&usb_cdc_driver, buf, rx_size);
}

TEST_CASE("usb_cdc_driver_init Test", "[USB_CDC_DRIVER]") {
  bool ret = usb_cdc_init(&usb_cdc_driver);
  TEST_ASSERT_EQUAL(true, ret);
}

TEST_CASE("usb_cdc_send Test", "[USB_CDC_DRIVER]") {
  const char *data = "TEST_STRING";
  size_t data_size = strlen(data);

  bool ret = usb_cdc_send(&usb_cdc_driver, (uint8_t *)data, data_size);
  TEST_ASSERT_EQUAL(true, ret);
}

TEST_CASE("usb_cdc_receive Test", "[USB_CDC_DRIVER]") {
  // TODO(Glibus): jak to kurwa zaimplementowac xd
  TEST_ASSERT_TRUE(1 == 1);
}
