#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#include <stdint.h>
#include <string.h>
#include "SRGBControl.h"
#include "OSPControl.h"
#include "Verification.h"
typedef struct { uint32_t timeMs; uint16_t tick10ms; } VerificationHostTimer_t;
typedef struct { uint8_t voltageFault; } VerificationHostPower_t;
extern VerificationHostTimer_t oAppTimer;
extern VerificationHostPower_t gPowerData;
void AppTimer_ResetReadCount(void);
static inline uint32_t __get_PRIMASK(void) { return 0U; }
static inline void __disable_irq(void) { }
static inline void __set_PRIMASK(uint32_t value) { (void)value; }
#endif
