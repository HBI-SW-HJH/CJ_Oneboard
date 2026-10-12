/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: demo_14SegDisp.h
 * \brief: The 14-Segment display demo implementation
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */
#ifndef DEMO_14SEGDISP_H
#define DEMO_14SEGDISP_H

#include "adi_common.h"

#ifdef ADI_E2B_IFACE_I2C_ENABLED
/*================================= INCLUDES ================================*/
#include "adi_pal.h"                    /*!< Platform abstraction layer */
#include "adi_dbg.h"                    /*!< Debug interface */
#include "adi_network.h"                /*!< Network layer */
#include "adi_app_utils.h"              /*!< Include common utilities */
#include "adi_eal.h"                    /*!< E2B abstraction layer */
#include "adi_eal_i2c.h"                /*!< E2B I2c abstraction layer */

/*================================= DEFINES =================================*/
#define SLAVE_ADDRESS           (0x70)  /*!< I2C device slave address */
#define MAX_NUM_DISPLAY           (2u)  /*!< Maximum number of display */
#define LED_DRIVER_RAM_SIZE      (17u)  /*!< First byte is RAM adddress pointer, which is 0, followed by 16 RAM locations */
#define ANIM1_SEQ_LEN             (2u)  /*!< Animation1 Sequence length */
#define ANIM2_SEQ_LEN             (2u)  /*!< Animation2 Sequence length */

/*================================ DATA TYPES ===============================*/
typedef void *DISPLAY_HANDLE;

typedef enum DISPLAY_MODE
{
    DISP_MODE_STATIC,              /*!< Display Sensor Data */
    DISP_MODE_ANIMATION_1,         /*!< Display Animation1 */
    DISP_MODE_ANIMATION_2          /*!< Display Animation2 */
} DISPLAY_MODE;

typedef struct DEMO_SEGDISP_DATA
{
    ADI_EAL_I2C_HANDLE  hI2cInfHandle;  /*!< The I2C EAL handle */
    DISPLAY_MODE        eMode;          /*!< Display modes */
    uint64_t            nPrevWriteTime; /*!< Track the previous write time */
    uint8_t             nAnim1Seq;      /*!< Animation1 Sequence */
    uint8_t             nAnim2Seq;      /*!< Animation2 Sequence */
    float               fNumOutput;     /*!< Display Sensor output values */
} DEMO_SEGDISP_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
/* 14 Segment display functions */
extern void demo_displayInit(ADI_EAL_I2C_HANDLE hI2cHandle, DISPLAY_HANDLE *hDispHandle);
extern void demo_displayClear(DISPLAY_HANDLE hDispHandle);
extern void demo_displaySetMode(DISPLAY_HANDLE hDispHandle, DISPLAY_MODE eMode);
extern void demo_displayWriteNum(DISPLAY_HANDLE hDispHandle, float fNum);
extern void demo_displayUpdateNumOutput(DISPLAY_HANDLE hDispHandle, float fNumOutput);
extern void demo_displayRun(DISPLAY_HANDLE hDispHandle);

#endif /* ADI_E2B_IFACE_I2C_ENABLED */
#endif /* DEMO_14SEGDISP_H */
