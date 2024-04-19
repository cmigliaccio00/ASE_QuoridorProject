#ifndef MENU
#define MENU 
#include "./Menu.h"
#endif

extern int Sfondo; 
int ColoreHighlight=Yellow; 

void VisualizzaMenu(QualeMenu quale){
	LCD_Clear(Sfondo); 
	
	switch(quale)
	{	
		case Iniziale:
			//Menu iniziale
			GUI_Text(90, 10, (uint8_t*)"Seleziona", Red, Sfondo);
			GUI_Text(50, 40, (uint8_t*)"MODALITA' DI GIOCO", Red, Sfondo);
			DisegnaCella(CreaPunto(55, 100), CreaPunto(185, 140)); 
			DisegnaCella(CreaPunto(55, 160), CreaPunto(185, 200));
			GUI_Text(70, 105, (uint8_t*)"SINGLE BOARD", Red, ColoreHighlight);
			GUI_Text(80, 165, (uint8_t*)"DUAL BOARD", Red, Grey);
			break; 
		case Singolo: 
			GUI_Text(80, 10, (uint8_t*)"SINGLE Board", Red, Sfondo);
			GUI_Text(45, 40, (uint8_t*)"SELEZIONA AVVERSARIO", Red, Sfondo);
			DisegnaCella(CreaPunto(55, 100), CreaPunto(185, 140)); 
			DisegnaCella(CreaPunto(55, 160), CreaPunto(185, 200));
			GUI_Text(100, 105, (uint8_t*)"HUMAN", Red, ColoreHighlight);
			GUI_Text(110, 165, (uint8_t*)"NPC", Red, Grey);
			break; 
		case Dual: 
			GUI_Text(85, 10, (uint8_t*)"DUAL board", Red, Sfondo);
			GUI_Text(60, 40, (uint8_t*)"SELEZIONA PLAYER", Red, Sfondo);
			DisegnaCella(CreaPunto(55, 100), CreaPunto(185, 140)); 
			DisegnaCella(CreaPunto(55, 160), CreaPunto(185, 200));
			GUI_Text(100, 105, (uint8_t*)"HUMAN", Red, ColoreHighlight);
			GUI_Text(110, 165, (uint8_t*)"NPC", Red, Grey);
			break; 
	}
	return; 
}


void CambiaEvidenziazione(int Cosa, int Quale)
{
	switch(Cosa){
		case MODE:
			switch(Quale)
			{
				case UP: 
					GUI_Text(70, 105, (uint8_t*)"SINGLE BOARD", Red, ColoreHighlight);
					GUI_Text(80, 165, (uint8_t*)"DUAL BOARD", Red, Sfondo);
					break; 
				case DOWN: 
					GUI_Text(70, 105, (uint8_t*)"SINGLE BOARD", Red, Sfondo);
					GUI_Text(80, 165, (uint8_t*)"DUAL BOARD", Red, ColoreHighlight);
					break; 
			}
			break; 
		case PLAYER_SEL: 
			switch(Quale)
			{
				case UP: 
					GUI_Text(100, 105, (uint8_t*)"HUMAN", Red, ColoreHighlight);
					GUI_Text(110, 165, (uint8_t*)"NPC", Red, Grey);
					break; 
				case DOWN: 
					GUI_Text(100, 105, (uint8_t*)"HUMAN", Red, Sfondo);
					GUI_Text(110, 165, (uint8_t*)"NPC", Red, ColoreHighlight);
					break; 
			}
			break; 
	}
	return; 
}


