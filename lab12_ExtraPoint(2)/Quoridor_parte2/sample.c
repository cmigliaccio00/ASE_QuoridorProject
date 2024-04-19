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
#include "button_EXINT/button.h"
#define RIT_const 0x004C4B40*3
#ifndef GIOCO
#define GIOCO
#include "Gioco/Gioco.h"
#endif
#ifndef CAN_lib
#define CAN_lib 
	#include "CAN/CAN.h"
#endif	
#ifndef RIT_lib
#define RIT_lib
	#include "RIT/RIT.h"

#endif	
#include "joystick/joystick.h"
#ifndef MENU
#define MENU 
#include "Gioco/Menu.h"
#endif
#include <stdlib.h>

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

extern int Sfondo; 
extern int game_mode;

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
  LCD_Initialization();
	joystick_init();
	CAN_Init();	
	BUTTON_init(); 
	init_RIT(RIT_const);									/* RIT Initialization 50 msec       	*/
	enable_RIT();
	
	//Messaggio iniziale
	LCD_Clear(Red); 
	GUI_Text(30, 120, (uint8_t*) "Premi INT0 per iniziare", Red, White); 
	
	
	//Timer che controlla la connessione CAN diviso 25
	init_timer(1, (0x017D7840)*3); 
	
	#if TIMING 
		init_timer(0, 0x017D7840);									/* Inizializzo il timer 25*/ 	
	#endif
	
	//Configurazione modalità POWER DOWN 
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
