# /****************************************************************************
# Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
# This software is proprietary & confidential to Analog Devices, Inc.
# and its licensors.
# *****************************************************************************
#  * \file: CMakeLists.txt
#  * \brief: CMake file for building object file of lwip component.
#  * Developed by: Automotive Software and Systems team, Bangalore, India
# ****************************************************************************/

# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.16)

set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Set LWIP_INCLUDE_DIRS for use by Filelists.cmake
set(LWIP_INCLUDE_DIRS "")
# Set LWIP_MBEDTLS_INCLUDE_DIRS (empty for now, can be configured if mbedtls is needed)
set(LWIP_MBEDTLS_INCLUDE_DIRS "")

# Select port based on target first, then OS
if(Target STREQUAL "win_x64")
    # Windows always uses win32 port regardless of OS setting
    set(INC_DIR "${INC_DIR}"
        "${LWIP_DIR}/ports/win32/include"
        "${LWIP_DIR}/ports/win32"
    )
    set(LWIP_INCLUDE_DIRS
        "${LWIP_DIR}/src/include"
        "${LWIP_DIR}/ports/win32/include"
        "${LWIP_DIR}/ports/win32"
    )
elseif(OS STREQUAL "freeRTOS")
    # FreeRTOS targets use freertos port
    set(INC_DIR "${INC_DIR}"
        "${LWIP_DIR}/ports/freertos/include"
        "${LWIP_DIR}/ports/freertos"
    )
    set(LWIP_INCLUDE_DIRS
        "${LWIP_DIR}/src/include"
        "${LWIP_DIR}/ports/freertos/include"
        "${LWIP_DIR}/ports/freertos"
    )
else()
    # Unix-like targets (rpi, linux, etc.) use unix port
    set(INC_DIR "${INC_DIR}"
        "${LWIP_DIR}/ports/unix/port/include"
        "${LWIP_DIR}/ports/unix"
    )
    set(LWIP_INCLUDE_DIRS
        "${LWIP_DIR}/src/include"
        "${LWIP_DIR}/ports/unix/port/include"
        "${LWIP_DIR}/ports/unix"
    )
endif()

# Add the paths for the include files
include_directories(
    "./src/include"
    "${INC_DIR}"
)

set (LWIP_DEFINITIONS LWIP_DEBUG)

# For NoOS builds, disable IPv6
if(OS STREQUAL "noOS")
    list(APPEND LWIP_DEFINITIONS LWIP_IPV6=0)
endif()

set (LWIP_COMPILER_FLAGS ${COMPILE_OPTS})
set (LWIP_MBEDTLS_DEFINITIONS "")
include(src/Filelists.cmake)