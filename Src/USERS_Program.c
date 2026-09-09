/*
 * USERS_Program.c
 */
#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#include "../Include/HAL/EEPROM/EEPROM_Interface.h"   /* EEPROM_WriteByte / EEPROM_ReadByte live here */
#include "../Include/HAL/USERS/USERS_Interface.h"
#include "../Include/HAL/USERS/USERS_Cfg.h"

static u16 USERS_u16RecordAddr(u16 A_u16Id)
{
    /* Id is 1-based (matches AS608 page_id); slot 0 is unused */
    return USERS_TABLE_BASE + (u16)(A_u16Id - 1) * USERS_RECORD_SIZE;
}

/* small wrapper: new driver returns a status and writes the byte via a
 * pointer, instead of returning it directly - keep that detail local to
 * this file so the rest of USERS_Program.c reads the same as before. */
static u8 USERS_u8ReadByte(u16 A_u16Addr)
{
    u8 Local_u8Val = 0;
    EEPROM_ReadByte(A_u16Addr, &Local_u8Val);
    return Local_u8Val;
}

u8 USERS_u8Exists(u16 A_u16Id)
{
    if (A_u16Id < 1 || A_u16Id > USERS_MAX_COUNT) return 0;
    return USERS_u8ReadByte(USERS_u16RecordAddr(A_u16Id)) == 1;
}

void USERS_voidSave(u16 A_u16Id, u8 *A_pu8Name)
{
    u16 Local_u16Addr = USERS_u16RecordAddr(A_u16Id);
    u8 i;

    EEPROM_WriteByte(Local_u16Addr, 1); /* used flag */

    for (i = 0; i < USERS_NAME_LEN; i++) {
        u8 Local_u8Ch = A_pu8Name[i];
        EEPROM_WriteByte(Local_u16Addr + 1 + i, Local_u8Ch);
        if (Local_u8Ch == '\0') {
            /* pad the rest of the name field with zeros */
            u8 j;
            for (j = i + 1; j < USERS_NAME_LEN; j++)
                EEPROM_WriteByte(Local_u16Addr + 1 + j, 0);
            break;
        }
    }
    EEPROM_WriteByte(Local_u16Addr + 1 + USERS_NAME_LEN, 0); /* force-terminate */
}

void USERS_voidDelete(u16 A_u16Id)
{
    EEPROM_WriteByte(USERS_u16RecordAddr(A_u16Id), 0); /* clear used flag only */
}

void USERS_voidGetName(u16 A_u16Id, u8 *A_pu8OutName)
{
    u16 Local_u16Addr;
    u8 i;

    if (A_u16Id < 1 || A_u16Id > USERS_MAX_COUNT) { A_pu8OutName[0] = '\0'; return; }

    Local_u16Addr = USERS_u16RecordAddr(A_u16Id) + 1;
    for (i = 0; i < USERS_NAME_LEN; i++)
        A_pu8OutName[i] = USERS_u8ReadByte(Local_u16Addr + i);
    A_pu8OutName[USERS_NAME_LEN] = '\0';
}
