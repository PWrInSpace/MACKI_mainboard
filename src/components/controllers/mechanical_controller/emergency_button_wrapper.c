// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "emergency_button_wrapper.h"

#include "macki_log.h"

#define TAG "EMERGENCY_BUTTON_WRAPPER"

bool emergency_button_init(emergency_button_t* button) {
  if (button->initialized) {
    MACKI_LOG_WARN(TAG, "Emergency button already initialized");
    return false;
  }

  if (!gpio_pin_config_input(button->gpio_pin_num, true, false, NULL,
                             GPIO_INTR_DISABLE)) {
    MACKI_LOG_ERROR(TAG, "Failed to configure GPIO pin %d",
                    button->gpio_pin_num);
    return false;
  }
  button->initialized = true;
  return true;
}

bool emergency_button_is_pressed(emergency_button_t* button) {
  if (!button->initialized) {
    MACKI_LOG_ERROR(TAG, "Emergency button not initialized");
    return false;
  }

  return gpio_pin_get_level(button->gpio_pin_num) == GPIO_LEVEL_LOW;
}

bool emergency_button_deinit(emergency_button_t* button) {
  if (!button->initialized) {
    MACKI_LOG_WARN(TAG, "Emergency button already deinitialized");
    return false;
  }

  if (!gpio_pin_config_input(button->gpio_pin_num, false, false, NULL,
                             GPIO_INTR_DISABLE)) {
    MACKI_LOG_ERROR(TAG, "Failed to deconfigure GPIO pin %d",
                    button->gpio_pin_num);
    return false;
  }

  button->initialized = false;
  return true;
}
