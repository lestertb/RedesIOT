# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(red_mesh)
set(EXTRA_COMPONENT_DIRS "wifi")
set(EXTRA_COMPONENT_DIRS "mqtt_control")
set(EXTRA_COMPONENT_DIRS "mesh_control")
set(EXTRA_COMPONENT_DIRS "dht22")
set(EXTRA_COMPONENT_DIRS "moisture_sensor")