# /****************************************************************************
# Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
# This software is proprietary & confidential to Analog Devices, Inc.
# and its licensors.
# *****************************************************************************
#  * \file: CMakeLists.txt
#  * \brief: CMake file for building object file of OSAL component.
#  * Developed by: Automotive Software and Systems team, Bangalore, India
# ****************************************************************************/

# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Set "This" variable for use in multiple places
set(This lwipport)
set(PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LWIP_DEFINITIONS LWIP_DEBUG)

# For NoOS builds, disable IPv6
if(OS STREQUAL "noOS")
    list(APPEND LWIP_DEFINITIONS LWIP_IPV6=0)
endif()
# include(${PORT_DIR}/CMakeCommon.cmake)

# Add sub-directory - select port based on target first, then OS
if(Target STREQUAL "win_x64")
    # Windows always uses win32 port regardless of OS setting
    set(PORT_SRC "${PORT_SRC}" "win32/sys_arch.c")
    set(INC_DIR "${INC_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/win32/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/win32"
        )
elseif(OS STREQUAL "freeRTOS")
    set(PORT_SRC "${PORT_SRC}" "freertos/sys_arch.c")
    if(Target STREQUAL "sc594_som")
        set(INC_DIR "${INC_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/system/FreeRTOS/Source/portable/CCES/ARM_CA5"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/system/FreeRTOS/Source/portable/CCES/ARM_CA5/osal"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/system/FreeRTOS/Source/portable/GCC/ARM_CA9"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/system/FreeRTOS/GeneratedSources"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/system/FreeRTOS/Source/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/system/FreeRTOS/Source/portable/CCES/osal"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_sc594_som/src/CustomizableFreeRTOSSources"
            "${CMAKE_CURRENT_SOURCE_DIR}/freertos/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/freertos"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/include"
        )
    elseif(Target STREQUAL "f746zg")
        set(INC_DIR "${INC_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_nucleo_f746zg/Core/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_nucleo_f746zg/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_nucleo_f746zg/Middlewares/Third_Party/FreeRTOS/Source/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/pal/pal_nucleo_f746zg/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1"
            "${CMAKE_CURRENT_SOURCE_DIR}/freertos/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/freertos"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/include"
        )
    endif()
elseif(OS STREQUAL "pthread" OR OS STREQUAL "noOS")
    # Unix-like targets (rpi, linux, etc.) use unix port
    set(PORT_SRC "${PORT_SRC}" "unix/port/sys_arch.c")
    set(INC_DIR "${INC_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/unix/port/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/unix"
        )
endif()

# Add the paths for the include files
include_directories(
    "./"
    "../../../../lib/dbg/inc"
    "../../../../lib/common/inc"
    "../../../../lib/common/regmaps"
    "../../../../lib/pal/inc"
    "../../../../lib/osal/inc"
    "${INC_DIR}"
)

# Create an object library from these sources
add_library("${This}" OBJECT "${PORT_SRC}" )

# Add compile definitions
target_compile_definitions("${This}" PRIVATE "${COMPILE_DEFS}" "${LWIP_DEFINITIONS}")

# Add compile options
target_compile_options("${This}" PRIVATE "${COMPILE_OPTS}")
