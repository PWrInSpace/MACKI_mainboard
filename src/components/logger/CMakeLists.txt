idf_component_register(SRCS "log_manager/log_manager.c" "log_receiver/log_receiver.c" "macki_log/macki_log.c"
                    INCLUDE_DIRS "log_manager" "log_receiver" "macki_log"
                    REQUIRES utils)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)