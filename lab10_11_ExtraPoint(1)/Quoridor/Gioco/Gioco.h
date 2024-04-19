#include "../GLCD/GLCD.h"

#define TIMING 1
#define COLORA_CELLA 1

typedef struct PUNTO {	int x, y; } _typePUNTO;
typedef struct CELLA {	_typePUNTO A, B; 
						int UP_Block, 
							DOWN_Block, 
							LEFT_Block, 
							RIGHT_Block; 
					  } _typeCELLA; 
typedef enum DIREZIONE {UP, DOWN, RIGHT, LEFT} DIR;
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

_typePUNTO CreaPunto (int x, int y); 
void DisegnaCella(_typePUNTO A, _typePUNTO B); 
_typeCELLA CreaCella(_typePUNTO A, _typePUNTO B, int, int, int, int); 
void DisegnaScacchiera(void);
void ColoraCella(int i, int j, int Colore); 
void MostraInfo(void); 
void QUORIDOR_Init(void); 
void MuoviCella(DIR Direzione);
void Check(void); 
void CancellaCelleEvidenziate(_typePLAYER *whoPt); 
void DisegnaMuro(int i, int j, ORIENTA OR, int Colore); 
void MuoviMuro(DIR Dir); 
void RuotaMuro(void); 
void ConfermaMuro(void); 
void MostraInfo2 (void);
void AggiornaPunteggio(VAL Cosa); 
void RegistraMossa(int X, int Y, MODALITA M, ORIENTA OR);
void AggiornaTimer(void); 
void CancellaWarning(void); 
void MostraWarning(char* Msg); 
int CheckMuro (DIR Dir);
void CancellaEvidenziateParticolare(_typePLAYER *whoPt, DIR Direzione); 
void CancellaEvidenziateParticolareTUTTE (_typePLAYER* whoPt);