// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "gpio_expander_wrapper.h"

#include "gpio_wrapper.h"
#include "macki_log.h"
#include "pca9574_driver.h"
#include "shared_delay_wrapper.h"
#include "shared_i2c_wrapper.h"

#define TAG "GPIO_EXPANDER_WRAPPER"

static bool esp_gpio_set(uint8_t pin, bool value) {
  return gpio_pin_set_level(pin,
                            value == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

static pca957_driver_t expander_instance[GPIO_EXPANDER_MAX] = {
    [GPIO_EXPANDER_1] =
        {
            .address = 0x20,
            ._pin_set = &esp_gpio_set,
            .pin_config = {.pins.pin0 = PCA9574_INPUT,
                           .pins.pin1 = PCA9574_INPUT,
                           .pins.pin2 = PCA9574_INPUT,
                           .pins.pin3 = PCA9574_INPUT,
                           .pins.pin4 = PCA9574_OUTPUT,
                           .pins.pin5 = PCA9574_OUTPUT,
                           .pins.pin6 = PCA9574_OUTPUT,
                           .pins.pin7 = PCA9574_OUTPUT},
            .reset_pin = GPIO_PIN_RESET_EXP_1,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
            ._delay = &delay_ms,
            .initiated = false,
        },
    [GPIO_EXPANDER_2] =
        {
            .address = 0x21,
            ._pin_set = &esp_gpio_set,
            .pin_config = {.pins.pin0 = PCA9574_OUTPUT,
                           .pins.pin1 = PCA9574_OUTPUT,
                           .pins.pin2 = PCA9574_OUTPUT,
                           .pins.pin3 = PCA9574_OUTPUT,
                           .pins.pin4 = PCA9574_OUTPUT,
                           .pins.pin5 = PCA9574_OUTPUT,
                           .pins.pin6 = PCA9574_INPUT,
                           .pins.pin7 = PCA9574_INPUT},
            .reset_pin = GPIO_PIN_RESET_EXP_2,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
            ._delay = &delay_ms,
            .initiated = false,
        },
};

bool init_gpio_expanders() {
  pca957_driver_status_t expander_ret;
  for (size_t i = 0; i < GPIO_EXPANDER_MAX; i++) {
    expander_ret = pca957_driver_init(&expander_instance[i]);
    if (expander_ret != PCA957_DRIVER_OK) {
      MACKI_LOG_ERROR(TAG, "Failed to initialize PCA9574 driver %d", i);
      return false;
    }
    expander_ret = pca957_driver_pull_enable(&expander_instance[i]);
    if (expander_ret != PCA957_DRIVER_OK) {
      MACKI_LOG_ERROR(TAG, "Failed to enable pull up-down on PCA9574 driver %d",
                      i);
      return false;
    }
  }

  // Set pull downs on the input pins (this code is so shit)
  expander_ret |= pca957_driver_set_pull(&expander_instance[GPIO_EXPANDER_1],
                                         PCA9574_PULL_DOWN, LIMIT_SW_1);
  expander_ret |= pca957_driver_set_pull(&expander_instance[GPIO_EXPANDER_1],
                                         PCA9574_PULL_DOWN, LIMIT_SW_2);
  expander_ret |= pca957_driver_set_pull(&expander_instance[GPIO_EXPANDER_1],
                                         PCA9574_PULL_DOWN, LIMIT_SW_3);
  expander_ret |= pca957_driver_set_pull(&expander_instance[GPIO_EXPANDER_1],
                                         PCA9574_PULL_DOWN, LIMIT_SW_4);
  expander_ret |= pca957_driver_set_pull(&expander_instance[GPIO_EXPANDER_2],
                                         PCA9574_PULL_DOWN, LIMIT_SW_5);
  expander_ret |= pca957_driver_set_pull(&expander_instance[GPIO_EXPANDER_2],
                                         PCA9574_PULL_DOWN, LIMIT_SW_6);
  if (expander_ret != PCA957_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to set pull down on input pins");
    return false;
  }
  return true;
}

bool expander_1_gpio_set(uint8_t pin, bool value) {
  if (pin >= GPIO_OUTPUT_PIN_NUM_MAX) {
    MACKI_LOG_ERROR(TAG, "Invalid pin number");
    return false;
  }
  pca957_driver_status_t ret =
      pca957_driver_set_level_pin(&expander_instance[GPIO_EXPANDER_1],
                              value == true ? PCA9574_HIGH : PCA9574_LOW, pin);
  return ret == PCA957_DRIVER_OK;
}

bool expander_2_gpio_set(uint8_t pin, bool value) {
  if (pin >= GPIO_OUTPUT_PIN_NUM_MAX) {
    MACKI_LOG_ERROR(TAG, "Invalid pin number");
    return false;
  }
  pca957_driver_status_t ret =
      pca957_driver_set_level_pin(&expander_instance[GPIO_EXPANDER_2],
                              value == true ? PCA9574_HIGH : PCA9574_LOW, pin);
  return ret == PCA957_DRIVER_OK;
}

gpio_expander_input_level_t gpio_expander_get_level(
    gpio_expander_instance_t instance, uint8_t pin) {
  if (instance >= GPIO_EXPANDER_MAX) {
    MACKI_LOG_ERROR(TAG, "Invalid expander instance");
    return false;
  }
  pca9574_pin_level_t ret =
      pca957_driver_get_level_pin(&expander_instance[instance], pin);

  switch (ret) {
    case PCA9574_HIGH:
      return GPIO_EXPANDER_INPUT_LEVEL_HIGH;
    case PCA9574_LOW:
      return GPIO_EXPANDER_INPUT_LEVEL_LOW;
    default:
      return GPIO_EXPANDER_INPUT_LEVEL_UNKNOWN;
  }
}
