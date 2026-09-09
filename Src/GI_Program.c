/*
 * GI_Program.c
 *
 *  Created on: Jul 18, 2026
 *      Author: hp
 */

#include "../Include/Lib/STD_Types.h"
#include "../Include/Lib/BIT_Math.h"

#include "../Include/MCAL/GI/GI_Interface.h"

#define SREG (*(volatile u8 *)0x5F)

void GI_voidEnable(void){
	SET_BIT(SREG, 7);
}

void GI_voidDisable(void){
	CLR_BIT(SREG, 7);
}
