idf_component_register(SRCS "rtc_wrapper/rtc_wrapper.c"
                    INCLUDE_DIRS "rtc_wrapper"
                    REQUIRES driver usb espressif__esp_tinyusb)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)