/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: dll_wrapper.c
 * \brief:File to intialize and close FTDI and PCAP handles.
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup pal
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "pcap.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>   /*! For printing */
#include "adi_pal.h" /*!< The class header */
#include "adi_dbg.h" /*!< Diagnostics */
#include "adi_memmap.h" /*! Memory map */
#include "ftd2xx.h"
#include "LibFT4222.h"
#include <time.h>
#include "dll_wrappers.h"
/*================================= DEFINES =================================*/

/*============================ SUPPRESS WARNING =============================*/
#if defined(_MSC_VER)
#pragma warning(disable:4191)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

/*================================ DATA TYPES ===============================*/


/*================================ PROTOTYPES ===============================*/


/*=================================== DATA ==================================*/


/*=================================== CODE ==================================*/

/**
 * @brief Initialise FTDI_LIB
 *
 * @param pLib pointer pointing to address of FTDI handler
 * @return true when the dll is initialised
 * @return flase when the dll is not initialised
 */
bool InitFtdiDll(ADI_FTDI_LIB *pLib)
{
    bool bSuccess = false;
    pLib->hLibModule  = LoadLibrary("ftd2xx.dll");

    if(pLib->hLibModule != NULL)
    {
        pLib->getDeviceInfo = (API_FT_CreateDeviceInfoList)GetProcAddress(pLib->hLibModule, "FT_CreateDeviceInfoList");
        pLib->getDeviceInfoDetail = (API_FT_GetDeviceInfoDetail)GetProcAddress(pLib->hLibModule, "FT_GetDeviceInfoDetail");
        pLib->openDevice = (API_FT_Open)GetProcAddress(pLib->hLibModule, "FT_Open");
        pLib->openDeviceEx = (API_FT_OpenEx)GetProcAddress(pLib->hLibModule, "FT_OpenEx");
        pLib->closeDevice = (API_FT_Close)GetProcAddress(pLib->hLibModule, "FT_Close");
        pLib->setTimeouts = (API_FT_SetTimeouts)GetProcAddress(pLib->hLibModule, "FT_SetTimeouts");
        bSuccess = true;
    }

    return bSuccess;
}

/**
 * @brief Initialise FT4222_LIB
 *
 * @param pLib pointer pointing to address of FT4222 handler
 * @return true when the dll is initialised
 * @return flase when the dll is not initialised
 */
bool InitFt4222Dll(ADI_FT4222_LIB *pLib)
{
    bool bSuccess = false;
    pLib->hLibModule  = LoadLibrary("LibFT4222-64.dll");

    if(pLib->hLibModule != NULL)
    {
        pLib->setClock = (API_FT4222_SetClock)GetProcAddress(pLib->hLibModule, "FT4222_SetClock");
        pLib->spiMasterInit = (API_FT4222_SPIMaster_Init)GetProcAddress(pLib->hLibModule, "FT4222_SPIMaster_Init");
        pLib->spiReadWrite = (API_FT4222_SPIMaster_SingleReadWrite)GetProcAddress(pLib->hLibModule,
                             "FT4222_SPIMaster_SingleReadWrite");
        pLib->unInitialize = (API_FT4222_UnInitialize)GetProcAddress(pLib->hLibModule, "FT4222_UnInitialize");
        pLib->setSuspendOut = (API_FT4222_SetSuspendOut)GetProcAddress(pLib->hLibModule, "FT4222_SetSuspendOut");
        pLib->gpioInit = (API_FT4222_GPIO_Init)GetProcAddress(pLib->hLibModule, "FT4222_GPIO_Init");
        pLib->gpioWrite = (API_FT4222_GPIO_Write)GetProcAddress(pLib->hLibModule, "FT4222_GPIO_Write");
        pLib->gpioRead = (API_FT4222_GPIO_Read)GetProcAddress(pLib->hLibModule, "FT4222_GPIO_Read");
        pLib->setWakeUpInterrupt = (API_FT4222_SetWakeUpInterrupt)GetProcAddress(pLib->hLibModule, "FT4222_SetWakeUpInterrupt");
        pLib->setInterruptTrigger = (API_FT4222_SetInterruptTrigger)GetProcAddress(pLib->hLibModule,
                                    "FT4222_SetInterruptTrigger");
        bSuccess = true;
    }

    return bSuccess;

}

/**
 * @brief Initialise PCAP_LIB
 *
 * @param pLib pointer pointing to address of PCAP handler
 *
 * @return true when the dll is initialised
 * @return flase when the dll is not initialised
 */
/*! Function for Initializing PCAP_LIB*/
bool InitPcapLib(ADI_PCAP_LIB *pLib)
{
    bool bSuccess = false;
    pLib->hLibModule = LoadLibrary("wpcap.dll");

    if(pLib->hLibModule != NULL)
    {
        pLib->findAllDevs = (API_pcap_findalldevs_ex)GetProcAddress(pLib->hLibModule, "pcap_findalldevs_ex");
        pLib->freeAllDevs = (API_pcap_freealldevs)GetProcAddress(pLib->hLibModule, "pcap_freealldevs");
        pLib->openDevice = (API_pcap_open)GetProcAddress(pLib->hLibModule, "pcap_open");
        pLib->setNonBlock = (API_pcap_setnonblock)GetProcAddress(pLib->hLibModule, "pcap_setnonblock");
        pLib->dispatch = (API_pcap_dispatch)GetProcAddress(pLib->hLibModule, "pcap_dispatch");
        pLib->inject = (API_pcap_inject)GetProcAddress(pLib->hLibModule, "pcap_inject");
        pLib->closeDevice = (API_pcap_close)GetProcAddress(pLib->hLibModule, "pcap_close");
        pLib->pcapStats = (API_pcap_stats)GetProcAddress(pLib->hLibModule, "pcap_stats");
        bSuccess = true;
    }

    return bSuccess;
}

/**
 * @brief Closes the loaded dll and resets the library handle
 *
 * @param pLib Pointer to the ADI_FTDI_LIB library object
 *
 */
void CloseFtdiDll(ADI_FTDI_LIB *pLib)
{
    /* If library was loaded, free the dll and clear the function addresses */
    if(pLib->hLibModule != NULL)
    {
        FreeLibrary(pLib->hLibModule);
        (void)memset(pLib, 0, sizeof(ADI_FTDI_LIB));
    }

}

/**
 * @brief Closes the loaded dll and resets the library handle
 *
 * @param pLib Pointer to the ADI_FT4222_LIB library object
 *
 */
void CloseFt4222Dll(ADI_FT4222_LIB *pLib)
{
    /* If library was loaded, free the dll and clear the function addresses */
    if(pLib->hLibModule != NULL)
    {
        FreeLibrary(pLib->hLibModule);
        (void)memset(pLib, 0, sizeof(ADI_FT4222_LIB));
    }
}

/**
 * @brief Closes the loaded dll and resets the library handle
 *
 * @param pLib Pointer to the ADI_PCAP_LIB library object
 *
 */
void ClosePcapLib(ADI_PCAP_LIB *pLib)
{
    /* If library was loaded, free the dll and clear the function addresses */
    if(pLib->hLibModule != NULL)
    {
        FreeLibrary(pLib->hLibModule);
        (void)memset(pLib, 0, sizeof(ADI_PCAP_LIB));
    }
}

/*============================ ENABLE WARNING =============================*/
#if defined(_MSC_VER)
#pragma warning(default:4191)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/** @} */

/*
 * EOF: www.analog.com
*/
