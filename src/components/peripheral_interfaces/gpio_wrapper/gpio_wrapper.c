// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "gpio_wrapper.h"

#include "macki_log.h"

#define TAG "GPIO_WRAPPER"

bool gpio_pin_config_output(pin_config_t* gpio, bool pull_up_en,
                            bool pull_down_en) {
  if (pull_up_en && pull_down_en) {
    MACKI_LOG_ERROR(TAG,
                    "Pull up and pull down cannot be enabled at the same time");
    return false;
  }

  gpio_config_t io_conf = {.pin_bit_mask = (uint64_t)(1ULL << gpio->pin_number),
                           .mode = GPIO_MODE_OUTPUT,
                           .pull_up_en = pull_up_en,
                           .pull_down_en = pull_down_en,
                           .intr_type = GPIO_INTR_DISABLE};

  esp_err_t ret = gpio_config(&io_conf);
  if (ret != ESP_OK) {
    MACKI_LOG_ERROR(TAG, "Error during GPIO configuration");
    return false;
  }

  gpio->mode = GPIO_OUTPUT;
  return true;
}

bool gpio_pin_set_level(pin_config_t* gpio, gpio_level_t level) {
  if (level != GPIO_LEVEL_LOW && level != GPIO_LEVEL_HIGH) {
    MACKI_LOG_ERROR(TAG, "Invalid GPIO level");
    return false;
  }

  if (gpio->mode != GPIO_OUTPUT) {
    MACKI_LOG_ERROR(TAG, "GPIO is not configured as output");
    return false;
  }

  esp_err_t ret = gpio_set_level(gpio->pin_number, level);
  if (ret != ESP_OK) {
    MACKI_LOG_ERROR(TAG, "Error during setting GPIO level");
    return false;
  }
  return true;
}

gpio_level_t gpio_pin_get_level(pin_config_t* gpio) {
  return gpio_get_level(gpio->pin_number) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
}

bool gpio_pin_config_input(pin_config_t* gpio, bool pull_up_en,
                           bool pull_down_en, gpio_isr_t isr_handler) {
  if (pull_up_en && pull_down_en) {
    MACKI_LOG_ERROR(TAG,
                    "Pull up and pull down cannot be enabled at the same time");
    return false;
  }

  gpio_config_t io_conf = {.pin_bit_mask = 1ULL << gpio->pin_number,
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = pull_up_en,
                           .pull_down_en = pull_down_en,
                           .intr_type = GPIO_INTR_DISABLE};

  esp_err_t ret = gpio_config(&io_conf);
  if (ret != ESP_OK) {
    MACKI_LOG_ERROR(TAG, "Error during GPIO configuration");
    return false;
  }

  gpio->mode = GPIO_INPUT;

  if (isr_handler != NULL) {
    ret = gpio_install_isr_service(0);
    if (ret != ESP_OK) {
      MACKI_LOG_ERROR(TAG, "Error during ISR service installation");
      return false;
    }

    ret = gpio_isr_handler_add(gpio->pin_number, isr_handler, NULL);
    if (ret != ESP_OK) {
      MACKI_LOG_ERROR(TAG, "Error during ISR handler addition");
      return false;
    }
  }

  return true;
}
