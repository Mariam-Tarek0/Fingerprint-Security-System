/*
 * LCD_Cfg.h
 * Data bus moved from PORTD to PORTB (PORTD is needed by the hardware
 * USART for the AS608 fingerprint sensor - an 8-bit LCD bus can't share
 * a port with that).
 */
#ifndef INCLUDE_HAL_LCD_LCD_CFG_H_
#define INCLUDE_HAL_LCD_LCD_CFG_H_

#define LCD_u8_CONTROL_PORT  DIO_u8_PORTB
#define LCD_u8_RS_PIN        DIO_u8_PIN0
#define LCD_u8_RW_PIN        DIO_u8_PIN1
#define LCD_u8_E_PIN         DIO_u8_PIN2

#define LCD_u8_DATA_PORT     DIO_u8_PORTA

#endif /* INCLUDE_HAL_LCD_LCD_CFG_H_ */
