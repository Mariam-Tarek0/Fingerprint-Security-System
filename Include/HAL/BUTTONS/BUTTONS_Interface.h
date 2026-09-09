/*
 * BUTTONS_Interface.h - debounced push-button reading via DIO_Interface
 */
#ifndef INCLUDE_HAL_BUTTONS_BUTTONS_INTERFACE_H_
#define INCLUDE_HAL_BUTTONS_BUTTONS_INTERFACE_H_

#define BTN_NONE    0
#define BTN_UP      1
#define BTN_DOWN    2
#define BTN_SELECT  3
#define BTN_BACK    4

u8 BTN_u8GetPressed(void);      /* returns BTN_NONE if nothing is currently pressed */
u8 BTN_u8WaitForPress(void);    /* blocks until a button is pressed & released */

#endif
