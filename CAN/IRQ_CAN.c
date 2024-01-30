/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <lpc17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include "string.h"
#include "../game/game.h"

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */   

volatile int current_player_id = -1;

/*
static int puntiRicevuti1 = 0;
static int puntiInviati1 = 0;

static int puntiRicevuti2 = 0;
static int puntiInviati2 = 0;

uint16_t val_RxCoordX = 0;         
uint16_t val_RxCoordY = 0;
*/

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {

	uint32_t move;
	uint32_t move_to_sent;
  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */
		
		move = CAN_RxMsg.data[0]; // x
		move |= CAN_RxMsg.data[1] << 8; // y
		move |= CAN_RxMsg.data[2] << 16; // vertical/horizontal move/wall
		move |= CAN_RxMsg.data[3] << 24; // id
		// handshake
		if(move == 0xFF000000){
			move_to_sent = 0xFE000000;
			memcpy(&CAN_TxMsg.data,&move_to_sent,4);
			CAN_TxMsg.id = 1;
			CAN_TxMsg.len = 4;
			CAN_TxMsg.format = STANDARD_FORMAT;
			CAN_TxMsg.type = DATA_FRAME;
			CAN_wrMsg(1, &CAN_TxMsg);
			current_player_id = 2;
			game_setup();
			return;
		}
		// acknowledgement
		if(move == 0xFE000000){
			current_player_id = 1;
			game_setup();
			PlayNPCAction();
			return;
		}
		DrawPlayerMove(move);
		
  }
	if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */

	}
		/*
	//check CAN controller 2 
	icr = 0;
	icr = (LPC_CAN2->ICR | icr) & 0xFF;             // clear interrupts 

	if (icr & (1 << 0)) {                          	// CAN Controller #2 meassage is received 
		CAN_rdMsg (2, &CAN_RxMsg);	                		// Read the message
    LPC_CAN2->CMR = (1 << 2);                    		// Release receive buffer 
		move = CAN_RxMsg.data[0]; // x
		move |= CAN_RxMsg.data[1]; // y
		move |= CAN_RxMsg.data[2]; // vertical/horizontal move/wall
		move |= CAN_RxMsg.data[3]; // id
		
		DrawPlayerMove(move);
		switch_turn();
		
	}
	if (icr & (1 << 1)) {                         // CAN Controller #2 meassage is transmitted 

	}
	*/
}
