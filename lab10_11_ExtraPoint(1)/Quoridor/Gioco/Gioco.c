#include "Gioco.h"
#include <string.h> 
#include <stdio.h>

//PARAMETRO DISEGNO ELEMENTI GRAFICI
static int lineColor=Black;
static int dimCella=29; 
static int margine=3; 
static int x_ini=5; 
static int y_ini=5; 
static int DimScacchiera=7;
static int H_Info=50; 
static int W_Info=72;
static int Col_Ev = Yellow;
int Col_Muro = Blue2;
static int ColDef_Muro = Brown; 
static int dimMuro; 
int Sfondo=Grey; 
int Scritte=Red;
//-----------------------------------
_typeCELLA SCACCHIERA[7][7];  
_typePLAYER P1, P2;
int Who;						MODALITA Mod;
_typeMURO MuroTmp; 	int FlagMuro;
int Timer=20;
int Mosse[100]; 		int CntMosse=0;			 
int FlagUP; 				int FlagDOWN; 
int FlagRIGHT; 			int FlagLEFT; 
int Vittoria=0; 
int FACE_UP=0;
int FACE_DOWN=0; 
	

/*=============================================

---------------------------------------------*/
void QUORIDOR_Init(void)
{
	int i, j; 
	P1.i=0; P1.j=3; 
	P1.NumMuri = 8; 
	P1.Colore = DarkOrange; 
	
	P2.i = 6; P2.j=3;  
	P2.NumMuri = 8; 
	P2.Colore=DeepSkyBlue; 
	
	//Posiziono i giocatori nella loro posizione iniziale
	ColoraCella(0,3, P1.Colore); 
	ColoraCella(6,3, P2.Colore); 
}

/*========================================================
Crea un _typePUNTO date le sue coordinate
--------------------------------------------------------*/
_typePUNTO CreaPunto (int x, int y)
{
	_typePUNTO Punto; 
	
	Punto.x = x; Punto.y = y; 
	return Punto; 
}

/*=======================================================
Disegna una cella sull'LCD dati i vertici (_typePUNTO)
-------------------------------------------------------*/
void DisegnaCella(_typePUNTO A, _typePUNTO B)
{
	LCD_DrawLine(A.x, A.y, B.x, A.y, lineColor); 
	LCD_DrawLine(A.x, B.y, B.x, B.y, lineColor); 
	LCD_DrawLine(A.x, A.y, A.x, B.y, lineColor); 
	LCD_DrawLine(B.x, A.y, B.x, B.y, lineColor); 
	return;
}

/*===================================================
Disegna una scacchiera di celle 7x7 (parametrizzato)
Creando la cella popolo anche la struttura dati
----------------------------------------------------*/
void DisegnaScacchiera(void)
{
	int i, j; 
	_typePUNTO _tmpA, _tmpB;		//vertici della cella
	
	//Disegno della scacchiera
	_tmpA=CreaPunto(x_ini, y_ini); 
	_tmpB=CreaPunto(x_ini+dimCella, y_ini+dimCella); 
	SCACCHIERA[0][0] = CreaCella(_tmpA, _tmpB,0,0,0,0); 
	for (i=0; i<DimScacchiera; i++)
	{	
		for (j=0; j<DimScacchiera; j++)
		{
			DisegnaCella(_tmpA, _tmpB);
			//Calcolo dei prossimi vertici
			if(j<6)
			{	_tmpA = CreaPunto(_tmpA.x+dimCella+margine+1, _tmpA.y); 
				_tmpB = CreaPunto(_tmpB.x+dimCella+margine+1, _tmpB.y);
				SCACCHIERA[i][j+1] = CreaCella(_tmpA, _tmpB,0,0,0,0); 
			}
		}
		//Preparo la prossima riga
		if(i<6)
		{	_tmpA = CreaPunto(x_ini, _tmpB.y+margine+1);
			_tmpB = CreaPunto(x_ini+dimCella, _tmpB.y+dimCella+margine+1); 
			SCACCHIERA[i+1][0] = CreaCella(_tmpA, _tmpB,0,0,0,0); 
		}
	}
	return;
}

void MostraInfo(void)
{
	_typePUNTO _tmpA, _tmpB; 
	
	//Contatore Muri P1
	_tmpA=CreaPunto(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+40); 
	_tmpB = CreaPunto(x_ini+W_Info,_tmpA.y+H_Info); 
	DisegnaCella(_tmpA, _tmpB);
	GUI_Text(_tmpA.x+10,_tmpA.y+3, (uint8_t *) "MURI P1", Scritte, Sfondo);
	
	
	//Timer
	_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
	_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y); 
	DisegnaCella(_tmpA, _tmpB); 
	GUI_Text(_tmpA.x+10,_tmpA.y+3, (uint8_t *) "MANCANO", Scritte, Sfondo);
	
	//Contatore Muri P2
	_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
	_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y);
	DisegnaCella(_tmpA, _tmpB); 
	GUI_Text(_tmpA.x+10,_tmpA.y+3, (uint8_t *) "MURI P2", Scritte, Sfondo);
}

/*=====================================================
Aggiorna il numero di muri disponibili per il player 1, 
per il player 2
----------------------------------------------------*/
void AggiornaPunteggio(VAL Cosa)
{
	_typePLAYER *whoPt;
	char Stringa[100]; 
	_typePUNTO _tmpA, _tmpB; 
	//Selezione del giocatore
	whoPt = (Who==1)	? (&P1) : (&P2); 
	
	switch(Cosa)
	{
		case MURI:
			if(Who==1)		//Aggiorna punteggio player 1
			{
				_tmpA=CreaPunto(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+40); 
				_tmpB = CreaPunto(x_ini+W_Info,_tmpA.y+H_Info); 
				sprintf(Stringa, "   %d   ", whoPt->NumMuri);               
				GUI_Text(_tmpA.x+10,_tmpA.y+25, (uint8_t *)Stringa, Black, Sfondo);
			}		
			else				//Aggiorna punteggio player 2 
			{
				_tmpA=CreaPunto(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+40); 
				_tmpB = CreaPunto(x_ini+W_Info,_tmpA.y+H_Info); 
				
				_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
				_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y);
				
				_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
				_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y);
				sprintf(Stringa, "   %d   ", whoPt->NumMuri); 
				GUI_Text(_tmpA.x+10,_tmpA.y+25, (uint8_t *) Stringa, Black, Sfondo);
			}
			break; 
		case TEMPO: 
			
			//TO DO
			break; 
		default: break; 
	}
	return;
}

void CancellaWarning(void)
{
	GUI_Text(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+15, 
				(uint8_t *) "                            ", Sfondo, Sfondo);
	return; 
}

//Viene mostrato nello spazio disponibile un messaggio Msg
//Usato per: muri esauriti, vittoria 
void MostraWarning(char* Msg)
{
	GUI_Text(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+15, 
				(uint8_t *)Msg, Red, LemonChiffon);
	return; 
}

/*================================================================
Vengono mostrate negli appositi riquadri: 
-Numero di Muri del Player 1
-Numero di Muri del Player 2
-Secondi rimanenti
----------------------------------------------------------------*/
void MostraInfo2 (void)
{
	_typePUNTO _tmpA, _tmpB; 
	char Stringa[100]; 
	
	//Contatore Muri P1
	_tmpA=CreaPunto(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+40); 
	_tmpB = CreaPunto(x_ini+W_Info,_tmpA.y+H_Info); 
	sprintf(Stringa, "   %d   ", P1.NumMuri);               
	GUI_Text(_tmpA.x+10,_tmpA.y+25, (uint8_t *)Stringa, Black, Sfondo);
	
	//Timer
	_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
	_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y); 
	sprintf(Stringa, " %d s  ", Timer); 
	GUI_Text(_tmpA.x+10,_tmpA.y+25, (uint8_t *)Stringa, Black, Sfondo);
	
	//Contatore Muri P2
	_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
	_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y);
	sprintf(Stringa, "   %d   ", P2.NumMuri); 
	GUI_Text(_tmpA.x+10,_tmpA.y+25, (uint8_t *)Stringa, Black, Sfondo);
}

void AggiornaTimer(void)
{
	_typePUNTO _tmpA, _tmpB; 
	char Stringa[100]; 
	
	_tmpA=CreaPunto(x_ini, y_ini+DimScacchiera*dimCella+(DimScacchiera-1)*margine+40); 
	_tmpB = CreaPunto(x_ini+W_Info,_tmpA.y+H_Info); 
	
	_tmpA = CreaPunto(_tmpB.x+2*margine, _tmpA.y);
	_tmpB = CreaPunto(_tmpB.x+2*margine+W_Info, _tmpB.y); 
	if(Timer<10)
		sprintf(Stringa, " 0%d s  ", Timer); 
	else
		sprintf(Stringa, " %d s  ", Timer); 
	GUI_Text(_tmpA.x+10,_tmpA.y+25, (uint8_t *)Stringa, Black, Sfondo);
}

/* Crea una CELLA dati i due vertici */
_typeCELLA CreaCella(_typePUNTO A, _typePUNTO B, int U, int D, int L, int R)
{
	_typeCELLA C; 
	C.UP_Block=U; 
	C.DOWN_Block=D; 
	C.LEFT_Block=L; 
	C.RIGHT_Block=R; 
	C.A = A; 		C.B = B; 
	return  C; 
}

/*======================================================
Dato il riferimento alla scacchiera, colora una 
certa cella.
------------------------------------------------------*/

void ColoraCella(int i, int j, int Colore)
{
		int ind; 
		_typePUNTO _tmpA = SCACCHIERA[i][j].A; 
		_typePUNTO _tmpB = SCACCHIERA[i][j].B; 
		
		//Coloro solo all'interno del bordino
		for (ind=1; ind<dimCella; ind++)
			LCD_DrawLine(_tmpA.x+1,_tmpA.y+ind, 
									_tmpA.x+dimCella-1, _tmpA.y+ind, 
									Colore); 
}

void CancellaCelleEvidenziate(_typePLAYER *whoPt)
{
	if(FlagUP) 		ColoraCella((whoPt->i)-1, whoPt->j, Sfondo);
	if(FlagDOWN)	ColoraCella((whoPt->i)+1, whoPt->j, Sfondo); 
	if(FlagLEFT)	ColoraCella(whoPt->i, (whoPt->j)-1, Sfondo);
	if(FlagRIGHT)	ColoraCella(whoPt->i, (whoPt->j)+1, Sfondo);
}

/*================================================================
Cancella le celle evidenziate nel caso in cui ci sia la mossa 
possibile FACE TO FACE
----------------------------------------------------------------*/
void CancellaEvidenziateParticolare(_typePLAYER* whoPt, DIR Direzione)
{
	//---------------face to face + movimento SOTTO
	if(Direzione==DOWN){	
			if(FACE_UP){
				if(FlagUP)		ColoraCella((whoPt->i)-2, whoPt->j, Sfondo);
			}
			if(FACE_DOWN){
				if(FlagUP)		ColoraCella((whoPt->i)-1, whoPt->j, Sfondo);
			}
			if(FlagLEFT) 	ColoraCella(whoPt->i, (whoPt->j)-1, Sfondo); 
			if(FlagRIGHT)	ColoraCella(whoPt->i, (whoPt->j)+1, Sfondo);
		}
	
		
		//--------------------face to face + movimento SOPRA
		else if(Direzione==UP)
		{
			if(FACE_UP){
				if(FlagDOWN)		ColoraCella((whoPt->i)+1, whoPt->j, Sfondo);
			}
			if(FACE_DOWN){
				if(FlagDOWN)		ColoraCella((whoPt->i)+2, whoPt->j, Sfondo);
			}
			if(FlagLEFT)		ColoraCella(whoPt->i, (whoPt->j)-1, Sfondo); 
			if(FlagRIGHT)		ColoraCella(whoPt->i, (whoPt->j)+1, Sfondo);
		}
		
		
		//---------------------face to face + movimento SINISTRA
		else if(Direzione==LEFT)
		{
			if(FACE_UP){
				if(FlagUP)		ColoraCella((whoPt->i)-2, whoPt->j, Sfondo);
				if(FlagDOWN)	ColoraCella(whoPt->i+1, (whoPt->j), Sfondo); 
			}
			else if(FACE_DOWN){
				if(FlagUP)		ColoraCella((whoPt->i)-1, whoPt->j, Sfondo);
				if(FlagDOWN)	ColoraCella(whoPt->i+2, (whoPt->j), Sfondo); 
			}
			if(FlagRIGHT)		ColoraCella(whoPt->i, (whoPt->j)+1, Sfondo);
		}
		
		
		//-------------------face to face + movimento DESTRA 
		else if(Direzione==RIGHT)
		{
			if(FACE_UP){
				if(FlagUP) 		ColoraCella((whoPt->i)-2, whoPt->j, Sfondo);		//cella 'particolare'
				if(FlagDOWN)	ColoraCella(whoPt->i+1, (whoPt->j), Sfondo); 
			}
			else if(FACE_DOWN){
				if(FlagUP)		ColoraCella((whoPt->i)-1, whoPt->j, Sfondo);
				if(FlagDOWN)	ColoraCella(whoPt->i+2, (whoPt->j), Sfondo); 		//cella 'particolare'
			}
			if(FlagLEFT)		ColoraCella(whoPt->i, (whoPt->j)-1, Sfondo); 
		}
}

void CancellaEvidenziateParticolareTUTTE (_typePLAYER* whoPt)
{
	if(FlagRIGHT){
				ColoraCella(whoPt->i, (whoPt->j)+1, Sfondo);
				DisegnaCella(SCACCHIERA[whoPt->i][(whoPt->j)+1].A, SCACCHIERA[whoPt->i][(whoPt->j)+1].B);
			}
	if(FlagLEFT){ 
				ColoraCella(whoPt->i, (whoPt->j)-1, Sfondo); 
				DisegnaCella(SCACCHIERA[whoPt->i][(whoPt->j)-1].A, SCACCHIERA[whoPt->i][(whoPt->j)-1].B);
			}
	
	if(FACE_UP)
			{
				if(FlagUP){ 
				ColoraCella((whoPt->i)-2, whoPt->j, Sfondo);
				DisegnaCella(SCACCHIERA[(whoPt->i)-2][whoPt->j].A, SCACCHIERA[(whoPt->i)-1][whoPt->j].B);
				}
				if(FlagDOWN){
					ColoraCella(whoPt->i, (whoPt->j)+1, Sfondo); 
					DisegnaCella(SCACCHIERA[whoPt->i][(whoPt->j)-1].A, SCACCHIERA[whoPt->i][(whoPt->j)-1].B);
				}
			}
			else if(FACE_DOWN)
			{
				if(FlagUP){ 
				ColoraCella((whoPt->i)-1, whoPt->j, Sfondo);
				DisegnaCella(SCACCHIERA[(whoPt->i)-1][whoPt->j].A, SCACCHIERA[(whoPt->i)-1][whoPt->j].B);
				}
				if(FlagDOWN){
					ColoraCella(whoPt->i, (whoPt->j)+2, Sfondo); 
					DisegnaCella(SCACCHIERA[whoPt->i][(whoPt->j)+2].A, SCACCHIERA[whoPt->i][(whoPt->j)-1].B);
				}
			}
			if(FlagLEFT){ 
				ColoraCella(whoPt->i, (whoPt->j)-1, Sfondo); 
				DisegnaCella(SCACCHIERA[whoPt->i][(whoPt->j)-1].A, SCACCHIERA[whoPt->i][(whoPt->j)-1].B);
			}
	return; 
}	

/*==============================================
Subroutine per spostare il token
----------------------------------------------*/
void MuoviCella(DIR Direzione)
{	_typePLAYER *whoPt;
	//Selezione del giocatore
	whoPt = (Who==1)	? (&P1) : (&P2); 
	ColoraCella (whoPt->i, whoPt->j, Sfondo);							//1. Cancello la cella 
	
	if(!FACE_UP && !FACE_DOWN)
		CancellaCelleEvidenziate(whoPt); 		//Cancellazione 'facile'
	else{																	//Sono nel caso particolare
		CancellaEvidenziateParticolare(whoPt, Direzione); 
	}
	
	switch(Direzione){																		//2. Come modifico le coordinate?
		case UP:
			if(FACE_UP){	FACE_UP=0; 	(whoPt->i)-=2; 	}			//sono in un caso speciale
			else (whoPt->i)--; 		
			break; 
		case DOWN: 	
			if(FACE_DOWN)	{FACE_DOWN=0; (whoPt->i)+=2; }				//sono in un caso speciale
			else(whoPt->i)++;			
			break; 
		case LEFT:  (whoPt->j)--; 		break; 
		case RIGHT: (whoPt->j)++;			break; 
		default: break; 
	}
	
	ColoraCella(whoPt->i, whoPt->j, whoPt->Colore); //3. Colora la cella nuova
	return; 
}

/*==========================================================
Registrazione delle mossa effettuata nel vettore apposito
----------------------------------------------------------*/
void RegistraMossa(int X, int Y, MODALITA M, ORIENTA OR)
{
	//Azzerare Mosse[CntMosse]
	Mosse[CntMosse]=0; 
	//Registro nuova posizione
	Mosse[CntMosse] |= ((uint8_t)X)<<0; 
	Mosse[CntMosse] |= ((uint8_t)Y)<<8; 
	//Registro Orientamento se serve
	if(M==MOVE)	Mosse[CntMosse] |= (1-OR)<<16; 
	else				Mosse[CntMosse] |= 0<<16; 
	//Registro Modalita' di gioco da cui proviene la mossa
	Mosse[CntMosse] |= (M << 20); 
	//Registro chi ha 'mosso'
	Mosse[CntMosse] |= (Who-1)<<24; 
	
	CntMosse++; 			//Conto la mossa/Mi sposto nel vettore delle mosse	
	if(CntMosse==100)	CntMosse=0; 
	return; 
}


/*==============================================================================
Evidenzio le celle in cui mi posso spostare nel caso scelga la modalità MOVE
Attenzione! Devo assicurarmi che dove voglio spostarmi non ci sia l'altro Player
------------------------------------------------------------------------------*/
void Check(void)
{
	_typePLAYER *whoPt, *whoPtOther;
	int i, j; 
	//Selezione del giocatore
	whoPt = (Who==1)	? (&P1) : (&P2);
	whoPtOther = (Who==1) 	? (&P2) : (&P1); 
	
	FlagUP= (whoPt->i > 0) && SCACCHIERA[(whoPt->i)][whoPt->j].UP_Block==0; 
	FlagDOWN=(whoPt->i < DimScacchiera-1) && SCACCHIERA[(whoPt->i)][whoPt->j].DOWN_Block==0; 
	FlagLEFT=(whoPt->j >0) && SCACCHIERA[whoPt->i][(whoPt->j)].LEFT_Block==0; 
	FlagRIGHT=(whoPt->j < DimScacchiera-1) && SCACCHIERA[whoPt->i][(whoPt->j)].RIGHT_Block==0; 
	
	//GESTISCE ANCHE IL CASO DI FACE TO FACE
	if(FlagUP){
		i=(whoPt->i)-1;			j=whoPt->j; 
		//Mi assicuro che lì non ci sia l'altro Player
		
		FlagUP = whoPtOther->i != i || whoPtOther->j != j; 
		if(whoPtOther->i != i || whoPtOther->j != j)
			ColoraCella((whoPt->i)-1, whoPt->j, Col_Ev);
		else		//Sopra ho l'altro Player, potrei pensare di scavalcarlo
		{
			i=whoPtOther->i; j=whoPtOther->j;
			FlagUP = 	Who==2 && 															//sono il Player 1
					!SCACCHIERA[i][j].UP_Block && 				//L'altro Player non ha un muro dietro
					whoPtOther->i>=1;
			if(	Who==2 && 															//sono il Player 1
					!SCACCHIERA[i][j].UP_Block && 				//L'altro Player non ha un muro dietro
					whoPtOther->i>=1)												//L'altro Player ha ancora una cella dietro
				ColoraCella((whoPt->i)-2, whoPt->j, Col_Ev), FACE_UP=1; 
			else 
				FACE_UP=0; 
		}
	}
	
	//GESTISCE ANCHE IL CASO DI FACE TO FACE
	if(FlagDOWN){
		i=(whoPt->i)+1;			j=whoPt->j; 
		
		FlagDOWN  = whoPtOther->i != i || whoPtOther->j != j;
		if(whoPtOther->i != i || whoPtOther->j != j) 
			ColoraCella((whoPt->i)+1, whoPt->j, Col_Ev); 
		else		//subito sotto ho l'altro player, potrei pensare di saltargli dietro
		{
			i=whoPtOther->i; j=whoPtOther->j;
			FlagDOWN=Who==1 && 															//sono il Player 1
					!SCACCHIERA[i][j].DOWN_Block && 				//L'altro Player non ha un muro dietro
					whoPtOther->i<=5; 
			if(	Who==1 && 															//sono il Player 1
					!SCACCHIERA[i][j].DOWN_Block && 				//L'altro Player non ha un muro dietro
					whoPtOther->i<=5)												//L'altro Player ha ancora una cella dietro
				ColoraCella((whoPt->i)+2, whoPt->j, Col_Ev), FACE_DOWN=1; 
			else FACE_DOWN=0; 
		}
	}
	
	if(FlagLEFT){
		i=whoPt->i;			j=whoPt->j-1; 
		FlagLEFT = whoPtOther->i != i || whoPtOther->j != j;
		if(whoPtOther->i != i || whoPtOther->j != j) 
			ColoraCella(whoPt->i, (whoPt->j)-1, Col_Ev); 
	}
	if(FlagRIGHT){ 
		i=whoPt->i;			j=whoPt->j+1; 
		FlagRIGHT = whoPtOther->i != i || whoPtOther->j != j;
		if(whoPtOther->i != i || whoPtOther->j != j)
			ColoraCella(whoPt->i, (whoPt->j)+1, Col_Ev);
	}
}

/*===================================================================
Disegna il muro in base ad una cella di riferimento i cui 
indici della SCACCHIERA[][] sono passati come parametri.
In entrambi i casi (VERTICALE/ORIZZONTALE) la cella di riferimento 
è quella in alto a sinistra.
--------------------------------------------------------------------*/
void DisegnaMuro(int i, int j, ORIENTA OR, int Colore)
{
	_typePUNTO _tmpA, _tmpB; 
	int ind;
	dimMuro=2*dimCella+margine;
	 
	switch(OR)
	{
		case ORIZZONTALE: 
			//Creo i vertici del rettangolo
			_tmpA = CreaPunto(SCACCHIERA[i][j].A.x, SCACCHIERA[i][j].B.y+1); 
			_tmpB = CreaPunto(SCACCHIERA[i+1][j+1].B.x, SCACCHIERA[i+1][j+1].A.y);
			//Registro le coordinate dei vertici
			MuroTmp.A= _tmpA; 
			MuroTmp.B=_tmpB;
			//Coloro muro
			for (ind=0; ind<margine; ind++)
				LCD_DrawLine(_tmpA.x,_tmpA.y+ind, 
										_tmpA.x+dimMuro, _tmpA.y+ind, 
										Colore); 
			break; 
		case VERTICALE:
			//Creo i vertici del rettangolo
			_tmpA = CreaPunto(SCACCHIERA[i][j].B.x+1, SCACCHIERA[i][j].A.y); 
			_tmpB = CreaPunto(SCACCHIERA[i+1][j+1].A.x-1, SCACCHIERA[i+1][j+1].B.y); 
		  //Registro le coordinate dei vertici
			MuroTmp.A= _tmpA; 
			MuroTmp.B=_tmpB;

			//Coloro Muro
			for (ind=0; ind<dimMuro; ind++)
				LCD_DrawLine(_tmpA.x,_tmpA.y+ind, 
										_tmpA.x+margine-1, _tmpA.y+ind, 
										Colore); 
			break; 
	}
	return;
}

/*==================================================
Sposta il Muro corrente in direzione DIR
--------------------------------------------------*/
void MuoviMuro(DIR Dir)
{
	ORIENTA O = MuroTmp.Quale; 
	//Cancella quello vecchio
	DisegnaMuro(MuroTmp.i, MuroTmp.j, O, Sfondo);
	
	//Cambia riferimenti
	switch(Dir)
	{
		case UP: MuroTmp.i--; break; 
		case DOWN: MuroTmp.i++; break; 
		case RIGHT: MuroTmp.j++; break; 
		case LEFT: MuroTmp.j--; break; 
	}
	//Disegna nuovo muro
	DisegnaMuro(MuroTmp.i, MuroTmp.j, O, Col_Muro); 
}

/*==============================================================
Ruota MuroTmp di 90 gradi rispetto al posizionamento corrente
--------------------------------------------------------------*/
void RuotaMuro(void)
{
	DisegnaMuro(MuroTmp.i, MuroTmp.j, MuroTmp.Quale, Sfondo);			//Cancella Vecchio
	MuroTmp.Quale = MuroTmp.Quale==ORIZZONTALE ? VERTICALE : ORIZZONTALE;
	DisegnaMuro(MuroTmp.i, MuroTmp.j, MuroTmp.Quale, Col_Muro); 	//Disegna Nuovo
	return; 
}

/*=================================================================
Rende definitiva la scelta di posizionamento del muro. Inoltre
vengono aggiornati i vincoli di movimento sulla scacchiera, 
viene registrata la mossa valida compiuta.
-----------------------------------------------------------------*/
void ConfermaMuro(void)
{
	int i, j; int x, y; 
	DisegnaMuro(MuroTmp.i, MuroTmp.j, MuroTmp.Quale, ColDef_Muro);
	i=MuroTmp.i; j=MuroTmp.j; 
	
	//Devo bloccare le posizioni associate al muro che ho piazzato
	if(MuroTmp.Quale==ORIZZONTALE)
	{
		SCACCHIERA[i][j].DOWN_Block=1; 
		SCACCHIERA[i][j+1].DOWN_Block=1; 
		SCACCHIERA[i+1][j].UP_Block=1; 
		SCACCHIERA[i+1][j+1].UP_Block=1; 
	}
	else {
		SCACCHIERA[i][j].RIGHT_Block=1; 
		SCACCHIERA[i][j+1].LEFT_Block=1; 
		SCACCHIERA[i+1][j].RIGHT_Block=1; 
		SCACCHIERA[i+1][j+1].LEFT_Block=1; 
	}
	
	//Registrazione mossa
	
	x = (MuroTmp.A.x+MuroTmp.B.x)/2; 
	y = (MuroTmp.A.y+MuroTmp.B.y)/2; 
	RegistraMossa(x, y, WALL, MuroTmp.Quale); 
}

/*==============================================================
Restituisce un booleano che mi dice se posso muovere o meno
il muro corrente in una certa direzione fornita come 
parametro 
---------------------------------------------------------------*/
int CheckMuro (DIR Dir)
{
	int Flag=1;
	
	switch(Dir){
				case UP:	 						Flag=MuroTmp.i>0; 		break; 
				case DOWN: 						Flag=MuroTmp.i<5;	 		break; 
				case LEFT: 						Flag=MuroTmp.j>0;			break; 	
				case RIGHT: 					Flag=MuroTmp.j<5;			break; 
				default: 																				break; 
			}

	return Flag; 
}
