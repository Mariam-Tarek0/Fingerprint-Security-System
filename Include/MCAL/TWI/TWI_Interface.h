/*
 * TWI_Interface.h
 *
 *  Created on: Aug 7, 2026
 *      Author: Mohammed Wael
 */

#ifndef INCLUDE_MCAL_TWI_TWI_INTERFACE_H_
#define INCLUDE_MCAL_TWI_TWI_INTERFACE_H_

#include "../../Lib/STD_Types.h"


 //Status Codes


#define I2C_START              0x08
#define I2C_REP_START          0x10
#define I2C_MT_SLA_W_ACK       0x18
#define I2C_MT_SLA_R_ACK       0x40
#define I2C_MT_DATA_ACK        0x28
#define I2C_MR_DATA_ACK        0x50
#define I2C_MR_DATA_NACK       0x58


 //Functions Prototypes


/* Initialize TWI as Master */
void I2C_Init(void);

/* Send START Condition */
void I2C_Start(void);

/* Send STOP Condition */
void I2C_Stop(void);

/* Send One Byte */
void I2C_Write(u8 Copy_u8Data);

/* Read One Byte with ACK */
u8 I2C_Read_ACK(void);

/* Read Last Byte with NACK */
u8 I2C_Read_NO_ACK(void);

/* Read TWI Status Register */
u8 I2C_Status(void);

#endif /* INCLUDE_MCAL_TWI_TWI_INTERFACE_H_ */
