idf_component_register(SRCS "ring_buffer/ring_buffer.c" "safe_bitwise_shifts/safe_bitwise_shifts.c"
    INCLUDE_DIRS "ring_buffer" "safe_bitwise_shifts"
    REQUIRES)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)