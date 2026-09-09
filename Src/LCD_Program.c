/*
 * LCD_Program.c
 *
 *  Created on: Jul 12, 2026
 *      Author: hp
 */

#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"
#define F_CPU 8000000UL
#include <util/delay.h>

#include "../Include/MCAL/DIO/DIO_Interface.h"

#include "../Include/HAL/LCD/LCD_Interface.h"
#include "../Include/HAL/LCD/LCD_Private.h"
#include "../Include/HAL/LCD/LCD_Cfg.h"



void LCD_voidInit(void){
	_delay_ms(35);
	// Function Set Command
	LCD_voidSendCmnd(0b00111000);
	_delay_us(40);
	// Display ON/OFF Command
	LCD_voidSendCmnd(0b00001111);
	_delay_us(40);
	// Display Clear
	LCD_voidSendCmnd(0b00000001);
	_delay_ms(2);
	// Entry Mode Set
	LCD_voidSendCmnd(0b00000110);
}

void LCD_voidSendCmnd(u8 Copy_u8Cmnd)
{
	// Step 1: RS = 0
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_RS_PIN, DIO_u8_LOW);

	// Step 2: RW = 0
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_RW_PIN, DIO_u8_LOW);

	// Step 3: Write Command
	DIO_u8SetPortValue(LCD_u8_DATA_PORT, Copy_u8Cmnd);

	// Step 4: E = 1
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_E_PIN, DIO_u8_HIGH);
	_delay_us(1);

	// Step 5: E = 0
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_E_PIN, DIO_u8_LOW);
}

void LCD_voidSendChar(u8 Copy_u8Char)
{
	// Step 1: RS = 0
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_RS_PIN, DIO_u8_HIGH);

	// Step 2: RW = 0
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_RW_PIN, DIO_u8_LOW);

	// Step 3: Write Command
	DIO_u8SetPortValue(LCD_u8_DATA_PORT, Copy_u8Char);

	// Step 4: E = 1
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_E_PIN, DIO_u8_HIGH);
	_delay_us(1);

	// Step 5: E = 0
	DIO_u8SetPinValue(LCD_u8_CONTROL_PORT, LCD_u8_E_PIN, DIO_u8_LOW);
}


u8 LCD_u8GoToXY(u8 Copy_u8LineNum, u8 Copy_u8Location)
{
	u8 Local_ErrorState = STD_TYPES_OK;
	// The location less than 39 to make sure we are in the DDRAM
	if(Copy_u8Location <= 39){
		switch(Copy_u8LineNum){
			case LCD_u8_LINE1: LCD_voidSendCmnd(0x80 + Copy_u8Location); break;
			case LCD_u8_LINE2: LCD_voidSendCmnd(0xc0 + Copy_u8Location); break;
		default: Local_ErrorState = STD_TYPES_NOK;
		}
	}
	else{
		Local_ErrorState = STD_TYPES_NOK;
	}
}

void LCD_voidClearDisplay(void){
	// Clear the Display
	LCD_voidSendCmnd(0b00000001);
	// Wait for more than 1.53 us
	_delay_ms(2);
}

void LCD_voidSendString(u8 *A_u8PtrToString){
	while(* A_u8PtrToString != '\0'){
		LCD_voidSendChar(*A_u8PtrToString);
		A_u8PtrToString++;
	}
}

void LCD_voidDisplayNumber(s32 A_s32Number){
    u32 local_u32Number = 1;

    // 1. Handle the edge case of 0
    if(A_s32Number == 0){
    	LCD_voidSendChar('0');
        return;
    }

    // 2. Handle negative numbers
    if(A_s32Number < 0){
    	LCD_voidSendChar('-');
        A_s32Number = -A_s32Number; // Convert to positive for processing
    }

    // 3. Reverse the number into local_u32Number
    while(A_s32Number != 0){
        local_u32Number = (local_u32Number * 10) + (A_s32Number % 10);
        A_s32Number /= 10;
    }

    // 4. Print the digits in the correct order
    while(local_u32Number != 1){
    	LCD_voidSendChar((local_u32Number % 10) + '0'); // '0' is cleaner than writing 48
        local_u32Number /= 10;
    }

	/*
		First Step : Reverse the number i will present
			650 % 10 = 0 , 10 + 0 = 10 ,,,,,,,,,, 65
			65 % 10 = 5 , 100 + 5 = 105 ,,,,,,,,,, 6.5
			6 % 10 = 6 , 1050 +  6 = 1056 ,,,,,,,,, 0
			The local_u32Number = 1056.

		Second Step : 	Print On the LCD
			1056 % 10 = 6 ,,,,,,, 6 + 48 = 54 (Number 6 in the Ascii Code) ,,,,,,,,, 1056 / 10 = 105
			105 % 10 = 5 ,,,,,,, 5 + 48 = 53 (Number 5 in the Ascii Code) ,,,,,,,,, 105 / 10 = 10
			10 % 10 = 0 ,,,,,,, 0 + 48 = 48 (Number 0 in the Ascii Code) ,,,,,,,,, 10 / 10 = 1
	*/
}








