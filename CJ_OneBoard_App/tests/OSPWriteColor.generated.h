static void OSP_WriteColor(OSP_Port_t port, uint16_t red, uint16_t green, uint16_t blue)
{
    uint8_t payload[6U];
    uint32_t pwmSum;
    uint32_t pwmLimit;

    gOSPControl.pwmRed[port] = (uint16_t)(((uint32_t)red * OSP_PWM_MAX) / 65535U);
    gOSPControl.pwmGreen[port] = (uint16_t)(((uint32_t)green * OSP_PWM_MAX) / 65535U);
    gOSPControl.pwmBlue[port] = (uint16_t)(((uint32_t)blue * OSP_PWM_MAX) / 65535U);
    if ((port == OSP_PORT_LOGO) && (gVerification.ActiveFlag == 0U))
    {
        pwmSum = (uint32_t)gOSPControl.pwmRed[port] + gOSPControl.pwmGreen[port] + gOSPControl.pwmBlue[port];
        pwmLimit = ((uint32_t)OSP_PWM_MAX * OSP_LOGO_CURRENT_LIMIT_MA) / 50U;
        if (pwmSum > pwmLimit)
        {
            gOSPControl.pwmRed[port] = (uint16_t)(((uint32_t)gOSPControl.pwmRed[port] * pwmLimit) / pwmSum);
            gOSPControl.pwmGreen[port] = (uint16_t)(((uint32_t)gOSPControl.pwmGreen[port] * pwmLimit) / pwmSum);
            gOSPControl.pwmBlue[port] = (uint16_t)(((uint32_t)gOSPControl.pwmBlue[port] * pwmLimit) / pwmSum);
        }
    }
    payload[0U] = (uint8_t)(OSP_CURRENT_BIT | (gOSPControl.pwmRed[port] >> 8U));
    payload[1U] = (uint8_t)gOSPControl.pwmRed[port];
    payload[2U] = (uint8_t)(OSP_CURRENT_BIT | (gOSPControl.pwmGreen[port] >> 8U));
    payload[3U] = (uint8_t)gOSPControl.pwmGreen[port];
    payload[4U] = (uint8_t)(OSP_CURRENT_BIT | (gOSPControl.pwmBlue[port] >> 8U));
    payload[5U] = (uint8_t)gOSPControl.pwmBlue[port];
    (void)OSP_Transfer(port, OSP_ADDR_BROADCAST, OSP_CMD_SET_PWM, payload, 6U, NULL, 0U, 0U);
    gOSPControl.dirty[port] = 0U;
}

