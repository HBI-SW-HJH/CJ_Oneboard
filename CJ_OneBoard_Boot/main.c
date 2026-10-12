#include "appConfig.h"

int main(void)
{
    SYS_Init();
    while (1)
    {
        LINT_Process();
        Reprogram_Process();
        if (oAppTimer.tickWdtMs >= WDT_CLEAR_MS)
        {
            oAppTimer.tickWdtMs = 0U;
            WDT_Clear();
        }
    }
}
