# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Thêm các thư mục components bổ sung
set(EXTRA_COMPONENT_DIRS 
    "$ENV{IDF_PATH}/components/bt/host/nimble" 
    "$ENV{IDF_PATH}/components/bt/host/nimble/nimble"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(neo-watch)