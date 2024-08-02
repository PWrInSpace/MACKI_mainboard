idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ""
<<<<<<< HEAD
                    REQUIRES peripheral_drivers communication_interfaces logger app utils)
=======
                    REQUIRES peripheral_drivers communication_interfaces)
>>>>>>> main

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)
