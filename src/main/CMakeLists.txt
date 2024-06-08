idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ""
                    REQUIRES peripheral_drivers)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)
