# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_LIB_PATH}/components)
#set(EXTRA_COMPONENT_DIRS /home/viper/esp/esp-idf-lib/components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(nightgown)
