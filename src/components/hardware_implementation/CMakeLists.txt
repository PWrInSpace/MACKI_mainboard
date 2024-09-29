idf_component_register(SRCS "solenoid_implementation.c"
    INCLUDE_DIRS "."
    REQUIRES ic_drivers i2c_sensor_defines alternative_ic_drivers)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)