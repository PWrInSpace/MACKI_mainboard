idf_component_register(SRCS "mechanical_controller/mechanical_controller.c" "sensor_controller/sensor_controller.c" "sensor_controller/timeout_utility.c" "sensor_controller/shared_i2c_wrapper.c"
    INCLUDE_DIRS "mechanical_controller" "sensor_controller"
    REQUIRES ic_drivers freertos peripheral_interfaces freertos ic_drivers_arduino)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)