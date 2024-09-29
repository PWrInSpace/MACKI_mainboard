idf_component_register(SRCS "logger_task/logger_task.c" "cli_task/cli_task.c" "sensor_task/sensor_task.c" "sensor_task/i2c_sensor_defines.c" "motor_task/motor_task.c"
    INCLUDE_DIRS "logger_task" "cli_task" "sensor_task" "motor_task"
    REQUIRES communication_interfaces logger cmd_parser macki_log freertos alternative_ic_drivers ic_drivers)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)