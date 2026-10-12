// @file    sys_init.h
// @brief   System initialisation for CJ 4DPLEX Logo Lamp
//
// @copyright 2026 CJ 4DPLEX

#ifndef SYS_INIT_H
#define SYS_INIT_H

#include <stdint.h>

typedef struct
{
    volatile uint32_t timeMs; // Elapsed time; never reset by Main.
    volatile uint32_t tickWdtMs;
} AppTimer_t;

extern AppTimer_t oAppTimer;

// 전체 HW + 애플리케이션 초기화 (main() 진입 직후 1회 호출)
void SYS_Init(void);

// SysTick 1ms 콜백 (SysTick_Init() 에 등록, 직접 호출 금지)
void AppTimer_TickHandler(void);

// Watchdog 클리어 (main() while(1) 에서 주기적으로 호출)
// WDT_ENABLE == 0 이면 빈 함수로 컴파일됨
void WDT_Clear(void);

#endif // SYS_INIT_H
