/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_smc.c
 * @brief: The EAL APIs for SMC
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_SMC_ENABLED

/*! \cond PRIVATE */
/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendSmcData(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nRegAddr, uint16_t nTxnLength,
                                  bool bSendDataToApp);
static void CheckSmcBlockMemory(uint32_t nInstNum, uint32_t nRemoteNum,
                                ADI_SMC_REGION_CONFIG anRegions[], uint8_t nNumRegions);
static void FillRegionHeader(ADI_SMC_REGION_CONFIG *poRegionCfg, uint8_t *pnSmcData, uint16_t *pnIndex);
static ADI_EAL_STATUS FillSmcLUT(uint32_t nInstNum, uint32_t nRemoteNum,
                                 ADI_SMC_REGION_CONFIG anRegions[], uint8_t nNumRegions, bool bConfigOverCallback);
/*================================= DEFINES ================================*/

/*================================= DATA TYPES ================================*/
/* Internal structure for SMC module */
typedef struct SMC_INTERNAL_DATA
{
    ADI_EAL_SMC_CFG *poSmcCfg;           /* Stores the SMC cfg */
} SMC_INTERNAL_DATA;

ADI_EAL_SMC_DATA
static SMC_INTERNAL_DATA gaoEalSmcData[ADI_E2BCORE_MAX_INST];

/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief Initializes the SMC component
 *
 * @param [in] nInstNum Instance number
 * @param [in] poSmcCfg Pointer to config structure
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS            API successful
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SMC_CODE
ADI_EAL_STATUS adi_eal_smcInit(uint32_t nInstNum, ADI_EAL_SMC_CFG *poSmcCfg)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(poSmcCfg != NULL, " SMC config param is NULL ");
    ADI_DBG_REQUIRE(poSmcCfg->pfSmcCallback != NULL, "SMC callback function pointer is NULL");
    ADI_DBG_REQUIRE(poSmcCfg->pSmcMemory != NULL, "Pointer to SMC memory is NULL");
    ADI_DBG_REQUIRE(poSmcCfg->nSmcMemorySize >= SMC_BUFFER_MIN_SIZE
                    && poSmcCfg->nSmcMemorySize <= SMC_BUFFER_MAX_SIZE, "Memory is either less than 36 bytes or greater than 1020");

    // Clear the instance memory
    (void) ADI_MEMSET(&gaoEalSmcData[nInstNum], 0, sizeof(gaoEalSmcData));

    // Save the configurations
    gaoEalSmcData[nInstNum].poSmcCfg = poSmcCfg;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Configures the SMC block
 *
 * @param [in] nInstNum     Instance number
 * @param [in] nRemoteNum   Remote Number
 * @param [in] poSmcSetting Pointer to the SMC block configurations
 * @return eEalStatus
 *             - #ADI_EAL_STATUS_SUCCESS           API successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *             - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SMC_CODE
ADI_EAL_STATUS adi_eal_smcSettings(uint32_t nInstNum, uint32_t nRemoteNum, ADI_SMC_CONFIG *poSmcSetting)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    /* General configuration for SMC block */
    uint16_t nStartAddr[6u] = {LCE_SMC_MISC_NW_ADDR, LCE_SMC_TIMEBASE_PERIOD_NS_NW_ADDR, LCE_SMC_TIMEBASE_PERIOD_S_NW_ADDR, LCE_SMC_STARTING_TIMESTAMP_LOWER_NW_ADDR, LCE_SMC_STARTING_TIMESTAMP_UPPER_NW_ADDR, IO_TIC_ENABLE_32_LCE_SMC_SQI_REG0_NW_ADDR};
    uint32_t nMask[6u];
    uint32_t nRegData[6u];

    /* Set config bit, detection on edge trigger for regions, disable mode, instant / delayed start of iterations, and the window for detecting triggers */
    nRegData[0u] = (poSmcSetting->bConfigA == true ? 0u : BITM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION) |
                   (poSmcSetting->bDetectTriggerEdge == true ? BITM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG : 0u) |
                   (poSmcSetting->bDisableAfterIteration == true ? BITM_LCE_SMC_MISC_SMC_DISABLE_MODE : 0u) |
                   (poSmcSetting->bSmcInstantStart == true ?  0u : BITM_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE) |
                   (poSmcSetting->eTriggerDetectWindow == ADI_EAL_SMC_TRIGGER_DETECT_WINDOW_OVER_PREV_ITERATION ?
                    BITM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT : 0u);
    nMask[0u] = BITM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION | BITM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG |
                BITM_LCE_SMC_MISC_SMC_DISABLE_MODE | BITM_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE |
                BITM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT;

    /* SMC timebase */
    nRegData[1u] = (uint32_t)poSmcSetting->nSmcTimebase & LCE_SMC_TIMEBASE_PERIOD_NS_VOL_MASK;
    nMask[1u] = LCE_SMC_TIMEBASE_PERIOD_NS_VOL_MASK;

    nRegData[2u] = (uint32_t)(poSmcSetting->nSmcTimebase >> 32u) & LCE_SMC_TIMEBASE_PERIOD_S_VOL_MASK;
    nMask[2u] = LCE_SMC_TIMEBASE_PERIOD_S_VOL_MASK;

    /* SMC block start time - starts iterations */
    nRegData[3u] = (uint32_t)poSmcSetting->nSmcStartTime & LCE_SMC_STARTING_TIMESTAMP_LOWER_VOL_MASK;
    nMask[3u] = LCE_SMC_STARTING_TIMESTAMP_LOWER_VOL_MASK;

    nRegData[4u] = (uint32_t)(poSmcSetting->nSmcStartTime >> 32u) & LCE_SMC_STARTING_TIMESTAMP_UPPER_VOL_MASK;
    nMask[4u] = LCE_SMC_STARTING_TIMESTAMP_UPPER_VOL_MASK;

    /* Enabling interrupts */
    uint8_t nIrqEn = (poSmcSetting->bEnRegionCollisionIrq == true ? SMC_RISING_REGION_COLLISION_IRQ_MASK : 0u) |
                     (poSmcSetting->bEnIterEndIrq == true ? SMC_RISING_ITERATION_END_IRQ_MASK : 0u) |
                     (poSmcSetting->bEnIterSkipIrq == true ? SMC_RISING_ITERATION_SKIP_IRQ_MASK : 0u) |
                     (poSmcSetting->bEnConfigChangeIrq == true ? SMC_RISING_CONFIG_CHANGE_IRQ_MASK : 0u);
    nRegData[5u] = (uint32_t)nIrqEn;
    nMask[5u] = nRegData[5u];

    /* Do a masked write of the above configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr[0u];
    oRegCfg.nLength = 6U;
    oRegCfg.pBuffer = &nRegData[0u];
    oRegCfg.anMask = &nMask[0u];
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the regmap transaction */
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    return eEalStatus;
}

/**
 * @brief Creates and configures SMC regions
 *
 * @param [in] nInstNum               Instance Number
 * @param [in] nRemoteNum             Remote Number
 * @param [in] anRegions              Pointer to the array of regions config structure
 * @param [in] nNumRegions            Number of regions to be configured
 * @param [in] bConfigOverCallback    if set true, the data to configure SMC regions is sent back to application as a pointer to regmap config (usually used for OTP config)
 *                                    where as false, sends the data directly to the node to configure the regions
 * @return eEalStatus
 *             - #ADI_EAL_STATUS_SUCCESS           API successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *             - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SMC_CODE
ADI_EAL_STATUS adi_eal_smcCreateRegions(uint32_t nInstNum, uint32_t nRemoteNum,
                                        ADI_SMC_REGION_CONFIG anRegions[], uint8_t nNumRegions, bool bConfigOverCallback)
{
    uint16_t nIndex = ADI_VAL_ZERO;
    uint16_t nAvailableSpace = ADI_VAL_ZERO;
    uint16_t nRegionLength = ADI_VAL_ZERO;
    uint16_t nCurrentByte = ADI_VAL_ZERO;
    uint8_t nRegionHeaderLen = ADI_VAL_ZERO;

    /* Check if Number of regions to be configured is greater than 0 */
    ADI_DBG_REQUIRE(nNumRegions > ADI_VAL_ZERO, "Number of regions to configure is zero");

    /* Check if enough SMC block memory is configured */
    if(!bConfigOverCallback)
    {
        CheckSmcBlockMemory(nInstNum, nRemoteNum, anRegions, nNumRegions);
    }

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint8_t *nSmcData = (uint8_t *)((void *)&gaoEalSmcData[nInstNum].poSmcCfg->pSmcMemory[0u]);
    /* As regmap transaction's reg data is limited we need to cap the buffer (given by app) usage */
    uint16_t nSmcMemorySize = gaoEalSmcData[nInstNum].poSmcCfg->nSmcMemorySize;
    /* Reset SMC buffer */
    (void)ADI_MEMSET(&nSmcData[nIndex], (int)ADI_VAL_ZERO, (uint32_t)nSmcMemorySize);

    /* Incase if the data for SMC block exceeds the max transaction length of a regmap io txn ,
     data is sent out and the next address to write is stored in nRegAddr variable */
    /* Leaving enough space for the addresses in LUT */
    uint16_t nRegAddr = SMC_BLOCK_REGION_START_ADDR + (2u * (uint16_t)nNumRegions) + ((nNumRegions % 2u == ADI_VAL_ZERO) ?
                        3u : 1u);
    /* Init available SMC buffer size */
    nAvailableSpace = nSmcMemorySize;

    do
    {
        for(uint8_t nRegionIndex = ADI_VAL_ZERO; nRegionIndex < nNumRegions; nRegionIndex++)
        {
            /* Calculate required SMC header length for current region */
            CalcRegionHeaderLen(&anRegions[nRegionIndex], &nRegionHeaderLen);

            /* Check for minimum SMC header length and available buffer space */
            if(nRegionHeaderLen >= (uint8_t)SMC_REGION_MANDATORY_HEADER_LEN && nAvailableSpace >= (uint16_t)nRegionHeaderLen)
            {

                /* Mark the address for the region */
                anRegions[nRegionIndex].nRegionAddress = nRegAddr + nIndex - SMC_BLOCK_BASE_ADDR;

                /* Fill SMC Region header section */
                FillRegionHeader(&anRegions[nRegionIndex], nSmcData, &nIndex);
                nAvailableSpace = nSmcMemorySize - nIndex;

                /* Region payload length */
                nRegionLength = anRegions[nRegionIndex].nRegionDataLength;
                nCurrentByte = ADI_VAL_ZERO;
            }
            else
            {
                /* Check Sparse and gPTP both fields are set for current SMC region
                 ie: nRegionHeaderLen = 0, is set in (CalcRegionHeaderLen) API */
                if(nRegionHeaderLen < (uint8_t)SMC_REGION_MANDATORY_HEADER_LEN)
                {
                    /* Set the address current region as 0 to remove region from SMC look up table */
                    anRegions[nRegionIndex].nRegionAddress = ADI_VAL_ZERO;
                    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                                "Remote : %d Invalid SMC region configuration Sparse and gPTP both fields are set for region number : %d \r\n",
                                nRemoteNum,
                                (nRegionIndex + 1u));
                }

                /* Check If any previous SMC region data present in buffer */
                if(nIndex > ADI_VAL_ZERO)
                {
                    /* If any data is left out , it has to be transmitted with nIndex as length */
                    eEalStatus = SendSmcData(nInstNum, nRemoteNum, nRegAddr, nIndex, bConfigOverCallback);
                    nRegAddr += nIndex;
                    nIndex = ADI_VAL_ZERO;
                    nAvailableSpace = nSmcMemorySize;
                    /* Reset SMC buffer */
                    (void)ADI_MEMSET(&nSmcData[nIndex], (int)ADI_VAL_ZERO, (uint32_t)nSmcMemorySize);
                }
            }

            while(nRegionLength > (uint16_t)ADI_VAL_ZERO)
            {
                /* If the region can fit in available space, pack the region into the buffer and proceed to fill next region header */
                if(nRegionLength < nAvailableSpace)
                {
                    (void)ADI_MEMCPY(&nSmcData[nIndex], &anRegions[nRegionIndex].pRegionData[nCurrentByte], nRegionLength);
                    nIndex += nRegionLength;
                    nRegionLength = ADI_VAL_ZERO;
                    nAvailableSpace = nSmcMemorySize - nIndex;
                }
                else
                {
                    /* If the region can't fit in the buffer, fill the region data partially till available space and transmit the packet */
                    (void)ADI_MEMCPY(&nSmcData[nIndex], &anRegions[nRegionIndex].pRegionData[nCurrentByte], nAvailableSpace);
                    nCurrentByte += nAvailableSpace;
                    nIndex += nAvailableSpace;
                    eEalStatus = SendSmcData(nInstNum, nRemoteNum, nRegAddr, nIndex, bConfigOverCallback);

                    if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
                    {
                        break;
                    }

                    nRegionLength = nRegionLength - nAvailableSpace;
                    nRegAddr += nIndex;
                    nIndex = ADI_VAL_ZERO;
                    nAvailableSpace = nSmcMemorySize;
                    /* Reset SMC buffer */
                    (void)ADI_MEMSET(&nSmcData[nIndex], (int)ADI_VAL_ZERO, (uint32_t)nSmcMemorySize);

                }
            }

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* If any data is left out , it has to be transmitted with nIndex as length */
        if(eEalStatus == ADI_EAL_STATUS_SUCCESS && nIndex > ADI_VAL_ZERO)
        {
            eEalStatus = SendSmcData(nInstNum, nRemoteNum, nRegAddr, nIndex, bConfigOverCallback);
            nIndex = ADI_VAL_ZERO;
            /* Reset SMC buffer */
            (void)ADI_MEMSET(&nSmcData[nIndex], (int)ADI_VAL_ZERO, (uint32_t)nSmcMemorySize);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Fill SMC look up table - that contains the number of regions and their respective relative addresses */
        eEalStatus = FillSmcLUT(nInstNum, nRemoteNum, anRegions, nNumRegions, bConfigOverCallback);

    } while(false);

    return eEalStatus;
}

/**
 * @brief Changes the selected configuration bit in the SMC block
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @param [in] bConfigSetA        If true, configures the SMC to make set A regions active while false makes set B regions active
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SMC_CODE
ADI_EAL_STATUS adi_eal_smcSetConfiguration(uint32_t nInstNum, uint32_t nRemoteNum, bool bConfigSetA)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the configuration of SMC block to activate either region with config A set or config B bit set */
    uint16_t nStartAddr = LCE_SMC_MISC_NW_ADDR;
    uint32_t nMask = BITM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION;
    uint32_t nRegData = ((bConfigSetA == true ? (uint32_t)0U : (uint32_t)1U) <<
                         BITP_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION);

    /* Do a mask write to the enable bit */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.anMask = &nMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the regmap transaction */
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    return eEalStatus;
}

/**
 * @brief Enable/Disable SMC module
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @param [in] bEnableSmc         If true, enables SMC module
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SMC_CODE
ADI_EAL_STATUS adi_eal_smcEnable(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnableSmc)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Enable or disable SMC block */
    uint16_t nStartAddr = LCE_SMC_MISC_NW_ADDR;
    uint32_t nMask = BITM_LCE_SMC_MISC_SMC_ENABLE;
    uint32_t nRegData = ((bEnableSmc == true ? (uint32_t)1U : (uint32_t)0U) << BITP_LCE_SMC_MISC_SMC_ENABLE);

    /* Do a mask write to the enable bit */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.anMask = &nMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the regmap transaction */
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    return eEalStatus;
}

/**
 * @brief Triggers SMC manually by setting regmap bits
 *
 * @param [in] nInstNum          Instance Number
 * @param [in] nRemoteNum        Remote Number
 * @param [in] nSmcRegMapTrigger Sets respective regmap bit to manually trigger SMC
 * @return eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SMC_CODE
ADI_EAL_STATUS adi_eal_smcManualTrigger(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nSmcRegMapTrigger)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Manually trigger the SMC region by using regmap bit available for SMC triggers */
    uint16_t nStartAddr = LCE_SMC_MISC_NW_ADDR;
    uint32_t nMask = BITM_LCE_SMC_MISC_SMC_REGMAP_MANUAL_TRIGGER;
    uint32_t nRegData = nSmcRegMapTrigger << BITP_LCE_SMC_MISC_SMC_REGMAP_MANUAL_TRIGGER;

    /* Mask write to set smc_regmap_manual_trigger bits */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.anMask = &nMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the regmap transaction */
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    return eEalStatus;
}

/**
 * @brief Calculates SMC region header length
 *
 * @param [in] poRegionCfg          Pointer to the SMC region configuration.
 * @param [in] pnRegionHeaderLen    Pointer to the length of SMC header.
 */
ADI_EAL_SMC_CODE
void CalcRegionHeaderLen(ADI_SMC_REGION_CONFIG *poRegionCfg, uint8_t *pnRegionHeaderLen)
{
    uint8_t nRegionHeaderLen = ADI_VAL_ZERO;

    /* SMC region mandatory header field length (4 bytes) */
    nRegionHeaderLen = SMC_REGION_MANDATORY_HEADER_LEN;

    if(poRegionCfg->bIsManuallyTriggered)
    {
        /* Add Manual mode field length (5 bytes) */
        nRegionHeaderLen += SMC_REGION_MANUALMODE_HEADER_FIELD_LEN;
    }

    /* Check for sparse and gPTP fields are set at same time which is invalid configuration */
    if(poRegionCfg->bIsSparseUsed == true && poRegionCfg->bIsGptpUsed == true)
    {
        /* Set Region Header length to 0 to indicate region configuration invalid */
        *pnRegionHeaderLen = ADI_VAL_ZERO;
    }
    else
    {
        if(poRegionCfg->bIsSparseUsed)
        {
            /* Add Sparse region field length (2 bytes) */
            nRegionHeaderLen += SMC_REGION_SPARSE_HEADER_FIELD_LEN;
        }

        if(poRegionCfg->bIsGptpUsed)
        {
            /* Add Gptp region field length (8 bytes) */
            nRegionHeaderLen += SMC_REGION_GPTP_HEADER_FIELD_LEN;
        }

        *pnRegionHeaderLen = nRegionHeaderLen;
    }

}

/******************************* STATIC FUNCTIONS *************************************/
ADI_EAL_SMC_CODE
static void CheckSmcBlockMemory(uint32_t nInstNum, uint32_t nRemoteNum,
                                ADI_SMC_REGION_CONFIG anRegions[], uint8_t nNumRegions)
{
    /* Initial variables */
    uint32_t nSmcBlockMem = ADI_VAL_ZERO;
    uint16_t nRequiredSmcMem = ADI_VAL_ZERO;
    bool bGotRegValue = false;
    uint8_t nRegionHeaderLen = ADI_VAL_ZERO;

    /* Get the register value containing the SMC block memory configuration */
    bGotRegValue = eal_utils_getConfigRemoteRegVal(nInstNum, nRemoteNum, LCE_SMC_MISC_NW_ADDR, &nSmcBlockMem);
    /* Assert if SMC block memory hasn't been configured yet */
    ADI_DBG_ENSURE(bGotRegValue, "Failed to get SMC block memory size");

    /* Extract the SMC memory allocated from the register */
    nSmcBlockMem = (nSmcBlockMem & BITM_LCE_SMC_MISC_SMC_MEMORY_SIZE) >> BITP_LCE_SMC_MISC_SMC_MEMORY_SIZE;

    /*The maximum allowed SMC interface buffer size is (16384 bytes)*/
    ADI_DBG_REQUIRE(nSmcBlockMem <= SMC_IF_MEMORY_MAX_SIZE, "SMC interface buffer size is greater than 16384 bytes");

    /* Loop through the regions to calculate the how much space would it take for the region's header and body */
    for(uint8_t nRegionIndex = ADI_VAL_ZERO; nRegionIndex < nNumRegions; nRegionIndex++)
    {
        /* Calculate required SMC header length for current region */
        CalcRegionHeaderLen(&anRegions[nRegionIndex], &nRegionHeaderLen);

        /* Check for valid SMC region header length and add required memory length for each SMC region  */
        if(nRegionHeaderLen >= (uint8_t)SMC_REGION_MANDATORY_HEADER_LEN)
        {
            /* Calculate required current SMC region header and data memory length */
            nRequiredSmcMem += ((uint16_t)nRegionHeaderLen + anRegions[nRegionIndex].nRegionDataLength);
        }
    }

    /* Calculate the required SMC LUT memory length for all configured regions */
    /* 
     * Extra bytes added as cushion for LUT memory length. i.e., Number of region
     * configured is odd 1 byte and even 3 bytes, So it won't overlap with 
     * region data-SW implementation detail 
     */
    nRequiredSmcMem += (uint16_t)SMC_NUMBER_OF_REGIONS_FIELD_LEN + (uint16_t)
    (SMC_REGION_START_ADDRESS_FIELD_LEN * (uint16_t)nNumRegions) + (uint16_t)
    ((nNumRegions % 2u == ADI_VAL_ZERO) ? 3u : 1u);

    /* Check if the required SMC memory is greater than the allocated SMC block memory */
    /* If yes, then display the number of required space and assert */
    if(nRequiredSmcMem > nSmcBlockMem)
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Insufficient SMC memory to configure the regions, required memory: %d bytes \r\n",
                    nRequiredSmcMem);
        ADI_DBG_ERROR();
    }
}

ADI_EAL_SMC_CODE
static void FillRegionHeader(ADI_SMC_REGION_CONFIG *poRegionCfg, uint8_t *pnSmcData, uint16_t *pnIndex)
{
    uint16_t nIndex = *pnIndex;

    /* Enable bit for the region, activates a region based on the configuration bits set */
    switch (poRegionCfg->eRegionEnConfig) 
    {
        case ADI_EAL_SMC_REGION_EN_CONFIG_BOTH:
            pnSmcData[nIndex] = ADI_SHIFT_RIGHT_24(SMC_REGION_MASK_EN_CONFIG_A_B);
            break;
        case ADI_EAL_SMC_REGION_EN_CONFIG_A:
            pnSmcData[nIndex] = ADI_SHIFT_RIGHT_24(SMC_REGION_MASK_EN_CONFIG_A);
            break;
        case ADI_EAL_SMC_REGION_EN_CONFIG_B:
            pnSmcData[nIndex] = ADI_SHIFT_RIGHT_24(SMC_REGION_MASK_EN_CONFIG_B);
            break;
        case ADI_EAL_SMC_REGION_EN_CONFIG_NONE:
            pnSmcData[nIndex] = ADI_SHIFT_RIGHT_24((uint32_t)SMC_REGION_DISABLE_CONFIG_A_B);
            break;
        default:
            /* Nothing to do */
            break;
    }

    nIndex++;

    /* Here comes the trigger, target, mode fields */
    pnSmcData[nIndex] = (((uint8_t)poRegionCfg->eRegionExecMode) << 6u) | (((uint8_t)poRegionCfg->eRegionTarget) << 3u) |
                        ((poRegionCfg->bIsManuallyTriggered == true ? (uint8_t)0u : (uint8_t)1u) << 2u) |
                        ((poRegionCfg->bIsSparseUsed == true ? (uint8_t)1u : (uint8_t)0u) << 1u) |
                        (poRegionCfg->bIsGptpUsed == true ? (uint8_t)1u : (uint8_t)0u);
    nIndex++;

    /* Region length field */
    pnSmcData[nIndex] = (uint8_t)((poRegionCfg->nRegionDataLength & 0xFF00U) >> 8u);
    nIndex++;
    pnSmcData[nIndex] = (uint8_t)(poRegionCfg->nRegionDataLength & 0x00FFU);
    nIndex++;

    /* Manual trigger / Sparse / gPTP fields */
    if(poRegionCfg->bIsManuallyTriggered)
    {
        pnSmcData[nIndex] = (uint8_t)(((poRegionCfg->nTriggerEn & 0xF0000U) >> 12u) 
        | ((poRegionCfg->nTriggerPolarity & 0xF0000U) >> 16u));
        nIndex++;
        pnSmcData[nIndex] = (uint8_t)(((poRegionCfg->nTriggerEn & 0x0F000U) >> 8u) 
        | ((poRegionCfg->nTriggerPolarity & 0x0F000U) >> 12u));
        nIndex++;
        pnSmcData[nIndex] = (uint8_t)(((poRegionCfg->nTriggerEn & 0x00F00U) >> 4u) 
        | ((poRegionCfg->nTriggerPolarity & 0x00F00U) >> 8u));
        nIndex++;
        pnSmcData[nIndex] = (uint8_t)((poRegionCfg->nTriggerEn & 0x000F0U) 
        | ((poRegionCfg->nTriggerPolarity & 0x000F0U) >> 4u));
        nIndex++;
        pnSmcData[nIndex] = (uint8_t)(((poRegionCfg->nTriggerEn & 0x0000FU) << 4u) 
        | (poRegionCfg->nTriggerPolarity & 0x0000FU));
        nIndex++;
    }

    /* If sparse is used to trigger the region - note the region gets triggers 
     * for the number of tries that is equal to "configured sparse - 1" */
    if(poRegionCfg->bIsSparseUsed)
    {
        pnSmcData[nIndex] = poRegionCfg->nSparseCount;
        nIndex++;
        /* Leaving one byte as SMC uses the 2nd byte for counting the number of attempts took to execute region */
        pnSmcData[nIndex] = 0u;
        nIndex++;
    }

    /* If Gptp time stamp triggers the region - add the timestamp in the region header */
    if(poRegionCfg->bIsGptpUsed)
    {
        uint8_t nCount = 7u;

        for(uint8_t j = 0u; j < 8u; j++)
        {
            pnSmcData[nIndex] = (uint8_t)((poRegionCfg->nTimeStamp & (0xFF00000000000000ULL >> (j * 8u))) >> (nCount * 8u));
            nCount--;
            nIndex++;
        }
    }
    /* Update the index to point to the next free space in SMC buffer */
    *pnIndex = nIndex;
}

ADI_EAL_SMC_CODE
static ADI_EAL_STATUS FillSmcLUT(uint32_t nInstNum, uint32_t nRemoteNum,
    ADI_SMC_REGION_CONFIG anRegions[], uint8_t nNumRegions, bool bConfigOverCallback)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint8_t *nSmcData = (uint8_t *)((void *)&gaoEalSmcData[nInstNum].poSmcCfg->pSmcMemory[0u]);
    uint16_t nIndex = ADI_VAL_ZERO;

    /* As regmap transaction's reg data is limited we need to cap the buffer (given by app) usage */
    uint16_t nSmcMemorySize = gaoEalSmcData[nInstNum].poSmcCfg->nSmcMemorySize;

    /* Reset SMC buffer */
    (void)ADI_MEMSET(&nSmcData[nIndex], (int)ADI_VAL_ZERO, (uint32_t)nSmcMemorySize);

    /* Start filling the LUT with the number of regions and address of those regions */
    nSmcData[nIndex] = nNumRegions;
    nIndex++;

    /* SMC memory base address */
    uint16_t nSmcBaseAddr = SMC_BLOCK_BASE_ADDR;

    /* Loop through the region configs to get the address field - which is filled during creation of regions */
    for(uint8_t nRegionIndex = ADI_VAL_ZERO; nRegionIndex < nNumRegions; nRegionIndex++)
    {
        /* Check if Current region data is configured successfully (nRegionAddress > 0) */
        if(anRegions[nRegionIndex].nRegionAddress != ADI_VAL_ZERO)
        {
            nSmcData[nIndex] = (uint8_t)((anRegions[nRegionIndex].nRegionAddress & 0xFF00U) >> 8u);
            nIndex++;
            nSmcData[nIndex] = (uint8_t)(anRegions[nRegionIndex].nRegionAddress & 0x00FFU);
            nIndex++;
        }
        else
        {
            /* Current region data is not configured decrease the number of regions count by one in LUT */
            nSmcData[ADI_VAL_ZERO]--;
        }
    }

    /* Check if any SMC region data configuration has failed */
    if(nNumRegions != nSmcData[ADI_VAL_ZERO])
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Remote:%d Failed to configure all SMC regions \r\n", nRemoteNum + 1U);
        ADI_DBG_ERROR();
        /* Set SMC configuration status as failed */
        eEalStatus = ADI_EAL_STATUS_SMC_REGION_CONFIG_FAILED;
    }
    else
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Remote:%d number of regions configured successfully : %d \r\n", nRemoteNum + 1U,
                    nSmcData[ADI_VAL_ZERO]);
        /* Send LUT region data if all regions data configured successfully */
        eEalStatus = SendSmcData(nInstNum, nRemoteNum, nSmcBaseAddr, nIndex, bConfigOverCallback);
    }

    return eEalStatus;
}

ADI_EAL_SMC_CODE
static ADI_EAL_STATUS SendSmcData(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nRegAddr, uint16_t nTxnLength,
                                  bool bSendDataToApp)
{
    /* Check if SMC region TX data length is less than or equal to SMC_BUFFER_MAX_SIZE */
    ADI_DBG_REQUIRE(nTxnLength <= SMC_BUFFER_MAX_SIZE, "SMC region TX data length is greater than 1020 bytes");
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegMapTxn;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint8_t *nSmcData = (uint8_t *)((void *)&gaoEalSmcData[nInstNum].poSmcCfg->pSmcMemory[0u]);
    ADI_EAL_SMC_CALLBACK_FUNC pfSmcCallback = gaoEalSmcData[nInstNum].poSmcCfg->pfSmcCallback;
    uint32_t nTempData32;
    uint32_t *pData32;

    /* Needs to be ceiled , in the case the buffer has data filled at odd space as region data gets filled */
    uint8_t nRegisterWrites = (uint8_t)((((uint32_t)nTxnLength + 3U) & (0x3FCU)) / 4u);

    /* Correcting the endianess, the data stored byte-wise in pSmcMemory provided by the application is
       shifted according to the endianess required by the system and stored at the same location by typecasting
       the consecutive 4 bytes to a 32 byte location, since the regmap io config accepts the pointer for regdata as uint32_t type */
    for(uint16_t i = 0u; i < nRegisterWrites; i++)
    {
        nTempData32 = ((uint32_t)nSmcData[0] << 24u) | ((uint32_t)nSmcData[1] << 16u) | ((uint32_t)nSmcData[2] << 8u) |
                      nSmcData[3];
        pData32 = (uint32_t *)((void *)nSmcData);
        *pData32 = nTempData32;
        nSmcData += 4u;
    }

    /* Create the regmap configuration for SMC region data */
    oRegMapTxn.anStartAddr = &nRegAddr;
    oRegMapTxn.eType       = ADI_E2BCORE_BURST_WRITE;
    oRegMapTxn.nLength     = nRegisterWrites;
    oRegMapTxn.pBuffer     = &gaoEalSmcData[nInstNum].poSmcCfg->pSmcMemory[0u];

    /* Either send the data to application back or send a regmap transaction directly depending on the user's configuration */
    if(!bSendDataToApp)
    {
        eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegMapTxn);
    }
    else
    {
        /* Send a callback to the application with region data */
        ADI_EAL_REGION_DATA oRegionData;
        oRegionData.poRegMapConfig = &oRegMapTxn;
        oRegionData.nValidBytes = nTxnLength;
        pfSmcCallback(nInstNum, nRemoteNum, ADI_EAL_SMC_CBK_EVT_REGIONS_DATA, &oRegionData);
    }

    return eEalStatus;
}

// ------------------------ EVENTS ------------------------
/**
 * @brief       This function allows the EAL top component to notify the
 *              SMC component of an interrupt arrival
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  eSmcIntEvt          SMC interrupt event
 */
ADI_EAL_SMC_CODE
bool eal_notify_SmcInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_SMC_INTERRUPT_EVT eSmcIntEvt)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");
    ADI_EAL_SMC_CFG *poSmcCfg = gaoEalSmcData[nInstNum].poSmcCfg;

    bool bInterruptProcessed = false;

    if(poSmcCfg != NULL)
    {
        ADI_DBG_REQUIRE(poSmcCfg->pfSmcCallback != NULL, "SMC callback function pointer is NULL");

        /* Give an IRQ callback to application depending on the event received  */
        switch(eSmcIntEvt)
        {
            case EAL_SMC_ITERATION_END_EVT:
            {
                /* SMC iteration end event */
                (poSmcCfg->pfSmcCallback)(nInstNum, nRemoteNum, ADI_EAL_SMC_CBK_EVT_ITERATION_END, NULL);
                break;
            }

            case EAL_SMC_ITERATION_SKIP_EVT:
            {
                /* SMC iteration skipped event */
                (poSmcCfg->pfSmcCallback)(nInstNum, nRemoteNum, ADI_EAL_SMC_CBK_EVT_ITERATION_SKIP, NULL);
                break;
            }

            case EAL_SMC_REGION_COLLISION_EVT:
            {
                /* SMC region collision event */
                (poSmcCfg->pfSmcCallback)(nInstNum, nRemoteNum, ADI_EAL_SMC_CBK_EVT_REGION_COLLISION, NULL);
                break;
            }

            case EAL_SMC_CONFIGURATION_CHANGE_EVT:
            {
                /* SMC configuration change event */
                (poSmcCfg->pfSmcCallback)(nInstNum, nRemoteNum, ADI_EAL_SMC_CBK_EVT_CONFIGURATION_CHANGE, NULL);
                break;
            }

            default:
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Unknown SMC interrupt");
                ADI_DBG_ERROR();
                break;
            }
        }

        bInterruptProcessed = true;
    }

    return bInterruptProcessed;
}


#endif /* ADI_E2B_IFACE_SMC_ENABLED */
/** @} */

/*
 * EOF: www.analog.com
*/
