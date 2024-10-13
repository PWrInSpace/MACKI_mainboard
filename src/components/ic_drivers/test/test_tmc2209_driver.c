// Copyright 2024 MACKI, Krzysztof Gliwinski
/// @note This is unused in the project

/*!

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "shared_gpio_wrapper.h"
#include "shared_uart_wrapper.h"
#include "tmc2209_driver.h"
#include "unity.h"

#define TEST_ASSERT_TMC2209_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(TMC2209_DRIVER_OK, condition)

static void delay_ms(size_t delay_ms) { vTaskDelay(pdMS_TO_TICKS(delay_ms)); }

// TODO(Glibus): change this shit (both of these) when gpio_interface refactored
static void init_gpio_pins() {
  gpio_config_t io_conf;
  io_conf.pin_bit_mask = (uint64_t)(1ULL << 45);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_up_en = false;
  io_conf.pull_down_en = false;
  io_conf.intr_type = GPIO_INTR_DISABLE;

  esp_err_t ret = gpio_config(&io_conf);
}

static bool gpio_set_pin(uint8_t pin_number, bool level) {
  esp_err_t ret = gpio_set_level((gpio_num_t)pin_number, (uint32_t)level);
  return ret == ESP_OK ? true : false;
}

static tmc2209_driver_t tmc2209_driver = {
    .address = 2,
    .en_pin = 45,
    .initialized = false,
    ._send_data = &uart_ic_send_data,
    ._receive_data = &uart_ic_receive_data,
    ._enable_pin_set = &gpio_set_pin,
    ._delay = &delay_ms,
    .uart_read_delay_ms = 100,
};

TEST_CASE("TMC2209 driver initialization", "[TMC2209_DRIVER]") {
  init_uart_comm_driver();
  init_gpio_pins();
  TEST_ASSERT_TMC2209_DRIVER_OK(tmc2209_driver_enable(&tmc2209_driver));
  delay_ms(2000);
  TEST_ASSERT_TMC2209_DRIVER_OK(tmc2209_driver_init(&tmc2209_driver));
}

TEST_CASE("TMC2209 driver deinitialization", "[TMC2209_DRIVER]") {
  //   TEST_ASSERT_TMC2209_DRIVER_OK(tmc2209_driver_deinit(&tmc2209_driver));
  TEST_ASSERT_TMC2209_DRIVER_OK(tmc2209_driver_disable(&tmc2209_driver));
  deinit_uart_comm_driver();
}

*/
