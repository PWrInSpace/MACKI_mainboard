idf_component_register(SRCS "mechanical_controller/mechanical_controller.c" "sensor_controller/sensor_controller.c"
    INCLUDE_DIRS "mechanical_controller" "sensor_controller"
    REQUIRES ic_drivers freertos peripheral_interfaces freertos)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)