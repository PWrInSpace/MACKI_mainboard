idf_component_register(SRCS "usb_cdc_driver/usb_cdc_driver.c" "i2c_driver/i2c_driver.c" "spi_driver/spi_driver.c" "uart_driver/uart_driver.c"
    INCLUDE_DIRS "usb_cdc_driver" "i2c_driver" "spi_driver" "uart_driver"
    REQUIRES driver usb espressif__esp_tinyusb driver)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)