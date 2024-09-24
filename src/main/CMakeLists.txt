idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES app utils freertos peripheral_interfaces cmd_parser)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)
