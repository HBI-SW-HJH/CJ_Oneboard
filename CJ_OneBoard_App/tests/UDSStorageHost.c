// Compile actual production C with host Flash/LIN stubs, not a translated model.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "appConfig.h"
#include "UDSStorageHost.h"

MockTimer_t oAppTimer;
volatile uint32_t g_dbg_lin_drop_count;
uint32_t failAddress;
int pageCount;
int cutAfter;
jmp_buf powerCut;
unsigned checks;
unsigned lampCalls;
void (*pageHook)(void);

static void Flash_EraseRow(uint32_t address)
{
    if (address != failAddress)
    {
        memset((void *)(uintptr_t)address, 0xFF, 256);
    }
}
void Flash_Read(uint32_t address, void *data, uint32_t size)
{
    memcpy(data, (const void *)(uintptr_t)address, size);
}
static void Flash_WriteWord(uint32_t address, uint32_t word)
{
    if ((address & ~255U) != failAddress)
    {
        *(uint32_t *)(uintptr_t)address &= word;
    }
}
static void Flash_WritePage(uint32_t address)
{
    (void)address;
    pageCount++;
    if (pageHook != NULL)
    {
        pageHook();
    }
    if ((cutAfter != 0) && (pageCount == cutAfter))
    {
        longjmp(powerCut, 1);
    }
}
uint8_t Flash_WriteRow(uint32_t address, const void *data, uint32_t size)
{
    const uint32_t *writeData = (const uint32_t *)data;
    uint32_t page;
    uint32_t word;
    Flash_EraseRow(address);
    for (page = 0U; page < 4U; page++)
    {
        for (word = 0U; word < 16U; word++)
        {
            Flash_WriteWord(address + (page * 64U) + (word * 4U), writeData[(page * 16U) + word]);
        }
        Flash_WritePage(address + (page * 64U));
    }
    return (uint8_t)(memcmp((const void *)(uintptr_t)address, data, size) == 0);
}
void LIN_RegisterFrames(LIN_Device_Frame_t *frames, uint8_t count) { (void)frames; (void)count; }
void LIN_Init(void (*rx)(const LIN_Device_Frame_t *), void (*tx)(LIN_Device_Frame_t *)) { (void)rx; (void)tx; }
void SRGBControl_Set(const LedControlCommand_t *c) { (void)c; lampCalls++; }
void OSPControl_Set(const LedControlCommand_t *c) { (void)c; lampCalls++; }
uint8_t SRGBControl_GetFaultMask(SRGB_FaultPart_t part) { (void)part; return 0; }
uint8_t OSPControl_GetFaultMask(OSP_FaultPart_t part) { (void)part; return 0; }
Reprogram_t gReprogram;
Verification_t gVerification;
uint8_t Reprogram_Request(const uint8_t *request, uint8_t length, uint8_t *response)
{
    (void)length;
    response[0] = 0x7F; response[1] = request[0]; response[2] = 0x11;
    return 3;
}
void Verification_Init(void) { memset(&gVerification, 0, sizeof(gVerification)); }
void Verification_Request(const uint8_t *data) { (void)data; }
XCP_Control_t gXCPControl;
void XCP_Init(void) { memset(&gXCPControl, 0, sizeof(gXCPControl)); }
void XCP_HandleRequest(const uint8_t *request) { (void)request; gXCPControl.response[0] = XCP_PID_RESPONSE; }

#include "../applications/src/crc32.c"
#include "../applications/src/pds.c"
#include "../applications/src/UDS.c"
#include "../applications/src/lin_task.c"

static uint8_t RecordValidAt(uint32_t address)
{
    PDS_Record_t record;
    Flash_Read(address, &record, sizeof(record));
    if ((record.magic != PDS_CONFIG_MAGIC) || (record.format != PDS_CONFIG_FORMAT) || (record.length < PDS_CONFIG_MIN_LENGTH) || (record.length > PDS_CONFIG_CAPACITY))
    {
        return 0U;
    }
    return (uint8_t)(CRC32_GetRunTimeCRC32((const uint8_t *)&record.index, (uint16_t)(sizeof(record) - sizeof(record.crc32))) == record.crc32);
}
static uint8_t RecordsEqual(void)
{
    PDS_Record_t mainRecord;
    PDS_Record_t backupRecord;
    Flash_Read(FLASH_PDS_SYSTEM_ADDR, &mainRecord, sizeof(mainRecord));
    Flash_Read(FLASH_PDS_SYSTEM_BACK_ADDR, &backupRecord, sizeof(backupRecord));
    return (uint8_t)(memcmp(&mainRecord, &backupRecord, sizeof(mainRecord)) == 0);
}
static uint8_t SetChairId(uint8_t chairId)
{
    ConFig_t config = gConFig;
    config.Chair_Number = (uint8_t)(chairId + 0x09U);
    return (uint8_t)(PDS_Write(&config) == PDS_WRITE_OK);
}

static void Check(int condition, const char *text)
{
    checks++;
    if (!condition)
    {
        fprintf(stderr, "FAIL %s\n", text);
        exit(1);
    }
}
static void Reset(void)
{
    failAddress = 0;
    cutAfter = 0;
    pageHook = NULL;
    memset((void *)0x10000000, 0xFF, 8192);
    PDS_Init();
    LINT_Init();
}
static void Receive(uint8_t nad, uint8_t pci, const uint8_t *data, uint8_t count)
{
    LIN_Device_Frame_t frame = { 0 };
    frame.frame_id = 0x3C;
    frame.length = 8;
    memset(frame.data, 0xFF, 8);
    frame.data[0] = nad;
    frame.data[1] = pci;
    memcpy(&frame.data[2], data, count);
    LINT_RxCallback(&frame);
}
static void SendPayload(uint8_t nad, const uint8_t *payload, uint8_t length)
{
    uint8_t data[6];
    uint8_t offset;
    uint8_t count;
    uint8_t sequence = 1;
    if (length <= 6)
    {
        Receive(nad, length, payload, length);
        return;
    }
    data[0] = length;
    memcpy(&data[1], payload, 5);
    Receive(nad, 0x10, data, 6);
    offset = 5;
    while (offset < length)
    {
        count = (uint8_t)(length - offset);
        count = count > 6 ? 6 : count;
        Receive(nad, (uint8_t)(0x20 | sequence), &payload[offset], count);
        offset = (uint8_t)(offset + count);
        sequence++;
    }
}
static uint8_t ReadResponse(uint8_t *payload, uint8_t expectedNad)
{
    uint8_t length = 0;
    uint8_t offset = 0;
    uint8_t sequence = 1;
    uint8_t start;
    uint8_t count;
    while (s_frames[FID_DIAG_TX_IDX].frameIsValid != FALSE)
    {
        LINT_TxCallback(&s_frames[FID_DIAG_TX_IDX]);
        const uint8_t *data = s_frames[FID_DIAG_TX_IDX].data;
        Check(data[0] == expectedNad, "Response preserves addressed old NAD");
        if (offset == 0)
        {
            length = data[1] < 0x10 ? data[1] : data[2];
            start = data[1] < 0x10 ? 2 : 3;
            count = data[1] < 0x10 ? length : 5;
        }
        else
        {
            Check(data[1] == (uint8_t)(0x20 | sequence), "CF sequence");
            sequence++;
            start = 2;
            count = (uint8_t)(length - offset);
            count = count > 6 ? 6 : count;
        }
        memcpy(&payload[offset], &data[start], count);
        offset = (uint8_t)(offset + count);
        for (unsigned i = start + count; i < 8; i++)
        {
            Check(data[i] == 0xFF, "Unused bytes padded FF");
        }
    }
    Check(offset == length, "Complete segmented response");
    return length;
}
static void InterruptWithNewRequest(void)
{
    const uint8_t read[] = { 0x22, 0xF2, 0x02 };
    pageHook = NULL;
    SendPayload(1, read, sizeof(read));
}

int main(void)
{
    void *flash = VirtualAlloc((void *)0x10000000, 8192, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    PDS_Record_t storedRecord;
    Check(flash == (void *)0x10000000, "Host virtual Flash address");
    Reset();
    Flash_Read(FLASH_PDS_SYSTEM_ADDR, &storedRecord, sizeof(storedRecord));
#ifdef CONFIG_EXTENSION_TEST
    Check(sizeof(ConFig_t) == 147, "Appended structure byte layout");
#else
    Check(sizeof(ConFig_t) == 27, "Base configuration byte layout");
#endif
    Check(gConFig.Chair_Number == 0x0A, "Default A => NAD1");
    Check((gConFig.Init_Number != 0U) && (gConFig.Init_Number != 0xFFFFFFFFU), "Init Number generated");
    Check(storedRecord.length == sizeof(ConFig_t), "Automatic sizeof persistence");
    Check(RecordValidAt(FLASH_PDS_SYSTEM_ADDR), "Primary CRC");
    Check(RecordValidAt(FLASH_PDS_SYSTEM_BACK_ADDR), "Backup CRC");
    Check(RecordsEqual(), "Main and Backup contain the same record");
    for (unsigned offset = 0; offset < 256U; offset++)
    {
        Check(((const uint8_t *)flash)[offset] == 0xFFU, "First Data Flash row remains unused");
    }
    Check(SetChairId(2), "Chair setter saves same record");
    PDS_Init();
    Check(gConFig.Chair_Number == 0x0B, "Chair survives reboot as B");

    Reset();
    LIN_Device_Frame_t idChange = { 0 };
    idChange.frame_id = 0x1A;
    idChange.length = 8;
    for (unsigned i = 0; i < 8; i += 2)
    {
        idChange.data[i] = 0x0A;
        idChange.data[i + 1] = 0x0B;
    }
    idChange.data[4] = 0x0C;
    LINT_RxCallback(&idChange);
    LINT_Process();
    Check(gConFig.Chair_Number == 0x0A, "Mismatched old ID pair is ignored");
    idChange.data[4] = 0x0A;
    idChange.data[7] = 0x0D;
    LINT_RxCallback(&idChange);
    LINT_Process();
    Check(gConFig.Chair_Number == 0x0A, "Mismatched new ID pair is ignored");
    idChange.data[7] = 0x0B;
    idChange.data[1] = 0x0E;
    LINT_RxCallback(&idChange);
    LINT_Process();
    Check(gConFig.Chair_Number == 0x0A, "Out-of-range new ID is ignored");
    idChange.data[1] = 0x0B;
    LINT_RxCallback(&idChange);
    LINT_Process();
    Check(gConFig.Chair_Number == 0x0B, "Four exact ID pairs change chair ID");
    Check(s_frames[FID_STATUS_TX_IDX].frame_id == 0x0B, "Status frame follows new ID");
    Check(s_frames[FID_ID_CHANGE_RX_IDX].frame_id == 0x1B, "Change frame follows new ID");
    PDS_Init();
    LINT_Init();
    Check((gConFig.Chair_Number == 0x0B) && (s_frames[FID_ID_CHANGE_RX_IDX].frame_id == 0x1B), "Changed ID survives reboot");
    idChange.frame_id = 0x1B;
    for (unsigned i = 0; i < 8; i += 2)
    {
        idChange.data[i] = 0x0B;
        idChange.data[i + 1] = 0x0C;
    }
    failAddress = FLASH_PDS_SYSTEM_BACK_ADDR;
    LINT_RxCallback(&idChange);
    LINT_Process();
    Check((gConFig.Chair_Number == 0x0B) && (s_frames[FID_STATUS_TX_IDX].frame_id == 0x0B), "Storage failure keeps old chair ID");

    Reset();
    Check(SetChairId(3), "Prepare old location data");
    PDS_Record_t oldRecord;
    Flash_Read(FLASH_PDS_SYSTEM_ADDR, &oldRecord, sizeof(oldRecord));
    memset(flash, 0xFF, 8192);
    memcpy((void *)0x10000A00UL, &oldRecord, sizeof(oldRecord));
    memcpy((void *)0x10000B00UL, &oldRecord, sizeof(oldRecord));
    PDS_Init();
    Check(gConFig.Chair_Number == 0x0A, "Old PDS locations are ignored");
    Check(RecordValidAt(FLASH_PDS_SYSTEM_ADDR), "New primary location saved");
    Check(RecordValidAt(FLASH_PDS_SYSTEM_BACK_ADDR), "New backup location saved");
    Check(RecordValidAt(0x10000A00UL), "Old location remains untouched");

    Reset();
    uint8_t snapshot[8192];
    memcpy(snapshot, flash, sizeof(snapshot));
    for (int cut = 1; cut <= 8; cut++)
    {
        memcpy(flash, snapshot, sizeof(snapshot));
        cutAfter = 0;
        PDS_Init();
        ConFig_t next = gConFig;
        next.Chair_Number = 0x0D;
        strcpy(next.Serial_Number, "26I16-0002");
        pageCount = 0;
        cutAfter = cut;
        if (setjmp(powerCut) == 0)
        {
            PDS_Write(&next);
        }
        cutAfter = 0;
        PDS_Init();
        int oldRecord = (gConFig.Chair_Number == 0x0A) && (strcmp(gConFig.Serial_Number, "26I16-0001") == 0);
        int newRecord = (gConFig.Chair_Number == 0x0D) && (strcmp(gConFig.Serial_Number, "26I16-0002") == 0);
        Check(oldRecord || newRecord, "Power cut retains old or fully committed new record");
        Check(RecordsEqual(), "Boot synchronizes both copies after power cut");
    }
    Reset();
    failAddress = FLASH_PDS_SYSTEM_BACK_ADDR;
    Check(SetChairId(2) == 0, "Backup write failure rejected");
    Check(gConFig.Chair_Number == 0x0A, "Failed save keeps active RAM identity");
    failAddress = FLASH_PDS_SYSTEM_ADDR;
    Check(SetChairId(2) == 1, "Main write failure still has one durable new copy");
    failAddress = 0;
    PDS_Init();
    Check(gConFig.Chair_Number == 0x0B, "Load newest backup after primary failure");
    Check(RecordValidAt(FLASH_PDS_SYSTEM_ADDR), "Repair redundant copy");
    Check(RecordsEqual(), "Boot repair synchronizes Main and Backup");

    Reset();
    pageCount = 0;
    failAddress = FLASH_PDS_SYSTEM_ADDR;
    Check(SetChairId(2), "One durable copy accepted before repair retry test");
    Check(pageCount == 16, "Backup once and Main repair bounded to three attempts");
    failAddress = 0;

#ifdef CONFIG_EXTENSION_TEST
    PDS_Record_t oldLayout;
    Flash_Read(FLASH_PDS_SYSTEM_ADDR, &oldLayout, sizeof(oldLayout));
    oldLayout.length = 23U;
    memset(&oldLayout.data[23U], 0xFF, PDS_CONFIG_CAPACITY - 23U);
    oldLayout.crc32 = CRC32_GetRunTimeCRC32((const uint8_t *)&oldLayout.index, sizeof(oldLayout) - sizeof(oldLayout.crc32));
    memcpy((void *)FLASH_PDS_SYSTEM_ADDR, &oldLayout, sizeof(oldLayout));
    memcpy((void *)FLASH_PDS_SYSTEM_BACK_ADDR, &oldLayout, sizeof(oldLayout));
    PDS_Init();
    Flash_Read(FLASH_PDS_SYSTEM_ADDR, &storedRecord, sizeof(storedRecord));
    Check(storedRecord.length == sizeof(ConFig_t), "Old saved length automatically extends");
    for (unsigned future = 0; future < sizeof(gConFig.future); future++)
    {
        Check(gConFig.future[future] == 0U, "Appended field gets default zero");
    }
#endif

    LIN_RingBuf_t ring = { 0 };
    LIN_QueueEntry_t entry = { 0 };
    for (unsigned cycle = 0; cycle < 20; cycle++)
    {
        for (unsigned i = 0; i < 32; i++)
        {
            entry.timeMs = i;
            Check(RingBuf_Push(&ring, &entry), "All 32 slots usable, across counter wrap");
        }
        Check(!RingBuf_Push(&ring, &entry), "33rd entry rejected without overwrite");
        for (unsigned i = 0; i < 32; i++)
        {
            Check(RingBuf_Pop(&ring, &entry), "Pop queued entry");
            Check(entry.timeMs == i, "FIFO order");
        }
        Check(!RingBuf_Pop(&ring, &entry), "Empty queue");
    }

    Reset();
    for (uint8_t chair = 0x0AU; chair <= 0x0DU; chair++)
    {
        gConFig.Chair_Number = chair;
        LINT_Init();
        Check(s_frames[FID_STATUS_TX_IDX].frame_id == chair, "Existing status FID unchanged");
        Check(s_frames[FID_XCP_RX_IDX].frame_id == (uint8_t)(XCP_REQUEST_ID_BASE + chair - 0x0AU), "XCP request follows Chair ID");
        Check(s_frames[FID_XCP_TX_IDX].frame_id == (uint8_t)(XCP_RESPONSE_ID_BASE + chair - 0x0AU), "XCP response follows Chair ID");
        Check(s_frames[FID_DIAG_RX_IDX].frame_id == 0x3CU && s_frames[FID_DIAG_TX_IDX].frame_id == 0x3DU, "Existing UDS FIDs unchanged");
    }
    Reset();
    const uint8_t readAll[] = { 0x22, 0xF2, 0x02 };
    uint8_t response[UDS_MAX_MESSAGE_LENGTH];
    SendPayload(1, readAll, sizeof(readAll));
    Check(!s_frames[FID_DIAG_TX_IDX].frameIsValid, "Early header has no ready response");
    LINT_Process();
    Check(ReadResponse(response, 1) == 32, "Product read includes read-only Init Number");
    Check(memcmp(&response[3], "26I16-0001*ZX1-0025*", 20) == 0, "Wire product fields use separators");
    Check((response[23] == 0) && (response[24] == 1) && (response[25] == '*') && (response[26] == 0x0A) && (response[27] == '*'), "Version, Chair and Init separators");
    uint8_t writeAll[32];
    memcpy(writeAll, response, sizeof(writeAll));
    writeAll[0] = 0x2E;
    writeAll[12] = '4';
    writeAll[24] = 0x12;
    Check((UDS_HandleRequest(writeAll, sizeof(writeAll), response) == 3) && (response[2] == UDS_NRC_REQUEST_OUT_OF_RANGE), "Version is firmware-owned");
    writeAll[24] = CONFIG_VERSION;
    writeAll[26] = 0x0C;
    writeAll[25] = '!';
    Check((UDS_HandleRequest(writeAll, sizeof(writeAll), response) == 3) && (response[2] == UDS_NRC_REQUEST_OUT_OF_RANGE), "Missing Version/Chair separator rejected");
    writeAll[25] = '*';
    writeAll[27] = '!';
    Check((UDS_HandleRequest(writeAll, sizeof(writeAll), response) == 3) && (response[2] == UDS_NRC_REQUEST_OUT_OF_RANGE), "Missing Chair/Init separator rejected");
    writeAll[27] = '*';
    SendPayload(1, writeAll, sizeof(writeAll));
    LINT_Process();
    Check(ReadResponse(response, 1) == 3, "Write acknowledgement SF to old NAD");
    Check(response[0] == 0x6E, "Positive only after durable save");
    Check(gConFig.Chair_Number == 0x0C, "Chair C switches NAD3");
    Check(s_frames[FID_STATUS_TX_IDX].frame_id == 0x0C, "Same Chair switches status FID");
    Check(s_frames[FID_ID_CHANGE_RX_IDX].frame_id == 0x1C, "UDS write also switches ID-change FID");
    Check(s_frames[FID_XCP_RX_IDX].frame_id == 0x22U && s_frames[FID_XCP_TX_IDX].frame_id == 0x26U, "UDS Chair write switches XCP FIDs");
    PDS_Init();
    Check((gConFig.version == CONFIG_VERSION) && (gConFig.Serial_Number[9] == '4') && (gConFig.Chair_Number == 0x0C), "All identity data survives reset together");

    Reset();
    uint32_t initNumber = gConFig.Init_Number;
    const uint8_t readSerial[] = { 0x22, 0xF1, 0x8C };
    Check(UDS_HandleRequest(readSerial, sizeof(readSerial), response) == 3 && response[2] == 0x31, "Individual Serial DID is unsupported");
    const uint8_t readInit[] = { 0x22, 0xF2, 0x03 };
    Check(UDS_HandleRequest(readInit, sizeof(readInit), response) == 3 && response[2] == 0x31, "Individual Init DID is unsupported");
    Check(gConFig.Init_Number == initNumber, "Init Number remains in complete product record");

    uint8_t slotKey[] = { (uint8_t)(initNumber >> 24), (uint8_t)(initNumber >> 16), (uint8_t)(initNumber >> 8), (uint8_t)initNumber };
    Check(s_discoverySlot == CRC32_GetRunTimeCRC32(slotKey, sizeof(slotKey)) % UDS_DISCOVERY_SLOT_COUNT, "Discovery slot depends only on fixed Init Number");
    uint8_t fixedSlot = s_discoverySlot;
    uint8_t oldStart[] = { 0x31, 0x01, 0xF3, 0x00, 0x5A };
    Check(UDS_HandleBroadcast(oldStart, sizeof(oldStart), response) == 0 && s_discoverySlot == fixedSlot, "Old salt start cannot change fixed slot");
    uint8_t discoveryRead[] = { 0x31, 0x03, 0xF3, 0x00, s_discoverySlot };
    SendPayload(0x7F, discoveryRead, sizeof(discoveryRead));
    LINT_Process();
    Check(ReadResponse(response, 0x7F) == 33, "Discovery result returns one complete product record");
    Check((response[0] == 0x71) && (response[3] == 0x00), "Discovery routine positive response");
    Check((((uint32_t)response[29] << 24) | ((uint32_t)response[30] << 16) | ((uint32_t)response[31] << 8) | response[32]) == initNumber, "Discovery identifies board by Init Number");

    uint8_t targetProduct[32];
    memcpy(targetProduct, writeAll, sizeof(targetProduct));
    targetProduct[2] = 0x05;
    targetProduct[26] = 0x0B;
    SendPayload(0x7F, targetProduct, sizeof(targetProduct));
    LINT_Process();
    Check(ReadResponse(response, 0x7F) == 3, "Targeted complete product write response");
    Check((response[0] == 0x6E) && (gConFig.Chair_Number == 0x0B) && (gConFig.Init_Number == initNumber), "Chair changes in complete product; Init Number remains fixed");
    targetProduct[31] ^= 0x01;
    SendPayload(0x7F, targetProduct, sizeof(targetProduct));
    LINT_Process();
    Check(!s_frames[FID_DIAG_TX_IDX].frameIsValid, "Different Init Number is ignored without write or response");
    Check((gConFig.Chair_Number == 0x0B) && (gConFig.Init_Number == initNumber), "Wrong target leaves board unchanged");

    Reset();
    const uint8_t badDid[] = { 0x22, 0x12, 0x34 };
    Check(UDS_HandleRequest(badDid, sizeof(badDid), response) == 3, "Unknown DID response");
    Check(response[2] == 0x31, "Unknown DID NRC31");
    Check(UDS_HandleRequest(readAll, 2, response) == 3, "Short request rejected");
    Check(response[2] == 0x13, "Short request NRC13");
    failAddress = FLASH_PDS_SYSTEM_BACK_ADDR;
    UDS_HandleRequest(writeAll, sizeof(writeAll), response);
    Check((response[0] == 0x7F) && (response[2] == 0x72), "UDS storage failure NRC72");
    failAddress = 0;
    ConFig_t flexibleConfig = gConFig;
    flexibleConfig.Chair_Number = 0x20;
    memcpy(flexibleConfig.Serial_Number, "ZZZZZ-9999", CONFIG_SERIAL_LENGTH);
    Check(PDS_Write(&flexibleConfig) == PDS_WRITE_OK, "PDS accepts configurable product data");
    PDS_Init();
    Check((gConFig.Chair_Number == 0x20) && (memcmp(gConFig.Serial_Number, "ZZZZZ-9999", CONFIG_SERIAL_LENGTH) == 0), "Configurable product data survives reset");
    Reset();

    const uint8_t ff[] = { 25, 0x2E, 0xF2, 0x02, '2', '6' };
    Receive(1, 0x10, ff, 6);
    Receive(1, 0x22, ff, 6);
    LINT_Process();
    Check(s_diag.length == 0, "Wrong CF sequence aborts without write");
    Receive(1, 0x10, ff, 6);
    LINT_Process();
    oAppTimer.timeMs += 1001;
    LINT_Process();
    Check(s_diag.length == 0, "Missing CF times out");
    const uint8_t oversized[] = { UDS_MAX_MESSAGE_LENGTH + 1, 0x22, 0xF2, 0x02, 0, 0 };
    Receive(1, 0x10, oversized, 6);
    LINT_Process();
    Check(s_diag.length == 0, "Oversized FF ignored safely");
    SendPayload(2, readAll, sizeof(readAll));
    LINT_Process();
    Check(!s_frames[FID_DIAG_TX_IDX].frameIsValid, "Other NAD does not respond");
    SendPayload(1, readAll, sizeof(readAll));
    LINT_Process();
    oAppTimer.timeMs += 1001;
    LINT_Process();
    Check(!s_frames[FID_DIAG_TX_IDX].frameIsValid, "Unconsumed TX response expires");

    Reset();
    pageHook = InterruptWithNewRequest;
    uint8_t raceWrite[32];
    memcpy(raceWrite, writeAll, sizeof(raceWrite));
    raceWrite[26] = 0x0A;
    SendPayload(1, raceWrite, sizeof(raceWrite));
    LINT_Process();
    Check(ReadResponse(response, 1) == 32, "New request during Flash write supersedes old response safely");
    Check((response[0] == 0x62) && (response[1] == 0xF2) && (response[2] == 0x02), "No stale Write response published");
    printf("PASS actual C storage/UDS/ring checks: %u\n", checks);
    return 0;
}
