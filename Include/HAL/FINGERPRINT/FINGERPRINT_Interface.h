/*
 * FINGERPRINT_Interface.h - AS608 fingerprint sensor driver
 * -----------------------------------------------------------------------
 * Built on top of your MUART_* driver (MCAL/UART). Talks the AS608's
 * binary packet protocol: send a "command" packet, read back an "ack"
 * packet whose first content byte is a confirmation code.
 * -----------------------------------------------------------------------
 */
#ifndef INCLUDE_HAL_FINGERPRINT_FINGERPRINT_INTERFACE_H_
#define INCLUDE_HAL_FINGERPRINT_FINGERPRINT_INTERFACE_H_

/* Confirmation codes we act on (subset of the full AS608 command set) */
#define FP_OK                 0x00
#define FP_ERR_RECEIVE        0x01
#define FP_ERR_NO_FINGER      0x02
#define FP_ERR_ENROLL_FAIL    0x03
#define FP_ERR_DISORDERLY     0x06
#define FP_ERR_TOO_FEW_POINTS 0x07
#define FP_ERR_NO_MATCH       0x08
#define FP_ERR_NOT_FOUND      0x09
#define FP_ERR_COMBINE_FAIL   0x0A
#define FP_ERR_BAD_PAGEID     0x0B
#define FP_ERR_DELETE_FAIL    0x10
#define FP_ERR_CLEAR_FAIL     0x11
#define FP_TIMEOUT            0xFE   /* our own code: module never replied */

void FP_voidInit(void);
u8   FP_u8VerifyPassword(void);

u8   FP_u8Enroll(u16 A_u16PageId);                       /* 2 scans -> store at slot */
u8   FP_u8Identify(u16 *A_pu16MatchedId, u16 *A_pu16Score); /* 1 scan -> search database */
u8   FP_u8Delete(u16 A_u16PageId);
u8   FP_u8EmptyDatabase(void);
u8   FP_u8GetTemplateCount(u16 *A_pu16Count);

#endif
