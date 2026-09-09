/*
 * BUZZER_Interface.h
 */
#ifndef INCLUDE_HAL_BUZZER_BUZZER_INTERFACE_H_
#define INCLUDE_HAL_BUZZER_BUZZER_INTERFACE_H_

void BUZZ_voidOn(void);
void BUZZ_voidOff(void);
void BUZZ_voidBeep(u16 A_u16Ms);
void BUZZ_voidAlarm(u16 A_u16Seconds);   /* intermittent alarm; returns after A_u16Seconds
                                             OR early if BTN_BACK is pressed */

#endif
