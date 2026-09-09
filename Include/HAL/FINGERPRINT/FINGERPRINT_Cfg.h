/*
 * FINGERPRINT_Cfg.h
 */
#ifndef INCLUDE_HAL_FINGERPRINT_FINGERPRINT_CFG_H_
#define INCLUDE_HAL_FINGERPRINT_FINGERPRINT_CFG_H_

#define FP_ADDRESS      0xFFFFFFFF   /* default AS608 module address */
#define FP_PASSWORD     0x00000000   /* default AS608 module password */
#define FP_MAX_USERS    127          /* usable template slots, 1..127 */

/* how many times we poll while waiting for the 0xEF reply-header byte;
 * each poll internally waits up to NON_BLOCKING_TIME_OUT loop-cycles
 * (see UART_Cfg.h). Raise this if your module answers slowly. */
#define FP_HEADER_WAIT_POLLS   300

#endif
