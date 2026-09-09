/*
 * CTRL_Program.c
 */
#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#include "../Include/MCAL/DIO/DIO_Interface.h"
#include "../Include/HAL/CTRL/CTRL_Interface.h"
#include "../Include/HAL/CTRL/CTRL_Cfg.h"

static u8 CTRL_u8PinFor(u8 A_u8DeviceId)
{
    switch (A_u8DeviceId) {
        case CTRL_DEV_LED1:  return CTRL_u8_LED1_PIN;
        case CTRL_DEV_LED2:  return CTRL_u8_LED2_PIN;
        case CTRL_DEV_LED3:  return CTRL_u8_LED3_PIN;
        default:             return 0;
//        default:             return CTRL_u8_MOTOR_PIN;
    }
}

void CTRL_voidToggle(u8 A_u8DeviceId)
{
    u8 Local_u8Pin = CTRL_u8PinFor(A_u8DeviceId);
    u8 Local_u8Cur;
    DIO_u8GetPinValue(CTRL_u8_PORT, Local_u8Pin, &Local_u8Cur);
    DIO_u8SetPinValue(CTRL_u8_PORT, Local_u8Pin, (Local_u8Cur == DIO_u8_HIGH) ? DIO_u8_LOW : DIO_u8_HIGH);
}

u8 CTRL_u8IsOn(u8 A_u8DeviceId)
{
    u8 Local_u8Val;
    DIO_u8GetPinValue(CTRL_u8_PORT, CTRL_u8PinFor(A_u8DeviceId), &Local_u8Val);
    return (Local_u8Val == DIO_u8_HIGH);
}
