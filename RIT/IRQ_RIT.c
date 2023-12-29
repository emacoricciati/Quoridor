/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "../time/time.h"
#include "../game/game.h"

extern Player p1,p2;
volatile int pressed = 0;
extern int turn;
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler (void)
{			

	static int up = 0;
	static int down = 0;
	static int left = 0;
	static int right = 0;
	static int select = 0;
	
		// joystick up
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		up++;
		switch(up){
			case 1:
				if(turn == 1){
					move_player(&p1,'u');
				}
				else if(turn == 2){
					move_player(&p2,'u');
				}
				break;
			default:
				break;
		}
	}
	else{
			up=0;
	}
	
	// joystick down
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		down++;
		switch(down){
			case 1:
				if(turn == 1){
					move_player(&p1,'d');
				}
				else if(turn == 2){
					move_player(&p2,'d');
				}
				break;
			default:
				break;
		}
	}
	else{
			down=0;
	}
	
	// joystick left
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
				if(turn == 1){
					move_player(&p1,'l');
				}
				else if(turn == 2){
					move_player(&p2,'l');
				}
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	// joystick right
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1:
				if(turn == 1){
					move_player(&p1,'r');
				}
				else if(turn == 2){
					move_player(&p2,'r');
				}
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	// joystick select
	
		if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick SELECT pressed */
		select++;
		switch(select){
			case 1:
				turn == 1 ? confirm_move(&p1) : confirm_move(&p2); // TODO
				break;
			default:
				break;
		}
	}
	else{
			select=0;
	}
	
		/* button management */
	if(pressed>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */
			switch(pressed){				
				case 2:
					LCD_Clear(White);
					reset_clock();
					init_game_matrix();
					init_players();
					display_grid();
					if(turn == 1){
						find_possible_moves(&p1);
					}
					else {
						find_possible_moves(&p2);
					}

					break;
				default:
					break;
			}
			pressed++;
		}
		else {	/* button released */
			pressed=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	disable_RIT();
	reset_RIT();
	enable_RIT();
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
