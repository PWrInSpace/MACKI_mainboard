idf_component_register(SRCS "usb_cdc_driver/usb_cdc_driver.c"
                    INCLUDE_DIRS "usb_cdc_driver"
                    REQUIRES driver usb espressif__esp_tinyusb)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)