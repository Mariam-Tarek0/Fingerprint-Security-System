/*
 * LCD_Interface.h
 *
 *  Created on: Jul 12, 2026
 *      Author: hp
 */

#ifndef INCLUDE_HAL_LCD_LCD_INTERFACE_H_
#define INCLUDE_HAL_LCD_LCD_INTERFACE_H_

#define LCD_u8_LINE1  1
#define LCD_u8_LINE2  2

void LCD_voidInit(void);

void LCD_voidSendCmnd(u8 Copy_u8Cmnd);

void LCD_voidSendChar(u8 Copy_u8Char);

u8 LCD_u8GoToXY(u8 Copy_u8LineNum, u8 Copy_u8Location);

void LCD_voidClearDisplay(void);

void LCD_voidSendString(u8 *A_u8PtrToString);

void LCD_voidDisplayNumber(s32 A_s32Number);

#endif /* INCLUDE_HAL_LCD_LCD_INTERFACE_H_ */
