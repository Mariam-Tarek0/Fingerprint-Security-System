/*
 * EEPROM_Interface.h
 *
 *  Created on: Aug 7, 2026
 *      Author: Mohammed Wael
 */

#ifndef INCLUDE_HAL_EEPROM_EEPROM_INTERFACE_H_
#define INCLUDE_HAL_EEPROM_EEPROM_INTERFACE_H_

#include "../../Lib/STD_Types.h"
#include "../../Lib/BIT_Math.h"


 // EEPROM Definitions


#define EEPROM_ADDRESS     0xA0

#define EEPROM_ERROR       0
#define EEPROM_SUCCESS     1


 //Functions Prototypes


/* Initialize EEPROM Driver */
void EEPROM_Init(void);

/* Write One Byte */
u8 EEPROM_WriteByte(u16 Copy_u16Address, u8 Copy_u8Data);

/* Read One Byte */
u8 EEPROM_ReadByte(u16 Copy_u16Address, u8 *Copy_pu8Data);

#endif /* INCLUDE_HAL_EEPROM_EEPROM_INTERFACE_H_ */
