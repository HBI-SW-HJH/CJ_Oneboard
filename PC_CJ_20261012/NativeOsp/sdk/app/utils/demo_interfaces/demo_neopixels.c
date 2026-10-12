/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: demo_neopixels.c
 * \brief: The ADAFruit Neo Pixels class of the application
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "demo_neopixels.h"

#ifdef ADI_E2B_IFACE_SPI_ENABLED

/*================================ DATA TYPES ===============================*/

/*=================================== DATA ==================================*/
ADI_MEM_DATA_CRIT_CACHE
static DEMO_NEO_DATA goDemoNeoData;

/* Copy of the SPI interface handle for EAL access */
ADI_MEM_DATA_NOCRIT_CACHE
static ADI_EAL_SPI_HANDLE ghNeoPixelSpiIntfHandle;

/*================================ PROTOTYPES ===============================*/
static void _updateFrame(void);

/*=================================== CODE ==================================*/
/*!
 * \brief   Initializes the ADAFruit Neo Pixel
 */
ADI_MEM_CODE_NOCRIT
void demo_neopixelsInit(ADI_EAL_SPI_HANDLE hSpiHandle)
{
    ghNeoPixelSpiIntfHandle = hSpiHandle;

    /* Clear the data */
    (void)ADI_MEMSET(&goDemoNeoData, 0, sizeof(DEMO_NEO_DATA));

#if ADI_DEMO_NEOPIXEL_MODE == LENGTH_MODE
    goDemoNeoData.eMode = E_LENGTH;
#elif ADI_DEMO_NEOPIXEL_MODE == ANIMATION_MODE
    demo_neopixels_startAnimation();
#endif
}

/*!
 * \brief   Routine run function of the ADA Fruit Neo Pixel
 */
ADI_MEM_CODE_CRIT
void demo_neopixelsRun()
{
    uint64_t nCurrTime;

    /* Get the current time */
    adi_pal_getCurrTime(&nCurrTime);

    /* If we are in animation mode & time has elapsed */
    if((goDemoNeoData.eMode == E_ANIMATION)
            && ((nCurrTime - goDemoNeoData.nPrevSendTime) > (1ULL * (uint64_t)SEC_TO_NS)))
    {
        /* Set the length and send the frame */
        goDemoNeoData.nLength = goDemoNeoData.nAnimateLength;
        _updateFrame();

        /* Update time */
        goDemoNeoData.nPrevSendTime = nCurrTime;

        /* Update length */
        goDemoNeoData.nAnimateLength = (goDemoNeoData.nAnimateLength + 1U) % NEO_MAX_LENGTH;
    }

    if((goDemoNeoData.eMode == E_LENGTH)
            && ((nCurrTime - goDemoNeoData.nPrevSendTime) > (18ULL * (uint64_t)MS_TO_NS)))
    {
        /* Update time */
        goDemoNeoData.nPrevSendTime = nCurrTime;

        uint32_t nLength = goDemoNeoData.nNeoLength;
        uint32_t nFilledLength = nLength;

        if(nLength <= 1u)
        {
            nFilledLength = 1u;
        }
        else if(nLength >= NEO_MAX_LENGTH)
        {
            nFilledLength = NEO_MAX_LENGTH;
        }
        else
        {
            nFilledLength = nLength + 1u;
        }

        uint32_t nRemLength = NEO_MAX_LENGTH - nFilledLength;

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\rSensor value: %2d  \t", (uint16_t)goDemoNeoData.nNumOutput);
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "LED output:");

        app_utils_setConsoleBgColor(CONSOLE_BG_COLOR_GREEN);

        for(uint32_t i = 0u; i < nFilledLength; i++)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "|");
        }

        app_utils_setConsoleBgColor(CONSOLE_BG_COLOR_WHITE);

        for(uint32_t i = 0u; i < nRemLength; i++)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "-");
        }

        app_utils_resetConsoleBgColor();
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "  (%5.1f)%%", (((float)(goDemoNeoData.nNeoLength) / (float)NEO_MAX_LENGTH)) * 100);

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r");

        demo_neopixels_setLength(nLength);
    }
}

/*!
 * \brief Starts animation and ignores length based changes
 */
ADI_MEM_CODE_NOCRIT
void demo_neopixels_startAnimation(void)
{
    goDemoNeoData.eMode = E_ANIMATION;
    goDemoNeoData.nAnimateLength = 0U;
}

/*!
 * \brief Stops the animation and reverts to length based changes
 */
ADI_MEM_CODE_NOCRIT
void demo_neopixels_stopAnimation(void)
{
    goDemoNeoData.eMode = E_LENGTH;
    demo_neopixels_setLength(NEO_MAX_LENGTH);
}

/*!
 * \brief       Allows other applications to update the length of the LED lighting
 * \param       nValue  The new length value
 */
ADI_MEM_CODE_CRIT
void demo_neopixels_setLength(uint32_t nValue)
{
    if((goDemoNeoData.eMode == E_LENGTH) && (nValue != goDemoNeoData.nLength))
    {
        if(nValue >= (NEO_MAX_LENGTH - 1U))
        {
            goDemoNeoData.nLength = (NEO_MAX_LENGTH - 1U);
        }
        else if(nValue < 1U)
        {
            goDemoNeoData.nLength = 0U;
        }
        else
        {
            goDemoNeoData.nLength = nValue;
        }

        _updateFrame();
    }
}

ADI_MEM_CODE_CRIT
void demo_neopixelsUpdateOutput(uint32_t nLength, uint32_t nSensorOutput)
{
    goDemoNeoData.nNeoLength = nLength;
    goDemoNeoData.nNumOutput = nSensorOutput;
}

/*!
 * \brief       Allows applications to update the length of the LED lighting and the CLI representation unconditionally
 * \param       nValue  The new length value
 */
ADI_MEM_CODE_CRIT
void demo_neopixels_setLength_updateConsole(uint32_t nValue)
{
    if(goDemoNeoData.eMode == E_LENGTH)
    {
        if(nValue >= (NEO_MAX_LENGTH - 1U))
        {
            goDemoNeoData.nLength = (NEO_MAX_LENGTH - 1U);
        }
        else if(nValue <= 1U)
        {
            goDemoNeoData.nLength = 1U;
        }
        else
        {
            goDemoNeoData.nLength = nValue;
        }

        uint32_t nFilledLength = goDemoNeoData.nLength;
        uint32_t nRemLength = NEO_MAX_LENGTH - nFilledLength;

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "LED output:");

        app_utils_setConsoleBgColor(CONSOLE_BG_COLOR_GREEN);

        for(uint32_t i = 0u; i < nFilledLength; i++)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "|");
        }

        app_utils_setConsoleBgColor(CONSOLE_BG_COLOR_WHITE);

        for(uint32_t i = 0u; i < nRemLength; i++)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "-");
        }

        app_utils_resetConsoleBgColor();

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\n");

        _updateFrame();
    }
}

/*================================== STATIC =================================*/
/*! Updates the frame based on the intensity */
ADI_MEM_CODE_CRIT
static void _updateFrame(void)
{
    uint32_t i;
    ADI_EAL_STATUS eEalStatus;
    uint8_t anNeoRgbData[NEO_MAX_LENGTH][NEO_CHANNELS];

    /* Form the data */
    (void)ADI_MEMSET(anNeoRgbData, 20, sizeof(anNeoRgbData));

    for(i = goDemoNeoData.nLength + 1U; i >= 1U; i--)
    {
        anNeoRgbData[i - 1][0U] = NEO_GREEN_INTENSITY_VAL;
        anNeoRgbData[i - 1][1U] = 0u;
        anNeoRgbData[i - 1][2U] = 0u;
    }

    ADI_EAL_SPI_IO_DATA oSpiIOData;
    oSpiIOData.pTxData = &anNeoRgbData[0U][0U];
    oSpiIOData.nWriteLength = (NEO_MAX_LENGTH * NEO_CHANNELS);
    oSpiIOData.apRxData = NULL;
    oSpiIOData.nReadLength = 0U;
    oSpiIOData.nChipSel = 0xFEU;

    eEalStatus = adi_eal_spiWrite(ghNeoPixelSpiIntfHandle, &oSpiIOData, 0U);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Could not Write SPI transaction");
}

#endif /* ADI_E2B_IFACE_SPI_ENABLED */

/** @} */

/*
 * EOF: https://www.analog.com/
*/
