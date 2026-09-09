/*
 * UART_Interface.h
 *
 *  Created on: Jul 30, 2026
 *      Author: hp
 */
#include "../../Lib/STD_Types.h"
#include "../../Lib/BIT_Math.h"

#ifndef INCLUDE_MCAL_UART_UART_INTERFACE_H_
#define INCLUDE_MCAL_UART_UART_INTERFACE_H_

typedef enum{
	UART_RX_INTERRUPT = 0,
	UART_UDRE_INTERRUPT,
	UART_TX_INTERRUPT
}UART_INTTERRUPTS;

void MUART_voidInit(void);
void MUART_voidSendByteSynchBlocking(u8 A_u8DataByte);
void MUART_voidSendByteSynchNonBlocking(u8 A_u8DataByte);
void MUART_voidSendStringSyncNonBlocking(u8 *A_puString);
void MUART_voidSendByteAsync(u8 A_u8DataByte);
u8 MUART_u8ReadByteAsync(void);
u8 MUART_u8ReadByteSyncBlocking (void);
u8 MUART_voidReadByteSynchNonBlocking(void);
void MUART_voidSetCallBack(UART_INTTERRUPTS A_InterruptSource, void(*A_PtoFunc)(void));


#endif /* INCLUDE_MCAL_UART_UART_INTERFACE_H_ */
