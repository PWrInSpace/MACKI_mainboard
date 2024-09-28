idf_component_register(SRCS "usb_cdc_interface/usb_cdc_interface.c" "uart_interface/uart_interface.c" "uart_console_interface/uart_console_interface.c"
    INCLUDE_DIRS "usb_cdc_interface" "uart_interface" "uart_console_interface"
    REQUIRES peripheral_drivers macki_log console)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)