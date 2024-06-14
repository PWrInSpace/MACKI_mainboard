idf_component_register(SRCS "logger_task/logger_task.c"
                    INCLUDE_DIRS "logger_task"
                    REQUIRES communication_interfaces logger)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)