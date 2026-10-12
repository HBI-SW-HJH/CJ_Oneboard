/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_dbg.h
 * \brief: Provides a common debug / diagnostics interface for all components
 * in this application
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup dbg
 *  @{
 */

#ifndef ADI_DBG_H
#define ADI_DBG_H

/*================================= INCLUDES ================================*/
#include "adi_common.h"
#if defined(__ADSPSC59x__)
#include <adi_types.h>
#endif
#include "adi_pal.h"


/*================================= DEFINES =================================*/
#ifdef ADI_DEBUG

#define LOG_DEBUG   (0U)
#define LOG_INFO    (1U)
#define LOG_WARN    (2U)
#define LOG_ERROR   (3U)

#ifdef __TASKING__
#define ADI_DBG_LOG(prio, stream, ...) do {\
    char str[8u];\
    if (prio == LOG_DEBUG)\
        /* str = "DEBUG"; */\
        break;\
    else if (prio == LOG_INFO)\
        /* str = "INFO"; */\
        break;\
    else if (prio == LOG_WARN)\
        (void) ADI_STRCPY(str, "WARN");\
    else if (prio == LOG_ERROR)\
        (void) ADI_STRCPY(str, "ERR");\
    else\
        (void) ADI_STRCPY(str, "UNKNOWN");\
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[%s] : %s : %d : ", str, __FILE__, __LINE__);\
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, __VA_ARGS__);\
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");\
} while (false)
#else
#define ADI_DBG_LOG(prio, stream, msg, ...) do {\
    char str[8u];\
    if (prio == LOG_DEBUG)\
        /* str = "DEBUG"; */\
        break;\
    else if (prio == LOG_INFO)\
        /* str = "INFO"; */\
        break;\
    else if (prio == LOG_WARN)\
        (void) ADI_STRCPY(str, "WARN");\
    else if (prio == LOG_ERROR)\
        (void) ADI_STRCPY(str, "ERR");\
    else\
        (void) ADI_STRCPY(str, "UNKNOWN");\
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[%s] : %s : %d : " msg "\r\n", \
                str, __FILE__, __LINE__, ##__VA_ARGS__);\
} while (false)
#endif

#define ADI_DBG_ERROR() {\
    ADI_DBG_LOG(LOG_ERROR, stderr, "Fatal error");\
    adi_pal_setFatalError();\
}

#define ADI_DBG_FUNC(func, errMsg) {\
    if((func) != 0) {\
        ADI_DBG_LOG(LOG_ERROR, stderr, "Bad function return: " errMsg);\
        ADI_DBG_ERROR();\
    }\
}

#define ADI_DBG_REQUIRE(cond, errMsg) {\
    if(!(cond)) {\
        ADI_DBG_LOG(LOG_ERROR, stderr, "Precondition error: " errMsg);\
        ADI_DBG_ERROR();\
    }\
}

#define ADI_DBG_ENSURE(cond, errMsg) {\
    if(!(cond)) {\
        ADI_DBG_LOG(LOG_ERROR, stderr, "Postcondition error: " errMsg);\
        ADI_DBG_ERROR();\
    }\
}

/* ADI_DEBUG */
#elif defined(ADI_E2B_CDD_DEBUG)
#define ADI_DBG_LOG(prio, stream, msg, ...)
#define ADI_DBG_ERROR() {\
    ADI_DBG_LOG(LOG_ERROR, stderr, "Fatal error");\
    adi_pal_setFatalError();\
}

#define ADI_DBG_FUNC(func, errMsg) {\
    if((func) != 0) {\
        ADI_DBG_LOG(LOG_ERROR, stderr, "Bad function return: " errMsg);\
        ADI_DBG_ERROR();\
    }\
}

#define ADI_DBG_REQUIRE(cond, errMsg) {\
    if(!(cond)) {\
        ADI_DBG_LOG(LOG_ERROR, stderr, "Precondition error: " errMsg);\
        ADI_DBG_ERROR();\
    }\
}

#define ADI_DBG_ENSURE(cond, errMsg) {\
    if(!(cond)) {\
        ADI_DBG_LOG(LOG_ERROR, stderr, "Postcondition error: " errMsg);\
        ADI_DBG_ERROR();\
    }\
}
/* ADI_E2B_CDD_DEBUG */
#else
#ifdef _TESTDEBUG
#if defined(__TASKING__)
#define ADI_DBG_LOG(prio, stream, ...)
#else
#define ADI_DBG_LOG(prio, stream, msg, ...)
#endif
#define ADI_DBG_ERROR()
#define ADI_DBG_FUNC(func, errMsg) (func)
#define ADI_DBG_REQUIRE(cond, errMsg)
#define ADI_DBG_ENSURE(cond, errMsg)
#else /* _TESTDEBUG */
/* Release mode defines */
#if defined(__TASKING__)
#define ADI_DBG_LOG(prio, stream, ...)
#else
#define ADI_DBG_LOG(prio, stream, msg, ...)
#endif
#define ADI_DBG_ERROR() adi_pal_setFatalError()
#define ADI_DBG_FUNC(func, errMsg) (func)
#define ADI_DBG_REQUIRE(cond, errMsg) ((void)(cond))
#define ADI_DBG_ENSURE(cond, errMsg) ((void)(cond))
#endif /* _TESTDEBUG */
#endif /* ADI_DEBUG */

#ifdef __TASKING__
#define ADI_DBG_PAL_LOG(IsPrompt, ...) ADI_PAL_LOG(IsPrompt, __VA_ARGS__)
#else
#define ADI_DBG_PAL_LOG(IsPrompt, msg, ...) ADI_PAL_LOG(IsPrompt, msg, ##__VA_ARGS__)
#endif

/* Format specifiers */
#if defined(__ADSPSC59x__) || defined(__NUCLEO_STM32__)
#define UINT32_FORMATTER "%ld"
#define UINT32_HEX_FORMATTER "%08lx"
#define UINT64_FORMATTER "%lld"
#elif defined(__linux__)
#define UINT32_FORMATTER "%d"
#define UINT32_HEX_FORMATTER "%08x"
#ifdef RPI3_RASPBIAN
#define UINT64_FORMATTER "%lld"
#else
#define UINT64_FORMATTER "%ld"
#endif
#else
#define UINT32_FORMATTER "%d"
#define UINT32_HEX_FORMATTER "%08x"
#define UINT64_FORMATTER "%I64d"
#endif

#endif /* ADI_DBG_H */

/** @} */

/*
 * EOF: www.analog.com
 */
