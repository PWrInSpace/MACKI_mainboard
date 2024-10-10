idf_component_register(SRCS "."
    INCLUDE_DIRS "."
    REQUIRES tmc2209_arduino macki_log)

# target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)