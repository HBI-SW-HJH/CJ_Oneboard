/*******************************************************************************
Copyright (c) 2024-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: fioOsp.h
 * \brief:The header file for Flexible IO OSP functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */

#ifndef ADI_FIOOSP_H
#define ADI_FIOOSP_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"         /* The EAL header */
#include "adi_eal_fioOsp.h"  /*!< E2B FIO OSP abstraction layer */

/*================================= DEFINES =================================*/

#define COLOR_COUNT_MAX 7

/*================================ DATA TYPES ===============================*/
/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct ADI_FIO_OSP_PWM_DATA
{
    uint16_t nBluePwm;       /**< blue PWM value */
    bool     bBlueDayMode;   /**< blue peak current - day or night mode */
    uint16_t nGreenPwm;      /**< green PWM value */
    bool     bGreenDayMode;  /**< green peak current - day or night mode */
    uint16_t nRedPwm;        /**< red PWM value */
    bool     bRedDayMode;    /**< red peak current - day or night mode */
} ADI_FIO_OSP_PWM_DATA;

typedef enum ADI_FIO_OSP_PWM_FREQ
{
    ADI_FIO_OSP_PWM_FREQ_586HZ = 0U,
    ADI_FIO_OSP_PWM_FREQ_1172HZ = 1U
} ADI_FIO_OSP_PWM_FREQ;

typedef enum ADI_FIO_OSP_TEMP_UPDATE_RATE
{
    ADI_FIO_OSP_TEMP_UPDATE_RATE_19_2_KHZ = 0U, /**< 19.2 KHz */
    ADI_FIO_OSP_TEMP_UPDATE_RATE_2_4_KHZ = 1U, /**< 2.4 KHz */
} ADI_FIO_OSP_TEMP_UPDATE_RATE;

typedef struct ADI_FIO_OSP_SETUP_REG
{
    ADI_FIO_OSP_PWM_FREQ ePwmFreq;                    /**< PWM frequency */
    bool bClkInv;                                     /**< Clock inversion */
    bool bCrcEnable;                                  /**< CRC enable */
    ADI_FIO_OSP_TEMP_UPDATE_RATE eTempUpdateRate;     /**< Temperature update rate */
    bool bShouldDevSleeponComErr;                     /**< Device sleep on communication error */
    bool bShouldDevSleeponLOSErr;                     /**< Device sleep on open/short connection */
    bool bShouldDevSleeponOverTemperature;            /**< Device sleep on over temperature */
    bool bShouldDevSleeponUnderVoltage;               /**< Device sleep on Under voltage */
} ADI_FIO_OSP_SETUP_REG;

typedef struct ADI_FIO_OSP_STATUS_REG
{
    uint8_t nState;         /**< Device State */
    bool nOtpCrc;           /**< OTP error flag */
    bool nCom;              /**< Communication direction */
    bool nCe_flag;       /**< Communication fault flag */
    bool nLos_flag;         /**< LED fault flag */
    bool nOt_flag;          /**< Overtemperature fault flag */
    bool nUv_flag;          /**< Undervoltage fault flag */
} ADI_FIO_OSP_STATUS_REG;

typedef struct ADI_FIO_OSP_CMD
{
    uint16_t inDeviceAddress;     /**< INPUT: device address*/
    uint8_t inCmdId;              /**< INPUT: OSP command identifier*/
    void *p_inParameter;          /**< INPUT: pointer to parameter structure*/
    uint8_t *p_outCmdBuffer;      /**< OUTPUT: buffer with requested OSP sequence*/
    uint8_t outCmdBufferLength;   /**< OUTPUT: length of requested OSP sequence*/
    uint8_t outResponseLength;    /**< OUTPUT: length of the expected response*/
    bool boutResponseMsg;         /**< OUTPUT: true if a response id expected*/
} ADI_FIO_OSP_CMD;

typedef enum ADI_FIO_OSP_CMDS
{
    FIO_OSP_RESET_CMD = 0x0U,
    FIO_OSP_INIT_BIDIR_CMD  = 0x2U,
    FIO_OSP_GO_SLEEP_CMD = 0x4U,
    FIO_OSP_GO_ACTIVE_CMD = 0x5U,
    FIO_OSP_GO_DEEP_SLEEP_CMD = 0x6U,
    FIO_OSP_READ_STATUS_CMD = 0x40U,
    FIO_OSP_READ_TEMP_CMD = 0x48U,
    FIO_OSP_READ_SETUP_CMD = 0x4CU,
    FIO_OSP_SET_SETUP_CMD = 0x4DU,
    FIO_OSP_READ_PWM_CMD = 0x4EU,
    FIO_OSP_SET_PWM_CMD = 0x4FU,
} ADI_FIO_OSP_CMDS;


typedef enum ADI_FIO_OSP_SET_STATE
{
    FIO_OSP_SET_STATE_SLEEP = 4,
    FIO_OSP_SET_STATE_ACTIVE = 5,
    FIO_OSP_SET_STATE_DEEP_SLEEP = 6,
} ADI_FIO_OSP_SET_STATE;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_fioOspResetCmd(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
        uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspInitBidir(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t *pnDeviceAddress, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspSetState(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
        ADI_FIO_OSP_SET_STATE eSetDeviceState, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspSetSetup(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
        ADI_FIO_OSP_SETUP_REG *poSetupReg, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspReadSetup(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
        ADI_FIO_OSP_SETUP_REG *poSetupReg, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspSetPwm(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
                                       ADI_FIO_OSP_PWM_DATA *poPwmData, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspReadPwm(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
                                        ADI_FIO_OSP_PWM_DATA *poPwmData, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspReadStatus(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
                                        ADI_FIO_OSP_STATUS_REG *oStatusData, uint8_t nId);
extern ADI_EAL_STATUS adi_fioOspReadTemp(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
                                        double *dTemperature, uint8_t nId);

#endif /* ADI_FIOUOSP_H */

/** @} */

/**
 * EOF: www.analog.com
 */
