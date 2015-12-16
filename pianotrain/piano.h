#include <windows.h>
#include <time.h>

#define OFFSETPENTA 5                 // offset previo al primer compas

typedef struct _pitch
{
	int notekey;
	double freq;
} PITCH;

typedef struct _timenota
{
	double time;
	double timestamp; 
	int valid;
	int pos,npos,ncompas,penta;
} TIMENOTA;

typedef struct runnota_
{
    __int64 nota;
	__int64 tie;
	__int64 notakey;
	__int64 acci;
	int tupla;
	int clef;
	int color;
	double dura;
    int pos;
	int dif;           // cambio diferencial del puntero indice
	int beat[16];       // comas de 16 tiempos como maximo
} RUNNOTA;

typedef struct _buffmidi
{
	int notekey;
	int freeable;
	TIMENOTA *timenota;
	clock_t timestamp;
	int source;
} BUFFMIDI;

typedef struct _hitsprob
{
	int note;        
	int penta;
	int masa[3];       // 0 nota natural , 1 sostenido, 2 bemol
	int numhits[3];    
	int numfails[3];
} HITSPROB;

typedef struct _config
{
	char compas[64];
	int maxintervalog;
	int maxintervalof;
	int minintervalog;
    int minintervalof;
	char armadura[64];
	char numcompases[64];
	int fingering;
	char bloquesritmicosg[256];
    char bloquesritmicosf[256];
	int maxintervalog_noteinterval;
	int maxintervalof_noteinterval;
	int minintervalog_noteinterval;
    int minintervalof_noteinterval;		
	int maxintervalo_flashnoteinterval;
	int minintervalo_flashnoteinterval;
	int randommodegenerationmusic;
	int randommusicqosritmo;
	int randommusicqoscompas;
    int randommusicqoslegato;
	int notereadlearning;
	int ritmo;
	int metronomo;
	int metronoteread;
	int metronoteinterval;
	int repeatg;
	int repeatf;
	int noteinfgeneric[2];
	int notesupgeneric[2];
	int noteinfrandommusic[2];
	int notesuprandommusic[2];
	int noteinfpaintkey[2];
	int notesuppaintkey[2];
	int iInDevice;
	int iOutDevice;
	int DefaultVelocity;
	int maxcompascreen;
	int maxvariacion;
	int silencios;
	int accuracynotes;
	int accuracytempo;
	int armaduranotereading;
	int armaduranoteinterval;
	int singoctava;
	int earoctava;
	double mintono_earnoteread;
	double maxtono_earnoteread;
	int notereadingacci;
	int notereadingchord;
	int notereadingdeep;
	int notereadingmemo;
	int notereadinglead;
	int notereadingfiguras;
	int notereadingfingering;
    int notereadqosritmo;
    int notereadqoslegato;
	int randommusicacci;
	int stoponerror;
	int maxintervalog_noteread;
	int minintervalog_noteread;
	int maxintervalof_noteread;
	int minintervalof_noteread;
	int oidoabrel;
	int tiposolfeo;
	int ocultanota;
	int memoespacial;
	int followme;	
	int sigueme;
	int soloritmo;
	int repsierrritmo;
	char midfile[128];
	char xmlfile[128];
	int playmode;
} CONFIG;


typedef struct runpenta_
{
	RUNNOTA runnota[64];	
	int hor;           // maxima cantidad de semifusas en el compas, maximo 64 en 4x4
	int ver;           // maxima cantidad de notas superpuestas en un acorde del compas
} RUNPENTA;

typedef struct ejecucion_
{
  RUNPENTA penta[2];
} EJECUCION;

typedef struct bloqueritmico_
{
    char bloques[32][32];	
	int numbloques;
} BLOQUERITMICO;

typedef struct ritmo_
{
   BLOQUERITMICO bloqueritmico[2];
} RITMO;

typedef struct nota_
{
	int nota;
	int acci;
	int notakey;
	int penta;
	int compas;
	int color;
	int posx;
	int finger;
	double dura;
} NOTA;

typedef struct chord_
{
	__int64 nota;
	__int64 acci;
	__int64 notakey;
	int penta;
	int compas;
	int color;
	double dura;
} CHORD;

typedef struct notereadinglist_
{ 
    double tdura; 
	clock_t itime; 
	clock_t etime; 
} NOTEREADINGLIST;

typedef struct memoespacial_
{
   int nota,sb,dedo;
   int color;
   NOTA abnota;
   HBITMAP hbmp[3];
} MEMOESPACIAL;

typedef struct statisdics_
{
   int notasslow;
   int notasfast;
   int notasjust;
   double notasslowqos;
   double notasfastqos;
   double notasjustqos;
   double durcompasqos;
   double durcompas;
   double durnotastotal;
   double ritmic;
} STATISTICS;

typedef struct compas_
{
	__int64 note[25];
	__int64 tie[25];
	__int64 notekey[25];
	__int64 acci[25];
	double dura[25];
	int tupla[25];   // tipo de tupla 3 (tresillo) 4 (quadruplet) ...
	int clef[25];  // a efecto de representacion, si 0 se representa la nota en el petagrama de sol, si 1 en el de fa
	TIMENOTA timenota[25];
	int color[25];
	int numnotas;
	int numsilencios;
	int armadura;     // >0 numero de sostenidos <0 numero de bemoles
    int numsignature;
    int densignature;
	int posx[25];
	int posy[25][25];
	int posxfin[25][25];       // posicion final del acorde
	int fingering[25];
	int seqcompas;
	int posxfincompas;
	int posxcompas;
	int posycompas;
	TIMENOTA timecompas;
	int errormarca[25];
	STATISTICS std;
} COMPAS;

typedef struct penta_
{
   int maxintervalo;
   int minintervalo;
   int allsilencio;
   COMPAS *compas;
} PENTA;

typedef struct puntero_
{
   int compas;
   int index;
   int canelem;   
   EJECUCION *eje;
} PUNTERO;

typedef struct score_
{
   int itempocompas;
   int tempocompas;
   int oktempocompas;
   int slowtempocompas;
   int fasttempocompas;
   int okduringnote;
   int slowduringnote;
   int fastduringnote;
   double reaction;       // tiempo de reaccion  para notereading
   double hits;
   double fails;
   double perhits;
   double perfails;
   double movmeanhits20;
   double qos;
} SCORE;

typedef struct melodia_
{
   int numsignature;
   int densignature;
   int armadura;     // >0 numero de sostenidos <0 numero de bemoles
   int fingering;
   int numcompases;
   PENTA penta[2];
   int noteinf[2];
   int notesup[2]; 
   int metronomo;    // numero de pulsos/minuto teoricos
   int metronomoreal;        // numero de pulsos/minuto en ejecucion
   int silencios;
   int repeatg;
   int repeatf;
   int randommodegenerationmusic;
   int oth;          // 0 ambas manos 1 left hand 2 right hand
   PUNTERO puneje;
   RITMO ritmo;
   SCORE score;
} MELODIA;

typedef struct playmelodiadata
{
   int compas;
   int interval;
} PLAYMELODIADATA;
    