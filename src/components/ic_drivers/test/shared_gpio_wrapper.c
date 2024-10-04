// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "shared_gpio_wrapper.h"

#include "gpio_wrapper.h"

void init_all_pins() {
  init_esp_gpio_output(GPIO_PIN_RESET_EXP_1);
  init_esp_gpio_output(GPIO_PIN_RESET_EXP_2);
}

bool init_esp_gpio_output(uint8_t pin_number) {
  bool ret = gpio_pin_config_output(pin_number, false, false);
  return ret;
}

bool esp_gpio_set_pin(uint8_t pin_number, bool value) {
  bool ret =
      gpio_pin_set_level(pin_number, value ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
  return ret;
}
