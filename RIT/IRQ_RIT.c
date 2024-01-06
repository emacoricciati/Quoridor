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
#include "../game/game.h"

extern Player p1,p2;
volatile int pressed = 0;
volatile int pressedK1 = 0;
volatile int pressedK2 = 0;
extern int turn;
extern int move_mode;
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
				if(move_mode == 1){
					turn == 1 ? move_player(&p1,'u') : move_player(&p2,'u');
				}
				else if(move_mode == 0){
					move_wall('u');
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
				if(move_mode == 1){
					turn == 1 ? move_player(&p1,'d') : move_player(&p2,'d');
				}
				else if(move_mode == 0){
					move_wall('d');
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
				if(move_mode == 1){
					turn == 1 ? move_player(&p1,'l') : move_player(&p2,'l');
				}
				else if(move_mode == 0){
					move_wall('l');
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
				if(move_mode == 1){
					turn == 1 ? move_player(&p1,'r') : move_player(&p2,'r');
				}
				else if(move_mode == 0){
					move_wall('r');
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
				if(move_mode == 1){
					turn == 1 ? confirm_move(&p1) : confirm_move(&p2);
				}
				else if(move_mode == 0){
					confirm_wall();
				}
				break;
			default:
				break;
		}
	}
	else{
			select=0;
	}
	
	/* buttons management */
	
	// INT0
	if(pressed>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */
			switch(pressed){				
				case 2:
					game_setup();
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
	
	//KEY1
	if(pressedK1>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			switch(pressedK1){				
				case 2:
					if(move_mode != -1) switch_mode();
					break;
				default:
					break;
			}
			pressedK1++;
		}
		else {	/* button released */
			pressedK1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 1 pin selection */
		}
	}
	
	//KEY2
	if(pressedK2>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY1 pressed */
			switch(pressedK2){				
				case 2:
					if(move_mode == 0){
						rotate_wall();
					}
					break;
				default:
					break;
			}
			pressedK2++;
		}
		else {	/* button released */
			pressedK2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 1 pin selection */
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
