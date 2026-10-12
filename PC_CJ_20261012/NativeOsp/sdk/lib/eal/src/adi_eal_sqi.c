/*******************************************************************************
Copyright (c) 2024-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_sqi.c
 * @brief: The EAL APIs for SQI
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_SQI_ENABLED

/*! \cond PRIVATE */

/*================================= DEFINES ================================*/

/*================================= DATA TYPES ================================*/
/* SQI interrupt state , just to make sure not to route the same interrupt to the application */
typedef enum SQI_IRQ_STATE
{
    SQI_EXITED_BAD,      /* Remote sqi exited bad level */
    SQI_ENTERED_WARNING, /* Remote sqi entered warning level */
    SQI_EXITED_WARNING,  /* Remote sqi exited warning level */
    SQI_ENTERED_BAD,     /* Remote sqi entered bad level */
} SQI_IRQ_STATE;

/* Internal structure for SQI module */
typedef struct SQI_INTERNAL_DATA
{
    ADI_EAL_SQI_CFG *poSqiCfg;           /* Stores the SQI cfg */
    SQI_IRQ_STATE nCurrRemoteIrqState[ADI_EAL_MAX_NUM_NODES]; /* Stores the current irq state of the remote node */
} SQI_INTERNAL_DATA;

ADI_EAL_SQI_DATA
static SQI_INTERNAL_DATA gaoEalSqiData[ADI_E2BCORE_MAX_INST];

/*================================ PROTOTYPES ===============================*/
static void InvokeCbk(ADI_EAL_SQI_CFG *poSQiCfg, uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SQI_CBK_EVT eCbkEvt,
                      SQI_IRQ_STATE eIrqState);

/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief Initializes the config
 *
 * @param [in] nInstNum Instance number
 * @param [in] poSqiCfg Pointer to config structure
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS            API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL      Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED       Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED    Failed to add the register map message
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SQI_CODE
ADI_EAL_STATUS adi_eal_sqiInit(uint32_t nInstNum, ADI_EAL_SQI_CFG *poSqiCfg)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(poSqiCfg != NULL, " SQI config param is NULL ");
    ADI_DBG_REQUIRE(poSqiCfg->pfSqiCallback != NULL, "SQI callback function pointer is NULL");

    // Clear the instance memory
    (void) ADI_MEMSET(&gaoEalSqiData[nInstNum], 0, sizeof(SQI_INTERNAL_DATA));

    // Save the configurations
    gaoEalSqiData[nInstNum].poSqiCfg = poSqiCfg;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Configures SQI module and enables TIC interrupts
 *
 * @param [in] nInstNum         Instance Number
 * @param [in] nRemoteNum       Remote Number
 * @param [in] poSqiCfg         Pointer to the coonfiguration settings for SQI module
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SQI_CODE
ADI_EAL_STATUS adi_eal_sqiConfig(uint32_t nInstNum, uint32_t nRemoteNum, ADI_SQI_CFG_DATA *poSqiCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    // Pre-conditions
    ADI_DBG_REQUIRE(poSqiCfg->nGoodToWarningLvl >= poSqiCfg->nWarningToGoodLvl,
                    "Warning to good threshold is greater than good to warning threshold");
    ADI_DBG_REQUIRE(poSqiCfg->nWarningToBadLvl >= poSqiCfg->nBadToWarningLvl,
                    "Bad to warning threshold is greater than warning to bad threshold");
    ADI_DBG_REQUIRE(poSqiCfg->nGoodToWarningLvl <= poSqiCfg->nBadToWarningLvl,
                    "Good to warning threshold is greater than bad to warning threshold");
    ADI_DBG_REQUIRE(poSqiCfg->eSqiSettleRange <= ADI_SQI_SETTLE_RANGE_100K, "Invalid settle range specified");

    // Enable SQI interrupts - rising and falling egde interrupts for bad and warning levels
    uint32_t nRegData = SQI_RISING_BAD_IRQ_TIC_FIELD_MASK | SQI_RISING_WARNING_IRQ_TIC_FIELD_MASK |
                        SQI_FALLING_WARNING_IRQ_TIC_FIELD_MASK | SQI_FALLING_BAD_IRQ_TIC_FIELD_MASK;
    uint16_t nStartAddr = IO_TIC_ENABLE_34_GPIO1_REG0_NW_ADDR;

    // Set the register map transaction configuration to enable SQI interrupts
    ADI_E2BCORE_REGMAP_IO_CONFIG oEnableSqiIrq;
    oEnableSqiIrq.anStartAddr = &nStartAddr;
    oEnableSqiIrq.nLength = 1U;
    oEnableSqiIrq.pBuffer = &nRegData;
    oEnableSqiIrq.anMask = &nRegData;
    oEnableSqiIrq.eType = ADI_E2BCORE_RND_MASK_WRITE;
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oEnableSqiIrq);

    // If status is OK, then proceed with configuring the SQI
    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        // SQI configurations for thresholds and PLCA TO ID's to monitor the SQI
        nStartAddr = PHY_OA_DCQ_JM_CFG0_NW_ADDR;
        uint32_t anRegData[6U] =
        {
            (uint32_t)((uint32_t)poSqiCfg->nSqiSelPlcaId << BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL),
            (uint32_t)poSqiCfg->eSqiSettleRange,
            (uint32_t)poSqiCfg->nGoodToWarningLvl,
            (uint32_t)poSqiCfg->nWarningToBadLvl,
            (uint32_t)poSqiCfg->nBadToWarningLvl,
            (uint32_t)poSqiCfg->nWarningToGoodLvl
        };

        // Set the register map transaction configuration to configure SQI
        ADI_E2BCORE_REGMAP_IO_CONFIG oSqiCfg;
        oSqiCfg.anStartAddr = &nStartAddr;
        oSqiCfg.nLength = 6U;
        oSqiCfg.pBuffer = &anRegData[0u];
        oSqiCfg.eType = ADI_E2BCORE_BURST_WRITE;

        // Send the register map transaction
        eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oSqiCfg);
    }

    return eEalStatus;
}

/**
 * @brief Gets the SQI index
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SQI_CODE
ADI_EAL_STATUS adi_eal_sqiGetIndex(uint32_t nInstNum, uint32_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Pause the jitter monitor before reading the index */
    uint16_t nStartAddr = PHY_OA_DCQ_JM_CFG0_NW_ADDR;
    uint32_t nRegData   = BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE;

    /* Set the register map configuration to pause the jitter monitor */
    ADI_E2BCORE_REGMAP_IO_CONFIG oWriteRegCfg;
    oWriteRegCfg.anStartAddr = &nStartAddr;
    oWriteRegCfg.nLength = 1U;
    oWriteRegCfg.pBuffer = &nRegData;
    oWriteRegCfg.anMask = &nRegData;
    oWriteRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the regmap transaction for pausing the jitter monitor */
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oWriteRegCfg);

    /* If status is OK, then send the regmap transaction to readback the SQI index value along with settle and valid flags */
    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Get the index value */
        uint16_t anStartAddr[2U] = {PHY_OA_DCQ_JM_RB0_NW_ADDR, PHY_OA_DCQ_JM_RB4_NW_ADDR};
        uint32_t nRegReadData[2U];

        /* Set the register map configuration to read the SQI index */
        ADI_E2BCORE_REGMAP_IO_CONFIG oReadRegCfg;
        oReadRegCfg.anStartAddr = &anStartAddr[0U];
        oReadRegCfg.nLength = 2U;
        oReadRegCfg.pBuffer = &nRegReadData[0u];
        oReadRegCfg.eType = ADI_E2BCORE_RND_READ;

        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySqi = true;

        eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oReadRegCfg);
    }

    return eEalStatus;
}

/**
 * @brief Enable/Disable SQI module
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @param [in] bEnableSqi         If true, enables SQI module
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SQI_CODE
ADI_EAL_STATUS adi_eal_sqiEnable(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnableSqi)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the SQI enable bit */
    uint16_t nStartAddr = PHY_OA_DCQ_JM_CFG0_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN;
    uint32_t nRegData = ((bEnableSqi == true ? (uint32_t)1U : (uint32_t)0U) << BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN);

    /* Do a mask write to the enable bit */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.anMask = &nMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the register write transaction */
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    return eEalStatus;
}

// ------------------------ EVENTS ------------------------
/**
 * @brief       This function allows the EAL top component to notify the
 *              SQI component of a register read event
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  poRegDat            Pointer to register read data
 */
ADI_EAL_SQI_CODE
void eal_notify_sqiRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
    ADI_EAL_SQI_CFG *poSQiCfg = gaoEalSqiData[nInstNum].poSqiCfg;

    if(poSQiCfg != NULL)
    {
        // Response for SQI status read
        if(poRegDat->poInfo->anStartAddr[0U] == PHY_OA_DCQ_JM_RB0_NW_ADDR)
        {
            // Get the SQI status and level
            ADI_EAL_SQI_CBK_EVT_SQI_STATUS_DATA oSqiStatusData;
            oSqiStatusData.bSqiValidFlag = ((poRegDat->poInfo->pBuffer[0U] & BITM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID) >>
                                            BITP_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID) == 1U ? true : false;
            oSqiStatusData.bSqiSettleFlag = ((poRegDat->poInfo->pBuffer[0U] & BITM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED) >>
                                             BITP_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED) == 1U ? true : false;
            uint32_t nSqiLvl = ((poRegDat->poInfo->pBuffer[1U] & BITM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX) >>
                                BITP_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX);
            oSqiStatusData.eSqiLvl = (ADI_SQI_LVL)nSqiLvl;

            // Notify the SQI status to the application
            if(poSQiCfg->pfSqiCallback != NULL)
            {
                ADI_EAL_SQI_CALLBACK_FUNC pfEalSqiCbk = poSQiCfg->pfSqiCallback;
                pfEalSqiCbk(nInstNum, nRemoteNum, ADI_EAL_SQI_CBK_EVT_SQI_STATUS, &oSqiStatusData);
            }

            /* Unpause the jitter monitor */
            uint16_t nAddr    = PHY_OA_DCQ_JM_CFG0_NW_ADDR;
            uint32_t nRegData = 0U;
            uint32_t nMask    = BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE;

            ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
            oRegCfg.anStartAddr = &nAddr;
            oRegCfg.nLength = 1U;
            oRegCfg.pBuffer = &nRegData;
            oRegCfg.anMask = &nMask;
            oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

            // Send the register write transaction
            if(adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg) != ADI_EAL_STATUS_SUCCESS)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to send reg transaction in sqi handler");
            }
            
            /* Clear the notify flag only after processing the SQI response */
            gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySqi = false;
        }
    }
}

/**
 * @brief       This function allows the EAL top component to notify the
 *              SQI component of an interrupt arrival
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  eSqiIntEvt          SQI interrupt event
 */
ADI_EAL_SQI_CODE
bool eal_notify_SqiInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_SQI_INTERRUPT_EVT eSqiIntEvt)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    ADI_EAL_SQI_CFG *poSQiCfg = gaoEalSqiData[nInstNum].poSqiCfg;

    bool bInterruptProcessed = false;

    if(poSQiCfg != NULL)
    {
        switch(eSqiIntEvt)
        {
            case EAL_SQI_EXITED_WARNING_EVT:
            {
                if(gaoEalSqiData[nInstNum].nCurrRemoteIrqState[nRemoteNum] != SQI_EXITED_WARNING)
                {
                    /* SQI exited from warning level */
                    InvokeCbk(poSQiCfg, nInstNum, nRemoteNum, ADI_EAL_SQI_CBK_EVT_SQI_EXITED_WARNING, SQI_EXITED_WARNING);
                }

                break;
            }

            case EAL_SQI_ENTERED_WARNING_EVT:
            {
                if(gaoEalSqiData[nInstNum].nCurrRemoteIrqState[nRemoteNum] != SQI_ENTERED_WARNING)
                {
                    /* SQI entered warning level */
                    InvokeCbk(poSQiCfg, nInstNum, nRemoteNum, ADI_EAL_SQI_CBK_EVT_SQI_ENTERED_WARNING, SQI_ENTERED_WARNING);
                }

                break;
            }

            case EAL_SQI_ENTERED_BAD_EVT:
            {
                if(gaoEalSqiData[nInstNum].nCurrRemoteIrqState[nRemoteNum] != SQI_ENTERED_BAD)
                {
                    /* SQI entered bad level */
                    InvokeCbk(poSQiCfg, nInstNum, nRemoteNum, ADI_EAL_SQI_CBK_EVT_SQI_ENTERED_BAD, SQI_ENTERED_BAD);
                }

                break;
            }

            case EAL_SQI_EXITED_BAD_EVT:
            {
                if(gaoEalSqiData[nInstNum].nCurrRemoteIrqState[nRemoteNum] != SQI_EXITED_BAD)
                {
                    /* SQI exited from bad level */
                    InvokeCbk(poSQiCfg, nInstNum, nRemoteNum, ADI_EAL_SQI_CBK_EVT_SQI_EXITED_BAD, SQI_EXITED_BAD);
                }

                break;
            }

            default:
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Unknown SQI interrupt");
                ADI_DBG_ERROR();
                break;
            }
        }

        bInterruptProcessed = true;
    }

    return bInterruptProcessed;
}

/* Invoke callback */
ADI_EAL_SQI_CODE
static void InvokeCbk(ADI_EAL_SQI_CFG *poSQiCfg, uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SQI_CBK_EVT eCbkEvt,
                      SQI_IRQ_STATE eIrqState)
{
    /* Get the callback function */
    ADI_EAL_SQI_CALLBACK_FUNC pfEalSqiCbk = poSQiCfg->pfSqiCallback;

    /* Invoke the callback */
    if(pfEalSqiCbk != NULL)
    {
        pfEalSqiCbk(nInstNum, nRemoteNum, eCbkEvt, NULL);
    }

    gaoEalSqiData[nInstNum].nCurrRemoteIrqState[nRemoteNum] = eIrqState;
}


#endif /* ADI_E2B_IFACE_SQI_ENABLED */
/** @} */

/*
 * EOF: www.analog.com
*/
