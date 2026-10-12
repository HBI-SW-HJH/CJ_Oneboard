/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: dll_wrappers.h
 * \brief: The DLL header
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup pal
 *  @{
 */

#ifndef DLL_WRAPPERS_H
#define DLL_WRAPPERS_H

/*================================= INCLUDES ================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "ftd2xx.h"
#include "LibFT4222.h"
#include "pcap.h"
#include <windows.h>
/*================================= DEFINES =================================*/
/*! The function pointers to the pcap library APIs */
typedef const char *(*API_pcap_lib_version)(void);
typedef int (*API_pcap_findalldevs_ex)(const char *source, struct pcap_rmtauth *auth, pcap_if_t **alldevs,
                                       char *errbuf);
typedef void (*API_pcap_freealldevs)(pcap_if_t *);
typedef pcap_t *(*API_pcap_open)(const char *source, int snaplen, int flags, int read_timeout,
                                 struct pcap_rmtauth *auth, char *errbuf);
typedef int (*API_pcap_setnonblock)(pcap_t *, int, char *);
typedef int (*API_pcap_dispatch)(pcap_t *, int, pcap_handler, u_char *);
typedef int (*API_pcap_inject)(pcap_t *, const void *, size_t);
typedef void (*API_pcap_close)(pcap_t *);
typedef int (*API_pcap_stats)(pcap_t *, struct pcap_stat *);

/*! The function pointers to the FTDI library APIs */
typedef FT_STATUS(*API_FT_CreateDeviceInfoList)(LPDWORD lpdwNumDevs);
typedef FT_STATUS(*API_FT_GetDeviceInfoDetail)(DWORD dwIndex, LPDWORD lpdwFlags, LPDWORD lpdwType, LPDWORD lpdwID,
        LPDWORD lpdwLocId, LPVOID lpSerialNumber, LPVOID lpDescription, FT_HANDLE *pftHandle);
typedef FT_STATUS(*API_FT_Open)(int deviceNumber, FT_HANDLE *pHandle);
typedef FT_STATUS(*API_FT_OpenEx)(PVOID pArg1, DWORD Flags, FT_HANDLE *pHandle);
typedef FT_STATUS(*API_FT_Close)(FT_HANDLE ftHandle);
typedef FT_STATUS(*API_FT_SetTimeouts)(FT_HANDLE ftHandle, ULONG ReadTimeout, ULONG WriteTimeout);
typedef FT4222_STATUS(*API_FT4222_SetClock)(FT_HANDLE ftHandle, FT4222_ClockRate clk);
typedef FT4222_STATUS(*API_FT4222_SPIMaster_Init)(FT_HANDLE ftHandle, FT4222_SPIMode ioLine, FT4222_SPIClock clock,
        FT4222_SPICPOL cpol, FT4222_SPICPHA cpha, uint8 ssoMap);
typedef FT4222_STATUS(*API_FT4222_SPIMaster_SingleReadWrite)(FT_HANDLE ftHandle, uint8 *readBuffer, uint8 *writeBuffer,
        uint16 bufferSize, uint16 *sizeTransferred, BOOL isEndTransaction);
typedef FT4222_STATUS(*API_FT4222_UnInitialize)(FT_HANDLE ftHandle);
typedef FT4222_STATUS(*API_FT4222_SetSuspendOut)(FT_HANDLE ftHandle, BOOL enable);
typedef FT4222_STATUS(*API_FT4222_GPIO_Init)(FT_HANDLE ftHandle, GPIO_Dir *gpioDir);
typedef FT4222_STATUS(*API_FT4222_GPIO_Write)(FT_HANDLE ftHandle, GPIO_Port portNum, BOOL bValue);
typedef FT4222_STATUS(*API_FT4222_GPIO_Read)(FT_HANDLE ftHandle, GPIO_Port portNum, BOOL *value);
typedef FT4222_STATUS(*API_FT4222_SetWakeUpInterrupt)(FT_HANDLE ftHandle, BOOL enable);
typedef FT4222_STATUS(*API_FT4222_SetInterruptTrigger)(FT_HANDLE ftHandle, GPIO_Trigger trigger);

/*================================ DATA TYPES ===============================*/
/*! Structure for FTDI LIB */
typedef struct ADI_FTDI_LIB
{
    HINSTANCE hLibModule;                             /*! Device handle */
    API_FT_CreateDeviceInfoList getDeviceInfo;        /*! Find all the FTDI devices connected */
    API_FT_GetDeviceInfoDetail getDeviceInfoDetail;   /*! Loop through all the available FTDI devices */
    API_FT_Open openDevice;                           /*! Open the FTDI device */
    API_FT_OpenEx openDeviceEx;                       /*! Open the FTDI device by location */
    API_FT_Close closeDevice;                         /*! Close the FTDI device */
    API_FT_SetTimeouts setTimeouts;                   /*! Sets the timeout for the FTDI device */
} ADI_FTDI_LIB;

/*! Structure for FT4222 LIB */
typedef struct ADI_FT4222_LIB
{
    HINSTANCE hLibModule;                              /*! Device handle */
    API_FT4222_SetClock setClock;                      /*! Set the clock */
    API_FT4222_SPIMaster_Init spiMasterInit;           /*! Initialize the SPI Master */
    API_FT4222_SPIMaster_SingleReadWrite spiReadWrite; /*! Used to read or write SPI Master */
    API_FT4222_UnInitialize unInitialize;              /*! Uninitialize the SPI Master */
    API_FT4222_SetSuspendOut setSuspendOut;            /*! Disable suspend out, it uses the same pin as GPIO2 */
    API_FT4222_GPIO_Init gpioInit;                     /*! Initialize the GPIO */
    API_FT4222_GPIO_Write gpioWrite;                   /*! GPIO Write */
    API_FT4222_GPIO_Read gpioRead;                     /*! GPIO Read */
    API_FT4222_SetWakeUpInterrupt setWakeUpInterrupt;  /*! Set Wake up interrupt */
    API_FT4222_SetInterruptTrigger setInterruptTrigger; /*! Set interrupt trigger */
} ADI_FT4222_LIB;

/*! Structure for PCAP LIB*/
typedef struct ADI_PCAP_LIB
{
    HINSTANCE hLibModule;                                   /*! Device handle */
    API_pcap_findalldevs_ex findAllDevs;                    /*! Retrieve the device list from the local machine */
    API_pcap_freealldevs freeAllDevs;                       /*! Free the device list */
    API_pcap_open openDevice;                               /*! Open the pcap device */
    API_pcap_setnonblock setNonBlock;                       /*! Set non-blocking mode and start the capture */
    API_pcap_dispatch dispatch;                             /*! Dispatch the pcap device */
    API_pcap_inject inject ;                                /*! Send the packet to the opened ethernet adapter */
    API_pcap_close closeDevice;                             /*! Close the adapter */
    API_pcap_stats pcapStats;                               /*! Get the stats */
} ADI_PCAP_LIB;



/*============================ PUBLIC PROTOTYPES ============================*/
extern bool InitFtdiDll(ADI_FTDI_LIB *pLib);            /*! Initialise FTDI_LIB */
extern bool InitFt4222Dll(ADI_FT4222_LIB *pLib);        /*! Initialise FT4222_LIB */
extern bool InitPcapLib(ADI_PCAP_LIB *pLib);            /*! Initialise PCAP_LIB */
extern void CloseFtdiDll(ADI_FTDI_LIB *pLib);           /*! Close FTDI_LIB */
extern void CloseFt4222Dll(ADI_FT4222_LIB *pLib);       /*! Close FTDI_LIB */
extern void ClosePcapLib(ADI_PCAP_LIB *pLib);           /*! Close FTDI_LIB */

#endif /* DLL_WRAPPERS_H */

/** @} */

/*
 * EOF: www.analog.com
 */
