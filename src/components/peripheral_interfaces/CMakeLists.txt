idf_component_register(SRCS "rtc_wrapper/rtc_wrapper.c" "i2c_wrapper/i2c_wrapper.c" "gpio_wrapper/gpio_wrapper.c" "adc_wrapper/adc_wrapper.c" "uart_wrapper/uart_wrapper.c"
    INCLUDE_DIRS "rtc_wrapper" "i2c_wrapper" "gpio_wrapper" "adc_wrapper" "uart_wrapper"
    REQUIRES peripheral_drivers macki_log)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)