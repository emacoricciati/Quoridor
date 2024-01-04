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
#include  "GLCD/HzLib.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"

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
	
	GUI_Text(20, 10, (uint8_t *) "Press INT0 to start a new", Black, White);
	GUI_Text(20, 30, (uint8_t *) "match", Black, White);

	init_timer(0, 0x000F4240); 						    /* Timer0 initialization 1 s */
	
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
