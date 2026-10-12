#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "generated_boot.c"
#include "../applications/src/UDS.c"
#include "../applications/src/lin_task.c"

HostNvm hostNvm;
HostSercom hostSercom;
HostTimer oAppTimer;
HostConfig gConFig;
uint32_t hostUid[4] = { 0x13572468, 0x24681357, 0x89ABCDEF, 0x10203040 };
volatile uint32_t g_dbg_lin_drop_count;
void LIN_RegisterFrames(LIN_Device_Frame_t *frames, uint8_t count) { (void)frames; (void)count; }
void LIN_Init(void (*rx)(const LIN_Device_Frame_t *), void (*tx)(LIN_Device_Frame_t *)) { (void)rx; (void)tx; }
uint32_t gCRC32Table[256];
uint8_t s_state;
static jmp_buf resetPoint;
static unsigned operations;
static unsigned cutAt;
static uint32_t failAddress;
static unsigned tests;
static uint8_t oldImage[196608];
static uint8_t newImage[196608];
static uint8_t response[130];
void HostAppStart(void);
void App_Reprogram_Process(void);
void App_Reprogram_HardFault(void);
uint8_t App_Reprogram_Request(const uint8_t *, uint8_t, uint8_t *);

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
void HostReset(void) { longjmp(resetPoint, 1); }
void HostJump(uint32_t address) { CHECK(address == REPROGRAM_APP_START); longjmp(resetPoint, 2); }
void Flash_Read(uint32_t address, void *data, uint32_t length) { CHECK(address != 0x400100 && address != 0x401000); memcpy(data, (const void *)(uintptr_t)address, length); }
void CRC32_Init(void)
{
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (unsigned j = 0; j < 8; j++) c = (c & 1) ? (c >> 1) ^ 0xEDB88320UL : c >> 1;
        gCRC32Table[i] = c;
    }
}
uint32_t CRC32_GetRunTimeCRC32(const uint8_t *data, uint16_t length) { return Reprogram_Crc(data, length); }
uint8_t HostCommand(uint8_t command)
{
    uint32_t region = (hostNvm.NVMCTRL_ADDR * 2) / 0x8000;
    CHECK(region < 16);
    if (command == NVMCTRL_CTRLA_CMD_UR_Val) hostNvm.NVMCTRL_LOCK |= (uint16_t)(1U << region);
    else hostNvm.NVMCTRL_LOCK &= (uint16_t)~(1U << region);
    return 1;
}
static void cutPoint(void)
{
    operations++;
    if (cutAt != 0 && operations == cutAt) longjmp(resetPoint, 3);
}
uint8_t HostWriteRow(uint32_t address, const uint32_t *data)
{
    CHECK((address & 255) == 0);
    CHECK(address == REPROGRAM_MAIN_ADDR || address == REPROGRAM_BACKUP_ADDR || (address >= REPROGRAM_APP_START && address <= REPROGRAM_APP_END - 255) || (address >= REPROGRAM_RECOVERY_START && address <= REPROGRAM_RECOVERY_END - 255));
    if (address < 0x80000) CHECK((hostNvm.NVMCTRL_LOCK & (1U << (address / 0x8000))) != 0);
    if (address == failAddress) return 0;
    memset((void *)(uintptr_t)address, 255, 256);
    cutPoint();
    for (unsigned page = 0; page < 4; page++)
    {
        memcpy((void *)(uintptr_t)(address + page * 64), (const uint8_t *)data + page * 64, 64);
        cutPoint();
    }
    return 1;
}
static void makeImage(uint8_t *image, unsigned size, unsigned seed)
{
    for (unsigned i = 0; i < size; i++) image[i] = (uint8_t)((i * 13 + seed) % 251);
    ((uint32_t *)image)[0] = 0x2000FFF8;
    ((uint32_t *)image)[1] = 0x00010101;
}
static void seedRecord(uint32_t flag, unsigned size, unsigned recovery, unsigned count)
{
    memset(&gUpdateMain, 0, sizeof(gUpdateMain));
    gUpdateMain.magic = REPROGRAM_MAGIC;
    gUpdateMain.index = 7;
    gUpdateMain.flag = flag;
    gUpdateMain.address = REPROGRAM_APP_START;
    gUpdateMain.length = size;
    gUpdateMain.imageCrc = Reprogram_Crc((const uint8_t *)REPROGRAM_APP_START, size);
    gUpdateMain.recoveryMagic = REPROGRAM_RECOVERY_MAGIC;
    gUpdateMain.appConfirmed = 1;
    gUpdateMain.restoreCount = count;
    if (recovery)
    {
        gUpdateMain.recoveryLength = recovery;
        gUpdateMain.recoveryCrc = Reprogram_Crc((const uint8_t *)REPROGRAM_RECOVERY_START, recovery);
    }
    gUpdateMain.crc = Reprogram_Crc((const uint8_t *)&gUpdateMain.magic, 252);
    memcpy((void *)REPROGRAM_MAIN_ADDR, &gUpdateMain, 256);
    memcpy((void *)REPROGRAM_BACKUP_ADDR, &gUpdateMain, 256);
}
static void setup(unsigned size)
{
    cutAt = 0;
    operations = 0;
    failAddress = 0;
    memset((void *)0x10000, 255, 0x70000);
    memset((void *)0x400000, 255, 0x2000);
    // PDS and the unused 64KB are sentinels, never update/restore destinations.
    memset((void *)0x400100, 0x51, 256);
    memset((void *)0x401000, 0x61, 256);
    memset((void *)0x40000, 0x71, 0x10000);
    makeImage(oldImage, size, 11);
    makeImage(newImage, size, 73);
    memcpy((void *)REPROGRAM_APP_START, oldImage, size);
    hostNvm.NVMCTRL_LOCK = 0;
    oAppTimer.timeMs = 0;
    gConFig.Init_Number = 0x12345678;
    memset(&gReprogram, 0, sizeof(gReprogram));
    gReprogram.selected = 1;
    gReprogram.session = 1;
    seedRecord(1, size, 0, 0);
}
static void checkMirrors(void)
{
    CHECK(memcmp((void *)REPROGRAM_MAIN_ADDR, (void *)REPROGRAM_BACKUP_ADDR, 256) == 0);
    Flash_Read(REPROGRAM_MAIN_ADDR, &gUpdateMain, 256);
    CHECK(gUpdateMain.crc == Reprogram_Crc((const uint8_t *)&gUpdateMain.magic, 252));
    for (unsigned i = 0; i < 256; i++)
    {
        CHECK(((uint8_t *)0x400100)[i] == 0x51);
        CHECK(((uint8_t *)0x401000)[i] == 0x61);
    }
    for (unsigned i = 0; i < 0x10000; i++) CHECK(((uint8_t *)0x40000)[i] == 0x71);
}
static uint8_t downloadStart(unsigned size)
{
    uint8_t request[] = { 0x34, 0, 0x44, 0, 1, 0, 0, 0, (uint8_t)(size >> 16), (uint8_t)(size >> 8), (uint8_t)size };
    return Reprogram_Request(request, sizeof(request), response);
}
static int boot(void)
{
    int result = setjmp(resetPoint);
    if (result == 0) Reprogram_Init();
    return result;
}
static void transfer(unsigned size)
{
    uint8_t request[130] = { 0x36, 1 };
    for (unsigned offset = 0; offset < size; offset += 128)
    {
        unsigned bytes = size - offset;
        if (bytes > 128) bytes = 128;
        memcpy(request + 2, newImage + offset, bytes);
        CHECK(Reprogram_Request(request, (uint8_t)(bytes + 2), response) == 2);
        CHECK(response[0] == 0x76 && response[1] == request[1]);
        CHECK(Reprogram_Request(request, (uint8_t)(bytes + 2), response) == 2);
        request[1]++;
    }
    uint32_t crc = Reprogram_Crc(newImage, size);
    uint8_t exitRequest[] = { 0x37, (uint8_t)(crc >> 24), (uint8_t)(crc >> 16), (uint8_t)(crc >> 8), (uint8_t)crc };
    CHECK(Reprogram_Request(exitRequest, sizeof(exitRequest), response) == 1 && response[0] == 0x77);
    CHECK(memcmp((void *)REPROGRAM_APP_START, newImage, size) == 0);
    CHECK(hostNvm.NVMCTRL_LOCK == 0);
}
static void fault(void)
{
    int result = setjmp(resetPoint);
    if (result == 0) App_Reprogram_HardFault();
    CHECK(result == 1);
}
static void testIndependentBoot(void)
{
    uint8_t select[] = { 0x31, 1, 0xF3, 2, 0, 0, 0, 0 };
    uint8_t session[] = { 0x10, 2 };
    uint8_t discovery[] = { 0x31, 3, 0xF3, 0, 0 };
    uint32_t selectedKey = 0;
    for (unsigned board = 0; board < 4; board++)
    {
        setup(26009);
        hostUid[0] = 0x13572468 + board;
        LINT_Init();
        uint32_t key = gUDSInitNumber;
        CHECK(key == Reprogram_Crc((const uint8_t *)hostUid, sizeof(hostUid)));
        CHECK(gConFig.Init_Number != key);
        discovery[4] = s_discoverySlot;
        CHECK(UDS_HandleBroadcast(discovery, sizeof(discovery), response) == 9);
        CHECK(response[8] == 1 && response[4] == (uint8_t)(key >> 24) && response[7] == (uint8_t)key);
        // The stored PDS bytes may be any layout, blank, or invalid.
        memset((void *)0x400100, board * 31, 256);
        memset((void *)0x401000, board * 47, 256);
        UDS_Init();
        CHECK(key == gUDSInitNumber);
        CHECK(((uint8_t *)0x400100)[0] == board * 31 && ((uint8_t *)0x401000)[255] == board * 47);
        gReprogram.selected = 0;
        CHECK(Reprogram_Request(session, sizeof(session), response) == 0);
        for (unsigned i = 0; i < 4; i++) select[4 + i] = (uint8_t)(key >> (24 - i * 8));
        CHECK(UDS_HandleBroadcast(select, sizeof(select), response) == 10 && response[9] == UDS_BOOT_NAD);
        CHECK(gReprogram.selected == 1);
        LIN_QueueEntry_t entry = { 0 };
        entry.generation = s_diag.generation;
        entry.frame.data[0] = (uint8_t)(board + 1);
        entry.frame.data[1] = 2; entry.frame.data[2] = 0x10; entry.frame.data[3] = 2;
        LINT_HandleDiagnostic(&entry);
        CHECK(s_txRing.head == s_txRing.tail);
        entry.frame.data[0] = UDS_BOOT_NAD;
        LINT_HandleDiagnostic(&entry);
        CHECK(s_txRing.head != s_txRing.tail && gReprogram.session == 1);
        LIN_Device_Frame_t frame = { 0 };
        LINT_TxCallback(&frame);
        CHECK(frame.data[0] == UDS_BOOT_NAD && frame.data[2] == 0x50);
        select[7] ^= 1;
        CHECK(UDS_HandleBroadcast(select, sizeof(select), response) == 0 && gReprogram.selected == 0);
        CHECK(Reprogram_Request(session, sizeof(session), response) == 0);
        const uint8_t writeProduct[] = { 0x2E, 0xF2, 2, 0 };
        CHECK(UDS_HandleBroadcast(writeProduct, sizeof(writeProduct), response) == 0);
        selectedKey = key;
        tests++;
    }
    // Two or more Boot nodes sharing the fixed NAD still require a matching key.
    for (unsigned i = 0; i < 4; i++) select[4 + i] = (uint8_t)(selectedKey >> (24 - i * 8));
    for (unsigned board = 0; board < 4; board++)
    {
        hostUid[0] = 0x13572468 + board;
        UDS_Init();
        CHECK(UDS_HandleBroadcast(select, sizeof(select), response) == (board == 3 ? 10 : 0));
        CHECK(Reprogram_Request(session, sizeof(session), response) == (board == 3 ? 6 : 0));
    }
    puts("PASS: actual Boot UDS/LIN: CPU-only identity, fixed NAD, four-board isolation, arbitrary PDS untouched");
}
int main(void)
{
    CHECK(VirtualAlloc((void *)0x10000, 0x70000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) == (void *)0x10000);
    CHECK(VirtualAlloc((void *)0x400000, 0x10000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) == (void *)0x400000);
    CHECK(sizeof(Reprogram_Record_t) == 256);
    CRC32_Init();
    testIndependentBoot();
    setup(26009);
    CHECK(downloadStart(26009) == 4);
    CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, oldImage, 26009) == 0);
    unsigned backupOperations = operations;
    transfer(26009);
    CHECK(boot() == 2);
    for (unsigned attempt = 0; attempt < 3; attempt++)
    {
        HostAppStart();
        fault();
        CHECK(boot() == (attempt == 2 ? 1 : 2));
    }
    CHECK(boot() == 2);
    checkMirrors();
    CHECK(gUpdateMain.flag == 0 && gUpdateMain.faultCount == 0 && gUpdateMain.restoreCount == 1 && gUpdateMain.recoveryUsed == 1);
    CHECK(memcmp((void *)REPROGRAM_APP_START, oldImage, 26009) == 0);
    CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, oldImage, 26009) == 0);
    HostAppStart();
    oAppTimer.timeMs = 10001;
    App_Reprogram_Process();
    checkMirrors();
    CHECK(gUpdateMain.restoreCount == 1 && gUpdateMain.appConfirmed == 1);
    for (unsigned attempt = 0; attempt < 3; attempt++) { HostAppStart(); fault(); }
    CHECK(boot() == 0);
    CHECK(gUpdateMain.flag == 2 && gUpdateMain.restoreCount == 1);
    tests++;
    puts("PASS: real Boot/App logic: update, three HardFaults, rollback, persisted count, no rollback loop");

    for (unsigned point = 1; point <= backupOperations; point++)
    {
        setup(26009);
        cutAt = point;
        int stopped = setjmp(resetPoint);
        if (stopped == 0) downloadStart(26009);
        CHECK(stopped == 3);
        cutAt = 0;
        CHECK(memcmp((void *)REPROGRAM_APP_START, oldImage, 26009) == 0);
        CHECK(boot() == 0);
        gReprogram.selected = 1;
        gReprogram.session = 1;
        CHECK(downloadStart(26009) == 4);
        CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, oldImage, 26009) == 0);
        checkMirrors();
        tests++;
    }
    printf("PASS: %u backup power cuts (each erase / page / metadata commit), retry keeps original App\n", backupOperations);

    setup(26009);
    memcpy((void *)REPROGRAM_RECOVERY_START, oldImage, 26009);
    memcpy((void *)REPROGRAM_APP_START, newImage, 26009);
    seedRecord(2, 26009, 26009, 9);
    CHECK(boot() == 1);
    unsigned restoreOperations = operations;
    for (unsigned point = 1; point <= restoreOperations; point++)
    {
        setup(26009);
        memcpy((void *)REPROGRAM_RECOVERY_START, oldImage, 26009);
        memcpy((void *)REPROGRAM_APP_START, newImage, 26009);
        seedRecord(2, 26009, 26009, 9);
        cutAt = point;
        CHECK(boot() == 3);
        cutAt = 0;
        CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, oldImage, 26009) == 0);
        int result = boot();
        CHECK(result == 1 || result == 2);
        CHECK(boot() == 2);
        checkMirrors();
        CHECK(gUpdateMain.flag == 0 && gUpdateMain.restoreCount == 10);
        CHECK(memcmp((void *)REPROGRAM_APP_START, oldImage, 26009) == 0);
        CHECK(hostNvm.NVMCTRL_LOCK == 0);
        tests++;
    }
    printf("PASS: %u restore power cuts, resumes restore, count increments exactly once\n", restoreOperations);

    setup(26009);
    CHECK(downloadStart(26009) == 4);
    memcpy((void *)REPROGRAM_APP_START, newImage, 512);
    CHECK(boot() == 0);
    gReprogram.selected = 1; gReprogram.session = 1;
    CHECK(downloadStart(26009) == 4);
    CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, oldImage, 26009) == 0);
    tests++;
    puts("PASS: interrupted download retry never backs up partial App");

    setup(26009);
    memcpy((void *)REPROGRAM_RECOVERY_START, oldImage, 26009);
    seedRecord(2, 26009, 26009, 0);
    ((uint8_t *)REPROGRAM_RECOVERY_START)[1000] ^= 1;
    CHECK(boot() == 0 && operations == 0);
    CHECK(memcmp((void *)REPROGRAM_APP_START, oldImage, 26009) == 0);
    setup(26009); seedRecord(2, 26009, 0, 0);
    CHECK(boot() == 0 && operations == 0);
    tests += 2;
    puts("PASS: missing / CRC-invalid backup is not restored");

    setup(26009); failAddress = REPROGRAM_RECOVERY_START + 256;
    CHECK(downloadStart(26009) == 3 && response[2] == 0x72);
    CHECK(memcmp((void *)REPROGRAM_APP_START, oldImage, 26009) == 0 && hostNvm.NVMCTRL_LOCK == 0);
    setup(26009);
    memcpy((void *)REPROGRAM_RECOVERY_START, oldImage, 26009);
    seedRecord(2, 26009, 26009, 0);
    failAddress = REPROGRAM_APP_START + 256;
    CHECK(boot() == 0 && hostNvm.NVMCTRL_LOCK == 0);
    failAddress = 0;
    CHECK(boot() == 1);
    checkMirrors();
    CHECK(gUpdateMain.restoreCount == 1);
    tests += 2;
    puts("PASS: backup/restore write failures preserve backup and retry state");

    setup(26009);
    gUpdateMain.length = 0;
    gUpdateMain.imageCrc = 0;
    CHECK(Reprogram_Store(1) != 0);
    CHECK(downloadStart(26009) == 4);
    CHECK(gUpdateMain.recoveryLength == 26009);
    setup(26009);
    CHECK(downloadStart(196609) == 3 && response[2] == 0x31 && operations == 0);
    setup(196608);
    CHECK(downloadStart(196608) == 4);
    CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, oldImage, 196608) == 0);
    transfer(196608);
    seedRecord(2, 196608, 196608, 0);
    CHECK(boot() == 1);
    CHECK(memcmp((void *)REPROGRAM_APP_START, oldImage, 196608) == 0);
    checkMirrors();
    tests += 3;
    puts("PASS: first IAR image length detection, 192KB limit, all six Regions copied, PDS/spare untouched");

    setup(26009);
    seedRecord(0, 26009, 0, 4);
    HostAppStart();
    uint8_t selectRequest[] = { 0x31, 1, 0xF3, 2, 0x12, 0x34, 0x56, 0x78 };
    uint8_t sessionRequest[] = { 0x10, 2 };
    uint8_t statusRequest[] = { 0x22, 0xF2, 0x10 };
    CHECK(App_Reprogram_Request(selectRequest, sizeof(selectRequest), response) == 9);
    oAppTimer.timeMs = 10001;
    App_Reprogram_Process();
    CHECK(App_Reprogram_Request(selectRequest, sizeof(selectRequest), response) == 9);
    CHECK(App_Reprogram_Request(statusRequest, sizeof(statusRequest), response) == 38);
    CHECK((response[37] & 4) != 0 && response[32] == 4);
    // Replace the image as an IAR download would, leaving the old metadata in Data Flash.
    memcpy((void *)REPROGRAM_APP_START, newImage, 26009);
    CHECK(App_Reprogram_Request(sessionRequest, sizeof(sessionRequest), response) == 6);
    CHECK(boot() == 0);
    gReprogram.selected = 1; gReprogram.session = 1;
    CHECK(downloadStart(26009) == 4);
    CHECK(memcmp((void *)REPROGRAM_RECOVERY_START, newImage, 26009) == 0);
    CHECK(gUpdateMain.restoreCount == 4);
    tests++;
    puts("PASS: selected App confirms at 10 seconds; IAR-replaced running image is backed up; F210 count survives");

    setup(26009);
    memset((void *)REPROGRAM_MAIN_ADDR, 255, 256);
    memset((void *)REPROGRAM_BACKUP_ADDR, 255, 256);
    CHECK(boot() == 2);
    memset((void *)REPROGRAM_APP_START, 255, 26009);
    CHECK(boot() == 0);
    gReprogram.selected = 1; gReprogram.session = 1;
    CHECK(downloadStart(26009) == 4);
    transfer(26009);
    CHECK(boot() == 2);
    tests++;
    puts("PASS: blank first installation and Boot-only download remain supported");
    printf("TOTAL PASS: %u scenarios; hardware Flash/LIN timing is NOT simulated\n", tests);
    return 0;
}
