/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../time/time.h"
#include "../game/game.h"

volatile int timer0_count = 0;
extern int turn;
extern Player p1,p2;
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	
	char str[20];
	timer0_count++;
	if(timer0_count == 20){
		timer0_count = 0;
		turn == 1 ? confirm_move(&p1) : confirm_move(&p2);
	}
	else {
		update_clock(1000);
	}
	sprintf(str,"%02d s", 20 - timer0_count);
	if(timer0_count >= 15){
		GUI_Text(105,243,(uint8_t *) str, Red, White);
	}
	else{
		GUI_Text(105,243,(uint8_t *) str, Black, White);
	}

  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
