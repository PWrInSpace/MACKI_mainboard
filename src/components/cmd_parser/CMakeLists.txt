idf_component_register(SRCS "cmd_parser.c"
                    INCLUDE_DIRS "."
                    REQUIRES macki_log console freertos)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)