# /****************************************************************************
# Copyright (c) 2023 - Analog Devices Inc. All Rights Reserved.
# This software is proprietary & confidential to Analog Devices, Inc.
# and its licensors.
# *****************************************************************************
#  * \file: CMakeLists.txt
#  * \brief: CMake file for building object file of lwip
# ****************************************************************************/

# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.16)

set(This adi_lwip)

set (LWIP_DEFINITIONS LWIP_DEBUG)
set(ADI_LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib")
set(LWIP_PORTS "${CMAKE_CURRENT_SOURCE_DIR}/../ports")
# Select port based on target first, then OS
if(Target STREQUAL "win_x64")
    # Windows always uses win32 port regardless of OS setting
    set(LWIP_PORTS_INC "${LWIP_PORTS_INC}"
                    "${LWIP_PORTS}"
                    "${LWIP_PORTS}/win32"
                    "${LWIP_PORTS}/win32/include"
    )
elseif(OS STREQUAL "freeRTOS")
    # FreeRTOS targets use freertos port
    set(LWIP_PORTS_INC "${LWIP_PORTS_INC}"
                    "${LWIP_PORTS}"
                    "${LWIP_PORTS}/freertos"
                    "${LWIP_PORTS}/freertos/include"
    )
else()
    # Unix-like targets (rpi, linux, etc.) use unix port
    set(LWIP_PORTS_INC "${LWIP_PORTS_INC}"
                    "${LWIP_PORTS}"
                    "${LWIP_PORTS}/unix"
                    "${LWIP_PORTS}/unix/port/include"
    )
endif()

# For NoOS builds, disable IPv6
if(OS STREQUAL "noOS")
    list(APPEND LWIP_DEFINITIONS LWIP_IPV6=0)
endif()    
# Add the paths for the include files
include_directories(
    # Library includes
    "${LIB_ROOT}/common/inc"
    "${LIB_ROOT}/common/regmaps"
    "${LIB_ROOT}/dbg/inc"
    "${LIB_ROOT}/network/inc"
    "${LIB_ROOT}/oaspi/inc"
    "${LIB_ROOT}/pal/inc"
    "${LIB_ROOT}/osal/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/include"
    "${LWIP_PORTS_INC}"
    # App includes
    "./"
    "${ADI_LWIP_DIR}/inc"
)

# Add the ADI-lwip sources
file(GLOB_RECURSE SOURCES
    "./src/*.c"
)

# Set executable
add_library("${This}" STATIC ${SOURCES})

# Add compile definitions
target_compile_definitions("${This}" PRIVATE "${COMPILE_DEFS}" "${LWIP_DEFINITIONS}")

# Add compile options
target_compile_options("${This}" PRIVATE "${COMPILE_OPTS}")
