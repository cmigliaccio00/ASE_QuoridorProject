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
#include "../Gioco/Gioco.h" 

extern int Who; 		
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern int Timer; 
extern int FACE_UP, FACE_DOWN; 
extern _typePLAYER P1, P2; 
extern int Mod; 
extern _typeMURO MuroTmp;
extern int Sfondo; 

void TIMER0_IRQHandler (void)
{
	//Aggiorno il contatore
	Timer--; 
	AggiornaTimer(); 
	
	//Tempo scaduto
	if(Timer==0){
		Timer=20;
		AggiornaTimer();
		//Undo Mossa non confermata
		if(Mod==MOVE){
			if(FACE_UP || FACE_DOWN) CancellaEvidenziateParticolareTUTTE((Who==1) ? &P1: &P2); 
			else CancellaCelleEvidenziate((Who==1) ? &P1: &P2); 
		}
		else if (Mod==WALL){
			DisegnaMuro(MuroTmp.i, MuroTmp.j, MuroTmp.Quale, Sfondo);
			Mod=MOVE; 
		}
		//Segna mossa invalida
		RegistraMossa(0, 0, 0, 1); 
		//Cambia player
		Who = (Who==1) ? 2: 1;
		Check(); 
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
