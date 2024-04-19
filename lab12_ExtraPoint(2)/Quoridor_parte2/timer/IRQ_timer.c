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
#ifndef GIOCO
#define GIOCO
#include "../Gioco/Gioco.h" 
#endif

#ifndef RIT_lib
#define RIT_lib
	#include "../RIT/RIT.h"

#endif	

extern int opposite; 
extern int game_mode; 
extern int ACK_Ricevuto; 

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

extern int StatoGioco; 
extern int FlagMuro; 

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
			CancellaEvidenziateTutte();  
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
		
		//Controllo se devo fare la mossa automatica
		if (game_mode==SINGLE && opposite==NPC && Who==2)	MossaNPC_wrapper(); 
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
	//Sono passati 3 secondi
	if(ACK_Ricevuto!=1 && game_mode==DUAL)
	{
			//Non c'è un'altra scheda
			LCD_Clear(White); 
			GUI_Text(30, 10, (uint8_t*)"Nessuna scheda collegata", Red, Sfondo);
	}
	else if (ACK_Ricevuto==1 && game_mode==DUAL)
	{
			VisualizzaMenu(game_mode+1); 				//Cambiamo Menu
			StatoGioco=PLAYER_SEL; 
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
