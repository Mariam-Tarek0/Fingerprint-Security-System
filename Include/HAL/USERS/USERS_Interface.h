/*
 * USERS_Interface.h - user-name records on the external 24C64 EEPROM
 * -----------------------------------------------------------------------
 * The AS608 stores fingerprint templates internally, indexed only by a
 * numeric slot (1..127) - it has no concept of a name. This module keeps
 * a small table in the external EEPROM (via your EEPROM_Write_Byte /
 * EEPROM_Read_Byte from TWI_Interface.h) mapping slot -> name.
 * -----------------------------------------------------------------------
 */
#ifndef INCLUDE_HAL_USERS_USERS_INTERFACE_H_
#define INCLUDE_HAL_USERS_USERS_INTERFACE_H_

u8   USERS_u8Exists(u16 A_u16Id);
void USERS_voidSave(u16 A_u16Id, u8 *A_pu8Name);
void USERS_voidDelete(u16 A_u16Id);
void USERS_voidGetName(u16 A_u16Id, u8 *A_pu8OutName /* USERS_NAME_LEN+1 bytes */);

#endif
