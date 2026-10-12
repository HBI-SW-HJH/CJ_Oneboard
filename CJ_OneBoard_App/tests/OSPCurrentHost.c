// Compile OSP_WriteColor extracted unchanged from the production source.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "OSPControl.h"
#include "Verification.h"
#ifdef TEST_LIMIT_MA
#undef OSP_LOGO_CURRENT_LIMIT_MA
#define OSP_LOGO_CURRENT_LIMIT_MA TEST_LIMIT_MA
#endif
OSP_Control_t gOSPControl;
Verification_t gVerification;
static uint8_t sent[6];
static unsigned checks;
static void Check(int ok, const char *message)
{
    checks++;
    if (!ok) { fprintf(stderr, "FAIL %s\n", message); exit(1); }
}
static uint8_t OSP_Transfer(OSP_Port_t port, uint16_t address, uint8_t command, const uint8_t *data, uint8_t length, uint8_t *response, uint8_t responseLength, uint32_t delay)
{
    (void)port; (void)response; (void)responseLength; (void)delay;
    Check(address == OSP_ADDR_BROADCAST && command == OSP_CMD_SET_PWM && length == 6, "SETPWM framing unchanged");
    memcpy(sent, data, 6);
    return 1;
}
#include "OSPWriteColor.generated.h"
static void Test(uint16_t red, uint16_t green, uint16_t blue)
{
    const uint16_t input[3] = { red, green, blue };
    const uint32_t raw[3] = { (uint32_t)red * 32767 / 65535, (uint32_t)green * 32767 / 65535, (uint32_t)blue * 32767 / 65535 };
    const uint32_t total = raw[0] + raw[1] + raw[2];
    for (int port = 0; port < 2; port++)
    {
        gOSPControl.dirty[port] = 1;
        OSP_WriteColor((OSP_Port_t)port, red, green, blue);
        const uint16_t output[3] = { gOSPControl.pwmRed[port], gOSPControl.pwmGreen[port], gOSPControl.pwmBlue[port] };
        const uint32_t sum = (uint32_t)output[0] + output[1] + output[2];
        Check(gOSPControl.dirty[port] == 0, "Transmit clears dirty flag");
        if (port == OSP_PORT_LOGO && gVerification.ActiveFlag == 0) Check((uint64_t)sum * 50 <= (uint64_t)OSP_LOGO_CURRENT_LIMIT_MA * 32767, "Logo total nominal current stays below define");
        for (int c = 0; c < 3; c++)
        {
            Check((sent[c * 2] & 0x80) != 0, "Day current selection preserved");
            Check((((sent[c * 2] & 0x7F) << 8) | sent[c * 2 + 1]) == output[c], "Transmitted PWM matches diagnostic expected value");
            Check(output[c] <= raw[c], "Limiter never increases a channel");
            if (gVerification.ActiveFlag != 0 || port == OSP_PORT_STROBE || (uint64_t)total * 50 <= (uint64_t)OSP_LOGO_CURRENT_LIMIT_MA * 32767)
                Check(output[c] == raw[c], "Verification, Strobe and under-limit Logo unchanged");
            else
            {
                double ideal = total ? (double)raw[c] * (OSP_LOGO_CURRENT_LIMIT_MA * 32767.0 / 50.0) / total : 0;
                Check(fabs(output[c] - ideal) < 2.0, "RGB ratio preserved within integer rounding");
            }
            if (input[c] == 0) Check(output[c] == 0, "Zero channel remains zero");
        }
    }
}
int main(void)
{
    Test(0, 0, 0); Test(65535, 65535, 65535);
    Test(65535, 0, 0); Test(0, 65535, 0); Test(0, 0, 65535);
    Test(65535, 32768, 32768); Test(53738, 0, 0); Test(53740, 0, 0);
    uint32_t state = 0x12345678;
    for (int i = 0; i < 20000; i++)
    {
        state = state * 1664525U + 1013904223U; uint16_t r = state >> 16;
        state = state * 1664525U + 1013904223U; uint16_t g = state >> 16;
        state = state * 1664525U + 1013904223U; uint16_t b = state >> 16;
        Test(r, g, b);
    }
    gVerification.ActiveFlag = 1;
    Test(65535, 65535, 65535); Test(65535, 0, 0);
    Test(65535, 32768, 12345); Test(0, 0, 0);
    gVerification.ActiveFlag = 0;
    Test(65535, 65535, 65535);
    OSP_WriteColor(OSP_PORT_LOGO, 65535, 65535, 65535);
    printf("PASS OSP limit=%u mA, white PWM=%u/%u/%u, checks=%u\n", (unsigned)OSP_LOGO_CURRENT_LIMIT_MA,
        gOSPControl.pwmRed[0], gOSPControl.pwmGreen[0], gOSPControl.pwmBlue[0], checks);
    return 0;
}
