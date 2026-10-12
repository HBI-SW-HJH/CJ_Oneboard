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
set(This Osal)

set(INC_DIR "./inc")

# Add sub-directory depending on the type of Operating system
if(OS STREQUAL "freeRTOS")
    set(OSAL_SRC "${OSAL_SRC}" "freeRTOS/prj_osal_freeRTOS.c")
    if(Target STREQUAL sc594_som)
        set(INC_DIR "${INC_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/system/FreeRTOS/Source/portable/CCES/ARM_CA5"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/system/FreeRTOS/Source/portable/CCES/ARM_CA5/osal"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/system/FreeRTOS/Source/portable/GCC/ARM_CA9"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/system/FreeRTOS/GeneratedSources"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/system/FreeRTOS/Source/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/system/FreeRTOS/Source/portable/CCES/osal"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_sc594_som/src/CustomizableFreeRTOSSources"
        )
    elseif(Target STREQUAL f746zg)
        set(INC_DIR "${INC_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_nucleo_f746zg/Core/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_nucleo_f746zg/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_nucleo_f746zg/Middlewares/Third_Party/FreeRTOS/Source/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../pal/pal_nucleo_f746zg/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1"
        )
    endif()
elseif(OS STREQUAL "pthread")
    set(OSAL_SRC "${OSAL_SRC}" "pthread/prj_osal_pthread.c")
elseif(OS STREQUAL "noOS")
    set(OSAL_SRC "${OSAL_SRC}" "noos/prj_osal_noos.c")
endif()

# Add the paths for the include files
include_directories(
    "./inc"
    "../dbg/inc"
    "../common/inc"
    "../common/regmaps"
    "../pal/inc"
    "../osal/inc"
    "${INC_DIR}"
)

# Create an object library from these sources
add_library("${This}" OBJECT "${OSAL_SRC}")

# Add compile definitions
target_compile_definitions("${This}" PRIVATE "${COMPILE_DEFS}")

# Add compile options
target_compile_options("${This}" PRIVATE "${COMPILE_OPTS}")
