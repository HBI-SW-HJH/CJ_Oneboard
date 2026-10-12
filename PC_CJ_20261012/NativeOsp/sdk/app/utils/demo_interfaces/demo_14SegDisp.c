/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: demo_14SegDisp.c
 * \brief: The 14-Segment display demo implementation
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "demo_14SegDisp.h"

#ifdef ADI_E2B_IFACE_I2C_ENABLED

/*================================= DEFINES =================================*/

/*=================================== DATA ==================================*/
ADI_MEM_DATA_CRIT_CACHE
static DEMO_SEGDISP_DATA gahDisplayData[MAX_NUM_DISPLAY];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t nDisplayNum;

ADI_MEM_DATA_CONST
static const uint16_t sevensegLookUptable[] =
{
    0x4000u, /* . */
    0x0C00u, /* / */
    0x0C3Fu, /* 0 */
    0x0006u, /* 1 */
    0x00DBu, /* 2 */
    0x008Fu, /* 3 */
    0x00E6u, /* 4 */
    0x00EDu, /* 5 */
    0x00FDu, /* 6 */
    0x0007u, /* 7 */
    0x00FFu, /* 8 */
    0x00EFu, /* 9 */
    0x1200u, /* : */
    0x0A00u, /* ; */
    0x2400u, /* < */
    0x00C8u, /* = */
    0x0900u, /* > */
    0x1083u, /* ? */
    0x02BBu, /* @ */
    0x00F7u, /* A */
    0x128Fu, /* B */
    0x0039u, /* C */
    0x120Fu, /* D */
    0x00F9u, /* E */
    0x0071u, /* F */
    0x00BDu, /* G */
    0x00F6u, /* H */
    0x1209u, /* I */
    0x001Eu, /* J */
    0x2470u, /* K */
    0x0038u, /* L */
    0x0536u, /* M */
    0x2136u, /* N */
    0x003Fu, /* O */
    0x00F3u, /* P */
    0x203Fu, /* Q */
    0x20F3u, /* R */
    0x00EDu, /* S */
    0x1201u, /* T */
    0x003Eu, /* U */
    0x0C30u, /* V */
    0x2836u, /* W */
    0x2D00u, /* X */
    0x1500u, /* Y */
    0x0C09u, /* Z */
    0x0039u, /* [ */
    0x2100u, /*   */
    0x000Fu, /* ] */
    0x0C03u, /* ^ */
    0x0008u, /* _ */
    0x0100u, /* ` */
    0x00DFu, /* a */
    0x00FCu, /* b */
    0x00D8u, /* c */
    0x00DEu, /* d */
    0x00FBu, /* e */
    0x0071u, /* f */
    0x048Eu, /* g */
    0x1070u, /* h */
    0x1200u, /* i */
    0x000Eu, /* j */
    0x3600u, /* k */
    0x0030u, /* l */
    0x10D4u, /* m */
    0x1050u, /* n */
    0x00DCu, /* o */
    0x0170u, /* p */
    0x0486u, /* q */
    0x0050u, /* r */
    0x2088u, /* s */
    0x0078u, /* t */
    0x001Cu, /* u */
    0x2004u, /* v */
    0x2814u, /* w */
    0x28C0u, /* x */
    0x200Cu, /* y */
    0x0848u, /* z */
};

ADI_MEM_DATA_CRIT_CACHE
static char aAnim1Texts[ANIM1_SEQ_LEN][4u] = { "ADI ", " E2B"};
ADI_MEM_DATA_CRIT_CACHE
static uint8_t aAnim2[ANIM2_SEQ_LEN][9u] =
{
    {0x00u, 0x00u, 0x2du, 0x00u, 0x2du, 0x00u, 0x2du, 0x00u, 0x2du},
    {0x00u, 0xc0u, 0x12u, 0xc0u, 0x12u, 0xc0u, 0x12u, 0xc0u, 0x12u}
};
/*================================ PROTOTYPES ===============================*/
static void RunAnimation1(DEMO_SEGDISP_DATA *pDisplay);
static void RunAnimation2(DEMO_SEGDISP_DATA *pDisplay);

/*=================================== CODE ==================================*/
/**
 * @brief Initializes the 14-Segment LED display
 * @param hI2cHandle  The I2C handle
 * @param hDispHandle Pointer to Display handle
 */
ADI_MEM_CODE_NOCRIT
void demo_displayInit(ADI_EAL_I2C_HANDLE hI2cHandle, DISPLAY_HANDLE *hDispHandle)
{
    ADI_EAL_STATUS eEalStatus;

    ADI_DBG_REQUIRE(nDisplayNum < MAX_NUM_DISPLAY, "Max number of instances created for demo display app");
    ADI_DBG_REQUIRE(hDispHandle != NULL, "Pointer to Display handle is null");

    DEMO_SEGDISP_DATA *pDisplay = &gahDisplayData[nDisplayNum];
    nDisplayNum ++;

    ADI_MEMSET((void *)pDisplay, 0u, sizeof(DEMO_SEGDISP_DATA));

    pDisplay->hI2cInfHandle = hI2cHandle;

    *hDispHandle = (void *)pDisplay;

    uint8_t anbuffTx[3u] = {0x21u, //Enable internal oscillator
                            0x81u, //Enable Display
                            0xEFu  //Set Brightness oscillator
                           };

    ADI_EAL_I2C_IO_DATA oI2cData =
    {
        .nAddress = SLAVE_ADDRESS,
        .nWriteLength = 1u,
        .pTxData = &anbuffTx[0u],
    };

    for(uint8_t i = 0u; i < 3u; i++)
    {
        eEalStatus = adi_eal_i2cWriteSync(pDisplay->hI2cInfHandle, &oI2cData, 0xABu);
        ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Write init failed");
        oI2cData.pTxData++;
    }
}

/**
 * @brief Clears the 14-Segment LED display data
 * @param hDispHandle Pointer to Display handle
 */
ADI_MEM_CODE_NOCRIT
void demo_displayClear(DISPLAY_HANDLE hDispHandle)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anBuffTx[LED_DRIVER_RAM_SIZE] = {0u};

    ADI_DBG_REQUIRE(hDispHandle != NULL, "Pointer to Display handle is null");
    DEMO_SEGDISP_DATA *pDisplay = (DEMO_SEGDISP_DATA *)hDispHandle;

    ADI_EAL_I2C_IO_DATA oI2cData =
    {
        .nAddress = SLAVE_ADDRESS,
        .nWriteLength = sizeof(anBuffTx),
        .pTxData = anBuffTx,
    };

    eEalStatus = adi_eal_i2cWriteSync(pDisplay->hI2cInfHandle, &oI2cData, 0xCDu);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Write sync failed");
}

/**
 * @brief Writing Sensor output to the display
 * @param hDispHandle Pointer to Display handle
 * @param nNum Sensor output value
 */
ADI_MEM_CODE_NOCRIT
void demo_displayWriteNum(DISPLAY_HANDLE hDispHandle, float fNum)
{
    ADI_EAL_STATUS eEalStatus;

    bool bflag = false;
    char str[6u];
    float tmp = 999.0f;
    uint8_t anBuffTx[LED_DRIVER_RAM_SIZE] = {0u};
    uint8_t nLC = 0u;

    ADI_DBG_REQUIRE(hDispHandle != NULL, "Pointer to Display handle is null");
    DEMO_SEGDISP_DATA *pDisplay = (DEMO_SEGDISP_DATA *)hDispHandle;

    /* Max number displayed can be 999 */
    if(fNum < tmp)
    {
        tmp = fNum;
    }

    if(tmp >= 100.0)
    {
        /* converting float to string and truncating fraction part */
        ADI_SPRINTF(str, "%-4.0f", tmp);
    }
    else
    {
        /* converting float to string */
        ADI_SPRINTF(str, "%4.1f", tmp);
    }

    for(nLC = 0u; nLC < ADI_STRLEN(str); nLC++)
    {
        uint16_t nCode = 0x0000u;

        /* Check if there is no spaces in string */
        if(str[nLC] != 0x20u)
        {
            nCode = sevensegLookUptable[str[nLC] - 46u];
        }

        /* Check if there is decimal point in the string */
        if(str[nLC] == 0x2Eu)
        {
            anBuffTx[1u + (2u * nLC) - 1] |= 0x40;
            anBuffTx[2u + (2u * nLC) - 1] |= 0x00u;
            bflag = true;
        }
        else
        {
            if(bflag == false)
            {
                anBuffTx[1u + 2u * nLC] = (uint8_t)nCode & 0xFFu;
                anBuffTx[2u + 2u * nLC] = (uint8_t)(nCode >> 8u) & 0xFFu;
            }
            else
            {
                anBuffTx[1u + (2u * nLC) - 2] = (uint8_t)nCode & 0xFFu;
                anBuffTx[2u + (2u * nLC) - 2] = (uint8_t)(nCode >> 8u) & 0xFFu;
            }
        }
    }

    /* Appending P character */
    anBuffTx[1u + (2u * nLC) - 2] = 0xF3;
    anBuffTx[2u + (2u * nLC) - 2] = 0x00;

    /* Write to display driver */
    ADI_EAL_I2C_IO_DATA oI2cData =
    {
        .nAddress = SLAVE_ADDRESS,
        .nWriteLength = sizeof(anBuffTx),
        .pTxData = anBuffTx,
    };

    eEalStatus = adi_eal_i2cWriteSync(pDisplay->hI2cInfHandle, &oI2cData, 0xABu);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Write sync failed");
}

/**
 * @brief Routine run function of the 14-Segment display
 * @param hDispHandle Pointer to Display handle
 */
ADI_MEM_CODE_CRIT
void demo_displayRun(DISPLAY_HANDLE hDispHandle)
{
    uint64_t nCurrTime;

    ADI_DBG_REQUIRE(hDispHandle != NULL, "Pointer to Display handle is null");
    DEMO_SEGDISP_DATA *pDisplay = (DEMO_SEGDISP_DATA *)hDispHandle;

    /* Get the current time */
    adi_pal_getCurrTime(&nCurrTime);

    /* If we are in static mode & time has elapsed */
    if((pDisplay->eMode == DISP_MODE_STATIC)
            && ((nCurrTime - pDisplay->nPrevWriteTime) > (20ULL * (uint64_t)MS_TO_NS)))
    {
        demo_displayWriteNum(hDispHandle, pDisplay->fNumOutput);
        pDisplay->nPrevWriteTime = nCurrTime;
    }

    /* If we are in animation mode & time has elapsed */
    else if((pDisplay->eMode == DISP_MODE_ANIMATION_1)
            && ((nCurrTime - pDisplay->nPrevWriteTime) > (2ULL * (uint64_t)SEC_TO_NS)))
    {
        RunAnimation1(hDispHandle);
        pDisplay->nPrevWriteTime = nCurrTime;
    }
    /* If we are in animation mode & time has elapsed */
    else if((pDisplay->eMode == DISP_MODE_ANIMATION_2)
            && ((nCurrTime - pDisplay->nPrevWriteTime) > (200ULL * (uint64_t)MS_TO_NS)))
    {
        RunAnimation2(hDispHandle);
        pDisplay->nPrevWriteTime = nCurrTime;
    }
}

/**
 * @brief Updating Sensor output to the display
 * @param hDispHandle Pointer to Display handle
 * @param nNumOutput Sensor output value
 */
ADI_MEM_CODE_NOCRIT
void demo_displayUpdateNumOutput(DISPLAY_HANDLE hDispHandle, float fNumOutput)
{
    ADI_DBG_REQUIRE(hDispHandle != NULL, "Pointer to Display handle is null");
    DEMO_SEGDISP_DATA *pDisplay = (DEMO_SEGDISP_DATA *)hDispHandle;

    pDisplay->fNumOutput = fNumOutput;

    /* Max number displayed can be 9999 */
    if(fNumOutput > 9999.0f)
    {
        pDisplay->fNumOutput = 9999.0f;
    }
}

/**
 * @brief Set Display to different modes
 * @param hDispHandle Pointer to Display handle
 * @param eMode Display mode
 */
ADI_MEM_CODE_NOCRIT
void demo_displaySetMode(DISPLAY_HANDLE hDispHandle, DISPLAY_MODE eMode)
{
    ADI_DBG_REQUIRE(hDispHandle != NULL, "Pointer to Display handle is null");
    DEMO_SEGDISP_DATA *pDisplay = (DEMO_SEGDISP_DATA *)hDispHandle;

    pDisplay->eMode = eMode;
}

/*=================================== STATIC ==================================*/
/**
 * @brief Display animation-1 pattern
 * @param pDisplay Pointer to Display data
 */
ADI_MEM_CODE_NOCRIT
static void RunAnimation1(DEMO_SEGDISP_DATA *pDisplay)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anBuffTx[LED_DRIVER_RAM_SIZE] = {0u};

    char *str = &aAnim1Texts[pDisplay->nAnim1Seq][0u];

    pDisplay->nAnim1Seq = (pDisplay->nAnim1Seq + 1u) % ANIM1_SEQ_LEN;

    for(uint8_t i = 0u; i < 4u; i++)
    {
        uint16_t nCode = 0x0000u;

        if((str[i] != '\0') && (str[i] >= '0' && str[i] <= 'z'))
        {
            nCode = sevensegLookUptable[str[i] - 46u];
        }

        anBuffTx[1u + 2u * i] = (uint8_t)nCode & 0xFFu;
        anBuffTx[2u + 2u * i] = (uint8_t)(nCode >> 8u) & 0xFFu;
    }

    /* Write to display driver */
    ADI_EAL_I2C_IO_DATA oI2cData =
    {
        .nAddress = SLAVE_ADDRESS,
        .nWriteLength = sizeof(anBuffTx),
        .pTxData = anBuffTx,
    };

    eEalStatus = adi_eal_i2cWriteSync(pDisplay->hI2cInfHandle, &oI2cData, 0xABu);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Write sync failed");
}

/**
 * @brief Display animation-2 pattern
 * @param pDisplay Pointer to Display data
 */
ADI_MEM_CODE_NOCRIT
static void RunAnimation2(DEMO_SEGDISP_DATA *pDisplay)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anBuffTx[LED_DRIVER_RAM_SIZE] = {0u};

    pDisplay->nAnim2Seq = (pDisplay->nAnim2Seq + 1u) % ANIM2_SEQ_LEN;

    ADI_MEMCPY(&anBuffTx[1u], &aAnim2[pDisplay->nAnim2Seq][1u], 8u);

    /* Write to display driver */
    ADI_EAL_I2C_IO_DATA oI2cData =
    {
        .nAddress = SLAVE_ADDRESS,
        .nWriteLength = sizeof(anBuffTx),
        .pTxData = anBuffTx,
    };

    eEalStatus = adi_eal_i2cWriteSync(pDisplay->hI2cInfHandle, &oI2cData, 0xABu);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Write sync failed");
}

#endif /* ADI_E2B_IFACE_I2C_ENABLED */
