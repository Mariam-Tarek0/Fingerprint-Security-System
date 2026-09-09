/*
 * BUZZER_Program.c
 */
#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#define F_CPU 8000000UL
#include <util/delay.h>

#include "../Include/MCAL/DIO/DIO_Interface.h"
#include "../Include/HAL/BUTTONS/BUTTONS_Interface.h"
#include "../Include/HAL/BUZZER/BUZZER_Interface.h"
#include "../Include/HAL/BUZZER/BUZZER_Cfg.h"

void BUZZ_voidOn(void)  { DIO_u8SetPinValue(BUZZ_u8_PORT, BUZZ_u8_PIN, DIO_u8_LOW); }
void BUZZ_voidOff(void) { DIO_u8SetPinValue(BUZZ_u8_PORT, BUZZ_u8_PIN, DIO_u8_HIGH); }

void BUZZ_voidBeep(u16 A_u16Ms)
{
    BUZZ_voidOn();
    while (A_u16Ms--) _delay_ms(1);
    BUZZ_voidOff();
}

void BUZZ_voidAlarm(u16 A_u16Seconds)
{
    u16 Local_u16Cycles = A_u16Seconds * 2; /* 2 on/off cycles per second */
    while (Local_u16Cycles--) {
        if (BTN_u8GetPressed() == BTN_BACK) break;
        BUZZ_voidOn();  _delay_ms(250);
        BUZZ_voidOff(); _delay_ms(250);
    }
    BUZZ_voidOff();
}
