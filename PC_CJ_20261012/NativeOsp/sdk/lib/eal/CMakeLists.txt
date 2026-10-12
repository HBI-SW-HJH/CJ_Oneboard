# /****************************************************************************
# Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
# This software is proprietary & confidential to Analog Devices, Inc.
# and its licensors.
# *****************************************************************************
#  * \file: CMakeLists.txt
#  * \brief: CMake file for building object file of Eal component.
#  * Developed by: Automotive Software and Systems team, Bangalore, India
# ****************************************************************************/

# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Set "This" varable for use in multiple places
set(This Eal)

# Add the paths for the include files
include_directories(
    "./inc"
    "../dbg/inc"
    "../common/inc"
    "../common/regmaps"
    "../pal/inc"
    "../network/inc"
    "../macphy/inc"
    "../e2bcore/inc"
    "../oaspi/inc"
    "../osal/inc"
)

# Add the sources
file(GLOB_RECURSE SOURCES
    "./src/*.c"
)

# Create a static library from these sources
add_library("${This}" STATIC "${SOURCES}")

# Add compile definitions
target_compile_definitions("${This}" PRIVATE "${COMPILE_DEFS}")

# Add compile options
target_compile_options("${This}" PRIVATE "${COMPILE_OPTS}")
