idf_component_register(SRCS "macki_log/macki_log.c"
                    INCLUDE_DIRS "macki_log"
                    REQUIRES logger)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)