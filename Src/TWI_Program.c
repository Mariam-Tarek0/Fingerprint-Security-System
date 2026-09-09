/*
 * TWI_Program.c
 *
 *  Created on: Aug 7, 2026
 *      Author: Mohammed Wael
 */

#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"
#include "../Include/MCAL/TWI/TWI_Interface.h"
#include "../Include/MCAL/TWI/TWI_Private.h"
#include "../Include/MCAL/TWI/TWI_Cfg.h"

 // Initialize TWI

void I2C_Init(void)
{
    /* Prescaler = 1 */
    TWSR = TWI_PRESCALER;

    /* Bit Rate */
    TWBR = TWI_BIT_RATE;

    /* Slave Address (Unused in Master Mode) */
    TWAR = (TWI_ADDRESS << 1);

    /* Enable TWI */
    TWCR = (1 << TWEN);
}


//Send START Condition

void I2C_Start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while(GET_BIT(TWCR,TWINT) == 0);
}


 //Send STOP Condition

void I2C_Stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}


 //Send Data Byte

void I2C_Write(u8 Copy_u8Data)
{
    TWDR = Copy_u8Data;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(GET_BIT(TWCR,TWINT) == 0);
}


 //Read Data with ACK

u8 I2C_Read_ACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    while(GET_BIT(TWCR,TWINT) == 0);

    return TWDR;
}


  //Read Data with NACK

u8 I2C_Read_NO_ACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);

    while(GET_BIT(TWCR,TWINT) == 0);

    return TWDR;
}


 //Read Status Register
u8 I2C_Status(void)
{
    return (TWSR & 0xF8);
}
