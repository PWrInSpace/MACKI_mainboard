idf_component_register(SRCS "procedure_parser.c" "procedure_parser_helper.c"
    INCLUDE_DIRS "."
    REQUIRES macki_log)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)