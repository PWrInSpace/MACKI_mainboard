idf_component_register(SRCS "tmp1075_driver/tmp1075_driver.c" "lis2dw12_driver/lis2dw12_driver.c"
                    INCLUDE_DIRS "tmp1075_driver" "lis2dw12_driver" "include"
                    REQUIRES macki_log utils)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)