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
#include "../Gioco/Gioco.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down_1=0;
volatile int down_2=0; 
volatile int down_INT=0;

//Variabili di stato dei 5 pulsanti
extern int Timer; 
extern MODALITA Mod; 
extern int FlagUP; 
extern int FlagDOWN; 
extern int FlagRIGHT; 
extern int FlagLEFT; 
extern int Who; 
extern int Vittoria; 
extern _typePLAYER P1, P2;
extern _typeMURO MuroTmp; 
extern MODALITA Mod;
extern int Col_Muro;
extern int Sfondo; 
extern int FACE_UP, FACE_DOWN; 

static int J_select=0;
static int J_down = 0;
static int J_up = 0; 
static int J_left=0; 
static int J_right=0; 

extern int FlagMuro;

_typePLAYER* whoPt; 
	
/*=====================================================
Questa ISR viene usata per controllare (ogni 50 ms): 
	+ Input dei bottoni INT0, KEY1, KEY2 
		(gestisco in questo modo anche il debouncing)
	+ Gli input da parte del JOYSTICK
	  Nel momento in cui mi arriva un input dal joystick:
			* Disabilito il TIMER0 con l'interrupt associato
			* Svolgo le azioni necessarie
			* Abilito il TIMER0 con l'interrupt associato
-----------------------------------------------------*/
void RIT_IRQHandler (void)
{	
	_typePLAYER *whoPt;
	
	//--------------Gestione DOWN--------------
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		LPC_RIT->RICOUNTER = 0;
		
		NVIC_DisableIRQ(TIMER0_IRQn),
		disable_timer(0); 
		
		J_down++;
		switch(J_down){
			case 1:
				//Controllo in che MODALITA sono
				if(Mod==MOVE && FlagDOWN){		MuoviCella(DOWN); 	}
				else if(Mod==WALL){				if(CheckMuro(DOWN))		MuoviMuro(DOWN); }
				break;
			default:			break;
		}
		NVIC_EnableIRQ(TIMER0_IRQn),
		enable_timer(0); 
	}
	else{
			J_down=0;
	}
	
	//--------------Gestione UP--------------
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		NVIC_DisableIRQ(TIMER0_IRQn),
		disable_timer(0); 
		LPC_RIT->RICOUNTER = 0;
		J_up++;
		switch(J_up){
			case 1:
				if(Mod==MOVE && FlagUP){		MuoviCella(UP);			}
				else if(Mod==WALL)	{			if(CheckMuro(UP))		MuoviMuro(UP); 	}
				break;
			default:		break;
		}
		NVIC_EnableIRQ(TIMER0_IRQn),
		enable_timer(0);
	}
	else{
			J_up=0;
	}
	
	//--------------Gestione LEFT--------------
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		
		NVIC_DisableIRQ(TIMER0_IRQn),
		disable_timer(0); 
		LPC_RIT->RICOUNTER = 0;
		J_left++;
		switch(J_left){
			case 1:
				if(Mod==MOVE && FlagLEFT){	MuoviCella(LEFT);	}
				else if(Mod==WALL) {	if(CheckMuro(LEFT))	MuoviMuro(LEFT); 	}
				break;
			default:	break;
		}
		NVIC_EnableIRQ(TIMER0_IRQn),
		enable_timer(0);
	}
	else{
			J_left=0;
	}
	
		//--------------Gestione RIGHT--------------
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		NVIC_DisableIRQ(TIMER0_IRQn),
		disable_timer(0); 
		LPC_RIT->RICOUNTER = 0;
		J_right++;
		switch(J_right){
			case 1:
				if(Mod==MOVE && FlagRIGHT){	MuoviCella(RIGHT);	}
				else if(Mod==WALL){		if(CheckMuro(RIGHT))	MuoviMuro(RIGHT); 	}
				break;
			default: break;
		}
		NVIC_EnableIRQ(TIMER0_IRQn),
		enable_timer(0);
	}
	else{
			J_right=0;
	}
	
	//--------------Gestione SELECT (CONFERMA + CAMBIO TURNO)--------------
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		NVIC_DisableIRQ(TIMER0_IRQn),
		disable_timer(0); 
		LPC_RIT->RICOUNTER = 0;
		J_select++;
		switch(J_select){
			case 1:
				if(Mod==MOVE)
				{
					//Selezione del giocatore
					whoPt = (Who==1)	? (&P1) : (&P2);
					RegistraMossa(whoPt->i, whoPt->j, MOVE, 0);
					
					//--------------------VITTORIA-----------------------
						if(Who==1 && whoPt->i==6)	{
							Vittoria=1; 
							CancellaWarning(); 
							MostraWarning("Vittoria a P1,FINE GIOCO   ");
							NVIC_DisableIRQ(TIMER0_IRQn),
							disable_timer(0);
							reset_RIT(); 
							disable_RIT(); 
						}						
						else if(Who==2 && whoPt->i==0)
						{	
							Vittoria=1; 
							CancellaWarning(); 
							MostraWarning("Vittoria a P2,FINE GIOCO   ");
							NVIC_DisableIRQ(TIMER0_IRQn),
							disable_timer(0); 
							reset_RIT(); 
							disable_RIT(); 
						}
					}
					else if (Mod==WALL)
					{
							whoPt = (Who==1)	? (&P1) : (&P2);
							ConfermaMuro(); 
							whoPt->NumMuri--; 
							AggiornaPunteggio(MURI); 
							Mod=MOVE; //Torna in modalità MOVE
					}
				break;
				default:	break;
		}
		
		//Cambio del giocatore (questo rimane)
		if(!Vittoria){
			whoPt = (Who==1) ? &P1 : &P2; 
			if(whoPt->NumMuri==0)
				CancellaWarning(); 
			Who = (Who==1) ? 2: 1;
			whoPt = (Who==1) ? &P1 : &P2; 
			if(whoPt->NumMuri==0)
				MostraWarning("0 muri rimasti, MUOVI TOKEN "); 
			FlagMuro=0;
			AggiornaPunteggio(MURI);
			Check();
			
			//do  il tempo al prossimo giocatore
			NVIC_EnableIRQ(TIMER0_IRQn);
			enable_timer(0);
			Timer=20; AggiornaTimer();
		}
	}
	else{
			J_select=0;
	}
	
	/*--------------------------- BOTTONI----------------------------------- */
	
	//-------------KEY 1 (CAMBIO MODALITA' DI GIOCO)
	if(down_1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			NVIC_DisableIRQ(TIMER0_IRQn),
		disable_timer(0); 
			
			LPC_RIT->RICOUNTER = 0;
			down_1++;				
			switch(down_1){
				case 2:				/* pay attention here: please see slides to understand value 2 */
				
				//Se sono in modalità MOVE devo passare in modalità MURO
				whoPt = (Who==1)	? (&P1) : (&P2);
				if(whoPt->NumMuri>0)
				{
					if(Mod==MOVE)
					{
						//Cancella le tessere evidenziate
						whoPt = (Who==1)	? (&P1) : (&P2);
						if(FACE_UP || FACE_DOWN) CancellaEvidenziateParticolareTUTTE((Who==1) ? &P1: &P2); 
						else CancellaCelleEvidenziate((Who==1) ? &P1: &P2); 
						//Cambio Modalita'
						Mod=WALL; 
						MuroTmp.Who=Who; 
						MuroTmp.Quale=ORIZZONTALE; 
						//Scelta del muro
						MuroTmp.i=2; MuroTmp.j=3; 
						DisegnaMuro(MuroTmp.i,MuroTmp.j, MuroTmp.Quale, Col_Muro);
						}
						//Viceversa devo tornare in modalità MOVE
						else if(Mod==WALL)
						{	whoPt = (Who==1)	? (&P1) : (&P2);
							//Ritorno alla modalità MOVE Token
							Mod=MOVE; 
							//Cancella Muro disegnato e non confermato!
							DisegnaMuro(MuroTmp.i, MuroTmp.j, MuroTmp.Quale, Sfondo);
							//Metti la cella dove stava prima
							ColoraCella(whoPt->i, whoPt->j, whoPt->Colore); 
							//Ridisegna le celle con Check()
							Check(); 
						}
				}
					break;
				default:
					break;
			}
			
			NVIC_EnableIRQ(TIMER0_IRQn),
		enable_timer(0);
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	//--------------KEY 2 (ROTAZIONE MURO)
	if(down_2!=0){ 
		
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY1 pressed */
			LPC_RIT->RICOUNTER = 0;
			NVIC_DisableIRQ(TIMER0_IRQn),
			disable_timer(0); 
			down_2++;				
			switch(down_2){
				case 2:				/* pay attention here: please see slides to understand value 2 */
					if(Mod==WALL)		RuotaMuro(); 
					break;
				default:
					break;
			}
			
			NVIC_EnableIRQ(TIMER0_IRQn),
		enable_timer(0);
		}
		else {	/* button released */
			down_2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	
	//--------------INT0 (ABILITAZIONE GIOCO)
	if(down_INT!=0){ 
		LPC_RIT->RICOUNTER = 0;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* KEY1 pressed */
			down_INT++;				
			switch(down_INT){
				case 2:				/* pay attention here: please see slides to understand value 2 */
					//Impostazione iniziale (in INT0)
					LPC_RIT->RICTRL = 0x06; 
					LPC_RIT->RICOUNTER = 0;
					Mod=MOVE;
					Who=1;
					MostraInfo2();		//Numeri rimasti e timer
					Check(); 
					FlagMuro=0;
				
					init_RIT(0x004C4B40/10000);									/* RIT Initialization 50 msec       	*/
					enable_RIT();
					
					#if TIMING
					//enable timer per contare
						enable_timer(0); 
					#endif
				
					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_INT=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	LPC_RIT->RICOUNTER = 0;
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	          // Set count value to 0
		
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
