# /****************************************************************************
# Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
# This software is proprietary & confidential to Analog Devices, Inc.
# and its licensors.
# *****************************************************************************
#  * \file: CMakeLists.txt
#  * \brief: CMake file for building object file of PAL component.
#  * Developed by: Automotive Software and Systems team, Bangalore, India
# ****************************************************************************/

# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Add sub-directory depending on the type of target
if(Target STREQUAL "win_x64")
    add_subdirectory(pal_win_x64)
elseif(Target STREQUAL "sc594_som")
    add_subdirectory(pal_sc594_som)
elseif((Target STREQUAL "linux") OR (Target STREQUAL "rpi"))
    add_subdirectory(pal_linux)
elseif((Target STREQUAL "nng"))
    add_subdirectory(pal_nng)
elseif((Target STREQUAL "f746zg"))
    add_subdirectory(pal_nucleo_f746zg)
endif()
