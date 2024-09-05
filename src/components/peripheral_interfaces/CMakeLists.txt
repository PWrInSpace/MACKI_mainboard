idf_component_register(SRCS "rtc_wrapper/rtc_wrapper.c" "i2c_wrapper/i2c_wrapper.c"
                    INCLUDE_DIRS "rtc_wrapper" "i2c_wrapper"
                    REQUIRES peripheral_drivers macki_log)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)