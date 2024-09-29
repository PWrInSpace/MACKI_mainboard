idf_component_register(SRCS "i2c_sensor_defines.c"
    INCLUDE_DIRS "."
    REQUIRES peripheral_interfaces)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)