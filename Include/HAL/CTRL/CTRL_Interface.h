/*
 * CTRL_Interface.h - toggle/read the devices unlocked after a successful scan
 */
#ifndef INCLUDE_HAL_CTRL_CTRL_INTERFACE_H_
#define INCLUDE_HAL_CTRL_CTRL_INTERFACE_H_

#define CTRL_DEV_LED1   0
#define CTRL_DEV_LED2   1
#define CTRL_DEV_LED3   2
#define CTRL_DEV_MOTOR  3

void CTRL_voidToggle(u8 A_u8DeviceId);
u8   CTRL_u8IsOn(u8 A_u8DeviceId);

#endif
