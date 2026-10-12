// @file    main.c
// @brief   CJ 4DPLEX Logo Lamp - application entry point
//
// @copyright 2026 CJ 4DPLEX

#include "appConfig.h"

int main(void)
{
    SYS_Init();

    //Test
    /*
    LedControlCommand_t command = { 0 };

    command.ledMask = 0x3FU;
    command.mode = 7U;
    command.active = 1U;
    //26 - 10% // 255 - max
    command.red = 255U * 257U;
    command.green = 255U * 257U;
    command.blue = 255U * 257U;
    command.timeMs = oAppTimer.timeMs;

    SRGBControl_Set(&command);
    OSPControl_Set(&command);
        */
  
    while (1)
    {
        LINT_Process();
        Reprogram_Process();

        if (oAppTimer.tick1ms >= 1U)
        {
            oAppTimer.tick1ms = 0U;
            SRGBControl_Process();
            OSPControl_Process();
            Verification_Process();
        }

        if (oAppTimer.tick10ms >= 10U)
        {
            oAppTimer.tick10ms = 0U;
            Power_Read();
        }
        //자동 검사시 해당 read시 플러커 1ms에 지연으로 간헐적 오차로 fail 발생 -20261008 jhhan
        if (gVerification.ActiveFlag == 0U)
        {
            if (oAppTimer.tick100ms >= 100U)
            {
                oAppTimer.tick100ms = 0U;
                SRGBControl_Read();
            }

            if (oAppTimer.tick100ms_1 >= 100U)
            {
                oAppTimer.tick100ms_1 = 0U;
                OSPControl_Read(OSP_PORT_LOGO);
            }

            if (oAppTimer.tick100ms_2 >= 100U)
            {
                oAppTimer.tick100ms_2 = 0U;
                OSPControl_Read(OSP_PORT_STROBE);
            }

            if (oAppTimer.tick2s >= 2000U)
            {
                oAppTimer.tick2s = 0U;
                OSPControl_CheckLogoOpen();
                OSPControl_CheckStrobeOpen();
            }
        }

        if (oAppTimer.tickWdtMs >= WDT_CLEAR_MS)
        {
            oAppTimer.tickWdtMs = 0U;
#if WDT_ENABLE == 1U
            WDT_Clear();
#endif
        }
    }
}

