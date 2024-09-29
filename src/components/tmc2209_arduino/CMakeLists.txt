idf_component_register(SRCS "src/TMC2209/TMC2209.cpp" "src/TMC2209/TMC2209_impl.cpp"
    INCLUDE_DIRS "src"
    REQUIRES espressif__arduino-esp32)

# target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)