/*******************************************************************************
Copyright (c) 2024-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: fioOsp.c
 * \brief:The header file for Flexible IO OSP functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */

 /*================================= INCLUDES ================================*/
#include "fioOsp.h"        /* OSP header file */
#include "adi_network.h"

/*================================= DEFINES =================================*/
#define FIO_OSP_PREAMBLE                   0xA0U /* Preamble for FIO OSP */
#define FIO_OSP_RESET_MSG_LENGTH           4U    /* Length of reset message */
#define FIO_OSP_INIT_MSG_LENGTH            4U    /* Length of init message */
#define FIO_OSP_INIT_RSP_MSG_LENGTH        6U    /* Length of init response message */
#define FIO_OSP_SET_STATE_MSG_LENGTH       4U    /* Length of go active / go sleep / go deep sleep message */
#define FIO_OSP_SET_SETUP_MSG_LENGTH       5U    /* Length of set setup message */
#define FIO_OSP_READ_SETUP_MSG_LENGTH      4U    /* Length of read setup message */
#define FIO_OSP_READ_SETUP_RSP_MSG_LENGTH  5U    /* Length of read setup response message */
#define FIO_OSP_PWM_DATA_MSG_LENGTH        10U   /* Length of PWM data message */
#define FIO_OSP_READ_PWM_MSG_LENGTH        4U    /* Length of read PWM message */
#define FIO_OSP_READ_PWM_RSP_MSG_LENGTH    10U   /* Length of read PWM response message */
#define FIO_OSP_READ_STATUS_MSG_LENGTH     4U    /* Length of the read status message */
#define FIO_OSP_READ_STATUS_RSP_MSG_LENGTH 5U   /* Length of the read status response message */
#define FIO_OSP_READ_TEMP_MSG_LENGTH       4u
#define FIO_OSP_READ_TEMP_RSP_MSG_LENGTH   5U    /* Length of read temperature response message */
#define MAX_ADDRESS                        1002U /* Maximum address */
/*================================ DATA TYPES ===============================*/

/*============================ PROTOTYPES ============================*/
static void FillHeader(uint8_t* pBuffer, uint16_t nDeviceAddress, uint8_t nMsgLength, ADI_FIO_OSP_CMDS eFioOspCmd);
static uint8_t FillCrc(uint8_t* pBuffer, uint16_t nMsgLength);
/*============================== CODE ==============================*/
/**
 * @brief Sends the reset command to the device, referred by the device address, synchronously
 *
 * @param [in] hFioOspHandle   Handle to the FIO OSP instance
 * @param [in] nDeviceAddress  Device address (set 0 for broadcast)
 * @param [in] nId             Identifier for the write request
 * @return     Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspResetCmd(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS, "Invalid device address");

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_RESET_MSG_LENGTH]; /* Buffer for TX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_RESET_MSG_LENGTH, FIO_OSP_RESET_CMD);

    /* Calculate CRC */
    nTxData[FIO_OSP_RESET_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_RESET_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_RESET_MSG_LENGTH;

    ADI_EAL_STATUS eEalStatus;
    if (nId == 0xFFU)
    {
        eEalStatus = adi_eal_fioOspWrite(hFioOspHandle, &oFioOspData, nId);
        if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            adi_network_flushTxBuffer(0U, 0ULL);
        }
    }
    else
    {
        eEalStatus = adi_eal_fioOspWriteSync(hFioOspHandle, &oFioOspData, nId);
    }

    return eEalStatus;
}

/**
 * @brief Sends Init_bidir command, synchronously (always with device address as 1 in downstream)
 *
 * @param [in]  hFioOspHandle    Handle to the FIO OSP instance
 * @param [out] pnDeviceAddress  Pointer to device address returned by in device in upstream direction
 * @param [in]  nId              Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspInitBidir(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t* pnDeviceAddress, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(pnDeviceAddress != NULL, "Invalid device address pointer");

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_INIT_MSG_LENGTH]; /* Buffer for TX data */
    uint8_t nRxData[FIO_OSP_INIT_RSP_MSG_LENGTH]; /* Buffer for RX data - init response gets stored in this buffer */
    uint8_t* apRxData[1U] = { &nRxData[0U] }; /* Array of pointer to RX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], 1U, FIO_OSP_INIT_MSG_LENGTH, FIO_OSP_INIT_BIDIR_CMD);

    /* Calculate CRC */
    nTxData[FIO_OSP_INIT_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_INIT_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_INIT_MSG_LENGTH;
    oFioOspData.nReadLength = FIO_OSP_INIT_RSP_MSG_LENGTH;
    oFioOspData.apRxData = apRxData;

    ADI_EAL_STATUS eEalStatus = adi_eal_fioOspWriteReadSync(hFioOspHandle, &oFioOspData, nId);

    return eEalStatus;
}

/**
 * @brief Sends GO_ACTIVE / GO_SLEEP / GO_DEEP_SLEEP command to the device referred by the device address, synchronously
 *
 * @param [in] hFioOspHandle     Handle to the FIO OSP instance
 * @param [in] nDeviceAddress    Device address (set 0 for broadcast)
 * @param [in] eSetDeviceState   Device state to be set (Active / Sleep / Deep Sleep)
 * @param [in] nId               Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspSetState(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    ADI_FIO_OSP_SET_STATE eSetDeviceState, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS, "Invalid device address");

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_SET_STATE_MSG_LENGTH]; /* Buffer for TX data */

    /* Set the command depending on the set state input */
    ADI_FIO_OSP_CMDS eFioOspCmd;

    if (eSetDeviceState == FIO_OSP_SET_STATE_SLEEP)
    {
        /* Set the command to go sleep */
        eFioOspCmd = FIO_OSP_GO_SLEEP_CMD;
    }
    else if (eSetDeviceState == FIO_OSP_SET_STATE_DEEP_SLEEP)
    {
        /* Set the command to go deep sleep */
        eFioOspCmd = FIO_OSP_GO_DEEP_SLEEP_CMD;
    }
    else
    {
        /* Set the command to go active */
        eFioOspCmd = FIO_OSP_GO_ACTIVE_CMD;
    }

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_SET_STATE_MSG_LENGTH, eFioOspCmd);

    /* Calculate CRC */
    nTxData[FIO_OSP_SET_STATE_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_SET_STATE_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_SET_STATE_MSG_LENGTH;

    ADI_EAL_STATUS eEalStatus;
    if (nId == 0xFFU)
    {
        eEalStatus = adi_eal_fioOspWrite(hFioOspHandle, &oFioOspData, nId);
        if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            adi_network_flushTxBuffer(0U, 0ULL);
        }
    }
    else
    {
        eEalStatus = adi_eal_fioOspWriteSync(hFioOspHandle, &oFioOspData, nId);
    }

    return eEalStatus;
}

/**
 * @brief Sends the SET_SETUP command (setup register data) to the device referred by the device address, synchronously
 *
 * @param [in] hFioOspHandle    Handle to the FIO OSP instance
 * @param [in] nDeviceAddress   Device address (set 0 for broadcast)
 * @param [in] poSetupReg       Pointer to the setup register data
 * @param [in] nId              Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspSetSetup(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    ADI_FIO_OSP_SETUP_REG* poSetupReg, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS, "Invalid device address");
    ADI_DBG_REQUIRE(poSetupReg != NULL, "Invalid setup register pointer");

    /* Message on bus : Preamble - Device address - Cmd - PSI - Payload (having setup register data) - CRC */
    uint8_t nTxData[FIO_OSP_SET_SETUP_MSG_LENGTH]; /* Buffer for TX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_SET_SETUP_MSG_LENGTH, FIO_OSP_SET_SETUP_CMD);

    /* Fill the payload with setup register data */
    nTxData[3U] = ((uint8_t)poSetupReg->ePwmFreq << 7U) | ((uint8_t)poSetupReg->bClkInv << 6U) |
        ((uint8_t)poSetupReg->bCrcEnable << 5U) | ((uint8_t)poSetupReg->eTempUpdateRate << 4U) |
        ((uint8_t)poSetupReg->bShouldDevSleeponComErr << 3U) | ((uint8_t)poSetupReg->bShouldDevSleeponLOSErr << 2U) |
        ((uint8_t)poSetupReg->bShouldDevSleeponOverTemperature << 1U) | ((uint8_t)poSetupReg->bShouldDevSleeponUnderVoltage);

    /* Calculate CRC */
    nTxData[FIO_OSP_SET_SETUP_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_SET_SETUP_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_SET_SETUP_MSG_LENGTH;

    ADI_EAL_STATUS eEalStatus = adi_eal_fioOspWriteSync(hFioOspHandle, &oFioOspData, nId);

    return eEalStatus;
}

/**
 * @brief Sends READ_SETUP command to the device referred by the device address, synchronously
 *
 * @param [in]  hFioOspHandle    Handle to the FIO OSP instance
 * @param [in]  nDeviceAddress   Device address (cannot send broadcast)
 * @param [out] poSetupReg       Pointer to the setup register data, response in upstream is stored in this structure
 * @param [in]  nId              Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspReadSetup(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    ADI_FIO_OSP_SETUP_REG* poSetupReg, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS && nDeviceAddress != 0U, "Invalid device address");
    ADI_DBG_REQUIRE(poSetupReg != NULL, "Invalid setup register pointer");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_READ_SETUP_MSG_LENGTH]; /* Buffer for TX data - request to read setup register */
    uint8_t nRxData[FIO_OSP_READ_SETUP_RSP_MSG_LENGTH]; /* Buffer for RX data - response for setup register read gets stored in this buffer */
    uint8_t* apRxData[1U] = { &nRxData[0U] }; /* Array of pointer to RX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_READ_SETUP_MSG_LENGTH, FIO_OSP_READ_SETUP_CMD);

    /* Calculate CRC */
    nTxData[FIO_OSP_READ_SETUP_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_READ_SETUP_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_READ_SETUP_MSG_LENGTH;
    oFioOspData.nReadLength = FIO_OSP_READ_SETUP_RSP_MSG_LENGTH;
    oFioOspData.apRxData = apRxData;

    eEalStatus = adi_eal_fioOspWriteReadSync(hFioOspHandle, &oFioOspData, nId);

    if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Extract the setup register data from the response */
        poSetupReg->ePwmFreq = (ADI_FIO_OSP_PWM_FREQ)((nRxData[3U] & 0x80U) >> 7U);
        poSetupReg->bClkInv = (bool)((nRxData[3U] & 0x40U) >> 6U);
        poSetupReg->bCrcEnable = (bool)((nRxData[3U] & 0x20U) >> 5U);
        poSetupReg->eTempUpdateRate = (ADI_FIO_OSP_TEMP_UPDATE_RATE)((nRxData[3U] & 0x10U) >> 4U);
        poSetupReg->bShouldDevSleeponComErr = (bool)((nRxData[3U] & 0x08U) >> 3U);
        poSetupReg->bShouldDevSleeponLOSErr = (bool)((nRxData[3U] & 0x04U) >> 2U);
        poSetupReg->bShouldDevSleeponOverTemperature = (bool)((nRxData[3U] & 0x02U) >> 1U);
        poSetupReg->bShouldDevSleeponUnderVoltage = (bool)(nRxData[3U] & 0x01U);
    }

    return eEalStatus;
}

/**
 * @brief Sends SET_PWM command to the device referred by the device address, synchronously
 *
 * @param [in] hFioOspHandle    Handle to the FIO OSP instance
 * @param [in] nDeviceAddress   Device address (set 0 for broadcast)
 * @param [in] poPwmData        Pointer to the PWM data
 * @param [in] nId              Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspSetPwm(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    ADI_FIO_OSP_PWM_DATA* poPwmData, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS, "Invalid device address");
    ADI_DBG_REQUIRE(poPwmData != NULL, "Invalid PWM data");

    /* Message on bus : Preamble - Device address - Cmd - PSI - Payload (having PWM data) - CRC */
    uint8_t nTxData[FIO_OSP_PWM_DATA_MSG_LENGTH]; /* Buffer for TX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_PWM_DATA_MSG_LENGTH, FIO_OSP_SET_PWM_CMD);

    /* Fill the payload with PWM data */
    /*
    nTxData[3U] = poPwmData->nRedPwm & 0xFFU;
    nTxData[4U] = ((poPwmData->nRedPwm & 0x7F00U) >> 8U) | (poPwmData->bRedDayMode == true ? 0x80U : 0U);
    nTxData[5U] = poPwmData->nGreenPwm & 0xFFU;
    nTxData[6U] = ((poPwmData->nGreenPwm & 0x7F00U) >> 8U) | (poPwmData->bGreenDayMode == true ? 0x80U : 0U);
    nTxData[7U] = poPwmData->nBluePwm & 0xFFU;
    nTxData[8U] = ((poPwmData->nBluePwm & 0x7F00U) >> 8U) | (poPwmData->bBlueDayMode == true ? 0x80U : 0U);
    */
    nTxData[3U] = ((poPwmData->nRedPwm & 0x7F00U) >> 8U) | (poPwmData->bRedDayMode == true ? 0x80U : 0U);
    nTxData[4U] = poPwmData->nRedPwm & 0xFFU;
    nTxData[5U] = ((poPwmData->nGreenPwm & 0x7F00U) >> 8U) | (poPwmData->bGreenDayMode == true ? 0x80U : 0U);
    nTxData[6U] = poPwmData->nGreenPwm & 0xFFU;
    nTxData[7U] = ((poPwmData->nBluePwm & 0x7F00U) >> 8U) | (poPwmData->bBlueDayMode == true ? 0x80U : 0U);
    nTxData[8U] = poPwmData->nBluePwm & 0xFFU;

    /* Calculate CRC */
    nTxData[FIO_OSP_PWM_DATA_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_PWM_DATA_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_PWM_DATA_MSG_LENGTH;

    ADI_EAL_STATUS eEalStatus;
    if (nId == 0xFFU)
    {
        eEalStatus = adi_eal_fioOspWrite(hFioOspHandle, &oFioOspData, nId);
        if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            adi_network_flushTxBuffer(0U, 0ULL);
        }
    }
    else
    {
        eEalStatus = adi_eal_fioOspWriteSync(hFioOspHandle, &oFioOspData, nId);
    }

    return eEalStatus;
}

/**
 * @brief Send READ_PWM command to the device referred by the device address, synchronously
 *
 * @param hFioOspHandle    Handle to the FIO OSP instance
 * @param nDeviceAddress   Device address (cannot send broadcast)
 * @param poPwmData        Pointer to the pwm read data, response in upstream is stored in this structure
 * @param nId              Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 */
ADI_EAL_STATUS adi_fioOspReadPwm(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    ADI_FIO_OSP_PWM_DATA* poPwmData, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS && nDeviceAddress != 0U, "Invalid device address");
    ADI_DBG_REQUIRE(poPwmData != NULL, "Invalid PWM data");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_READ_PWM_MSG_LENGTH]; /* Buffer for TX data - request to read pwm register */
    uint8_t nRxData[FIO_OSP_READ_PWM_RSP_MSG_LENGTH]; /* Buffer for RX data - response for pwm register read gets stored in this buffer */
    uint8_t* apRxData[1U] = { &nRxData[0U] }; /* Array of pointer to RX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_READ_PWM_MSG_LENGTH, FIO_OSP_READ_PWM_CMD);

    /* Calculate CRC */
    nTxData[FIO_OSP_READ_PWM_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_READ_PWM_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_READ_PWM_MSG_LENGTH;
    oFioOspData.nReadLength = FIO_OSP_READ_PWM_RSP_MSG_LENGTH;
    oFioOspData.apRxData = apRxData;

    eEalStatus = adi_eal_fioOspWriteReadSync(hFioOspHandle, &oFioOspData, nId);

    if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Extract the PWM data from the response */
        poPwmData->nBluePwm = (uint16_t)nRxData[7U] | ((uint16_t)(nRxData[8U] & 0x7FU) << 8U);
        poPwmData->bBlueDayMode = (bool)((nRxData[8U] & 0x80U) >> 7U);
        poPwmData->nGreenPwm = (uint16_t)nRxData[5U] | ((uint16_t)(nRxData[6U] & 0x7FU) << 8U);
        poPwmData->bGreenDayMode = (bool)((nRxData[6U] & 0x80U) >> 7U);
        poPwmData->nRedPwm = (uint16_t)nRxData[3U] | ((uint16_t)(nRxData[4U] & 0x7FU) << 8U);
        poPwmData->bRedDayMode = (bool)((nRxData[4U] & 0x80U) >> 7U);
    }

    return eEalStatus;
}

ADI_EAL_STATUS adi_fioOspReadStatus(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    ADI_FIO_OSP_STATUS_REG* oStatusData, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS && nDeviceAddress != 0U, "Invalid device address");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_READ_STATUS_MSG_LENGTH]; /* Buffer for TX data - request to read pwm register */
    uint8_t nRxData[FIO_OSP_READ_STATUS_RSP_MSG_LENGTH]; /* Buffer for RX data - response for pwm register read gets stored in this buffer */
    uint8_t* apRxData[1U] = { &nRxData[0U] }; /* Array of pointer to RX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_READ_STATUS_MSG_LENGTH, FIO_OSP_READ_STATUS_CMD);

    /* Calculate CRC */
    nTxData[FIO_OSP_READ_STATUS_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_READ_STATUS_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_READ_STATUS_MSG_LENGTH;
    oFioOspData.nReadLength = FIO_OSP_READ_STATUS_RSP_MSG_LENGTH;
    oFioOspData.apRxData = apRxData;

    eEalStatus = adi_eal_fioOspWriteReadSync(hFioOspHandle, &oFioOspData, nId);

    if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        oStatusData->nState = ((nRxData[3u] & 0xC0) >> 6u);
        oStatusData->nOtpCrc = ((nRxData[3u] & 0x20) >> 5u);
        oStatusData->nCom = ((nRxData[3u] & 0x10) >> 4u);
        oStatusData->nCe_flag = ((nRxData[3u] & 0x8) >> 3u);
        oStatusData->nLos_flag = ((nRxData[3u] & 0x4) >> 2u);
        oStatusData->nOt_flag = ((nRxData[3u] & 0x2) >> 1u);
        oStatusData->nUv_flag = ((nRxData[3u] & 0x1));
    }

    return eEalStatus;
}

ADI_EAL_STATUS adi_fioOspReadTemp(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, uint16_t nDeviceAddress,
    double* dTemperature, uint8_t nId)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(hFioOspHandle != NULL, "Invalid FIO OSP handle");
    ADI_DBG_REQUIRE(nDeviceAddress <= MAX_ADDRESS && nDeviceAddress != 0U, "Invalid device address");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Message on bus : Preamble - Device address - Cmd - PSI - CRC */
    uint8_t nTxData[FIO_OSP_READ_TEMP_MSG_LENGTH]; /* Buffer for TX data - request to read pwm register */
    uint8_t nRxData[FIO_OSP_READ_TEMP_RSP_MSG_LENGTH]; /* Buffer for RX data - response for pwm register read gets stored in this buffer */
    uint8_t* apRxData[1U] = { &nRxData[0U] }; /* Array of pointer to RX data */

    /* Fill the header for message i.e Preamble - Device address - Cmd - PSI */
    FillHeader(&nTxData[0U], nDeviceAddress, FIO_OSP_READ_TEMP_MSG_LENGTH, FIO_OSP_READ_TEMP_CMD);

    /* Calculate CRC */
    /* [수정] FIO_OSP_READ_PWM_MSG_LENGTH → FIO_OSP_READ_TEMP_MSG_LENGTH 로 수정 */
    nTxData[FIO_OSP_READ_TEMP_MSG_LENGTH - 1U] = FillCrc(&nTxData[0U], FIO_OSP_READ_TEMP_MSG_LENGTH);

    /* Send the message */
    ADI_EAL_FIO_OSP_IO_DATA oFioOspData;
    (void)memset(&oFioOspData, 0U, sizeof(oFioOspData));
    oFioOspData.pTxData = &nTxData[0U];
    oFioOspData.nWriteLength = FIO_OSP_READ_TEMP_MSG_LENGTH;
    oFioOspData.nReadLength = FIO_OSP_READ_TEMP_RSP_MSG_LENGTH;
    oFioOspData.apRxData = apRxData;

    eEalStatus = adi_eal_fioOspWriteReadSync(hFioOspHandle, &oFioOspData, nId);

    if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        dTemperature[0u] = 1.08 * (uint8_t)nRxData[3u] - 126.0;
    }

    return eEalStatus;
}

/*============================== STATIC CODE ==============================*/
static void FillHeader(uint8_t* pBuffer, uint16_t nDeviceAddress, uint8_t nMsgLength, ADI_FIO_OSP_CMDS eFioOspCmd)
{
    /* Fill the preamble */
    pBuffer[0U] = FIO_OSP_PREAMBLE;

    /* Fill the device address */
    pBuffer[0U] |= (uint8_t)((nDeviceAddress & 0x3C0U) >> 6U);
    pBuffer[1U] = (uint8_t)((nDeviceAddress & 0x3FU) << 2U);

    /* Fill the PSI */
    uint8_t nPSI = nMsgLength == 12U ? 7U : (nMsgLength - 4U);
    pBuffer[1U] |= ((nPSI & 0x6U) >> 1U);
    pBuffer[2U] = (nPSI & 0x1U) << 7U;

    /* Fill the command */
    pBuffer[2U] |= (uint8_t)eFioOspCmd;
}

static uint8_t FillCrc(uint8_t* pBuffer, uint16_t nMsgLength)
{
    /* Calculate CRC */
    uint8_t nCrc = 0x00U;
    uint8_t nExtract;

    for (uint8_t i = 0; i < nMsgLength - 1U; i++)
    {
        nExtract = *pBuffer;

        for (uint8_t mask = 8U; mask > 0U; mask--)
        {
            if (((nExtract >> (mask - 1)) & 0x01U) == ((nCrc >> 7) & 0x01U))
            {
                nCrc = nCrc << 1;
            }
            else
            {
                nCrc = (nCrc << 1) ^ 0x2FU;
            }

            nCrc &= 0xFFU;
        }

        pBuffer++;
    }

    return nCrc;
}