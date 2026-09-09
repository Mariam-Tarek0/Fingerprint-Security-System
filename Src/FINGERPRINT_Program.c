/*
 * FINGERPRINT_Program.c
 * -----------------------------------------------------------------------
 * IMPORTANT NOTE ON THE RECEIVE STRATEGY (read this before touching RX code):
 *
 * Your MUART_voidReadByteSynchNonBlocking() returns 255 both when nothing
 * arrived (timeout) AND when the byte that really arrived IS 0xFF. That's
 * a problem here because every AS608 reply contains four 0xFF bytes (the
 * module's default 32-bit address, 0xFFFFFFFF, is echoed in every packet).
 * If we used that function for the whole packet, we'd misread a real 0xFF
 * as "no data" constantly.
 *
 * Fix used below: we only need a *timeout* for the very first byte of a
 * reply (the header, which is always 0xEF - never 0xFF, so no ambiguity
 * there). We poll for it with the non-blocking function. Once we've seen
 * 0xEF, the module is actively mid-transmission, so we safely switch to
 * MUART_u8ReadByteSyncBlocking() (no timeout, no ambiguity) for the rest
 * of that same packet - a real AS608 always finishes sending a packet it
 * has started.
 * -----------------------------------------------------------------------
 */
//#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#include "../Include/MCAL/UART/UART_Interface.h"
#include "../Include/HAL/FINGERPRINT/FINGERPRINT_Interface.h"
#include "../Include/HAL/FINGERPRINT/FINGERPRINT_Cfg.h"

#define FP_HEADER_HI  0xEF
#define FP_HEADER_LO  0x01
#define PID_COMMAND   0x01

#define CMD_GEN_IMG      0x01
#define CMD_IMG2TZ       0x02
#define CMD_SEARCH       0x04
#define CMD_REG_MODEL    0x05
#define CMD_STORE        0x06
#define CMD_DELETE_CHAR  0x0C
#define CMD_EMPTY        0x0D
#define CMD_VFY_PWD      0x13
#define CMD_TEMPLATE_NUM 0x1D

/* -------- low level packet send -------- */

static void FP_voidSendPacket(u8 A_u8Pid, const u8 *A_pu8Content, u16 A_u16ContentLen)
{
    u16 Local_u16PktLen = A_u16ContentLen + 2; /* content + 2-byte checksum */
    u16 Local_u16Checksum = A_u8Pid + (Local_u16PktLen >> 8) + (Local_u16PktLen & 0xFF);
    u16 i;

    MUART_voidSendByteSynchBlocking(FP_HEADER_HI);
    MUART_voidSendByteSynchBlocking(FP_HEADER_LO);

    MUART_voidSendByteSynchBlocking((u8)(FP_ADDRESS >> 24));
    MUART_voidSendByteSynchBlocking((u8)(FP_ADDRESS >> 16));
    MUART_voidSendByteSynchBlocking((u8)(FP_ADDRESS >> 8));
    MUART_voidSendByteSynchBlocking((u8)(FP_ADDRESS));

    MUART_voidSendByteSynchBlocking(A_u8Pid);
    MUART_voidSendByteSynchBlocking((u8)(Local_u16PktLen >> 8));
    MUART_voidSendByteSynchBlocking((u8)(Local_u16PktLen & 0xFF));

    for (i = 0; i < A_u16ContentLen; i++) {
        MUART_voidSendByteSynchBlocking(A_pu8Content[i]);
        Local_u16Checksum += A_pu8Content[i];
    }

    MUART_voidSendByteSynchBlocking((u8)(Local_u16Checksum >> 8));
    MUART_voidSendByteSynchBlocking((u8)(Local_u16Checksum & 0xFF));
}

/* Poll (non-blocking, bounded) for the 0xEF header byte. Safe: 0xEF is
 * never confused with the "timeout" sentinel (255 / 0xFF). */
static u8 FP_u8WaitForHeader(void)
{
    u16 Local_u16Polls = FP_HEADER_WAIT_POLLS;
    while (Local_u16Polls--) {
        u8 Local_u8Byte = MUART_voidReadByteSynchNonBlocking();
        if (Local_u8Byte == FP_HEADER_HI)
            return 1;
    }
    return 0;
}

/* Reads one reply packet (after the 0xEF header byte has already been seen).
 * Fills A_pu8Content (must hold at least A_u8MaxLen bytes) and A_pu8Len.
 * Returns 1 on success, 0 on a malformed packet. */
static u8 FP_u8ReadRestOfPacket(u8 *A_pu8Content, u8 *A_pu8Len, u8 A_u8MaxLen)
{
    u8 Local_u8Byte;
    u16 Local_u16PktLen, Local_u16ContentLen, i;

    Local_u8Byte = MUART_u8ReadByteSyncBlocking();
    if (Local_u8Byte != FP_HEADER_LO) return 0;

    for (i = 0; i < 4; i++)                 /* address, ignored */
        MUART_u8ReadByteSyncBlocking();

    MUART_u8ReadByteSyncBlocking();         /* pid, ignored (expected PID_ACK) */

    Local_u16PktLen  = (u16)MUART_u8ReadByteSyncBlocking() << 8;
    Local_u16PktLen |= MUART_u8ReadByteSyncBlocking();

    Local_u16ContentLen = Local_u16PktLen - 2; /* minus checksum bytes */
    if (Local_u16ContentLen > A_u8MaxLen) Local_u16ContentLen = A_u8MaxLen;

    for (i = 0; i < Local_u16ContentLen; i++)
        A_pu8Content[i] = MUART_u8ReadByteSyncBlocking();

    MUART_u8ReadByteSyncBlocking();         /* checksum hi, ignored */
    MUART_u8ReadByteSyncBlocking();         /* checksum lo, ignored */

    *A_pu8Len = (u8)Local_u16ContentLen;
    return 1;
}

/* Sends a command, waits for the ack, returns the confirmation code
 * (content[0]). Extra ack bytes (e.g. search results) land in A_pu8Extra. */
static u8 FP_u8DoCommand(const u8 *A_pu8Content, u16 A_u16ContentLen,
                          u8 *A_pu8Extra, u8 A_u8ExtraMax, u8 *A_pu8ExtraLen)
{
    u8 Local_au8Buf[16];
    u8 Local_u8Len = 0;

    FP_voidSendPacket(PID_COMMAND, A_pu8Content, A_u16ContentLen);

    if (!FP_u8WaitForHeader())
        return FP_TIMEOUT;

    if (!FP_u8ReadRestOfPacket(Local_au8Buf, &Local_u8Len, sizeof(Local_au8Buf)))
        return FP_TIMEOUT;

    if (Local_u8Len == 0)
        return FP_TIMEOUT;

    if (A_pu8Extra && A_pu8ExtraLen) {
        u8 n = (Local_u8Len - 1 > A_u8ExtraMax) ? A_u8ExtraMax : (Local_u8Len - 1);
        u8 i;
        for (i = 0; i < n; i++) A_pu8Extra[i] = Local_au8Buf[1 + i];
        *A_pu8ExtraLen = n;
    }
    return Local_au8Buf[0]; /* confirmation code */
}

/* -------- public API -------- */

void FP_voidInit(void)
{
    MUART_voidInit();
}

u8 FP_u8VerifyPassword(void)
{
    u8 Local_au8Content[5];
    Local_au8Content[0] = CMD_VFY_PWD;
    Local_au8Content[1] = (u8)(FP_PASSWORD >> 24);
    Local_au8Content[2] = (u8)(FP_PASSWORD >> 16);
    Local_au8Content[3] = (u8)(FP_PASSWORD >> 8);
    Local_au8Content[4] = (u8)(FP_PASSWORD);
    return FP_u8DoCommand(Local_au8Content, sizeof(Local_au8Content), 0, 0, 0);
}

static u8 FP_u8GetImage(void)
{
    u8 Local_au8Content[1] = { CMD_GEN_IMG };
    return FP_u8DoCommand(Local_au8Content, 1, 0, 0, 0);
}

static u8 FP_u8Img2Tz(u8 A_u8BufferId)
{
    u8 Local_au8Content[2] = { CMD_IMG2TZ, A_u8BufferId };
    return FP_u8DoCommand(Local_au8Content, 2, 0, 0, 0);
}

static u8 FP_u8RegModel(void)
{
    u8 Local_au8Content[1] = { CMD_REG_MODEL };
    return FP_u8DoCommand(Local_au8Content, 1, 0, 0, 0);
}

static u8 FP_u8Store(u8 A_u8BufferId, u16 A_u16PageId)
{
    u8 Local_au8Content[4];
    Local_au8Content[0] = CMD_STORE;
    Local_au8Content[1] = A_u8BufferId;
    Local_au8Content[2] = (u8)(A_u16PageId >> 8);
    Local_au8Content[3] = (u8)(A_u16PageId & 0xFF);
    return FP_u8DoCommand(Local_au8Content, sizeof(Local_au8Content), 0, 0, 0);
}

u8 FP_u8Delete(u16 A_u16PageId)
{
    u8 Local_au8Content[5];
    Local_au8Content[0] = CMD_DELETE_CHAR;
    Local_au8Content[1] = (u8)(A_u16PageId >> 8);
    Local_au8Content[2] = (u8)(A_u16PageId & 0xFF);
    Local_au8Content[3] = 0x00;
    Local_au8Content[4] = 0x01; /* delete 1 template */
    return FP_u8DoCommand(Local_au8Content, sizeof(Local_au8Content), 0, 0, 0);
}

u8 FP_u8EmptyDatabase(void)
{
    u8 Local_au8Content[1] = { CMD_EMPTY };
    return FP_u8DoCommand(Local_au8Content, 1, 0, 0, 0);
}

u8 FP_u8GetTemplateCount(u16 *A_pu16Count)
{
    u8 Local_au8Content[1] = { CMD_TEMPLATE_NUM };
    u8 Local_au8Extra[2], Local_u8ExtraLen = 0;
    u8 Local_u8Code = FP_u8DoCommand(Local_au8Content, 1, Local_au8Extra, sizeof(Local_au8Extra), &Local_u8ExtraLen);
    if (Local_u8Code == FP_OK && Local_u8ExtraLen == 2)
        *A_pu16Count = ((u16)Local_au8Extra[0] << 8) | Local_au8Extra[1];
    return Local_u8Code;
}

u8 FP_u8Enroll(u16 A_u16PageId)
{
    u8 Local_u8Code;

    do {
        Local_u8Code = FP_u8GetImage();
    } while (Local_u8Code == FP_ERR_NO_FINGER); /* caller shows "place finger" prompt */
    if (Local_u8Code != FP_OK) return Local_u8Code;

    Local_u8Code = FP_u8Img2Tz(1);
    if (Local_u8Code != FP_OK) return Local_u8Code;

    /* NOTE: caller (main.c) prompts "remove finger, place again" between
     * the two scans on the LCD. */

    do {
        Local_u8Code = FP_u8GetImage();
    } while (Local_u8Code == FP_ERR_NO_FINGER);
    if (Local_u8Code != FP_OK) return Local_u8Code;

    Local_u8Code = FP_u8Img2Tz(2);
    if (Local_u8Code != FP_OK) return Local_u8Code;

    Local_u8Code = FP_u8RegModel();
    if (Local_u8Code != FP_OK) return Local_u8Code;

    return FP_u8Store(1, A_u16PageId);
}

u8 FP_u8Identify(u16 *A_pu16MatchedId, u16 *A_pu16Score)
{
    u8 Local_u8Code = FP_u8GetImage();
    if (Local_u8Code != FP_OK) return Local_u8Code;

    Local_u8Code = FP_u8Img2Tz(1);
    if (Local_u8Code != FP_OK) return Local_u8Code;

    u8 Local_au8Content[6];
    Local_au8Content[0] = CMD_SEARCH;
    Local_au8Content[1] = 0x01;
    Local_au8Content[2] = 0x00; Local_au8Content[3] = 0x00; /* start page 0 */
    Local_au8Content[4] = (u8)(FP_MAX_USERS >> 8);
    Local_au8Content[5] = (u8)(FP_MAX_USERS & 0xFF);

    u8 Local_au8Extra[4], Local_u8ExtraLen = 0;
    Local_u8Code = FP_u8DoCommand(Local_au8Content, sizeof(Local_au8Content), Local_au8Extra, sizeof(Local_au8Extra), &Local_u8ExtraLen);

    if (Local_u8Code == FP_OK && Local_u8ExtraLen == 4) {
        *A_pu16MatchedId = ((u16)Local_au8Extra[0] << 8) | Local_au8Extra[1];
        *A_pu16Score     = ((u16)Local_au8Extra[2] << 8) | Local_au8Extra[3];
    }
    return Local_u8Code;
}
