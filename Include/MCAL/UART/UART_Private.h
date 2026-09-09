/*
 * UART_Private.h
 *
 *  Created on: Jul 30, 2026
 *      Author: hp
 */

#ifndef INCLUDE_MCAL_UART_UART_PRIVATE_H_
#define INCLUDE_MCAL_UART_UART_PRIVATE_H_

#define UDR   (*(volatile u8 *)0x2C)
#define UCSRA (*(volatile u8 *)0x2B)
#define UCSRB (*(volatile u8 *)0x2A)
#define UCSRC (*(volatile u8 *)0x40)
#define UBRRL (*(volatile u8 *)0x29)
#define UBRRH (*(volatile u8 *)0x40)

#define BAUD_RATE_EQUATION ((F_CPU / (16UL * BAUDRATE_VALUE)) - 1UL)

#endif /* INCLUDE_MCAL_UART_UART_PRIVATE_H_ */
