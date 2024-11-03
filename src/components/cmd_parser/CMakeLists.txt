idf_component_register(SRCS "cmd_command_register.c" "cmd_parser.c" "cmd_procedure.c"
    INCLUDE_DIRS "."
    REQUIRES macki_log communication_interfaces freertos controllers procedure)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)