/*******************************************************************************
Copyright (c) 2020-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_oaspi.h
 * \brief: Header for the OA SPI component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup oaspi
 *  @{
 */

#ifndef ADI_OASPI_H
#define ADI_OASPI_H

/*================================= INCLUDES ================================*/
#include "adi_memmap.h"
#include "adi_dbg.h"
#include "adi_common.h"
#include "adi_queue.h"

/*================================== MEMMAP =================================*/
#define ADI_OASPI_MEM_DATA_CRIT_CACHE           ADI_MEM_DATA_CRIT_CACHE     /*!< L2 Cached data placement macro */
#define ADI_OASPI_MEM_DATA_NOCRIT_CACHE         ADI_MEM_DATA_NOCRIT_CACHE   /*!< L3 Cached data placement macro */
#define ADI_OASPI_MEM_DATA_CRIT_NOCACHE         ADI_MEM_DATA_CRIT_NOCACHE   /*!< L2 Cached data placement macro */
#define ADI_OASPI_MEM_DATA_NOCRIT_NOCACHE       ADI_MEM_DATA_NOCRIT_NOCACHE /*!< L3 Uncached data placement macro */
#define ADI_OASPI_MEM_CODE_CRIT                 ADI_MEM_CODE_CRIT           /*!< L2 Code placement macro */
#define ADI_OASPI_MEM_CODE_NOCRIT               ADI_MEM_CODE_NOCRIT         /*!< L3 Code placement macro */
#define ADI_OASPI_MEM_ISR                       ADI_MEM_IRQ                 /*!< ISR placement macro */
#define ADI_OASPI_MEM_ALIGN_4                   ADI_MEM_ALIGN4              /*!< 4 byte Memory alignment macro */
#define ADI_OASPI_MEM_ALIGN_16                  ADI_MEM_ALIGN16             /*!< 16 byte Memory alignment macro */

/*! \cond PRIVATE */
/*================================= DEFINES =================================*/
/*! \endcond */
/*================================ DATA TYPES ===============================*/
/*! @brief OA-SPI API status */
typedef enum ADI_OASPI_STATUS
{
    ADI_OASPI_STATUS_SUCCESS,       /*!< Success */
    ADI_OASPI_STATUS_QUEUE_FULL,    /*!< Queue full */
    ADI_OASPI_STATUS_UNK_ERROR,     /*!< Unknown error! */
} ADI_OASPI_STATUS;

/*! @brief OA-SPI error types */
typedef enum ADI_OASPI_ERR
{
    ADI_OASPI_ERR_DATA_HDR_BAD,        /*!< Header bad received in the data footer */
    ADI_OASPI_ERR_REG_WRITE_FAILED,    /*!< Register write failed */
    ADI_OASPI_ERR_REG_READ_FAILED,     /*!< Register read failed */
    ADI_OASPI_ERR_PARITY_FAILED,       /*!< Parity check failed in the receive chunk */
    ADI_OASPI_ERR_RX_FRAME_DROP,       /*!< Frame drop bit set in the receive chunk */
    ADI_OASPI_ERR_SYNC_LOST,           /*!< MAC-PHY sync lost (Reconfigure the MAC-PHY) */
    ADI_OASPI_ERR_RX_FRAME_CRC_FAILED, /*!< CRC error in the received frame, if CRC is validated by MAC_PHY then the error could be due SPI MISO line errors else it could be due to bit errors either in network line / SPI MISO line */
} ADI_OASPI_ERR;

/*! @brief Types of events from OA-SPI module */
typedef enum ADI_OASPI_EVT
{
    ADI_OASPI_EVT_TRANSMIT_CHUNKS,  /*!< Ask application to transmit data / control chunk(s) */
    ADI_OASPI_EVT_REG_READ,         /*!< Notify application of register read event */
    ADI_OASPI_EVT_REG_WRITE,        /*!< Notify application of register write event */
    ADI_OASPI_EVT_TX_FRAME,         /*!< Notify application of completion of frame send */
    ADI_OASPI_EVT_RX_FRAME,         /*!< Notify application of reception of a frame */
    ADI_OASPI_EVT_ERR,              /*!< Notify application of errors */
    ADI_OASPI_EVT_INT,              /*!< Notify application of assertion of extended status */
    ADI_OASPI_EVT_TERMINATED,       /*!< Notify application that the OA-SPI stack is terminated */
} ADI_OASPI_EVT;

/*! Callback function */
typedef void (*ADI_OASPI_CBK)(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);

/*! @brief OA-SPI module configuration structure */
typedef struct ADI_OASPI_CFG
{
    ADI_OASPI_CBK pCbk;               /*!< Pointer to the callback structure */
    void          *pCbParam;          /*!< This parameter will be passed back during invocation of callback */
    uint8_t       nChunkSize;         /*!< Chunk size */
    uint8_t       nCPS;               /*!< Chunk payload selector */
    bool          bAppendCrcbyHost;   /*!< Indicates whether the CRC for the TX frame is appended before reaching MAC */
    bool          bValidateCrcbyHost; /*!< Indicates whether the CRC for the RX frame is to be validated by host */
    /*!< Receive frame timestamp selection - if set to true 32-bit timestamps are captured,
        else 64-bit timestamps are captured (default, aligned with TRM) */
    bool          bUse32BitTimestamp; /*!< If true, use 32-bit timestamps; if false (default), use 64-bit timestamps */
    bool          bEnablePrioFifos;  /*!< If true, use dual HP/LP priority FIFOs with VS-based priority scheduling.
                                          If false (default), single FIFO with VS bits used for port routing. */
} ADI_OASPI_CFG;

/*! @brief Descriptor for control chunk transfer */
typedef struct ADI_OASPI_CTRL_DESC
{
    uint8_t         nLength;        /*!< Number of registers to write / read from */
    uint8_t         nMMS;           /*!< MMS (Memory map selector) of the registers */
    uint16_t        nStartAddr;     /*!< Address to start write / read from */
    uint32_t        *pRegData;      /*!< Pointer to the write / read data (Application should provide memory for this) */
    ADI_OASPI_CBK   pfCallback;     /*!< Callback function to notify write / read complete */
    void           *pCbParam;       /*!< This parameter will be passed back during invocation of callback */

    /* ~~~~~~~ Internal ~~~~~~~ */
    bool            bIsCtrlWrite;   /*!< Flag indicating whether the control descriptor is write / read
                                         (maintained internally, need not be filled by application) */
} ADI_OASPI_CTRL_DESC;

/*! @brief Descriptor for ethernet frame transfer */
typedef struct ADI_OASPI_DATA_DESC
{
    uint16_t        nFrameSize;       /*!< Size of the ethernet frame (in bytes) */
    uint8_t         *pFrameBuff;      /*!< Pointer to the ethernet frame (Application should provide memory for this) */
    uint8_t         nVendorSpecific;  /*!< Value to set in vendor specific bits */
    ADI_EGRESS_TS_REG eTsReg;         /*!< Egress timestamp capture settings (Valid for TX descriptor) */
    ADI_OASPI_CBK   pfCallback;       /*!< Callback function to notify frame transfer / receive complete */
    void           *pCbParam;         /*!< This parameter will be passed back during invocation of callback */
    bool
    bTimestampValid;  /*!< Flag indicating whether timestamp is valid - after validating the parity for timestamp */
    uint32_t        nIngressTimestampSec;     /*!< Ingress timestamp - Seconds field */
    uint32_t        nIngressTimestampNanosec; /*!< Ingress timestamp - Nanoseconds field */
    bool            bTimeStampPresent;     /*!< Flag indicating whether ingress timestamp is present in the frame */
} ADI_OASPI_DATA_DESC;

/*! @brief Data for #ADI_OASPI_EVT_TRANSMIT_CHUNKS event */
typedef struct ADI_OASPI_EVT_TRANSMIT_CHUNKS_DATA
{
    uint8_t     *pTxBuff;             /*!< SPI TX data */
    uint8_t     *pRxBuff;             /*!< SPI RX data */
    uint32_t    nSpiTrxLen;           /*!< SPI transfer length */
} ADI_OASPI_EVT_TRANSMIT_CHUNKS_DATA;

/*! Data for #ADI_OASPI_EVT_REG_READ event */
typedef ADI_OASPI_CTRL_DESC ADI_OASPI_EVT_REG_READ_DATA;

/*! Data for #ADI_OASPI_EVT_REG_WRITE event */
typedef ADI_OASPI_CTRL_DESC ADI_OASPI_EVT_REG_WRITE_DATA;

/*! Data for #ADI_OASPI_EVT_TX_FRAME event */
typedef ADI_OASPI_DATA_DESC ADI_OASPI_EVT_TX_FRAME_DATA;

/*! Data for #ADI_OASPI_EVT_RX_FRAME event */
typedef ADI_OASPI_DATA_DESC ADI_OASPI_EVT_RX_FRAME_DATA;

/*! @brief Data for #ADI_OASPI_EVT_ERR event */
typedef struct ADI_OASPI_EVT_ERR_DATA
{
    ADI_OASPI_ERR   eError;           /*!< Error type */
    void            *pData;           /*!< Data specific to error */
} ADI_OASPI_EVT_ERR_DATA;

/*! Data for #ADI_OASPI_EVT_INT event */
typedef void ADI_OASPI_EVT_INT_DATA;

/*! Data for #ADI_OASPI_EVT_TERMINATED event */
typedef void ADI_OASPI_EVT_TERMINATED_DATA;

/*================================ PROTOTYPES ===============================*/
/*! Initializes OA-SPI instance */
extern void adi_oaspi_Init(uint32_t nInstNum, const ADI_OASPI_CFG *poCfg);
/*! Terminates OA-SPI instance */
extern void adi_oaspi_Terminate(uint32_t nInstNum);
/*! Sets the sync bit for the OA-SPI instance */
extern void adi_oaspi_SetSync(uint32_t nInstNum, bool bIsSyncSet);
/*! Periodic OA-SPI instance tick function */
extern void adi_oaspi_Run(uint32_t nInstNum);
/*! Submit a register write request */
extern ADI_OASPI_STATUS adi_oaspi_WriteReg(uint32_t nInstNum, ADI_OASPI_CTRL_DESC *poDesc);
/*! Submit a register read request */
extern ADI_OASPI_STATUS adi_oaspi_ReadReg(uint32_t nInstNum, ADI_OASPI_CTRL_DESC *poDesc);
/*! Check if TX descriptor queue is full (bIsHp=true → HP FIFO, false → LP FIFO) */
extern bool adi_oaspi_IsTxDescQFull(uint32_t nInstNum, bool bIsHp);
/*! Submit a data descriptor for transmitting frames */
extern ADI_OASPI_STATUS adi_oaspi_SubmitTxDesc(uint32_t nInstNum, ADI_OASPI_DATA_DESC *poDesc);
/*! Submit a data descriptor for receiving frames */
extern ADI_OASPI_STATUS adi_oaspi_SubmitRxDesc(uint32_t nInstNum, ADI_OASPI_DATA_DESC *poDesc);
/*! Notify OA-SPI module of SPI completion */
extern void adi_oaspi_NotifySpiCompletion(uint32_t nInstNum);
/*! Process OA-SPI IRQ */
extern void adi_oaspi_ProcessIrq(uint32_t nInstNum);
/*! Is control TX buffer empty? */
extern bool adi_oaspi_IsCtrlTxBufEmpty(uint32_t nInstNum);
/*! Is data TX buffer empty? */
extern bool adi_oaspi_IsDataTxBufEmpty(uint32_t nInstNum);
/*! Is HW TX buffer empty? */
extern bool adi_oaspi_IsHWTxBufEmpty(uint32_t nInstNum);
/*! Configure the maximum transmit credits (TXC).
    In default mode (single FIFO), only nMaxTxcLp is used.
    In priority FIFO mode, nMaxTxcLp and nMaxTxcHp configure the LP and HP FIFOs respectively. */
extern void adi_oaspi_ConfigureMaxTxc(uint32_t nInstNum, uint8_t nMaxTxcLp, uint8_t nMaxTxcHp);
#ifdef ADI_SERDES_MODE
extern void adi_oaspi_ResetDataQueue(uint32_t nInstNum);
#endif
#endif /* ADI_OASPI_H */

/** @} */

/**
 * EOF: www.analog.com
 */
