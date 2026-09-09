/*
 * UART_Cfg.h
 * Baud rate changed from 9600 to 57600 - the AS608's default/fixed
 * factory baud rate (some modules allow changing it via SetSysPara,
 * but 57600 is the out-of-box default on virtually all AS608 boards).
 */
#ifndef INCLUDE_MCAL_UART_UART_CFG_H_
#define INCLUDE_MCAL_UART_UART_CFG_H_

#define BAUDRATE_VALUE 57600
#define NON_BLOCKING_TIME_OUT 5000
#define NO_VALUE_RETURNED 255

#endif /* INCLUDE_MCAL_UART_UART_CFG_H_ */
