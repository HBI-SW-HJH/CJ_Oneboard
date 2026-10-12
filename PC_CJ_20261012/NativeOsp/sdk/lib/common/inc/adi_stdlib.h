/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_stdlib.h
 * \brief: Standard library macro replacements intended for adaptation by the integrator
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup common
 *  @{
 */

#ifndef ADI_STDLIB_H
#define ADI_STDLIB_H

/* Include standard library headers */
#ifdef ADI_USE_STDLIB_IMPL

#include <stdint.h>  /*!< Integer types */
#include <stdbool.h> /*!< Boolean definitions */
#include <stddef.h>  /*!< Standard defines */
#include <ctype.h>   /*!< type utilities */
#include <stdlib.h>  /*!< Standard library */
#include <string.h>  /*!< String functions */
#include <stdio.h>   /*!< Standard I/O */
#include <limits.h>  /*!< Limits for various data types */

/*================================== DEFINES ================================*/
#ifndef ADI_STRNCPY
#define ADI_STRNCPY         strncpy
#endif

#ifndef ADI_SNPRINTF
#define ADI_SNPRINTF        snprintf
#endif

#ifndef ADI_VSNPRINTF
#define ADI_VSNPRINTF       vsnprintf
#endif

#ifndef ADI_MEMCPY
#define ADI_MEMCPY          memcpy
#endif

#ifndef ADI_MEMSET
#define ADI_MEMSET          memset
#endif

#ifndef ADI_MEMCMP
#define ADI_MEMCMP          memcmp
#endif

#ifndef ADI_STRLEN
#define ADI_STRLEN          strlen
#endif

#ifndef ADI_STRCMP
#define ADI_STRCMP          strcmp
#endif

#ifndef ADI_STRCPY
#define ADI_STRCPY          strcpy
#endif

#ifndef ADI_STRNCMP
#define ADI_STRNCMP         strncmp
#endif

#ifndef ADI_STRCHR
#define ADI_STRCHR          strchr
#endif

#ifndef ADI_STRTOK
#define ADI_STRTOK          strtok
#endif

#ifndef ADI_ATOI
#define ADI_ATOI            atoi
#endif

#ifndef ADI_STRTOL
#define ADI_STRTOL          strtol
#endif

#ifndef ADI_STRTOLL
#define ADI_STRTOLL         strtoll
#endif

#ifndef ADI_SPRINTF
#define ADI_SPRINTF         sprintf
#endif

#ifndef ADI_SSCANF
#define ADI_SSCANF          sscanf
#endif

#ifndef ADI_FFLUSH
#define ADI_FFLUSH          fflush
#endif

#ifndef ADI_PRINTF
#define ADI_PRINTF          printf
#endif

#else /* ADI_USE_STDLIB_IMPL */

/* Provide alternative implementations for the standard library functions */

#endif /* ADI_USE_STDLIB_IMPL */


#endif /* ADI_STDLIB_H */

/** @} */

/*
 * EOF: www.analog.com
 */
