# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# Include the components directory of the main application:
#
set(EXTRA_COMPONENT_DIRS 
    "../components/logger"
    "../components/utils"
    "../components/peripheral_drivers"
    "../components/peripheral_interfaces"
    "../components/macki_log"
    "../components/ic_drivers"
)
set(IDF_TARGET "esp32s3")
# Set the components to include the tests for.
# This can be overriden from CMake cache:
# - when invoking CMake directly: cmake -D TEST_COMPONENTS="xxxxx" ..
# - when using idf.py: idf.py -T xxxxx build
#
set(TEST_FILES_DIRS
    "logger"    
    "utils"
    "peripheral_drivers"
    "ic_drivers"
)

set(TEST_COMPONENTS ${TEST_FILES_DIRS} CACHE STRING "List of components to test")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(MACKI_MainBoard)