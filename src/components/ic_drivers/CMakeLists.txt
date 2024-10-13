idf_component_register(SRCS "ads1115_driver/ads1115_driver.c" "tmp1075_driver/tmp1075_driver.c" "lis2dw12_driver/lis2dw12_driver.c" "pca9574_driver/pca9574_driver.c" "solenoid_driver/solenoid_driver.c" "vl53l0x_driver/vl53l0x_driver.c"
    INCLUDE_DIRS "include" "tmp1075_driver" "lis2dw12_driver" "pca9574_driver" "ads1115_driver" "solenoid_driver" "vl53l0x_driver"
    REQUIRES utils)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)