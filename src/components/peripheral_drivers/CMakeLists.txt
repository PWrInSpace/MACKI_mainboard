idf_component_register(SRCS "usb_cdc_driver/usb_cdc_driver.c" "i2c_driver/i2c_driver.c" "spi_sd_driver/spi_sd_driver.c" "uart_comm_driver/uart_comm_driver.c" "adc_driver/adc_driver.c"
    INCLUDE_DIRS "usb_cdc_driver" "i2c_driver" "spi_sd_driver" "uart_comm_driver" "adc_driver"
    REQUIRES driver usb espressif__esp_tinyusb driver macki_log esp_adc)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)