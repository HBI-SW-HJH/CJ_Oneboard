#ifndef STARTUP_PIC32CMJH01_H
#define STARTUP_PIC32CMJH01_H

#include <stdint.h>

#define WEAK_ALIAS __attribute__((weak, alias("Default_Handler")))

typedef void (*ISR_t)(void);

extern uint32_t CSTACK$$Limit;
extern const ISR_t g_pfnVectors[];

#endif // STARTUP_PIC32CMJH01_H
