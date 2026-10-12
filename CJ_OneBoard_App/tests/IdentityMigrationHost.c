// Actual App PDS and UDS code with simulated Flash only. No hardware access.
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "appConfig.h"

MockTimer_t oAppTimer;
LIN_Diagnostic_t s_diag;
LIN_Device_Frame_t s_frames[FRAME_TABLE_SIZE];
LIN_RingBuf_t s_txRing;
volatile uint32_t g_dbg_lin_tp_error_count;
static unsigned checks;
static uint8_t flashRows[2][256];
static uint32_t failAddress;
static int cutAfter, pageCount;
static jmp_buf powerCut;

static void Check(int ok, const char *message)
{
    checks++;
    if (!ok) { fprintf(stderr, "FAIL %s\n", message); exit(1); }
}
void Flash_Read(uint32_t address, void *data, uint32_t size)
{
    memcpy(data, flashRows[address == FLASH_PDS_SYSTEM_ADDR], size);
}
uint8_t Flash_WriteRow(uint32_t address, const void *data, uint32_t size)
{
    uint8_t *row = flashRows[address == FLASH_PDS_SYSTEM_ADDR];
    if (address == failAddress) return 0;
    memset(row, 0xFF, 256);
    for (unsigned page = 0; page < 4; page++)
    {
        memcpy(row + page * 64, (const uint8_t *)data + page * 64, 64);
        if ((++pageCount == cutAfter) && cutAfter) longjmp(powerCut, 1);
    }
    return memcmp(row, data, size) == 0;
}
uint8_t Reprogram_Request(const uint8_t *request, uint8_t length, uint8_t *response)
{
    (void)length;
    response[0] = 0x7F; response[1] = request[0]; response[2] = 0x11;
    return 3;
}
#include "../applications/src/crc32.c"
#include "../applications/src/pds.c"
#include "../applications/src/UDS.c"

static void SeedLegacy(void)
{
    PDS_Record_t record;
    CRC32_Init();
    memset(&record, 0xFF, sizeof(record));
    record.index = 17;
    record.magic = PDS_CONFIG_MAGIC;
    record.length = 28;
    record.format = 1;
    record.data[0] = CONFIG_VERSION; record.data[1] = 0;
    memcpy(record.data + 2, "26J02-0042", 11);
    memcpy(record.data + 13, "CJ01-0001", 10);
    record.data[23] = 0x0C;
    record.data[24] = 0x78; record.data[25] = 0x56;
    record.data[26] = 0x34; record.data[27] = 0x12;
    record.crc32 = CRC32_GetRunTimeCRC32((uint8_t *)&record.index, sizeof(record) - 4);
    memcpy(flashRows[0], &record, sizeof(record));
    memcpy(flashRows[1], &record, sizeof(record));
    failAddress = 0; cutAfter = 0; pageCount = 0;
}
static void CheckIdentity(void)
{
    Check(strcmp(gConFig.Serial_Number, "26J02-0042") == 0, "Serial survives format conversion");
    Check(strcmp(gConFig.Part_Number, "ZX1-0025") == 0, "Part converted to eight characters");
    Check(gConFig.Chair_Number == 0x0C, "Chair survives format conversion");
    Check(gConFig.Init_Number == 0x12345678UL, "Init Number survives format conversion");
    Check(gConFig.version == CONFIG_VERSION, "Version retained");
}
int main(int argc, char **argv)
{
    uint8_t response[UDS_MAX_MESSAGE_LENGTH];
    const uint8_t read[] = { 0x22, 0xF2, 0x02 };
    const uint8_t targetRead[] = { 0x31, 3, 0xF3, 1, 0x82, 0xDA, 0x5C, 0x68 };
    for (int scenario = 0; scenario < 3; scenario++)
    {
        SeedLegacy();
        if (scenario == 1) memset(flashRows[1], 0xFF, 256);
        if (scenario == 2) memset(flashRows[0], 0xFF, 256);
        PDS_Init(); CheckIdentity();
        Check(gPdsMain.format == 2 && gPdsMain.length == 27, "New PDS format and size");
        Check(memcmp(flashRows[0], flashRows[1], 256) == 0, "Both migrated copies agree");
        pageCount = 0; PDS_Init(); CheckIdentity();
        Check(pageCount == 0, "No repeated migration writes");
    }
    for (int cut = 1; cut <= 8; cut++)
    {
        SeedLegacy(); cutAfter = cut;
        if (setjmp(powerCut) == 0) PDS_Init();
        cutAfter = 0; PDS_Init(); CheckIdentity();
        Check(memcmp(flashRows[0], flashRows[1], 256) == 0, "Power-cut recovery synchronizes migrated copies");
    }
    for (unsigned side = 0; side < 2; side++)
    {
        SeedLegacy(); failAddress = side ? FLASH_PDS_SYSTEM_ADDR : FLASH_PDS_SYSTEM_BACK_ADDR;
        PDS_Init(); CheckIdentity();
        failAddress = 0; PDS_Init(); CheckIdentity();
    }
    pageCount = 0;
    UDS_Init();
    Check(gConFig.Init_Number == 0x82DA5C68UL && pageCount == 0, "UDS CPU key overrides stored key without writing PDS");
    Check(UDS_HandleRequest(read, sizeof(read), response) == 32, "F202 response is 32 bytes");
    Check(memcmp(response + 3, "26J02-0042*ZX1-0025*", 20) == 0, "Eight-byte Part wire layout");
    Check(response[23] == 0 && response[24] == 1 && response[25] == '*' && response[26] == 0x0C && response[27] == '*', "Version/Chair offsets");
    Check(response[28] == 0x82 && response[29] == 0xDA && response[30] == 0x5C && response[31] == 0x68, "Big-endian CPU Init offset");
    Check(UDS_HandleBroadcast(targetRead, sizeof(targetRead), response) == 33, "Target read response is 33 bytes");
    if (argc > 1)
    {
        FILE *file = fopen(argv[1], "wb");
        Check(file != NULL, "Open cross-language wire fixture");
        Check(fwrite(response, 1, 33, file) == 33, "Save actual C target-read response");
        fclose(file);
    }
    uint8_t scan[] = { 0x31, 3, 0xF3, 0, s_discoverySlot };
    Check(UDS_HandleBroadcast(scan, sizeof(scan), response) == 33, "Discovery Scan uses new layout");
    scan[4] = (uint8_t)((s_discoverySlot + 1) % UDS_DISCOVERY_SLOT_COUNT);
    Check(UDS_HandleBroadcast(scan, sizeof(scan), response) == 0, "Other discovery slot remains silent");
    UDS_HandleRequest(read, sizeof(read), response);
    uint8_t write[32]; memcpy(write, response, 32);
    write[0] = 0x2E; write[2] = 0x05; write[26] = 0x0D;
    Check(UDS_HandleBroadcast(write, sizeof(write), response) == 3 && response[0] == 0x6E, "Targeted new-layout write accepted");
    Check(gConFig.Chair_Number == 0x0D && gConFig.Init_Number == 0x82DA5C68UL, "Write selects the correct target and preserves CPU Init");
    write[31] ^= 1;
    Check(UDS_HandleBroadcast(write, sizeof(write), response) == 0, "Wrong target ignored");
    uint8_t oldWrite[33] = { 0x2E, 0xF2, 0x02 };
    Check(UDS_HandleRequest(oldWrite, sizeof(oldWrite), response) == 3 && response[2] == 0x13, "Old-length write rejected");
    printf("PASS identity migration and wire checks: %u\n", checks);
    return 0;
}
