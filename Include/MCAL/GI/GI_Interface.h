/*
 * GI_Interface.h
 *
 *  Created on: Jul 18, 2026
 *      Author: hp
 */

#ifndef INCLUDE_MCAL_GI_GI_INTERFACE_H_
#define INCLUDE_MCAL_GI_GI_INTERFACE_H_

#define SREG (*(volatile u8*)0x5F)

void GI_voidEnable(void);

void GI_voidDisable(void);

#endif /* INCLUDE_MCAL_GI_GI_INTERFACE_H_ */
