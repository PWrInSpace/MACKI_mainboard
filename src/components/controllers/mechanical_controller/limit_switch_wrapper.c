// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "limit_switch_wrapper.h"

limit_switch_state_t check_limit_switch_state(limit_switch_t* limit_switch) {
  gpio_expander_input_level_t level = gpio_expander_get_level(
      limit_switch->gpio_expander_instance, limit_switch->limit_switch_pin_num);
  if (level == GPIO_EXPANDER_INPUT_LEVEL_HIGH) {
    limit_switch->state = LIMIT_SWITCH_PRESSED;
    return LIMIT_SWITCH_PRESSED;
  } else {
    limit_switch->state = LIMIT_SWITCH_NOT_PRESSED;
    return LIMIT_SWITCH_NOT_PRESSED;
  }
}
