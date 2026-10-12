# ============================================================================
# IAR EWARM Toolchain Configuration for ARM Cortex-M0
# ============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0)

# ============================================================================
# IAR installation path
#   Priority 1: IAR_EWARM_PATH environment variable (set in .vscode/settings.json
#               or system environment)
#   Priority 2: auto-detect latest ewarm-* under C:/iar/
#   Priority 3: hard-coded fallback (last resort)
# ============================================================================
if(DEFINED ENV{IAR_EWARM_PATH})
    set(IAR_BIN "$ENV{IAR_EWARM_PATH}" CACHE PATH "IAR EWARM installation path" FORCE)
    message(STATUS "App_MoodLamp_RGBi: IAR path from IAR_EWARM_PATH: ${IAR_BIN}")
else()
    file(GLOB _iar_candidates "C:/iar/ewarm-*")
    if(_iar_candidates)
        list(SORT _iar_candidates)
        list(GET _iar_candidates -1 IAR_BIN)
        set(IAR_BIN "${IAR_BIN}" CACHE PATH "IAR EWARM installation path" FORCE)
        message(STATUS "App_MoodLamp_RGBi: IAR path auto-detected: ${IAR_BIN}")
    else()
        set(IAR_BIN "C:/iar/ewarm-9.70.4" CACHE PATH "IAR EWARM installation path")
        message(WARNING "App_MoodLamp_RGBi: IAR not found under C:/iar/, using fallback: ${IAR_BIN}")
    endif()
endif()

# ============================================================================
# IAR Toolchain Executables
# ============================================================================
set(CMAKE_C_COMPILER     "${IAR_BIN}/arm/bin/iccarm.exe")
set(CMAKE_ASM_COMPILER   "${IAR_BIN}/arm/bin/iasmarm.exe")
set(CMAKE_LINKER         "${IAR_BIN}/arm/bin/ilinkarm.exe")
set(CMAKE_AR             "${IAR_BIN}/arm/bin/iarchive.exe")
set(CMAKE_MAKE_PROGRAM   "${IAR_BIN}/common/bin/ninja.exe" CACHE FILEPATH "Ninja build tool" FORCE)

# ============================================================================
# IAR Compiler Architecture ID (Required for CMake 4.x)
# ============================================================================
set(CMAKE_C_COMPILER_ID IAR)
set(CMAKE_C_COMPILER_ARCHITECTURE_ID ARM)
set(CMAKE_ASM_COMPILER_ID IAR)
set(CMAKE_ASM_COMPILER_ARCHITECTURE_ID ARM)

# ============================================================================
# Skip Compiler Testing (Cross-Compilation)
# ============================================================================
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)
