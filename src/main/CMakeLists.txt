idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES logger app utils)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)
