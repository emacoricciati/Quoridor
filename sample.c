/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "GLCD/HzLib.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "game/game.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
	
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	BUTTON_init();
	joystick_init();
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	enable_RIT();
	init_timer(0, 0x000F4240); 						    /* Timer0 initialization 1 s */ //0x017D7840 -> 1s   
	init_timer(1, 0x00E4E1C0); 						    /* Timer1 initialization 5 s */ //0x07735940 -> 15 s  
	
	initial_screen();
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						

  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
