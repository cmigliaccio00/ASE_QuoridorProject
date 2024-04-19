#include "../GLCD/GLCD.h"
#ifndef CAN_lib
#define CAN_lib 
	#include "../CAN/CAN.h"
#endif
#define TIMING 1
#define COLORA_CELLA 1

typedef struct PUNTO {	int x, y; } _typePUNTO;
typedef struct {int i, j; } CASELLA; 
typedef struct CELLA {	_typePUNTO A, B; 
						int UP_Block, 
							DOWN_Block, 
							LEFT_Block, 
							RIGHT_Block; 
						int CentroMuro; 
					  } _typeCELLA; 
typedef enum DIREZIONE {UP, DOWN, RIGHT, LEFT, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT} DIR;
typedef enum O {ORIZZONTALE, VERTICALE} ORIENTA;
typedef enum MOD {MOVE, WALL} MODALITA; 
typedef struct MURO {	int Who;
						int i, j; 
						_typePUNTO A, B; 
						ORIENTA Quale; 
					} _typeMURO; 
typedef enum Val {MURI, TEMPO} VAL; 
/*======================================================================
Attenzione: _typePUNTO qui viene usato per individuare le celle della 
scacchiera NON per la gestione dei pixel!!
----------------------------------------------------------------------*/
typedef struct PLAYER {	int i, j; 
						int NumMuri; 
						int Colore; 
					  } _typePLAYER; 

typedef enum {SINGLE, DUAL} GAME_MODE;			//Modalità di gioco	
typedef enum {HUMAN, NPC} PLAYER_TYPE; 				//Tipo di avversario

typedef enum {Iniziale, Singolo, Dual}	QualeMenu; 	//Tipo di Menu

typedef enum {MODE, PLAYER_SEL, PLAY} STATO; 



CASELLA CreaCasella  (int i, int j); 
_typePUNTO CreaPunto (int x, int y); 
void DisegnaCella(_typePUNTO A, _typePUNTO B); 
_typeCELLA CreaCella(_typePUNTO A, _typePUNTO B, int, int, int, int); 
void DisegnaScacchiera(void);
void ColoraCella(int i, int j, int Colore); 
void MostraInfo(void); 
void QUORIDOR_Init(void); 
void MuoviCella(DIR Direzione);
void Check(void); 

void DisegnaMuro(int i, int j, ORIENTA OR, int Colore); 
void MuoviMuro(DIR Dir); 
void RuotaMuro(void); 
void ConfermaMuro(void); 
void ConfermaMuroSenzaRegistra(void);
void MostraInfo2 (void);
void AggiornaPunteggio(VAL Cosa); 
void RegistraMossa(int X, int Y, MODALITA M, ORIENTA OR);
void AggiornaTimer(void); 
void CancellaWarning(void); 
void MostraWarning(char* Msg); 
int CheckMuro (DIR Dir);
void CancellaEvidenziateTutte (void);

void MossaNPC(_typePLAYER* whoPt);
void MossaNPC_wrapper(void);
void ControllaVittoria(_typePLAYER* whoPt);

void TrasmettiCAN_Msg(int Mossa);

