idf_component_register(SRCS "mechanical_controller/mechanical_controller.c" "mechanical_controller/gpio_expander_wrapper.c" "mechanical_controller/limit_switch_wrapper.c" "sensor_controller/sensor_controller.c" "sensor_controller/timeout_utility.c" "sensor_controller/sensor_converters.c" "shared_wrappers/shared_i2c_wrapper.c" "shared_wrappers/shared_delay_wrapper.c"
    INCLUDE_DIRS "mechanical_controller" "sensor_controller" "shared_wrappers"
    REQUIRES ic_drivers freertos peripheral_interfaces freertos ic_drivers_arduino)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)