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
#ifndef GIOCO
#define GIOCO
#include "../Gioco/Gioco.h"
#endif
extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

extern int ACK_Trasmesso;
extern int ACK_Ricevuto; 
extern int Timer; 
extern _typeMURO MuroTmp;
extern _typePLAYER P1, P2; 
extern int mine; 

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {
	int Mossa; int Pl, VH, Y, X; 
  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */

  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */
		
		//Costruzione della mossa
		Mossa = CAN_RxMsg.data[0] << 24;
		Mossa = Mossa | (CAN_RxMsg.data[1] << 16); 
		Mossa = Mossa | (CAN_RxMsg.data[2] << 8); 
		Mossa = Mossa | (CAN_RxMsg.data[3]); 
		
		//Messaggio di controllo
		if(	(Mossa & 0xFF000000) == 0xFF000000	)
			ACK_Ricevuto=1; 
		//Mossa invalida
		else if ((Mossa & 0x00010000) == 0x00010000)
		{
				//Tocca a me
				Timer=20; AggiornaTimer();
		}
		else {
			//Unpack mossa + visualizzazione
			Mossa = (int)CAN_RxMsg.data[0]; 
			Pl = (int)(CAN_RxMsg.data[1]&0xF0)>>4; 
			VH = (int)(CAN_RxMsg.data[1]&0x0F);
			Y=(int)(CAN_RxMsg.data[2]); 
			X=(int)(CAN_RxMsg.data[3]); 
			
			switch(Mossa){
				case MOVE:
					ColoraCella(Y,X, (Pl==1) ? P1.Colore : P2.Colore);
					break; 
				case WALL:
					MuroTmp.Quale=(ORIENTA)VH; 
					MuroTmp.i = Y; 
					MuroTmp.j = X; 
					ConfermaMuroSenzaRegistra(); 
					if(Pl==1)	{
						P1.NumMuri--; 
						AggiornaPunteggio(MURI);
					}else {
							P2.NumMuri--; 
							AggiornaPunteggio(MURI);
					}
					break;
				default:
					break; 
			}
			//Poi tocca a me
			Timer=20; AggiornaTimer();
			if(mine==NPC)
				MossaNPC_wrapper(); 
		}
  }
	else
		if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
			// do nothing in this example
			ACK_Trasmesso=1; 
		}
}
