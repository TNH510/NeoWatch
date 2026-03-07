idf_component_register(SRCS "main.c"
                    ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_i2c.c
                    ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_lcd.c
                    ${CMAKE_SOURCE_DIR}/source/src/system/system_display.c
                    ${CMAKE_SOURCE_DIR}/source/src/system/system_manager.c
                    ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_rtc.c
                    ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp.c

                    REQUIRES main
                    INCLUDE_DIRS ".")

include_directories(${CMAKE_SOURCE_DIR}/source/inc/bsp)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/common)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/driver)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/middleware)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/system)
include_directories(${CMAKE_SOURCE_DIR}/source/src/bsp)
include_directories(${CMAKE_SOURCE_DIR}/source/src/common)
include_directories(${CMAKE_SOURCE_DIR}/source/src/driver)
include_directories(${CMAKE_SOURCE_DIR}/source/src/middleware)
include_directories(${CMAKE_SOURCE_DIR}/source/src/system)
include_directories(${CMAKE_SOURCE_DIR}/ui/export/c)

# Add NimBLE include directories
include_directories($ENV{IDF_PATH}/components/bt/host/nimble/nimble/nimble/host/include)
include_directories($ENV{IDF_PATH}/components/bt/host/nimble/nimble/porting/nimble/include)
include_directories($ENV{IDF_PATH}/components/bt/host/nimble/nimble/nimble/include)
include_directories($ENV{IDF_PATH}/components/bt/host/nimble/port/include)