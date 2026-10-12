/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_smc.h
 * \brief:The header file for SMC functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_SMC_H
#define ADI_EAL_SMC_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */

#ifdef ADI_E2B_IFACE_SMC_ENABLED


/*================================= DEFINES =================================*/
#define SMC_BUFFER_MIN_SIZE                      (36u)  /*!< The minimum size is marked as 36 bytes cause the buffer is only checked while filling the region data, and the maximum region header is 18 bytes which should still be
                                                              able to fit before or after filling the region data */
#define SMC_BUFFER_MAX_SIZE                      (1020u) /*!< Provided memory should not be greater than 1020 bytes as regmap io transaction can only send 1020 bytes of data in burst write mode */
#define SMC_MAX_NUM_REG_WR                       (255u)  /*!< The maximum number of register writes supported in burst write mode is 255 */
#define SMC_IF_MEMORY_MAX_SIZE                   (16384u) /*!< The maximum allowed SMC interface buffer size is (16384 bytes).The address range for SMC block is (0X7000-0XAFFF) both inclusive */
#define SMC_REGION_TRIGGER_SA_IF_(N)             (0x00000001 << (N+8u))  /*!< Selects the trigger to be enabled/polarity in manual mode region for SA_IF pins */
#define SMC_REGION_TRIGGER_REGMAP_(N)            (0x00000001 << (N))     /*!< Selects the trigger to be enabled/polarity in manual mode region for regmap bits */
#define SMC_NUMBER_OF_REGIONS_FIELD_LEN          (1u) /*!< SMC LUT number of regions field length (1 bytes) */
#define SMC_REGION_START_ADDRESS_FIELD_LEN       (2u) /*!< SMC LUT region starting address field length (2 bytes) */
#define SMC_REGION_MANDATORY_HEADER_LEN          (4u) /*!< SMC region mandatory header field length (4 bytes) */
#define SMC_REGION_MANUALMODE_HEADER_FIELD_LEN   (5u) /*!< Manual mode field length (5 bytes)  */
#define SMC_REGION_SPARSE_HEADER_FIELD_LEN       (2u) /*!< Sparse region field length (2 bytes) */
#define SMC_REGION_GPTP_HEADER_FIELD_LEN         (8u) /*!< Gptp region field length (8 bytes) */
#define SMC_BLOCK_BASE_ADDR                 (0x7000U) /*!< Base address of SMC block */
#define SMC_BLOCK_REGION_START_ADDR         (SMC_BLOCK_BASE_ADDR + 1U) /*!< Address where the region address get stored in SMC block memory */
#define SMC_REGION_EN_CONFIG_A              (0X80000000u) /*!< Enable config A in SMC region */
#define SMC_REGION_EN_CONFIG_B              (0X10000000u) /*!< Enable config B in SMC region */
#define SMC_REGION_EN_CONFIG_A_B            (0X90000000u) /*!< Enable config A & B in SMC region */
#define SMC_REGION_DISABLE_CONFIG_A_B       (0X00000000u) /*!< Disable config A & B in SMC region */
#define SMC_REGION_MASK_EN_CONFIG_A         (0X80000000u) /*!< Mask to Enable config A in SMC region */
#define SMC_REGION_MASK_EN_CONFIG_B         (0X10000000u) /*!< Mask to Enable config B in SMC region */
#define SMC_REGION_MASK_EN_CONFIG_A_B       (0X90000000u) /*!< Mask to enable config A & B in SMC region */
 
/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Enum for SMC region enable bit
 *
 */
typedef enum ADI_EAL_SMC_REGION_EN_CONFIG
{
    ADI_EAL_SMC_REGION_EN_CONFIG_A, /*!< Activates the region when config A is selected in SMC block */
    ADI_EAL_SMC_REGION_EN_CONFIG_B, /*!< Activates the region when config B is selected in SMC block */
    ADI_EAL_SMC_REGION_EN_CONFIG_BOTH, /*!< Activates the region in both cases when config A / B is set , i.e the region is active when SMC is enabled */
    ADI_EAL_SMC_REGION_EN_CONFIG_NONE, /*!< Disables the region for both config A and B */
} ADI_EAL_SMC_REGION_EN_CONFIG;

/**
 * @brief Enum for SMC region targets
 *
 */
typedef enum ADI_EAL_SMC_REGION_TARGET
{
    ADI_EAL_SMC_REGION_TARGET_T1S,  /*!< Target for region data is T1S */
    ADI_EAL_SMC_REGION_TARGET_LCE,  /*!< Target for region data is LCE */
    ADI_EAL_SMC_REGION_TARGET_FIFO0,  /*!< Target for region data is FIFO 0 */
    ADI_EAL_SMC_REGION_TARGET_FIFO1,  /*!< Target for region data is FIFO 1 */
    ADI_EAL_SMC_REGION_TARGET_FIFO2,  /*!< Target for region data is FIFO 2 */
    ADI_EAL_SMC_REGION_TARGET_FIFO3   /*!< Target for region data is FIFO 3 */
} ADI_EAL_SMC_REGION_TARGET;

/**
 * @brief Enum for SMC region execution mode
 *
 */
typedef enum ADI_EAL_SMC_REGION_EXEC_MODE
{
    ADI_EAL_SMC_REGION_EXEC_FUNC = 1u, /*!< Region executes exclusively in functional mode */
    ADI_EAL_SMC_REGION_EXEC_SAFE,      /*!< Region executes exclusively in safe mode */
    ADI_EAL_SMC_REGION_EXEC_BOTH       /*!< Region executes both in functional and safe mode */
} ADI_EAL_SMC_REGION_EXEC_MODE;

/**
 * @brief Enum for different callback events
 */
typedef enum ADI_EAL_SMC_CBK_EVT
{
    ADI_EAL_SMC_CBK_EVT_ITERATION_END,       /*!< Iteration end event */
    ADI_EAL_SMC_CBK_EVT_ITERATION_SKIP,      /*!< Iteration skip event */
    ADI_EAL_SMC_CBK_EVT_REGION_COLLISION,    /*!< Region collision event */
    ADI_EAL_SMC_CBK_EVT_CONFIGURATION_CHANGE,/*!< Configuration bit change event */
    ADI_EAL_SMC_CBK_EVT_REGIONS_DATA         /*!< Callback to the application with pointer to the regions data that can be either sent on T1S / used for OTP configs */
} ADI_EAL_SMC_CBK_EVT;

/**
 * @brief Enum for configuring detection of manual trigger window, used in case if level trigger is selected
 *
 */
typedef enum ADI_EAL_SMC_TRIGGER_DETECT_WINDOW
{
    ADI_EAL_SMC_TRIGGER_DETECT_WINDOW_INSTANT,               /*!< Trigger succeeds if its current(instant) value matches the appropriate polarity */
    ADI_EAL_SMC_TRIGGER_DETECT_WINDOW_OVER_PREV_ITERATION,   /*!< Trigger succeeds if its value at any time during the previous iteration matched the appropriate polarity */
} ADI_EAL_SMC_TRIGGER_DETECT_WINDOW;

/**
 * @brief Structure for region data callback
 *
 */
typedef struct ADI_EAL_REGION_DATA
{
    ADI_E2BCORE_REGMAP_IO_CONFIG *poRegMapConfig; /*!< Pointer to the Regmap IO config that has region data */
    uint16_t nValidBytes;                         /*!< Number of valid region data bytes */
} ADI_EAL_REGION_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief Prototype of EAL SMC component callback */
typedef void (*ADI_EAL_SMC_CALLBACK_FUNC)(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SMC_CBK_EVT eCbkEventType,
        const void *pArg);

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief Data for regions */
typedef ADI_EAL_REGION_DATA ADI_EAL_SMC_CBK_EVT_REGION_DATA;
/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Structure to configure the SMC region
 *
 */
typedef struct ADI_SMC_REGION_CONFIG
{
    ADI_EAL_SMC_REGION_EN_CONFIG eRegionEnConfig; /*!< Enable bit for the region */
    ADI_EAL_SMC_REGION_EXEC_MODE
    eRegionExecMode; /*!< Selects the mode in which the region has to be executed , can be functional/safe/both */
    ADI_EAL_SMC_REGION_TARGET eRegionTarget;      /*!< Target for the region, can be T1S/LCE/FIFOs */
    bool bIsManuallyTriggered;                    /*!< Indicates if the region is manually triggered */
    bool bIsSparseUsed;                           /*!< Indicates if sparse based execution is used */
    bool bIsGptpUsed;                             /*!< Indicates if gPTP based execution is used */
    uint16_t nRegionDataLength;                   /*!< Region data length, only includes the payload length without any headers */
    uint32_t nTriggerEn;                          /*!< Triggers for the region to execute incase manual trigger is selected , values can be given using SMC_REGION_TRIGGER_SA_IF_(N)/SMC_REGION_TRIGGER_REGMAP_(N) */
    uint32_t nTriggerPolarity;                    /*!< Indicates the polarities for the triggers that are enabled, takes values similar to nTriggerEn */
    uint8_t nSparseCount;                         /*!< Indicates the number of times the region should be attempted before each execution */
    uint64_t nTimeStamp;                          /*!< gPTP timestamp , the region executes only once after this time has passed */
    uint8_t *pRegionData;                         /*!< Pointer to region data */

    /*========== Internal data, needn't to be filled by the application ==========*/
    uint16_t nRegionAddress;               /*!< Stores the region starting address, for SMC header LUT (to be filled internally)*/
} ADI_SMC_REGION_CONFIG;

/**
 * @brief Structure for SMC block settings
 *
 */
typedef struct ADI_SMC_CONFIG
{
    uint64_t nSmcStartTime; /*!< Determines at what time SMC begins the iterations once it is enabled */
    uint64_t nSmcTimebase;  /*!< Timebase determines the gap between each iteration cycle */
    ADI_EAL_SMC_TRIGGER_DETECT_WINDOW
    eTriggerDetectWindow;   /*!< Determines the time window in which to evaluate manual-mode triggers when `smc_manual_trigger_config` is set to `LEVEL`. */
    bool bConfigA;                  /*!< If set true, activates all regions having config A bit set else aactivates all regions having config B bit set */
    bool bDisableAfterIteration;    /*!< If set true, the disable mode is configured to end the execution after the current iteration else it is configured to end after execution of the next region */
    bool bDetectTriggerEdge;        /*!< If set true, SMC interprets the manual trigger signals as edge triggered else it is set to level triggered */
    bool bSmcInstantStart;  /*!< If set true, SMC starts the iterations instantly after it has been enabled , else nSmcStartTime will determine the time to begin the iterations */
    bool bEnIterEndIrq;     /*!< if set true, enables TIC interrupt for iteration end event */
    bool bEnIterSkipIrq;    /*!< if set true, enables TIC interrupt for iteration skip event */
    bool bEnRegionCollisionIrq; /*!< if set true, enables TIC interrupt for region collision event */
    bool bEnConfigChangeIrq; /*!< if set true, enables TIC interrupt for configuration bit change event */
} ADI_SMC_CONFIG;

/* ~~~~~~~~~~~~~~~~~~~ SMC module configuration structure ~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Structure for configuring SMC module
 * 
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL SMC APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_SMC_CFG
{
    ADI_EAL_SMC_CALLBACK_FUNC pfSmcCallback;  /*!< Function that is called by the SMC component
                                                       to notify any event to the application */
    uint32_t *pSmcMemory;     /*!< Memory to be allocated by application for SMC (must be 4-byte or 8-byte aligned) */
    uint16_t nSmcMemorySize; /*!< Size of the memory, allocated by application - SMC buffer maximum allowed size is 255 words as regmap io transaction can only send 1020 bytes of data in burst write mode */
} ADI_EAL_SMC_CFG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_smcInit(uint32_t nInstNum, ADI_EAL_SMC_CFG *poSmcCfg);
extern ADI_EAL_STATUS adi_eal_smcSetConfiguration(uint32_t nInstNum, uint32_t nRemoteNum, bool bConfigSetA);
extern ADI_EAL_STATUS adi_eal_smcCreateRegions(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_SMC_REGION_CONFIG anRegions[], uint8_t nNumRegions, bool bConfigOverCallback);
extern ADI_EAL_STATUS adi_eal_smcEnable(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnableSmc);
extern ADI_EAL_STATUS adi_eal_smcSettings(uint32_t nInstNum, uint32_t nRemoteNum, ADI_SMC_CONFIG *poSmcSetting);
extern ADI_EAL_STATUS adi_eal_smcManualTrigger(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nSmcRegMapTrigger);

#endif /* ADI_E2B_IFACE_SMC_ENABLED */
#endif /* ADI_EAL_SMC_H */

/** @} */

/**
 * EOF: www.analog.com
 */

