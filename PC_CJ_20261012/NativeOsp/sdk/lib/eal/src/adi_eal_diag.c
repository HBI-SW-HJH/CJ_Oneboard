/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_diag.c
 * @brief: The EAL APIs for diagnostics
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_DIAG_ENABLED

/*! \cond PRIVATE */
// ---------------------- DATA TYPES ----------------------

// ------------------------- DATA -------------------------
/// TODO: (Post 2.0.0) Change these to dynamic memory assignment
/** The internal data structure for diag component */
ADI_EAL_DIAG_DATA
static ADI_EAL_DIAG_CFG gaoDiagCfg[ADI_E2BCORE_MAX_INST];
/*! \endcond */
/*=================================== CODE ==================================*/
/*! \cond PRIVATE */
// --------------------- PRIVATE CODE ---------------------

/*! \endcond */
// --------------------- PUBLIC CODE ---------------------
/**
 * @brief       Initializes the diag component
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  poDiagCfg           Configurations for diag component
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS   Diag component successfully initialized
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_DIAG_CODE
ADI_EAL_STATUS adi_eal_diagInit(uint32_t nInstNum, ADI_EAL_DIAG_CFG *poDiagCfg)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(poDiagCfg != NULL, "Diagnostic config param is NULL");
    ADI_DBG_REQUIRE(poDiagCfg->pfDiagCallback != NULL, "Diagnostic callback function pointer is NULL");

    // Clear the instance memory
    (void) ADI_MEMSET(&gaoDiagCfg[nInstNum], 0, sizeof(ADI_EAL_DIAG_CFG));

    // Save the configurations
    gaoDiagCfg[nInstNum].pfDiagCallback = poDiagCfg->pfDiagCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Get remote node's PLCA statistics
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_DIAG_CODE
ADI_EAL_STATUS adi_eal_diagGetPlcaStatus(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // ~~~~~ Register map message configuration ~~~~~
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    // Read the PHY status registers
    uint16_t anRegAddr[1U] = { (uint16_t)PHY_OA_PLCA_CTRL1_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 5U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    // Send the register map message
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &aoRegMapCfg[0U]);
}

/**
 * @brief       Get remote node's PHY statistics
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_DIAG_CODE
ADI_EAL_STATUS adi_eal_diagGetPhyStatus(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // ~~~~~ Register map message configuration ~~~~~
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    // Read the PHY statistics registers
    uint16_t anRegAddr[1U] = { (uint16_t)PHY_STD_STATUS_REGISTER_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    // Send the register map message
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &aoRegMapCfg[0U]);
}

/**
 * @brief       Get remote node's PCS statistics
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_DIAG_CODE
ADI_EAL_STATUS adi_eal_diagGetPcsStatus(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // ~~~~~ Register map message configuration ~~~~~
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    // Read the MAC statistics registers
    uint16_t anRegAddr[1U] = { (uint16_t)PHY_STD_PCS_STATUS_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 3U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    // Send the register map message
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &aoRegMapCfg[0U]);
}

/**
 * @brief       Get remote node's MAC statistics
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_DIAG_CODE
ADI_EAL_STATUS adi_eal_diagGetMacStatus(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // ~~~~~ Register map message configuration ~~~~~
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    // Read the MAC statistics registers
    uint16_t anRegAddr[1U] = { (uint16_t)MAC_RX_FRM_CNT_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = NUM_MAC_STATUS_REG;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    // Send the register map message
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &aoRegMapCfg[0U]);
}

/**
 * @brief       Get remote node's gPTP statistics
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_DIAG_CODE
ADI_EAL_STATUS adi_eal_diagGetPtpStatus(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // ~~~~~ Register map message configuration ~~~~~
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    // Read the PTP registers
    uint16_t anRegAddr[1U] = { (uint16_t)IO_GPTP_SYNC_COUNT_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 21U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    // Send the register map message
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &aoRegMapCfg[0U]);
}

/**
 * @brief       Log FIFO status
 *
 * @param [in]  poStatusData      FIFO status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogFifoStatus(ADI_EAL_FIFO_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* =============================== FIFO STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " FIFO status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO empty status           : %d\r\n",
                poStatusData->bTxFifoThresholdEmpty);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO has threshold 1 bytes  : %d\r\n",
                poStatusData->bTxFifoThreshold1);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO has threshold 2 bytes  : %d\r\n",
                poStatusData->bTxFifoThreshold2);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO has threshold 3 bytes  : %d\r\n",
                poStatusData->bTxFifoThreshold3);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO full status            : %d\r\n",
                poStatusData->bTxFifoThresholdFull);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO empty status           : %d\r\n",
                poStatusData->bRxFifoThresholdEmpty);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO has threshold 1 bytes  : %d\r\n",
                poStatusData->bRxFifoThreshold1);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO has threshold 2 bytes  : %d\r\n",
                poStatusData->bRxFifoThreshold2);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO has threshold 3 bytes  : %d\r\n",
                poStatusData->bRxFifoThreshold3);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO full status            : %d\r\n",
                poStatusData->bRxFifoThresholdFull);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO read bytes             : %d\r\n",
                poStatusData->nRxFifoReadBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO write bytes            : %d\r\n",
                poStatusData->nRxFifoWriteBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO read bytes             : %d\r\n",
                poStatusData->nTxFifoReadBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO write bytes            : %d\r\n",
                poStatusData->nTxFifoWriteBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX FIFO level (in bytes)       : %d\r\n",
                poStatusData->nTxFifoLevel);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX FIFO level (in bytes)       : %d\r\n",
                poStatusData->nRxFifoLevel);
    /* =========================================================================== */
}

#ifdef ADI_E2B_IFACE_SPI_ENABLED
/**
 * @brief       Log SPI status
 *
 * @param [in]  poStatusData      SPI status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogSpiStatus(ADI_EAL_SPI_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* ================================ SPI STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " SPI status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSPI bus busy status                      : " "%d" "\r\n",
                poStatusData->bBusBusy);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes read from the MISO line  : " "%d" "\r\n",
                poStatusData->nNumRdBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of SPI transactions processed     : " "%d" "\r\n",
                poStatusData->nNumSpiTxns);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes written on the MOSI line : " UINT32_FORMATTER "\r\n",
                poStatusData->nNumWrBytes);
}
#endif /* ADI_E2B_IFACE_SPI_ENABLED */

#ifdef ADI_E2B_IFACE_I2C_ENABLED
/**
 * @brief       Log I2C status
 *
 * @param [in]  poStatusData      I2C status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogI2cStatus(ADI_EAL_I2C_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* ================================ I2C STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " I2C status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tI2C bus busy status                    : " "%d" "\r\n",
                poStatusData->bBusBusy);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tDevice address not acknowledged        : " "%d" "\r\n",
                poStatusData->bAddrNack);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tData not acknowledged                  : " "%d" "\r\n",
                poStatusData->bDataNack);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes read from the I2C bus  : " "%d" "\r\n",
                poStatusData->nNumRdBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of I2C transactions processed   : " "%d" "\r\n",
                poStatusData->nNumI2cTxns);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes written on the I2C bus : " UINT32_FORMATTER "\r\n",
                poStatusData->nNumWrBytes);
}
#endif /* ADI_E2B_IFACE_I2C_ENABLED */

#ifdef ADI_E2B_IFACE_UART_ENABLED
/**
 * @brief       Log UART status
 *
 * @param [in]  poStatusData      UART status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogUartStatus(ADI_EAL_UART_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* =============================== UART STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " UART status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes written by the UART interface : %d\r\n",
                poStatusData->nNumTxBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes read by the UART interface    : %d\r\n",
                poStatusData->nNumRxBytes);
}
#endif /* ADI_E2B_IFACE_UART_ENABLED */

#ifdef ADI_E2B_IFACE_PORT_ENABLED
/**
 * @brief       Log Port controller status
 *
 * @param [in]  poStatusData      Port controller status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogPortCtrlStatus(ADI_EAL_PORTCTRL_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* =============================== PORT STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " Port Ctrl status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tActive output : %d\r\n",
                poStatusData->nActiveOutput);
}
#endif /* ADI_E2B_IFACE_PORT_ENABLED */

#ifdef ADI_E2B_IFACE_ADC_ENABLED
/**
 * @brief       Log ADC status
 *
 * @param [in]  poStatusData    ADC status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogAdcStatus(ADI_EAL_ADC_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* ================================ ADC STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ADC status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of samples captured : %d\r\n", poStatusData->nNumSamples);
}
#endif /* ADI_E2B_IFACE_ADC_ENABLED */

#ifdef ADI_E2B_IFACE_ISELED_ENABLED
/**
 * @brief       Log ISELED status
 *
 * @param [in]  poStatusData  ISELED status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogIseledStatus(ADI_EAL_ISELED_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* =============================== ISELED STATUS ============================= */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ISELED status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of upstream transmitted packets         : %d \r\n",
                poStatusData->nNumWrPackets);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of downstream transmitted packets       : %d \r\n",
                poStatusData->nNumRdPackets);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tISELED bus busy status                         : %d \r\n",
                poStatusData->bBusBusy);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tISELED bus contention                          : %d \r\n",
                poStatusData->bContention);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tISELED receive frame EOC error                 : %d \r\n",
                poStatusData->bRcvFrameEOCError);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tISELED receive frame CRC error                 : %d \r\n",
                poStatusData->bRcvFrameCRCError);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tISELED receive frame Freq sync error           : %d \r\n",
                poStatusData->bRcvFrameFreqSyncError);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tISELED receive frame Frame sync error          : %d \r\n",
                poStatusData->bRcvFrameFrameSyncError);

}
#endif /* ADI_E2B_IFACE_ISELED_ENABLED */

#ifdef ADI_E2B_IFACE_LIN_ENABLED
/**
 * @brief       Log LIN status
 *
 * @param [in]  poStatusData  LIN status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogLinStatus(ADI_EAL_LIN_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* ================================ LIN STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " LIN status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tBreak field collision detected     : %d \r\n",
                poStatusData->bIsBrkFldCollision);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSync field collision detected      : %d \r\n",
                poStatusData->bIsSyncFldCollision);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPID field collision detected       : %d \r\n",
                poStatusData->bIsPIDFldCollision);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTransmission collision detected    : %d \r\n",
                poStatusData->bIsTransCollision);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tFrame error detected               : %d \r\n",
                poStatusData->bIsFrameErrDetect);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tFrame response too short detected  : %d \r\n",
                poStatusData->bIsFrameRespShort);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tWakeup signal detected             : %d \r\n",
                poStatusData->bIsWakeupSigDetect);
}
#endif /* ADI_E2B_IFACE_LIN_ENABLED */

#ifdef ADI_E2B_IFACE_FIO_ENABLED
/**
 * @brief       Log FlexibleIO status
 *
 * @param [in]  poStatusData  FlexibleIO status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagLogFlexibleIOStatus(ADI_EAL_FIO_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* ================================ FIO STATUS =============================== */
    /* =========================================================================== */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "FlexibleIO status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes read from the interface     : %d \r\n",
                poStatusData->nNumRdBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of bytes written to the interface      : %d \r\n",
                poStatusData->nNumWrBytes);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of transactions processed by the interface       : %d \r\n",
                poStatusData->nNumTransactions);
}
#endif /* ADI_E2B_IFACE_FIO_ENABLED */

/**
 * @brief       Extract FIFO status from register data array
 *
 * @param [in]  anData          Register data array
 * @param [in]  poStatusData    FIFO status object
 */
ADI_EAL_DIAG_CODE
void adi_eal_diagExtractFifoStatus(uint32_t *anData, ADI_EAL_FIFO_STATUS_DATA *poStatusData)
{
    /* =========================================================================== */
    /* ================================ FIFO STATUS ============================== */
    /* =========================================================================== */
    poStatusData->bTxFifoThresholdEmpty = ((anData[0U] & 0x0001U) != 0U);
    poStatusData->bTxFifoThreshold1 = ((anData[0U] & 0x0002U) != 0U);
    poStatusData->bTxFifoThreshold2 = ((anData[0U] & 0x0004U) != 0U);
    poStatusData->bTxFifoThreshold3 = ((anData[0U] & 0x0008U) != 0U);
    poStatusData->bTxFifoThresholdFull = ((anData[0U] & 0x0010U) != 0U);
    poStatusData->bRxFifoThresholdEmpty = ((anData[0U] & 0x10000U) != 0U);
    poStatusData->bRxFifoThreshold1 = ((anData[0U] & 0x20000U) != 0U);
    poStatusData->bRxFifoThreshold2 = ((anData[0U] & 0x40000U) != 0U);
    poStatusData->bRxFifoThreshold3 = ((anData[0U] & 0x80000U) != 0U);
    poStatusData->bRxFifoThresholdFull = ((anData[0U] & 0x100000U) != 0U);
    poStatusData->nRxFifoReadBytes = (uint16_t)anData[1U];
    poStatusData->nRxFifoWriteBytes = (uint16_t)((anData[1U] & 0xFFFF0000U) >> 16U);
    poStatusData->nTxFifoReadBytes = (uint16_t)anData[3U];
    poStatusData->nTxFifoWriteBytes = (uint16_t)((anData[3U] & 0xFFFF0000U) >> 16U);
    poStatusData->nRxFifoLevel = (uint16_t)anData[4U];
    poStatusData->nTxFifoLevel = (uint16_t)((anData[4U] & 0xFFFF0000U) >> 16U);
    /* =========================================================================== */
}

/**
 * @brief       This function allows the EAL top component to notify the
 *              diag component of a register read event
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  poRegDat            Pointer to register read data
 */
ADI_EAL_DIAG_CODE
void eal_notify_DiagRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
    // Get the config structure
    ADI_EAL_DIAG_CFG *poDiagCfg = &gaoDiagCfg[nInstNum];

    ADI_DBG_REQUIRE(poDiagCfg != NULL, "Diag config is not initialised");
    ADI_DBG_REQUIRE(poDiagCfg->pfDiagCallback != NULL, "Diag callback not registered");

    do
    {
        // Response for PLCA status read
        if(poRegDat->poInfo->anStartAddr[0U] == PHY_OA_PLCA_CTRL1_NW_ADDR && poRegDat->poInfo->nLength == 5U)
        {
            // Process PLCA status data
            ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS_DATA oPlcaStatusData;
            adi_common_extractPlcaStatus(poRegDat->poInfo->pBuffer, &oPlcaStatusData);
            ADI_EAL_DIAG_CALLBACK_FUNC pfEalDiagCbk = poDiagCfg->pfDiagCallback;
            pfEalDiagCbk(nInstNum, nRemoteNum, ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS, &oPlcaStatusData);
            break;
        }

        // Response for MAC status read
        if(poRegDat->poInfo->anStartAddr[0U] == MAC_RX_FRM_CNT_NW_ADDR
                && poRegDat->poInfo->nLength == NUM_MAC_STATUS_REG)
        {
            // Process MAC status data
            ADI_EAL_DIAG_CBK_EVT_MAC_STATUS_DATA oMacStatusData;
            adi_common_extractMacStatus(poRegDat->poInfo->pBuffer, &oMacStatusData);
            ADI_EAL_DIAG_CALLBACK_FUNC pfEalDiagCbk = poDiagCfg->pfDiagCallback;
            pfEalDiagCbk(nInstNum, nRemoteNum, ADI_EAL_DIAG_CBK_EVT_MAC_STATUS, &oMacStatusData);
            break;
        }

        // Response for PHY status read
        if(poRegDat->poInfo->anStartAddr[0U] == PHY_STD_STATUS_REGISTER_NW_ADDR)
        {
            // Process PHY status data
            ADI_EAL_DIAG_CBK_EVT_PHY_STATUS_DATA oPhyStatusData;
            adi_common_extractPhyStatus(poRegDat->poInfo->pBuffer, &oPhyStatusData);
            ADI_EAL_DIAG_CALLBACK_FUNC pfEalDiagCbk = poDiagCfg->pfDiagCallback;
            pfEalDiagCbk(nInstNum, nRemoteNum, ADI_EAL_DIAG_CBK_EVT_PHY_STATUS, &oPhyStatusData);
            break;
        }

        // Response for PCS status read
        if(poRegDat->poInfo->anStartAddr[0U] == PHY_STD_PCS_STATUS_NW_ADDR)
        {
            // Process PCS status data
            ADI_EAL_DIAG_CBK_EVT_PCS_STATUS_DATA oPcsStatusData;
            adi_common_extractPcsStatus(poRegDat->poInfo->pBuffer, &oPcsStatusData);
            ADI_EAL_DIAG_CALLBACK_FUNC pfEalDiagCbk = poDiagCfg->pfDiagCallback;
            pfEalDiagCbk(nInstNum, nRemoteNum, ADI_EAL_DIAG_CBK_EVT_PCS_STATUS, &oPcsStatusData);
            break;
        }

        // Response for gPTP status read
        if(poRegDat->poInfo->anStartAddr[0U] == IO_GPTP_SYNC_COUNT_NW_ADDR && poRegDat->poInfo->nLength == 21U)
        {
            // Process gPTP status data
            ADI_EAL_DIAG_CBK_EVT_PTP_STATUS_DATA oPtpStatusData;
            adi_common_extractPtpStatus(poRegDat->poInfo->pBuffer, &oPtpStatusData);
            ADI_EAL_DIAG_CALLBACK_FUNC pfEalDiagCbk = poDiagCfg->pfDiagCallback;
            pfEalDiagCbk(nInstNum, nRemoteNum, ADI_EAL_DIAG_CBK_EVT_PTP_STATUS, &oPtpStatusData);
            break;
        }
    } while(false);
}


#endif /* ADI_E2B_IFACE_DIAG_ENABLED */
/** @} */

/**
 * EOF: www.analog.com
 */
