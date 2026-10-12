#ifndef UDS_STORAGE_HOST_H
#define UDS_STORAGE_HOST_H

#include <stdint.h>
#include <setjmp.h>

extern uint32_t failAddress;
extern int pageCount;
extern int cutAfter;
extern jmp_buf powerCut;
extern unsigned checks;
extern unsigned lampCalls;
extern void (*pageHook)(void);

#endif // UDS_STORAGE_HOST_H
