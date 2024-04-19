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
#ifndef GIOCO
#define GIOCO
#include "../Gioco/Gioco.h"
#endif

#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#ifndef MENU
#define MENU 
#include "../Gioco/Menu.h"
#endif

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
extern int FlagUPRIGHT; extern int FlagUPLEFT; 
extern int FlagDOWNLEFT; extern int FlagDOWNRIGHT; 
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
static int J_up_left=0; 
static int J_up_right=0; 
static int J_down_left=0; 
static int J_down_right=0;

extern int FlagMuro;

extern int mode_tmp;  
extern int typeP_tmp; 
extern int game_mode; 
extern int mine; 
extern int opposite; 

extern int StatoGioco; 
extern int ACK_Trasmesso; 
int Iniziato=0; 

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
	//GIU+DESTRA--> DOWNRIGHT (diagonale)
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0)
	{
		//LPC_RIT->RICOUNTER = 0;
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
		J_down_right++; 
		switch(J_down_right){
			case 1: 
				if(StatoGioco==PLAY && Mod==MOVE && FlagDOWNRIGHT)	MuoviCella(DOWNRIGHT); 
				break; 
			default: break; 
		}
	}
	//GIU+SINISTRA --> DOWNLEFT
	else if ((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0)
	{
//		LPC_RIT->RICOUNTER = 0;
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
		J_down_left++;
		switch(J_down_left){
			case 1: 
				if(StatoGioco==PLAY && Mod==MOVE && FlagDOWNLEFT)	MuoviCella(DOWNLEFT); 
				break; 
			default: break; 
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
//		LPC_RIT->RICOUNTER = 0;
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
		J_down++;
		switch(J_down){
			case 1:
				//Devo ancora scegliere la modalità 
				switch(StatoGioco){
					case MODE:
						if(mode_tmp==SINGLE)
							mode_tmp=DUAL, CambiaEvidenziazione(MODE, DOWN); 
						else 
							mode_tmp=SINGLE, CambiaEvidenziazione(MODE, UP);
						break; 
					case PLAYER_SEL: 
						if(typeP_tmp==HUMAN)
							typeP_tmp=NPC, CambiaEvidenziazione(PLAYER_SEL, DOWN); 
						else 
							typeP_tmp=HUMAN, CambiaEvidenziazione(PLAYER_SEL, UP); 
						break; 
					case PLAY: 
						//Controllo in che MODALITA sono
						if(Mod==MOVE && FlagDOWN){		MuoviCella(DOWN); 	}
						else if(Mod==WALL){			if(CheckMuro(DOWN))		MuoviMuro(DOWN); }
						break; 
						default: break; 
				}	
				break;
			default:			break;
		}
//		if(StatoGioco==PLAY){
//			NVIC_EnableIRQ(TIMER0_IRQn),
//			enable_timer(0); 
//		}
	}
	else{
			J_down=0;
			J_down_right=0; 
			J_down_left=0;
	}
	
	//--------------Gestione UP--------------
	//SU + DESTRA --> UPRIGHT
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0  && (LPC_GPIO1->FIOPIN & (1<<28)) == 0)
	{
//		LPC_RIT->RICOUNTER = 0;
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
		J_up_right++; 
		switch(J_up_right){
			case 1: 
				if(StatoGioco==PLAY && Mod==MOVE && FlagUPRIGHT)	MuoviCella(UPRIGHT); 
				break; 
			default: break; 
		}
	}
	//SU+SINISTRA --> UPLEFT
	else if((LPC_GPIO1->FIOPIN & (1<<29)) == 0  && (LPC_GPIO1->FIOPIN & (1<<27)) == 0)
	{
//		LPC_RIT->RICOUNTER = 0;
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
		J_up_left++; 
		switch(J_up_left){
			case 1: 
				if(StatoGioco==PLAY && Mod==MOVE && FlagUPLEFT)	MuoviCella(UPLEFT); 
				break; 
			default: break; 
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
//		LPC_RIT->RICOUNTER = 0;
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
		
		J_up++;
		
		switch(J_up){
			case 1:
			switch(StatoGioco){
					case MODE:
						if(mode_tmp==SINGLE)
							mode_tmp=DUAL, CambiaEvidenziazione(MODE, DOWN); 
						else 
							mode_tmp=SINGLE, CambiaEvidenziazione(MODE, UP);
						break; 
					case PLAYER_SEL: 
						if(typeP_tmp==HUMAN)
							typeP_tmp=NPC, CambiaEvidenziazione(PLAYER_SEL, DOWN); 
						else 
							typeP_tmp=HUMAN, CambiaEvidenziazione(PLAYER_SEL, UP); 
						break; 
					case PLAY: 
						//Controllo in che MODALITA sono
						if(Mod==MOVE && FlagUP){		MuoviCella(UP);			}
						else if(Mod==WALL)	{			if(CheckMuro(UP))		MuoviMuro(UP); 	}
						break; 
						default: 
							break; 
				}	
				break;
			default:		break;
		}
//		if(StatoGioco==PLAY){
//			NVIC_EnableIRQ(TIMER0_IRQn),
//			enable_timer(0);
//		}
	}
	else{
			J_up=0;
			J_up_left=0;
			J_up_right=0; 
	}
	
	//--------------Gestione LEFT--------------
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		
//		NVIC_DisableIRQ(TIMER0_IRQn),
//		disable_timer(0); 
//		LPC_RIT->RICOUNTER = 0;
		J_left++;
		switch(J_left){
			case 1:
				if(Mod==MOVE && FlagLEFT){	MuoviCella(LEFT);	}
				else if(Mod==WALL) {	if(CheckMuro(LEFT))	MuoviMuro(LEFT); 	}
				break;
			default:	break;
		}
//		NVIC_EnableIRQ(TIMER0_IRQn),
//		enable_timer(0);
	}
	else{
			J_left=0;
	}
	
		//--------------Gestione RIGHT--------------
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
//		NVIC_DisableIRQ(TIMER0_IRQn),
//		disable_timer(0); 
//		LPC_RIT->RICOUNTER = 0;
		J_right++;
		switch(J_right){
			case 1:
				if(Mod==MOVE && FlagRIGHT){	MuoviCella(RIGHT);	}
				else if(Mod==WALL){		if(CheckMuro(RIGHT))	MuoviMuro(RIGHT); 	}
				break;
			default: break;
		}
//		NVIC_EnableIRQ(TIMER0_IRQn),
//		enable_timer(0);
	}
	else{
			J_right=0;
	}
	
	//--------------Gestione SELECT (CONFERMA + CAMBIO TURNO)--------------
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
//		if(StatoGioco==PLAY){
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
//		}
//		LPC_RIT->RICOUNTER = 0;
		J_select++;
		switch(J_select){
			case 1:
				switch (StatoGioco){
					//Selezione Modalita' di gioco
					case MODE: 
						game_mode=mode_tmp; 
						if(game_mode==DUAL){
							//Mando mossa con 0xFF (Preparazione del messaggio CAN)
							LCD_Clear(White); 
							GUI_Text(30, 10, (uint8_t*)" Attendo conferma scheda", Red, Sfondo);
							TrasmettiCAN_Msg(0xFF000000);
							ACK_Trasmesso=1; 
							enable_timer(1); 								//Faccio partire il timer per aspettarmi un messaggio 0xFF
						}
						if(game_mode==SINGLE){
							VisualizzaMenu((STATO)(game_mode+1)); 				//Cambiamo Menu
							StatoGioco=PLAYER_SEL; 
						}
						break; 

					//Selezione del tipo di giocatore
					case PLAYER_SEL:
						if(game_mode==SINGLE)		opposite=typeP_tmp; 
						else {mine=typeP_tmp;} 
						
							//Ho scelto il tipo di giocatore
							LCD_Clear(Sfondo); 
							DisegnaScacchiera(); 
							MostraInfo(); 
							QUORIDOR_Init();
						
//							LPC_RIT->RICTRL = 0x06; 
//							LPC_RIT->RICOUNTER = 0;
							StatoGioco=PLAY; 
							Mod=MOVE;
							Who=1;
							MostraInfo2();		//Numeri rimasti e timer
							Check(); 
							FlagMuro=0;
						
															/* RIT Initialization 50 msec       	*/
							enable_RIT();
							
							#if TIMING
							//enable timer per contare
								enable_timer(0); 
							#endif
						
						//In modalità DUAL??
						break; 
						
					//Gioco Partito
					case PLAY: 
						Iniziato=1; 
						//Devo fare cose diverse in base a quale modalità sono
						if(Mod==MOVE)
						{
							//Selezione del giocatore
							whoPt = (Who==1)	? (&P1) : (&P2);
							RegistraMossa(whoPt->i, whoPt->j, MOVE, (ORIENTA)0);									//Comunicare Mossa CAN
							//--------------------CONTROLLO VITTORIA-----------------------
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
									Mod=MOVE; 
							}
						break; 
				}
				
				break;
				default:	
					break;
		}
		
		//Cambio del giocatore (questo rimane)
		if(StatoGioco==PLAY && Iniziato){
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
//				NVIC_EnableIRQ(TIMER0_IRQn);
//				enable_timer(0);
				Timer=20; AggiornaTimer();

				//Mossa AUTOMATICA
				if (game_mode==SINGLE && opposite==NPC && Who==2)		MossaNPC_wrapper();
			}
		}
	}
	else{
			J_select=0;
	}
	
	/*--------------------------- BOTTONI----------------------------------- */
	
	//-------------KEY 1 (CAMBIO MODALITA' DI GIOCO)
	if(down_1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
//			NVIC_DisableIRQ(TIMER0_IRQn),
//		disable_timer(0); 
//			
//			LPC_RIT->RICOUNTER = 0;
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
						CancellaEvidenziateTutte(); 
						
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
//			NVIC_EnableIRQ(TIMER0_IRQn),
//			enable_timer(0);
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
//			LPC_RIT->RICOUNTER = 0;
//			NVIC_DisableIRQ(TIMER0_IRQn),
//			disable_timer(0); 
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
//		LPC_RIT->RICOUNTER = 0;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* KEY1 pressed */
			down_INT++;				
			switch(down_INT){
				case 2:				/* pay attention here: please see slides to understand value 2 */
					
					//Qui devo visualizzare il Menu
					VisualizzaMenu(Iniziale); 
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
