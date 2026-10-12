// @file    crc32.h
// @brief   CRC-32 (IEEE 802.3 / Ethernet polynomial 0xEDB88320)
//
// @copyright 2026 CJ 4DPLEX

#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

extern uint32_t gCRC32Table[256U];

void     CRC32_Init(void);
uint32_t CRC32_GetRunTimeCRC32(const uint8_t *pBuf, uint16_t length);

#endif // CRC32_H
