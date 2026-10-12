/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_otp.h
 * \brief: Header for the OTP low level functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef OTP_H
#define OTP_H

/*================================= INCLUDES ================================*/
#include "adi_e2bcore.h"

#ifdef ADI_E2B_IFACE_OTP_ENABLED


/*================================= DEFINES =================================*/
/* No. of OTP blocks */
#define ADI_OTP_MAX_NUM_BLOCKS                  (64U)            /*!< Max number of OTP blocks */

#define ADI_OTP_NUM_HV_DATA_BLOCKS              (3U)             /*!< Number of HV data blocks */
#define ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS     (5U)             /*!< Number of designer scripts blocks */
#define ADI_OTP_NUM_MAX_USER_SCRIPTS_BLOCKS     (ADI_OTP_MAX_NUM_BLOCKS - ADI_OTP_NUM_HV_DATA_BLOCKS - ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS) /*!< Max number of user scripts blocks */

/* OTP block length parameters */
#define ADI_OTP_SECDED_LEN_BYTES                (1U)             /*!< Number of bytes for secded */
#define ADI_OTP_PAYLOAD_LEN_BYTES               (14U)            /*!< Number of bytes for payload */
#define ADI_OTP_CHKSUM_LEN_BYTES                (1U)             /*!< Number of bytes for checksum */
#define ADI_OTP_BLOCK_LEN_BYTES                 (ADI_OTP_SECDED_LEN_BYTES + ADI_OTP_PAYLOAD_LEN_BYTES + ADI_OTP_CHKSUM_LEN_BYTES) /*!< Number of bytes for one OTP block */

#define ADI_OTP_PHY_ID_LEN_BYTES                (14U)            /*!< Number of bytes for Phy identifier */

#define ADI_OTP_OEM_DATA_LEN_BYTES              (14U)            /*!< Number of bytes for OEM data */
#define ADI_OTP_MFG_DATA_LEN_BYTES              (14U)            /*!< Number of bytes for MFG data */

#define ADI_OTP_PLCA_LUT_CODE_LEN               (16U)            /*!< Number of bytes for PLCA lookup table data */


#define ADI_OTP_MACRO_BASE_ADDR                 (0x0000U)        /*!< The OTP macro base address */
#define ADI_OTP_RD_CONTENT_BASE_ADDR            (0xB500U)        /*!< The OTP read content base address */
#define ADI_OTP_RD_CONTENT_MMS                  (0x0AU)          /*!< The OTP read content mms */

#define ADI_OTP_PING_RESP_TIMEOUT_MS            (100U)           /*!< The timeout for ping response in milliseconds */
#define ADI_OTP_ADC_RESP_TIMEOUT_MS             (1000U)           /*!< The timeout for ADC sample in milliseconds */
#define ADI_OTP_REG_READ_RESP_TIMEOUT_MS        (100U)           /*!< The timeout for OTP reg read response in milliseconds */
#define ADI_OTP_REG_PST_POLL_DELAY_MS           (3U)             /*!< The delay for OTP reg PST poll in milliseconds */
#define ADI_OTP_REG_PGM_DONE_TIMEOUT_MS         (1000U)          /*!< The timeout for OTP reg PGM complete response in milliseconds */
#define ADI_OTP_BOOTLOADER_DONE_TIMEOUT_MS      (1000U)          /*!< The timeout for OTP bootloader done response in milliseconds */

#define ADI_OTP_REG_READ_MAXLEN                 (16U)            /*!< The max length of OTP reg read */

#define ADI_OTP_DATA_USER_MAX_REGNUM            (64u)            /*!< The max number of register addr-value pairs */

#define NUM_AVDD_SAMPLES                        (10u)            /*!< Number of AVDD samples to check if it is within limits before OTP programming */

#define ADI_OTP_SECDED_DATA_MASK                (0x7Fu)          /*!< OTP seceded data mask*/

#define ADI_OTP_READTYPE_SINGLE_ENDED           (0x00u)          /*!< OTP Read type is single ended*/

#define ADI_OTP_READTYPE_DIFFERENTIAL_REDUNDANT (0x01u)          /*!< OTP Read type is differential redundant */

/*================================ DATA TYPES ===============================*/
/*! @brief OTP APIs return types */
typedef enum ADI_OTP_STATUS
{
    ADI_OTP_STATUS_NO_ERR,                  /*!< No error */
    ADI_OTP_STATUS_INSUFF_BLOCKS,           /*!< Number of OTP blocks are not sufficient to program OTP cfg data */
    ADI_OTP_STATUS_APP_TARGET_BLOCK_USED,   /*!< The target block specified to program application data is already used */
    ADI_OTP_STATUS_PGM_FAIL,                /*!< OTP programming failed */
    ADI_OTP_STATUS_READ_FAIL,               /*!< Readback of OTP controller registers failed */
    ADI_OTP_STATUS_VERIF_FAIL,              /*!< OTP readback mismatch with the programmed */
    ADI_OTP_STATUS_ECC_SINGLE_BIT_CORRECTED,/*!< ECC single-bit error detected and corrected */
    ADI_OTP_STATUS_ECC_DOUBLE_BIT_ERROR,    /*!< ECC double-bit error detected (uncorrectable) */
    ADI_OTP_STATUS_BOOT_PASS_WITH_ECC_CORR, /*!< OTP Boot successful with atleast one ECC corrected block */
    ADI_OTP_STATUS_BOOT_TIMEOUT,            /*!< OTP Boot didn't complete within a timeout period after soft reset */
    ADI_OTP_STATUS_BOOT_FAIL,               /*!< OTP Boot fail due to atleast one corrupted OTP block which is not ECC-recoverable */
    ADI_OTP_STATUS_FAIL,                    /*!< OTP generic failure */
    ADI_OTP_AVDD_CHECK_FAILED,              /*!< OTP check AVDD voltage prior to programming failed */
    ADI_OTP_JTEMP_CHECK_FAILED,             /*!< OTP check junction temperature prior to programming failed */
    ADI_OTP_STATUS_LOCKED,                  /*!< OTP Block is Locked */
} ADI_OTP_STATUS;

/*! @brief The OTP block boot status */
typedef enum ADI_OTP_BLOCK_BOOT_STATUS
{
    ADI_OTP_BLOCK_BOOT_STATUS_NO_ERR,         /*!< No error */
    ADI_OTP_BLOCK_BOOT_STATUS_FAIL,           /*!< Block boot fail */
} ADI_OTP_BLOCK_BOOT_STATUS;

/*! @brief Different types of OTP blocks */
typedef enum ADI_OTP_BLOCK_TYPE
{
    ADI_OTP_BLOCK_TYPE_USER_SCRIPT = 0x1u,         /*!< LV user script block */
    ADI_OTP_BLOCK_TYPE_FLEXIO_PROG = 0x2u          /*!< FlexIO program block */
} ADI_OTP_BLOCK_TYPE;

#if 0
/*! Register address modes for user scripts */
typedef enum ADI_OTP_ADDR_MODE
{
    ADI_OTP_ADDR_MODE_8BIT,                 /*!< 8-bit addresses */
    ADI_OTP_ADDR_MODE_16BIT                 /*!< 16-bit addresses */
} ADI_OTP_ADDR_MODE;
#endif

/*! @brief Register data modes for user scripts */
typedef enum ADI_OTP_DATA_MODE
{
    ADI_OTP_DATA_MODE_16BIT,                /*!< 16-bit data words */
    ADI_OTP_DATA_MODE_32BIT                 /*!< 32-bit data words */
} ADI_OTP_DATA_MODE;

/*! @brief Block boot status */
typedef enum ADI_OTP_BLOCK_STATUS
{
    ADI_OTP_BLOCK_STATUS_UNINITIALIZED = 0u,   /*!< Block is empty */
    ADI_OTP_BLOCK_STATUS_VALID         = 1u,   /*!< Boot successful without any ECC correction */
    ADI_OTP_BLOCK_STATUS_RECOVERABLE   = 2u,   /*!< Boot successful after ECC correction */
    ADI_OTP_BLOCK_STATUS_CORRUPTED     = 3u    /*!< Boot failed as there are non-recoverable errors */
} ADI_OTP_BLOCK_STATUS;

/*! @brief Data for the designer blocks in the OTP */
typedef struct ADI_OTP_DATA_DESIGNER
{
    /*=============== Block-0 ================*/
    uint8_t anDevMacAddr[6U];                           /*!< Device unicast MAC address */
    uint8_t anMulticastMacAddr[6U];                     /*!< Multicast MAC address */

    /*=============== Block-1 ================*/
    uint16_t nBootloaderCfg;                            /*!< IO map bootloader_cfg 0xC80E */
    uint16_t nDeviceId;                                 /*!< Device ID */
    uint8_t nDeviceVer;                                 /*!< Device version */
    uint8_t anPhyIdentifier[ADI_OTP_PHY_ID_LEN_BYTES];  /*!< PHY Identifier */
    uint8_t nAoCfgFields0;                              /*!< AO map configuration_fields_0 0xB702 */
    uint8_t nAoCfgFields1;                              /*!< AO map configuration_fields_1 0xB703 */

    /*=============== Block-2 ================*/
    uint8_t anOemData[ADI_OTP_OEM_DATA_LEN_BYTES];      /*!< OEM Data */

    /*=============== Block-3 ================*/
    uint8_t anMfgData[ADI_OTP_MFG_DATA_LEN_BYTES];      /*!< Manufacturer Data */

    /*=============== Block-4 ================*/
    uint8_t nPlcaLocalId;                               /*!< Local PLCA node ID */
    uint8_t nPlcaNodeCount;                             /*!< PLCA node count */
    bool bPlcaEnable;                                   /*!< Enable PLCA */
    uint8_t anPlcaLutCode[ADI_OTP_PLCA_LUT_CODE_LEN];   /*!< PLCA Lokk up table codes. Only 4-bit LSBs are valid for each code */
    bool bPlcaLutEn;                                    /*!< Enable PLCA LUT */

} ADI_OTP_DATA_DESIGNER;

/*! @brief Data for the user script block in the OTP */
typedef struct ADI_OTP_DATA_USER
{
    ADI_OTP_DATA_MODE eDataMode;                            /*!< Data mode */
    uint8_t nIdx;                                           /*!< Register map identifier */
    uint32_t nNumRegs;                                      /*!< Number of registers */
    ADI_E2BCORE_REG_ADDR_VAL
    aoRegData[ADI_OTP_DATA_USER_MAX_REGNUM];                /*!< Array of register addr-value pairs */
} ADI_OTP_DATA_USER;

/*! @brief Data for the FlexIO block in the OTP */
typedef struct ADI_OTP_DATA_FIO
{
    uint8_t nIdx;                                           /*!< Register map identifier */
    uint32_t nNumInstruction;                               /*!< Number of instructions */
    uint16_t *panInstructionMemData;                        /*!< Instruction memory contents for configuring FIO */
} ADI_OTP_DATA_FIO;


/*! @brief Structure to store all the data for OTP of one remote node */
typedef struct ADI_OTP_REMOTE_DATA
{
    ADI_OTP_DATA_DESIGNER oOtpDesignerData;                                     /*!< Object to store designer script data */
    bool bDesignerBlockWrEn[ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS];               /*!< Individual flags to enable write to each designer block */

    ADI_OTP_DATA_USER *paoOtpUserData;                                          /*!< Array of user data structures */
    bool bUserDataWrEn;                                                         /*!< Flag to enable write to user script blocks */
    uint8_t nNumOtpUserData;                                                    /*!< Number of objects in the aoOtpUserData array */

    ADI_OTP_DATA_FIO *paoOtpFIOData;                                            /*!< Array of FIO data structures */
    bool bFIODataWrEn;                                                          /*!< Flag to enable write of FIO instructions in to OTP blocks*/
    uint8_t nNumOtpFIOData;                                                     /*!< Number of objects in the aoOtpFIOData array */

    ADI_OTP_DATA_USER *paoOtpUserData2;
    /*!< Array of user data structures that has to be written after the FIO blocks */
    bool bUserData2WrEn;                                                        /*!< Flag to enable write to user script blocks that has to be written after the FIO blocks */
    uint8_t nNumOtpUserData2;                                                   /*!< Number of objects in the aoOtpUserData2 array */

    uint8_t *panAppData;                                                        /*!< Pointer to the application data to be written in OTP */
    uint16_t nAppDataLenBytes;                                                  /*!< Length of the application data to be written in OTP in bytes */
    uint8_t  nAppDataTgtOtpBlock;                                               /*!< Target OTP block no. to start writing application data */
    bool bAppDataWrEn;                                                          /*!< Flag to enable writing application data */

    /* Below fields are not to be updated in the Otp config.
    They are used internally by the OTP driver and are also available
    to the application for reference */
    uint8_t anOtpWriteData[ADI_OTP_MAX_NUM_BLOCKS][ADI_OTP_BLOCK_LEN_BYTES];  /*!< Raw block data for writing */
    uint8_t anOtpReadData[ADI_OTP_MAX_NUM_BLOCKS][ADI_OTP_BLOCK_LEN_BYTES];   /*!< Read block data */
    bool    abOtpBlockWrEn[ADI_OTP_MAX_NUM_BLOCKS];                           /*!< Write enable flag for each block - Used internally */
    ADI_OTP_BLOCK_STATUS aeBlockBootStatus[ADI_OTP_MAX_NUM_BLOCKS];           /*!< Boot status of each OTP block */
    uint8_t nLastOtpBlockWritten;                                             /*!< Last block written in OTP */
    uint8_t nUserScriptStartBlock;                                            /*!< First empty block after designer scripts where user scripts are to be written. This is derived by reading the boot status of OTP blocks */
    uint8_t nFIOInstructionStartBlock;                                        /*!< First empty block after user scripts where Flexible IO instructions are to be written. This is derived by reading the boot status of OTP blocks */
    uint8_t nUserScript2StartBlock;                                           /*!< First empty block after Flexible IO instructions where user scripts are to be written. This is derived by reading the boot status of OTP blocks */
    uint8_t nNumUserScriptBlocks;                                             /*!< No. of user scripts OTP blocks to be written */
    uint8_t nNumFIOInstructionBlocks;                                         /*!< No. of flexible I/O instructions OTP blocks to be written */
    uint8_t nNumUserScript2Blocks;                                            /*!< No. of user scripts to be written in OTP blocks after FIO blocks */
    uint8_t nNumWriteBlocks;                                                  /*!< Number of total OTP blocks to be written */
} ADI_OTP_REMOTE_DATA;

/*! @brief Structure containing Remote node config used at programming time.
Each remote is programmed one at a time */
typedef struct ADI_OTP_PGMTIME_REMOTE_CFG
{
    uint64_t *paRemoteMacAddr;             /*!< Array of remote MAC addresses */
    uint8_t   anControllerMacAddr[6U];     /*!< Controller MAC address */
    uint8_t   nRemotePlcaId;               /*!< PLCA ID configured in remote node during OTP programming */
    bool      bSampleMacAddr;              /*!< Flag to enable MAC address sampling from 4 SAIF pins during progamming */
} ADI_OTP_PGMTIME_REMOTE_CFG;

/*! @brief Structure containing OTP data for all the remote nodes in the network */
typedef struct ADI_OTP_CONFIG
{
    bool                        bPgmOverOaspi;      /*!< Indicates to program OTP over OASPI */
    uint32_t                    nNumNodes;          /*!< Number of remote nodes */
    ADI_OTP_REMOTE_DATA         *paoRemoteOtpData;  /*!< Array of remote otp data structures */
    ADI_OTP_PGMTIME_REMOTE_CFG  *poPgmTimeCfg;      /*!< Poiner to object for storing program time configurations */
} ADI_OTP_CONFIG;



/*! @brief Structure to store the register read response in the callback from EAL */
typedef struct ADI_OTP_REG_IO
{
    uint16_t anAddr[1U];                              /*!< Indicates the address of the read */
    uint8_t  nNumData;                                /*!< Indicates the number of reads */
    uint32_t anData[ADI_OTP_REG_READ_MAXLEN];         /*!< Array populated with the data read */
    bool bReadRespReceived;                           /*!< Flag to check if OTP controller is waiting for reg read response */
    ADI_E2BCORE_REGMAP_IO_TYPE eType;                 /*!< Indicates the type of REGMAP_IO transaction */
} ADI_OTP_REG_IO;

/*! @brief Internal OTP manager to maintain local variables for OTP programming and verification */
typedef struct ADI_OTP_MGR
{
    bool            bInUse;                /*!< Indicates if the OTP manager is in use */
    uint32_t        nEalInstNum;           /*!< OTP driver uses this EAL instance num to access the E2B network */
    uint32_t
    nE2bCfgRemoteNum;         /*!< OTP driver uses this remote number to access the remote node through EAL & E2BCore */
    uint32_t        nDevNum;               /*!< Device number to be used to access the network */
    ADI_OTP_CONFIG *poOtpCfg;              /*!< Pointer to the OTP config structure passed by the application */
    ADI_OTP_REG_IO  oRegIo;                /*!< Object to store the register read response from EAL callback */
    uint8_t         nAdcSampleRcvdCount;   /*!< Number of ADC samples received */
    uint16_t        aAdcSample[NUM_AVDD_SAMPLES];
    /*!< ADC sample values - To check AVDD voltage level or junction temperature */
    bool            bRemotePingRespRcvd;   /*!< Flag to check for remote ping response received */
} ADI_OTP_MGR;


typedef void *ADI_OTP_HANDLE;                         /*!< The OTP handle */

/*============================= EXTERN PROTOTYPES ===========================*/
extern void adi_otp_init(ADI_OTP_HANDLE *phOtpHandle, ADI_OTP_CONFIG *poOtpCfg, uint32_t nDevNum, bool bEnableFrameDup);
extern void adi_otp_terminate(ADI_OTP_HANDLE hOtpHandle);

extern void adi_otp_useRemoteMacAddr(ADI_OTP_HANDLE hOtpHandle, uint64_t nUcastAddr, uint64_t nMcastAddr);
extern ADI_OTP_STATUS adi_otp_setupRemoteNode(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_checkAVDD(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum, double fMinAvddVal,
                                        double fMaxAvddVal);
extern ADI_OTP_STATUS adi_otp_checkJunctionTemp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum, double fMinJunctionTemp,
        double fMaxJunctionTemp);
extern ADI_OTP_STATUS adi_otp_readBlockStatus(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_generateOtpWriteData(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_programOtp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_readVerifyOtp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_verifyOtpLock(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_verifyBootStatus(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_lockOtp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum);
extern ADI_OTP_STATUS adi_otp_readOtpContents(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum, uint8_t nStartBlock,
        uint8_t nBlockCount, uint8_t *pBuffer);
extern ADI_OTP_STATUS adi_otp_readUserSectionData(ADI_OTP_HANDLE hOtpHandle,
        uint32_t nRemoteNum, uint8_t nStartBlockNum, uint8_t *pRdDataBuffer,
        uint16_t nRdDataLen);

#endif /* ADI_E2B_IFACE_OTP_ENABLED */
#endif /* OTP_H */

/** @} */

/*
 * EOF: www.analog.com
 */
