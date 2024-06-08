idf_component_register(SRCS "usb_cdc_interface/usb_cdc_interface.c"
                    INCLUDE_DIRS "usb_cdc_interface"
                    REQUIRES peripheral_drivers)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)