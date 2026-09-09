/*
 * UART_Progarm.c
 *
 *  Created on: Jul 30, 2026
 *      Author: hp
 */

#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#include "../Include/MCAL/UART/UART_Interface.h"
#include "../Include/MCAL/UART/UART_Private.h"
#include "../Include/MCAL/UART/UART_Cfg.h"

#define F_CPU 8000000UL
static void(*FuncPtr[3])(void) = {NULL, NULL, NULL};

void MUART_voidInit(void)
{
	u16 local_u16BuadRateValue = BAUD_RATE_EQUATION;

	// Set Baudrate
	UBRRL = (u8)local_u16BuadRateValue;
	UBRRH = (u8)(local_u16BuadRateValue >> 8);

	// Character Size 8-bit
	CLR_BIT(UCSRB,2);

	// Mode --> Asynch
	// Parity --> Disable
	// Stop bit --> 1 stop bit
	UCSRC = CONC_BIT(1,0,0,0,0,1,1,0);

	// Enable Reciever and Transimitter
	SET_BIT(UCSRB,3);
	SET_BIT(UCSRB,4);
}


void MUART_voidSendByteSynchBlocking(u8 A_u8DataByte)
{
	// Wait until UDRE is empty (Polling)
	while(GET_BIT(UCSRA,5) == 0);

	// Send Data into TX register and start Transmission
	UDR = A_u8DataByte;

	// Wait until UDRE is empty (Polling)
	while(GET_BIT(UCSRA,6) == 0);

	// Clear TX Flag
	SET_BIT(UCSRA,6);
}

void MUART_voidSendByteSynchNonBlocking(u8 A_u8DataByte)
{
	u32 local_u32TimeOut = 0;
	// Wait until UDRE is empty or time out
	while(GET_BIT(UCSRA,5) == 0){
		local_u32TimeOut++; // 1 2 3 4 5 ...... 5000
		if(local_u32TimeOut == NON_BLOCKING_TIME_OUT){
			break;
		}
	}


	if(local_u32TimeOut != NON_BLOCKING_TIME_OUT){
		local_u32TimeOut = 0;
		UDR = A_u8DataByte;
		while(GET_BIT(UCSRA,5) == 0){
			local_u32TimeOut++; // 1 2 3 4 5 ...... 5000
			if(local_u32TimeOut == NON_BLOCKING_TIME_OUT){
				break;
			}
		}
	}

	// Clear TX Flag
	SET_BIT(UCSRA,6);
}


void MUART_voidSendStringSyncNonBlocking(u8 *A_puString){
	while(*A_puString != '\0'){
		MUART_voidSendByteSynchNonBlocking(*A_puString++);
	}
}

void MUART_voidSendByteAsync(u8 A_u8DataByte)
{
	if(GET_BIT(UCSRA,5) == 1){
		UDR = A_u8DataByte;
	}
}

u8 MUART_u8ReadByteAsync(void){
	return UDR;
}

u8 MUART_u8ReadByteSyncBlocking (void)
{
	// wait for Rx Flag
	while (GET_BIT(UCSRA,7)==0);

	return UDR;
}

u8 MUART_voidReadByteSynchNonBlocking(void)
{
	u8 local_u8Value = NO_VALUE_RETURNED;
	u32 local_u32TimeOut = 0;
	while(GET_BIT(UCSRA,7)==0){
		local_u32TimeOut++;
		if(local_u32TimeOut == NON_BLOCKING_TIME_OUT){
			break;
		}
	}

	if(local_u32TimeOut != NON_BLOCKING_TIME_OUT){
		local_u8Value = UDR;
	}
	return local_u8Value;
}


void MUART_voidSetCallBack(UART_INTTERRUPTS A_InterruptSource, void(*A_PtoFunc)(void))
{
	if((A_InterruptSource <= UART_TX_INTERRUPT) && (A_PtoFunc != NULL)){
		FuncPtr[A_InterruptSource] = A_PtoFunc;
	}
}

void __vector_13(void) __attribute__((signal));
void __vector_13(void)
{
	FuncPtr[UART_RX_INTERRUPT]();
}

void __vector_14(void) __attribute__((signal));
void __vector_14(void){
	FuncPtr[UART_UDRE_INTERRUPT]();
}

void __vector_15(void) __attribute__((signal));
void __vector_15(void){
	FuncPtr[UART_TX_INTERRUPT]();
}

