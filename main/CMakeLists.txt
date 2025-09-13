idf_component_register(SRCS "main.c"

                    ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_i2c.c
                    ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_lcd.c
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