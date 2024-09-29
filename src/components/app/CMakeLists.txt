idf_component_register(SRCS "logger_task/logger_task.c" "cli_task/cli_task.c" "sensor_task/sensor_task.c"
    INCLUDE_DIRS "logger_task" "cli_task" "sensor_task"
    REQUIRES communication_interfaces logger cmd_parser macki_log)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)