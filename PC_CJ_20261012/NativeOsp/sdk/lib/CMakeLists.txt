# /****************************************************************************
# Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
# This software is proprietary & confidential to Analog Devices, Inc.
# and its licensors.
# *****************************************************************************
#  * \file: CMakeLists.txt
#  * \brief: Library CMake file.
#            This calls all the individual components and builds object files
#            for each of this component.
#  * Developed by: Automotive Software and Systems team, Bangalore, India
# ****************************************************************************/

# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Add sub-directories for the lib components

# oaspi component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/oaspi/CMakeLists.txt")
  add_subdirectory(oaspi)
endif()

# e2bcore component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/e2bcore/CMakeLists.txt")
  add_subdirectory(e2bcore)
endif()

# Common component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/common/CMakeLists.txt")
  add_subdirectory(common)
endif()

# Platform Abstraction Layer component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pal/CMakeLists.txt")
  add_subdirectory(pal)
endif()

# OS Abstraction Layer component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/osal/CMakeLists.txt")
  add_subdirectory(osal)
endif()

# Network layer component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/network/CMakeLists.txt")
  add_subdirectory(network)
endif()

# EAL component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/eal/CMakeLists.txt")
  add_subdirectory(eal)
endif()

# PTP component
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ptp/CMakeLists.txt")
  add_subdirectory(ptp)
endif()

# LWIP component moved to app/utils/lwip
