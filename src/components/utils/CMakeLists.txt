idf_component_register(SRCS "ring_buffer/ring_buffer.c"
                    INCLUDE_DIRS "ring_buffer"
                    REQUIRES)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)