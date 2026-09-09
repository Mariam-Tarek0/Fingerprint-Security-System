/*
 * TWI_Private.h
 *
 *  Created on: Aug 7, 2026
 *      Author: Mohammed Wael
 */

#ifndef INCLUDE_MCAL_TWI_TWI_PRIVATE_H_
#define INCLUDE_MCAL_TWI_TWI_PRIVATE_H_


 //TWI Registers


#define TWBR    (*(volatile u8 *)0x20)
#define TWSR    (*(volatile u8 *)0x21)
#define TWAR    (*(volatile u8 *)0x22)
#define TWDR    (*(volatile u8 *)0x23)
#define TWCR    (*(volatile u8 *)0x56)


 //TWCR Bits


#define TWIE    0
#define TWEN    2
#define TWWC    3
#define TWSTO   4
#define TWSTA   5
#define TWEA    6
#define TWINT   7

#endif /* INCLUDE_MCAL_TWI_TWI_PRIVATE_H_ */
