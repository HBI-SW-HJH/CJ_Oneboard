cmake_minimum_required(VERSION 3.16)

project(OspBridge C)

# Newer MinGW packages may resolve the target-specific assembler first. On
# this workstation that copy exits abnormally, while the assembler beside
# gcc is valid. Keep all binutils on the compiler's own tool directory.
if(MINGW)
    get_filename_component(MINGW_TOOL_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    add_compile_options("-B${MINGW_TOOL_DIR}/")
    add_link_options("-B${MINGW_TOOL_DIR}/")
endif()

# ============================================================================
# SDK ??? ??? ???
#
# ????????????? ?????????: NativeOsp/sdk/ ??? ??????????SDK
# ???/???/??? ????????? ??? ?????????. ??????????
# NativeOsp/SDK_SETUP.md ??????????
#


# Use only the project-local SDK copy under NativeOsp/sdk.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
    set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
    message(STATUS "[OspBridge] SDK: using project-local NativeOsp/sdk (${SDK_ROOT})")
else()
    message(FATAL_ERROR
        "[OspBridge] NativeOsp/sdk was not found. This project is intentionally self-contained; copy the SDK into NativeOsp/sdk before building."
    )
endif()

set(LIB_ROOT  "${SDK_ROOT}/lib")
set(APP_UTILS "${SDK_ROOT}/app/utils")

set(Target win_x64)
set(OS noOS)
set(COMPILE_DEFS  -D_DEBUG -DCORE0 -DDEBUG -DUSE_EAL -DCUST_BOARD -DWIN_X64 -DADI_USE_STDLIB_IMPL -DNoOS)
set(COMPILE_OPTS  -Wall -Wextra -Wformat -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function
                  -fdata-sections -ffunction-sections -O3 -std=gnu11)

add_subdirectory("${LIB_ROOT}/common" "${CMAKE_BINARY_DIR}/common")
add_subdirectory("${LIB_ROOT}/pal"     "${CMAKE_BINARY_DIR}/pal")
add_subdirectory("${LIB_ROOT}/network" "${CMAKE_BINARY_DIR}/network")
add_subdirectory("${LIB_ROOT}/eal"     "${CMAKE_BINARY_DIR}/eal")
add_subdirectory("${LIB_ROOT}/osal"    "${CMAKE_BINARY_DIR}/osal")

include_directories(
    "${LIB_ROOT}/common/inc"
    "${LIB_ROOT}/common/regmaps"
    "${LIB_ROOT}/dbg/inc"
    "${LIB_ROOT}/e2bcore/inc"
    "${LIB_ROOT}/network/inc"
    "${LIB_ROOT}/eal/inc"
    "${LIB_ROOT}/oaspi/inc"
    "${LIB_ROOT}/pal/inc"
    "${LIB_ROOT}/ptp/inc"
    "${LIB_ROOT}/osal/inc"
    "${APP_UTILS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils"
)

link_directories("${LIB_ROOT}/bin")

file(GLOB APP_UTIL_SOURCES "${APP_UTILS}/*.c")

add_library(OspBridge SHARED
    ${APP_UTIL_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/adi_Eth10BaseT1s_cfg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ex_fioOsp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ilas_led.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/iseled.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/fioOsp.c"
    $<TARGET_OBJECTS:Common>
    $<TARGET_OBJECTS:Pal>
    $<TARGET_OBJECTS:Network>
    $<TARGET_OBJECTS:Eal>
    $<TARGET_OBJECTS:Osal>
)

target_compile_definitions(OspBridge PRIVATE ${COMPILE_DEFS})
target_compile_options(OspBridge PRIVATE ${COMPILE_OPTS})
target_link_libraries(OspBridge PRIVATE oaspi_win_x64 e2b_win_x64)
target_link_options(OspBridge PRIVATE "LINKER:--gc-sections")

set_target_properties(OspBridge PROPERTIES
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
)

# ??? ??PAL DLL(ftd2xx ??? ????build/ ????????
add_custom_command(
    TARGET OspBridge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${LIB_ROOT}/pal/pal_win_x64/lib"
        "$<TARGET_FILE_DIR:OspBridge>"
)



