/**
 * @file    startup_pic32cmjh01.c
 * @brief   Startup & vector table for PIC32CM5164JH01032-E/PT
 *          Cortex-M0+, uses Microchip DFP IRQn enum.
 *
 *  Keep the IAR runtime for .data/.bss initialization.
 *  Reset_Handler initializes SRAM ECC before entering that runtime.
 *
 *  Handler names follow the Microchip DFP convention in device_vectors.h.
 *  SERCOM0_6_Handler is the shared handler used by the LIN interface.
 *
 * @copyright 2026 CJ 4DPLEX
 */

#include <stdint.h>
#include "device.h"     /* includes pic32cm5164jh01032.h with IRQn typedef */
#include "flash_device.h"
#include "pds.h"
#include "startup_pic32cmjh01.h"
#include "Reprogram.h"
/* =========================================================== */
/*  IAR Normal 라이브러리 사용                                  */
/*  - .data/.bss 초기화는 IAR cstartup이 처리                  */
/*  - Reset에서 SRAM ECC 초기화 후 IAR cstartup 실행           */
/*  - SP 초기값은 IAR linker가 제공하는 CSTACK$$Limit 사용      */
/* =========================================================== */
extern __noreturn void __iar_program_start(void);

/* PIC32CM JH01 SRAM ECC must be initialized with aligned 32-bit writes.
 * __stackless prevents access to uninitialized CSTACK during the fill.
 * The address cast/IAR attributes are hardware startup exceptions.
 * This clears SRAM only; Flash/Data Flash/User Row are not touched.
 */
__stackless __noreturn void Reset_Handler(void)
{
    uint32_t address;

    __disable_irq();
    for (address = DRAM_ADDR; address < (DRAM_ADDR + DRAM_SIZE); address += 4U)
    {
        *(volatile uint32_t *)address = 0U;
    }
    SCB->VTOR = (uint32_t)g_pfnVectors;
    __DSB();
    __ISB();
    __enable_irq();
    __iar_program_start();
}

/* =========================================================== */
/*                    Application ISR Declarations             */
/* =========================================================== */
extern void SysTick_Handler(void);   /* systick_device.c  */
extern void SERCOM0_6_Handler(void);   /* lin_device.c      */
/* =========================================================== */
/*                    Default / Weak Stubs                     */
/* =========================================================== */
void Default_Handler(void);
void HardFault_Handler(void);

void NMI_Handler(void)          WEAK_ALIAS;
void SVC_Handler(void)          WEAK_ALIAS;
void PendSV_Handler(void)       WEAK_ALIAS;

/*
 * Peripheral IRQ handlers — names match DFP IRQn enum in pic32cm5164jh01032.h
 *
 * IRQ  0 : MCLK / OSCCTRL / OSC32KCTRL / PAC / PM / SUPC  (shared)
 * IRQ  1 : WDT
 * IRQ  2 : RTC
 * IRQ  3 : EIC
 * IRQ  4 : FREQM
 * IRQ  5 : MCRAMC
 * IRQ  6 : NVMCTRL
 * IRQ  7 : DMAC
 * IRQ  8 : EVSYS
 * IRQ  9 : SERCOM0 / SERCOM6  (shared)
 * IRQ 10 : SERCOM1 / SERCOM7  (shared)
 * IRQ 11 : SERCOM2
 * IRQ 12 : SERCOM3
 * IRQ 13 : SERCOM4
 * IRQ 14 : SERCOM5
 * IRQ 15 : CAN0
 * IRQ 16 : CAN1
 * IRQ 17 : TCC0
 * IRQ 18 : TCC1
 * IRQ 19 : TCC2
 * IRQ 20 : TC0 / TC5  (shared)
 * IRQ 21 : TC1 / TC6  (shared)
 * IRQ 22 : TC2 / TC7  (shared)
 * IRQ 23 : TC3
 * IRQ 24 : TC4
 * IRQ 25 : ADC0
 * IRQ 26 : ADC1
 * IRQ 27 : AC
 * IRQ 28 : DAC
 * IRQ 29 : PDEC
 * IRQ 30 : PTC
 * IRQ 31 : ICM
 */
void SYSTEM_Handler(void)       WEAK_ALIAS;   /* IRQ 0  */
void WDT_Handler(void)          WEAK_ALIAS;   /* IRQ 1  */
void RTC_Handler(void)          WEAK_ALIAS;   /* IRQ 2  */
void EIC_Handler(void)          WEAK_ALIAS;   /* IRQ 3  */
void FREQM_Handler(void)        WEAK_ALIAS;   /* IRQ 4  */
void MCRAMC_Handler(void)       WEAK_ALIAS;   /* IRQ 5  */
void NVMCTRL_Handler(void)      WEAK_ALIAS;   /* IRQ 6  */
void DMAC_Handler(void)         WEAK_ALIAS;   /* IRQ 7  */
void EVSYS_Handler(void)        WEAK_ALIAS;   /* IRQ 8  */
/* SERCOM0_6_Handler is defined by lin_device.c. */      /* IRQ 9  */
void SERCOM1_7_Handler(void)    WEAK_ALIAS;   /* IRQ 10 */
void SERCOM2_Handler(void)      WEAK_ALIAS;   /* IRQ 11 */
void SERCOM3_Handler(void)      WEAK_ALIAS;   /* IRQ 12 */
void SERCOM4_Handler(void)      WEAK_ALIAS;   /* IRQ 13 */
void SERCOM5_Handler(void)      WEAK_ALIAS;   /* IRQ 14 */
void CAN0_Handler(void)         WEAK_ALIAS;   /* IRQ 15 */
void CAN1_Handler(void)         WEAK_ALIAS;   /* IRQ 16 */
void TCC0_Handler(void)         WEAK_ALIAS;   /* IRQ 17 */
void TCC1_Handler(void)         WEAK_ALIAS;   /* IRQ 18 */
void TCC2_Handler(void)         WEAK_ALIAS;   /* IRQ 19 */
void TC0_5_Handler(void)        WEAK_ALIAS;   /* IRQ 20 */
void TC1_6_Handler(void)        WEAK_ALIAS;   /* IRQ 21 */
void TC2_7_Handler(void)        WEAK_ALIAS;   /* IRQ 22 */
void TC3_Handler(void)          WEAK_ALIAS;   /* IRQ 23 */
void TC4_Handler(void)          WEAK_ALIAS;   /* IRQ 24 */
void ADC0_Handler(void)         WEAK_ALIAS;   /* IRQ 25 */
void ADC1_Handler(void)         WEAK_ALIAS;   /* IRQ 26 */
void AC_Handler(void)           WEAK_ALIAS;   /* IRQ 27 */
void DAC_Handler(void)          WEAK_ALIAS;   /* IRQ 28 */
void PDEC_Handler(void)         WEAK_ALIAS;   /* IRQ 29 */
void PTC_Handler(void)          WEAK_ALIAS;   /* IRQ 30 */
void ICM_Handler(void)          WEAK_ALIAS;   /* IRQ 31 */

/* =========================================================== */
/*                    Vector Table                             */
/* =========================================================== */
__attribute__((section(".vectors"), used))
const ISR_t g_pfnVectors[] =
{
    /* ---- Cortex-M0+ core exceptions ---- */
    (ISR_t)&CSTACK$$Limit,       /*  0: Initial SP (IAR CSTACK)  */
    Reset_Handler,               /*  1: SRAM ECC + IAR cstartup */
    NMI_Handler,                 /*  2: NMI                      */
    HardFault_Handler,           /*  3: HardFault                */
    (ISR_t)0,                    /*  4: Reserved                 */
    (ISR_t)0,                    /*  5: Reserved                 */
    (ISR_t)0,                    /*  6: Reserved                 */
    (ISR_t)0,                    /*  7: Reserved                 */
    (ISR_t)0,                    /*  8: Reserved                 */
    (ISR_t)0,                    /*  9: Reserved                 */
    (ISR_t)0,                    /* 10: Reserved                 */
    SVC_Handler,                 /* 11: SVC                      */
    (ISR_t)0,                    /* 12: Reserved                 */
    (ISR_t)0,                    /* 13: Reserved                 */
    PendSV_Handler,              /* 14: PendSV                   */
    SysTick_Handler,             /* 15: SysTick                  */

    /* ---- External (peripheral) IRQs — must match DFP IRQn enum exactly ---- */
    SYSTEM_Handler,              /* IRQ  0: MCLK/OSCCTRL/OSC32KCTRL/PAC/PM/SUPC */
    WDT_Handler,                 /* IRQ  1: WDT                  */
    RTC_Handler,                 /* IRQ  2: RTC                  */
    EIC_Handler,                 /* IRQ  3: EIC                  */
    FREQM_Handler,               /* IRQ  4: FREQM                */
    MCRAMC_Handler,              /* IRQ  5: MCRAMC               */
    NVMCTRL_Handler,             /* IRQ  6: NVMCTRL              */
    DMAC_Handler,                /* IRQ  7: DMAC                 */
    EVSYS_Handler,               /* IRQ  8: EVSYS                */
    SERCOM0_6_Handler,           /* IRQ  9: SERCOM0 + SERCOM6    */
    SERCOM1_7_Handler,           /* IRQ 10: SERCOM1 + SERCOM7    */
    SERCOM2_Handler,             /* IRQ 11: SERCOM2              */
    SERCOM3_Handler,             /* IRQ 12: SERCOM3              */
    SERCOM4_Handler,             /* IRQ 13: SERCOM4              */
    SERCOM5_Handler,             /* IRQ 14: SERCOM5              */
    CAN0_Handler,                /* IRQ 15: CAN0                 */
    CAN1_Handler,                /* IRQ 16: CAN1                 */
    TCC0_Handler,                /* IRQ 17: TCC0                 */
    TCC1_Handler,                /* IRQ 18: TCC1                 */
    TCC2_Handler,                /* IRQ 19: TCC2                 */
    TC0_5_Handler,               /* IRQ 20: TC0 + TC5            */
    TC1_6_Handler,               /* IRQ 21: TC1 + TC6            */
    TC2_7_Handler,               /* IRQ 22: TC2 + TC7            */
    TC3_Handler,                 /* IRQ 23: TC3                  */
    TC4_Handler,                 /* IRQ 24: TC4                  */
    ADC0_Handler,                /* IRQ 25: ADC0                 */
    ADC1_Handler,                /* IRQ 26: ADC1                 */
    AC_Handler,                  /* IRQ 27: AC                   */
    DAC_Handler,                 /* IRQ 28: DAC                  */
    PDEC_Handler,                /* IRQ 29: PDEC                 */
    PTC_Handler,                 /* IRQ 30: PTC                  */
    ICM_Handler,                 /* IRQ 31: ICM                  */
};

/* =========================================================== */
/*  HardFault Handler                                          */
/*  PA4 빠른 토글 (~5Hz) 로 HardFault 진입 표시               */
/*    - HardFault  : PA4 빠른 깜빡임                           */
/*    - Default    : PA4 High 고착                             */
/*    - 정상 루프  : PA4 느린 토글 (10ms)                      */
/* =========================================================== */
void HardFault_Handler(void)
{
    Reprogram_HardFault();
    __DSB(); 
    SCB->AIRCR = (0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
    __DSB();
    while (1U) 
    {
    }
}
/* =========================================================== */
/*  Default Handler                                            */
/*  미처리 인터럽트 → PA4 High 고착                            */
/* =========================================================== */
void Default_Handler(void)
{
    PORT_REGS->GROUP[0].PORT_DIRSET = (1UL << 4U);
    PORT_REGS->GROUP[0].PORT_OUTSET = (1UL << 4U);
    __disable_irq();
    while (1U) {}
}
