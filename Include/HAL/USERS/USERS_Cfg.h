/*
 * USERS_Cfg.h
 */
#ifndef INCLUDE_HAL_USERS_USERS_CFG_H_
#define INCLUDE_HAL_USERS_USERS_CFG_H_

#define USERS_NAME_LEN     15   /* max characters in a name (+1 for null) */
#define USERS_RECORD_SIZE  (1 + USERS_NAME_LEN + 1) /* used-flag + name + '\0' = 17 bytes */
#define USERS_MAX_COUNT    127  /* matches FP_MAX_USERS (AS608 slot count) */
#define USERS_TABLE_BASE   0x0000

#endif
