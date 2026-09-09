/*
 * EEPROM_Program.c
 *
 *  Created on: Aug 7, 2026
 *      Author: Mohammed Wael
 */

#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"
#include <util/delay.h>
#include "../Include/MCAL/TWI/TWI_Interface.h"
#include "../Include/HAL/EEPROM/EEPROM_Interface.h"


//Initialize EEPROM

void EEPROM_Init(void)
{
    I2C_Init();
}

 // Write One Byte

u8 EEPROM_WriteByte(u16 Copy_u16Address, u8 Copy_u8Data)
{
    /* Send Start Condition */
    I2C_Start();

    if(I2C_Status() != I2C_START)
        return EEPROM_ERROR;

    /* Send Slave Address + Write */
    I2C_Write((u8)(EEPROM_ADDRESS | ((Copy_u16Address & 0x0700)>>7)));

    if(I2C_Status() != I2C_MT_SLA_W_ACK)
        return EEPROM_ERROR;

    /* Send Word Address */
    I2C_Write((u8)Copy_u16Address);

    if(I2C_Status() != I2C_MT_DATA_ACK)
        return EEPROM_ERROR;

    /* Send Data */
    I2C_Write(Copy_u8Data);

    if(I2C_Status() != I2C_MT_DATA_ACK)
        return EEPROM_ERROR;

    /* Stop */
    I2C_Stop();

    /* EEPROM Write Cycle */
    _delay_ms(10);

    return EEPROM_SUCCESS;
}


 // Read One Byte

u8 EEPROM_ReadByte(u16 Copy_u16Address, u8 *Copy_pu8Data)
{
    /* Send Start */
    I2C_Start();

    if(I2C_Status() != I2C_START)
        return EEPROM_ERROR;

    /* Slave Address + Write */
    I2C_Write((u8)(EEPROM_ADDRESS | ((Copy_u16Address & 0x0700)>>7)));

    if(I2C_Status() != I2C_MT_SLA_W_ACK)
        return EEPROM_ERROR;

    /* Word Address */
    I2C_Write((u8)Copy_u16Address);

    if(I2C_Status() != I2C_MT_DATA_ACK)
        return EEPROM_ERROR;

    /* Repeated Start */
    I2C_Start();

    if(I2C_Status() != I2C_REP_START)
        return EEPROM_ERROR;

    /* Slave Address + Read */
    I2C_Write((u8)(EEPROM_ADDRESS | ((Copy_u16Address & 0x0700)>>7) | 0x01));

    if(I2C_Status() != I2C_MT_SLA_R_ACK)
        return EEPROM_ERROR;

    /* Read Byte */
    *Copy_pu8Data = I2C_Read_NO_ACK();

    if(I2C_Status() != I2C_MR_DATA_NACK)
        return EEPROM_ERROR;

    /* Stop */
    I2C_Stop();

    return EEPROM_SUCCESS;
}
