/*
 * BUTTONS_Program.c
 */
#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#define F_CPU 8000000UL
#include <util/delay.h>

#include "../Include/MCAL/DIO/DIO_Interface.h"
#include "../Include/HAL/BUTTONS/BUTTONS_Interface.h"
#include "../Include/HAL/BUTTONS/BUTTONS_Cfg.h"

static u8 BTN_u8IsLow(u8 A_u8Pin)
{
    u8 Local_u8Val;
    DIO_u8GetPinValue(BTN_u8_PORT, A_u8Pin, &Local_u8Val);
    return (Local_u8Val == DIO_u8_LOW);
}

u8 BTN_u8GetPressed(void)
{
    if (BTN_u8IsLow(BTN_u8_UP_PIN))     { _delay_ms(20); if (BTN_u8IsLow(BTN_u8_UP_PIN))     return BTN_UP; }
    if (BTN_u8IsLow(BTN_u8_DOWN_PIN))   { _delay_ms(20); if (BTN_u8IsLow(BTN_u8_DOWN_PIN))   return BTN_DOWN; }
    if (BTN_u8IsLow(BTN_u8_SELECT_PIN)) { _delay_ms(20); if (BTN_u8IsLow(BTN_u8_SELECT_PIN)) return BTN_SELECT; }
    if (BTN_u8IsLow(BTN_u8_BACK_PIN))   { _delay_ms(20); if (BTN_u8IsLow(BTN_u8_BACK_PIN))   return BTN_BACK; }
    return BTN_NONE;
}

u8 BTN_u8WaitForPress(void)
{
    u8 Local_u8Btn;
    do {
        Local_u8Btn = BTN_u8GetPressed();
    } while (Local_u8Btn == BTN_NONE);

    while (BTN_u8GetPressed() != BTN_NONE)  /* wait for release */
        _delay_ms(10);

    return Local_u8Btn;
}
