cmake_minimum_required(VERSION 3.21)

# ============================================================================
# Toolchain Configuration (must be set before project())
# IAR EWARM only. IAR path resolved via IAR_EWARM_PATH env var (see iar.toolchain.cmake).
# ============================================================================
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iar.toolchain.cmake")
endif()
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================================
# Project Definition
# ============================================================================
project(app_moodLamp_rgbi VERSION 1.0.0 LANGUAGES C ASM)

# ============================================================================
# Language Standards & Tooling
# ============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable GNU/IAR extensions required by CMSIS and startup intrinsics.
add_compile_options(--language=gnu,extended)

# ============================================================================
# Output Directories
# ============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# ============================================================================
# Configuration Library (Interface Target)
# ============================================================================
add_library(appConfig INTERFACE)
add_library(linStackConfig INTERFACE)

# ============================================================================
# BSP Selection (Conditional Compilation)
# ============================================================================
target_compile_definitions(appConfig INTERFACE
    PIC32CMJH01032
)

target_compile_definitions(linStackConfig INTERFACE
    PIC32CMJH01032
    LIN_DRV_PIC32CM_SERCOM_INDEX=2
    LIN_DRV_PIC32CM_USE_EXTERNAL_PLIB
)

# LIN Stack portable layer selection (PIC32CMJH uses USART RXBRK for LIN)
set(PIC32CM_JH ON)

# Propagate target selection to subprojects
set(PIC32CMJH01032 ON)

# ============================================================================
# External Library Dependencies (Git Submodules + Local BSP)
# ============================================================================
# Libraries are managed as git submodules in deps/extern/
set(EXTERN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../codes/comsect1/deps/extern")

# Source root paths
set(STD_SRC       "${EXTERN_PATH}/hatbit-lib-std")
set(MDW_OS_SRC    "${EXTERN_PATH}/hatbit-lib-mdw-os")
set(MOODLAMP_SRC  "${EXTERN_PATH}/hatbit-lib-moodlamp")

# BSP path - IAR only.
# Keep the MCC vendor snapshot outside the comsect1 boundary.
set(BSP_VENDOR_SRC "../codes/CJ_4DPLEX_RGBi/src")
set(BSP_DEFAULT "${BSP_VENDOR_SRC}/config/default")
set(BSP_PACKS "${BSP_VENDOR_SRC}/packs")
# Propagate to submodules - {toolchain}/src/ matches LIN stack expected layout:
#   BSP_PIC32CM_PATH/config/default  and  BSP_PIC32CM_PATH/packs/
set(BSP_PIC32CM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BSP_VENDOR_SRC}")

# Production library paths
set(LIN_STACK_SRC_ROOT "${EXTERN_PATH}/hatbit-lib-lin-stack")
set(MDW_STORAGE_SRC_ROOT "${EXTERN_PATH}/hatbit-lib-mdw-storage-manager")

# Verify required paths exist
if(NOT EXISTS "${STD_SRC}")
    message(FATAL_ERROR "hatbit-lib-std not found at: ${STD_SRC}\nDid you run 'git submodule update --init --recursive'?")
endif()
if(NOT EXISTS "${MDW_OS_SRC}")
    message(FATAL_ERROR "mdw-os library not found at: ${MDW_OS_SRC}\nDid you run 'git submodule update --init --recursive'?")
endif()
if(NOT EXISTS "${MDW_OS_SRC}/cmake/CMakeLists.txt")
    message(FATAL_ERROR "mdw-os CMake project not found at: ${MDW_OS_SRC}/cmake/CMakeLists.txt")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${BSP_DEFAULT}")
    message(FATAL_ERROR "Local BSP not found at: ${BSP_DEFAULT}")
endif()
if(NOT EXISTS "${LIN_STACK_SRC_ROOT}")
    message(FATAL_ERROR "LIN stack library not found at: ${LIN_STACK_SRC_ROOT}")
endif()
if(NOT EXISTS "${MOODLAMP_SRC}")
    message(FATAL_ERROR "MoodLamp library not found at: ${MOODLAMP_SRC}")
endif()
if(NOT EXISTS "${MOODLAMP_SRC}/cmake/CMakeLists.txt")
    message(FATAL_ERROR "MoodLamp library CMake project not found at: ${MOODLAMP_SRC}/cmake/CMakeLists.txt")
endif()
if(NOT EXISTS "${MDW_STORAGE_SRC_ROOT}")
    message(FATAL_ERROR "mdw-storage library not found at: ${MDW_STORAGE_SRC_ROOT}")
endif()
if(NOT EXISTS "${MDW_STORAGE_SRC_ROOT}/cmake/CMakeLists.txt")
    message(FATAL_ERROR "mdw-storage CMake project not found at: ${MDW_STORAGE_SRC_ROOT}/cmake/CMakeLists.txt")
endif()
if(NOT EXISTS "${LIN_STACK_SRC_ROOT}/cmake/CMakeLists.txt")
    message(FATAL_ERROR "LIN stack CMake project not found at: ${LIN_STACK_SRC_ROOT}/cmake/CMakeLists.txt")
endif()

# comsect1 architecture standard vocabulary (registered first — all submodules depend on it)
add_subdirectory("${STD_SRC}/cmake" "${CMAKE_BINARY_DIR}/comsect1-std-build")

add_subdirectory("${MOODLAMP_SRC}/cmake" "${CMAKE_BINARY_DIR}/moodlamp-build")
add_subdirectory("${MDW_OS_SRC}/cmake" "${CMAKE_BINARY_DIR}/mdw-os-build")
add_subdirectory("${MDW_STORAGE_SRC_ROOT}/cmake" "${CMAKE_BINARY_DIR}/mdw-storage-build")
add_subdirectory("${LIN_STACK_SRC_ROOT}/cmake" "${CMAKE_BINARY_DIR}/lin-stack-build")

# ============================================================================
# BSP Sources - MCC + IAR startup
# ============================================================================
# MCC-generated BSP sources
set(BSP_COMMON_SOURCES
  ${BSP_DEFAULT}/initialization.c
  ${BSP_DEFAULT}/peripheral/clock/plib_clock.c
  ${BSP_DEFAULT}/peripheral/port/plib_port.c
  ${BSP_DEFAULT}/peripheral/nvmctrl/plib_nvmctrl.c
  ${BSP_DEFAULT}/peripheral/evsys/plib_evsys.c
  ${BSP_DEFAULT}/peripheral/adc/plib_adc0.c
  ${BSP_DEFAULT}/peripheral/nvic/plib_nvic.c
  ${BSP_DEFAULT}/peripheral/sercom/usart/plib_sercom0_usart.c
  ${BSP_DEFAULT}/peripheral/sercom/spi_slave/plib_sercom1_spi_slave.c
  ${BSP_DEFAULT}/peripheral/sercom/spi_master/plib_sercom3_spi_master.c
  # SERCOM2: MCC PLIB handles init + ISR; hal_sercom2_lin.c bridges callbacks to LIN driver
  ${BSP_DEFAULT}/peripheral/sercom/usart/plib_sercom2_usart.c
  ${BSP_DEFAULT}/peripheral/dmac/plib_dmac.c
)

# IAR startup sources
set(BSP_TOOLCHAIN_SOURCES
  ${BSP_DEFAULT}/startup_iar.c
  ${BSP_DEFAULT}/interrupts.c
  ${BSP_DEFAULT}/exceptions.c
)

# ============================================================================
# Executable Target - comsect1 Architecture v1.0.0
# ============================================================================
add_executable(${CMAKE_PROJECT_NAME}
  ../codes/main.c
  ../codes/comsect1/api/app_moodlamp_rgbi.c

  ../codes/comsect1/infra/bootstrap/ida_core_moodlamp_rgbi.c
  ../codes/comsect1/infra/bootstrap/poi_core_moodlamp_rgbi.c
  ../codes/comsect1/infra/service/os/svc_os.c
  ../codes/comsect1/project/features/startup/ida_startup_moodlamp_rgbi.c
  ../codes/comsect1/project/features/startup/poi_startup_moodlamp_rgbi.c
  ../codes/comsect1/infra/platform/hal/hal_system.c

  ../codes/comsect1/project/features/rgbi/ida_rgbi_moodlamp_rgbi.c
  ../codes/comsect1/project/features/rgbi/poi_rgbi_moodlamp_rgbi.c
  ../codes/comsect1/project/features/rgbi/prx_rgbi_moodlamp_rgbi.c
  ../codes/comsect1/infra/platform/hal/hal_rgbi_spi.c
  ../codes/comsect1/infra/platform/hal/hal_time.c

  ../codes/comsect1/project/datastreams/stm_rgbi_pwm.c
  ../codes/comsect1/project/datastreams/stm_rgbi_status.c

  ../codes/comsect1/infra/service/osp/svc_osp.c
  ../codes/comsect1/infra/service/osp/svc_osp_manchester.c
  ../codes/comsect1/infra/service/rgbi/svc_rgbi.c
  ../codes/comsect1/infra/service/flicker/svc_flicker.c
  ../codes/comsect1/infra/service/color_table/svc_color_table.c

  # BSP generated sources (toolchain-specific)
  ${BSP_COMMON_SOURCES}
  ${BSP_TOOLCHAIN_SOURCES}
  ../codes/comsect1/infra/platform/hal/hal_sercom2_lin.c

  # Production features
  ../codes/comsect1/project/features/color_mix/db_color_mix_moodlamp_rgbi.c
  ../codes/comsect1/project/features/color_mix/db_color_tables_moodlamp_rgbi.c

  ../codes/comsect1/project/features/color_mix/ida_color_mix_moodlamp_rgbi.c
  ../codes/comsect1/project/features/color_mix/prx_color_mix_moodlamp_rgbi.c
  ../codes/comsect1/project/features/color_mix/poi_color_mix_moodlamp_rgbi.c
  ../codes/comsect1/project/features/led_output/ida_led_output_moodlamp_rgbi.c
  ../codes/comsect1/project/features/led_output/poi_led_output_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/ida_lin_stack_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/ida_lin_ncid_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/ida_lin_protocol_kgm_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/poi_lin_injector_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/poi_lin_runtime_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/prx_lin_protocol_hatbit_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/prx_lin_protocol_hkmc_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/prx_lin_protocol_kgm_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/prx_lin_protocol_default_moodlamp_rgbi.c
  ../codes/comsect1/project/features/lin_stack/prx_lin_ncid_delegate_moodlamp_rgbi.c
  ../codes/comsect1/project/features/rs485/ida_rs485_moodlamp_rgbi.c
  ../codes/comsect1/project/features/rs485/prx_rs485_moodlamp_rgbi.c
  ../codes/comsect1/project/features/rs485/poi_rs485_moodlamp_rgbi.c
  ../codes/comsect1/project/features/storage/ida_storage_moodlamp_rgbi.c
  ../codes/comsect1/project/features/storage/poi_storage_moodlamp_rgbi.c
  ../codes/comsect1/infra/service/storage/svc_storage.c
  ../codes/comsect1/infra/platform/hal/hal_storage_flash.c
  ../codes/comsect1/project/features/measure/ida_measure_moodlamp_rgbi.c
  ../codes/comsect1/project/features/measure/poi_measure_moodlamp_rgbi.c
  ../codes/comsect1/infra/platform/hal/hal_adc.c
  ../codes/comsect1/project/datastreams/stm_measurement.c
  ../codes/comsect1/project/features/safety/ida_safety_moodlamp_rgbi.c
  ../codes/comsect1/project/features/safety/poi_safety_moodlamp_rgbi.c
  ../codes/comsect1/project/datastreams/stm_errordiag.c
  ../codes/comsect1/project/features/application/ida_application_moodlamp_rgbi.c
  ../codes/comsect1/project/features/application/poi_application_moodlamp_rgbi.c
  ../codes/comsect1/project/datastreams/stm_systemstate.c
  ../codes/comsect1/infra/platform/hal/hal_rs485_uart.c
  ../codes/comsect1/infra/platform/hal/hal_pm.c

  ../codes/comsect1/project/datastreams/stm_moodlamp.c
  ../codes/comsect1/project/datastreams/stm_moodlamp_output.c
  ../codes/comsect1/project/datastreams/stm_moodlamp_control.c
  ../codes/comsect1/project/datastreams/stm_debug_pwm.c
  ../codes/comsect1/project/datastreams/stm_lin_ncid.c
  ../codes/comsect1/infra/service/rs485/svc_rs485.c
  ../codes/comsect1/infra/service/utility/svc_node_mask.c

  # _hatbit_linBusCfg.c removed - poi_lin_injector.c provides inline config
  # with correct STACK_CLOCK (50ms) matching project's LinClock period
)

# Flash config via #pragma location (IAR)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  ../codes/comsect1/infra/platform/bsp/bsp_flash_config.c
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  appConfig
  linStackConfig
  comsect1_std
  hml_MoodLampEngine
  _mdwOS
  _mdwStorage
  mdwLinEngine
)

# ============================================================================
# Include Directories - comsect1 Architecture v1.0.0
# ============================================================================
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ../codes
  ../codes/comsect1/api
  ../codes/comsect1/infra/bootstrap
  ../codes/comsect1/project/config
  ../codes/comsect1/project/features/rgbi
  ../codes/comsect1/infra/platform/bsp
  ../codes/comsect1/infra/platform/hal
  ../codes/comsect1/project/datastreams
  ../codes/comsect1/infra/service/osp
  ../codes/comsect1/infra/service/os
  ../codes/comsect1/infra/service/rgbi
  ../codes/comsect1/infra/service/utility
  ../codes/comsect1/infra/service/flicker
  ../codes/comsect1/infra/service/color_table
  ../codes/comsect1/infra/service/storage

  # BSP (project-local copy)
  ${BSP_DEFAULT}
  ${BSP_DEFAULT}/peripheral/clock
  ${BSP_DEFAULT}/peripheral/port
  ${BSP_DEFAULT}/peripheral/nvmctrl
  ${BSP_DEFAULT}/peripheral/evsys
  ${BSP_DEFAULT}/peripheral/nvic
  ${BSP_DEFAULT}/peripheral/adc
  ${BSP_DEFAULT}/peripheral/sercom/usart
  ${BSP_DEFAULT}/peripheral/sercom/spi_master
  ${BSP_DEFAULT}/peripheral/sercom/spi_slave
  ${BSP_DEFAULT}/peripheral/dmac
  ${BSP_PACKS}/PIC32CM2532JH01032_DFP
  ${BSP_PACKS}/CMSIS/CMSIS/Core/Include

  # Production feature includes
  ../codes/comsect1/project/features/startup
  ../codes/comsect1/project/features/color_mix
  ../codes/comsect1/project/features/led_output
  ../codes/comsect1/project/features/lin_stack
  ../codes/comsect1/project/features/rs485
  ../codes/comsect1/project/features/storage
  ../codes/comsect1/project/features/measure
  ../codes/comsect1/project/features/safety
  ../codes/comsect1/project/features/application
  ../codes/comsect1/infra/service/rs485

)

# ============================================================================
# Compile Definitions
# ============================================================================
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
  PIC32CMJH01032
  __CHECK_DEVICE_DEFINES
  $<$<CONFIG:Release>:NDEBUG>
)

# ============================================================================
# Compiler Options
# ============================================================================
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
  --cpu Cortex-M0+
  --endian little
  $<$<CONFIG:Debug>:--debug -On --no_cse --no_unroll --no_inline>
  $<$<CONFIG:Release>:-Ohs>
)

# ============================================================================
# Linker Options
# ============================================================================
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
  --config ${CMAKE_SOURCE_DIR}/flash.icf
  --entry __iar_program_start
  --map $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/${CMAKE_PROJECT_NAME}.map
  $<$<CONFIG:Release>:--inline>
)

# ============================================================================
# Architecture Validation
# ============================================================================
add_custom_target(arch_check
  COMMAND powershell -ExecutionPolicy Bypass
          -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_architecture.ps1"
          -RepoRoot "${CMAKE_CURRENT_SOURCE_DIR}/.."
  COMMENT "Checking comsect1 architecture rules..."
)

# ============================================================================
# Post-Build: Generate HEX File
# ============================================================================
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND "${IAR_BIN}/arm/bin/ielftool.exe"
          --ihex
          $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
          "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/${CMAKE_PROJECT_NAME}.hex"
  COMMENT "Generating HEX file"
)

