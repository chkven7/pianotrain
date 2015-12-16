
#include "piano.h"
#include <string.h>
#include <stdio.h>
#include <io.h>

#define MAX_VOICES 30
#define DECSIZE 10

extern "C"{
	char decorations[];
}

typedef enum {
ABC2MIDI,
ABC2ABC,
YAPS,
ABCMATCH} programname;

//programname fileprogram = ABC2ABC;

extern "C" {  
  void init_abbreviations();
  void parsefile(char *);
  void free_abbreviations();
  void parseroff();
  void parseron();
  void resetglobalsparseabc();
}
int armadura(int minota,int acci,int ar);
int isinchord(int notekey,__int64 chord,__int64 tie,int &istie);
void LiberaMelodia(MELODIA *);
extern void addnotasilencio(MELODIA *melodia,int j,int c,double dura);
extern void addnotasilenciohead(MELODIA *melodia,int j,int c,double dura);
extern void addcompassilencio(MELODIA *melodia,int penta,int compas,int armadura,int num,int den,double dura);
extern void allsilencio(MELODIA *melodia,int penta);
extern EJECUCION *generaejecucion(PENTA *penta,int compas);
extern int verificaejecucionmidi(EJECUCION *eje,int &pos);
extern int verificacompas(MELODIA *melodia,int penta,int compas);
extern int nokey; /* signals no key signature assumed */
extern int voicecodes ;  /* from parseabc.c */
extern char voicecode[16][30]; /*for interpreting V: string */
extern char *mode[];
extern int modekeyshift[];

static MELODIA *melodia;
static double length; // por defecto negra
static int penta=-1;
static int voice,canvoice[64];
static int compas[64];
static int istuple;
static int broken;
static int chordon;
static int numts;
static int dents;
static int armad;
static int index=-1;
static int desp;
static int countevent;
static int numcompases;   
static int numsilencios;   
static int numcompa[64];  //64 voces como maximo
static int parsefin;
static int startmusicline;

static int notas00[44]={36,38,40,41,43,45,47,    \
	                    48,50,52,53,55,57,59,    \
   	 		            60,62,64,65,67,69,71,    \
				        72,74,76,77,79,81,83,    \
						84,86,88,89,91,93,95,    \
						96,98,100,101,103,105,107,108 \
};

static int outnotasminfStaff[15]={36,35,33,31,29,28,26,24, \
                             23,21,19,17,16,14,12 \
};

extern TCHAR titlebar[256];

void resetglobalsabctopiano(void)
{
   int r;

   length=-1;     // =0.25 por defecto negra
   penta=-1;
   for(r=0;r<64;r++){ canvoice[r]=compas[r]=numcompa[r]=0; }
   istuple=0;
   voice=0;
   parsefin=0;
   chordon=0;
   broken=0;
   numts=0;
   dents=0;
   armad=0;
   index=-1;
   desp=0;
   countevent=0;
   numcompases=0;
   numsilencios=0;
   startmusicline=0;
   return;
}

extern "C" void event_eof()   // fin del procesado
{
   if(parsefin==1) return;
   if(countevent) printf("event_eof \n");   
   return;
}

extern "C" void event_blankline()
{
   if(parsefin==1) return;
   if(countevent) printf("event_blankline \n");
   parseroff();
   return;
}

extern "C" void event_text(char *p)
{
   if(parsefin==1) return;
   if(countevent) printf("event_text \n");
   return;
}

extern "C" void event_reserved(char p)
{
   if(parsefin==1) return;
   if(countevent) printf("event_reserved \n");
   return;
}

extern "C" void event_tex(char *s)
{
    if(parsefin==1) return;
	if(countevent) printf("event_tex \n");
	return;
}

extern "C" void event_linebreak()
{
	if(parsefin==1) return;
	if(countevent) printf("event_linebreak \n");
	return;
}

extern "C" void event_startmusicline()
/* encountered the start of a line of notes */
{
    if(parsefin==1) return;
	if(countevent) printf("event_stratmusicline \n");
	startmusicline=1;
	return;
}

extern "C" void event_endmusicline(char endchar)
/* encountered the end of a line of notes */
{
    if(parsefin==1) return;
	if(countevent) printf("event_endmusicline \n");
	return;
}

extern "C" void event_error(char *s)
{
    if(parsefin==1) return;
	if(countevent) printf("event_error \n");
	return;
}

extern "C" void event_warning(char *s)
{
    if(parsefin==1) return;
	if(countevent) printf("event_warning \n");
	return;
}

extern "C" void event_comment(char *s) // s es un apuntador a los comentarios %%
{
    if(parsefin==1) return;
	if(countevent) printf("event_comment \n");
	return;
}

extern "C" void event_specific(char *package, char *s)  // da informacion sobre quien a generado el abc file
{
    if(parsefin==1) return;
	if(countevent) printf("event_specific \n");
	return;
}

extern "C" void event_info(char *f)
/* handles info field I: */
{
    if(parsefin==1) return;
	if(countevent) printf("event_info \n");
	return;
}

// en el original event_field recibe un apuntador a char y debe ser una variable  char
extern "C" void event_field(char k, char *f)  // titulo
{
    if(parsefin==1) return;
	if(countevent) printf("event_field \n");
	if(k=='T')
	   strcpy(titlebar,f);
	return;
}

extern "C" void event_part(char *s)
{
   if(parsefin==1) return;
   if(countevent) printf("event_part \n");
   return;
}

void initvoiceonpenta(MELODIA *melodia,int penta)
{
	int r,j;

	melodia->numcompases=numcompases;
	melodia->penta[penta].compas=(COMPAS *) new COMPAS[numcompases];
	melodia->penta[penta].allsilencio=0;
	for(r=0;r<numcompases;r++){
	   for(j=0;j<25;j++){
		  melodia->penta[penta].compas[r].note[j]=0;
		  melodia->penta[penta].compas[r].clef[j]=penta;
		  melodia->penta[penta].compas[r].tupla[j]=1;
		  melodia->penta[penta].compas[r].tie[j]=0;
		  melodia->penta[penta].compas[r].notekey[j]=0;
		  melodia->penta[penta].compas[r].acci[j]=0;
	      melodia->penta[penta].compas[r].dura[j]=0;
		  melodia->penta[penta].compas[r].timenota[j].time=-1;
		  melodia->penta[penta].compas[r].timenota[j].valid=0;
		  melodia->penta[penta].compas[r].errormarca[j]=-1;
          melodia->penta[penta].compas[r].fingering[j]=-1;
		  melodia->penta[penta].compas[r].color[j]=0;		          		  
	   }
	}
    return;
}

extern "C" void event_voice(int n, char *s, struct voice_params *vp)  // n es el valor que acompaña a V:
{
	if(countevent){ 
	   printf("event_voice \n"); 
	   //++voice;
	   if(canvoice[n]==0) ++canvoice[n];
	   penta=n-1;
	   return; 
	}
	if(n>2){ parsefin=1; return; }
	index=-1;
	istuple=0;
    //++penta;
	penta=n-1; //prueba
	//compas[penta]=0;
/*
	melodia->numcompases=numcompases;
	melodia->penta[penta].compas=(COMPAS *) new COMPAS[numcompases];
	melodia->penta[penta].allsilencio=0;
	for(r=0;r<numcompases;r++){
	   for(j=0;j<25;j++){
		  melodia->penta[penta].compas[r].note[j]=0;
		  melodia->penta[penta].compas[r].clef[j]=penta;
		  melodia->penta[penta].compas[r].tupla[j]=1;
		  melodia->penta[penta].compas[r].tie[j]=0;
		  melodia->penta[penta].compas[r].notekey[j]=0;
		  melodia->penta[penta].compas[r].acci[j]=0;
	      melodia->penta[penta].compas[r].dura[j]=0;
		  melodia->penta[penta].compas[r].timenota[j].time=-1;
		  melodia->penta[penta].compas[r].timenota[j].valid=0;
		  melodia->penta[penta].compas[r].errormarca[j]=-1;
          melodia->penta[penta].compas[r].fingering[j]=-1;
		  melodia->penta[penta].compas[r].color[j]=0;		          		  
	   }
	}
	*/
	return;
}

extern "C" void event_length(int n)  // longitud por defecto de las notas 1/n
{
    if(parsefin==1) return;
	if(countevent){ printf("event_length \n"); return; }
	if(compas[penta]>=numcompases) return;
	length=1.0/n;
	return;
}

extern "C" void event_refno(int n)  // lee el primer campo, el indice o numero de melodias en el tono, X:1 (n sera 1)
{
    if(parsefin==1) return;
	if(countevent) printf("event_refno \n");
    parseron();
	return;
}

extern "C" void event_tempo(int n, int a, int b,int relative,char *pre,char *post) // n numero de (a/b) figuras por minuto
{
    if(parsefin==1) return;
	if(countevent){ printf("event_temp \n"); return; }
    if(compas[penta]>=numcompases) return;
	melodia->metronomo=n;
	return;
}

extern "C" void event_timesig(int n, int m,int checkbars)  // n y m contiene la fraccion del compas
{
    if(parsefin==1) return;
	if(countevent){ printf("event_timesig \n"); return; }
	if(compas[penta]>=numcompases) return;
	melodia->numsignature=n;  
	melodia->densignature=m;
	numts=n;
	dents=m;
	if(length<0){
	   double val;
	   val=n*1.0/m;
	   if(val<0.75) length=1/16.0;
	   else length=1/8.0;
	}
	return;
}

extern "C" void event_key(int sharps,char *s,int modeindex,char modmap[7],char modmul[7],int gotkey,int gotclef,char *clefname,
          int octave,int xtranspose,int gotoctave,int gottranspose,int explict)
{
    if(parsefin==1) return;
	if(countevent){ printf("event_key \n"); return; }
	if(compas[penta]>=numcompases) return;
	melodia->armadura=sharps;
	armad=sharps;
	return;
}


extern "C" void event_spacing(int n,int m)
{
    if(parsefin==1) return;
	if(countevent) printf("event_spacing \n");
	if(compas[penta]>=numcompases) return;
	return;
}


extern "C" void event_rest(int decorators[DECSIZE],int n,int m,int type)
{
	double dura;

    if(parsefin==1) return;
	if(countevent){ printf("event_rest \n"); return; }
	if(compas[penta]>=numcompases) return;
	++index;
	dura=length*n/m;
	melodia->penta[penta].compas[compas[penta]].note[index]=0;
	melodia->penta[penta].compas[compas[penta]].clef[index]=penta;
	melodia->penta[penta].compas[compas[penta]].tupla[index]=1;
	melodia->penta[penta].compas[compas[penta]].tie[index]=0;
    melodia->penta[penta].compas[compas[penta]].acci[index]=0;	
    melodia->penta[penta].compas[compas[penta]].notekey[index]=0;
 	melodia->penta[penta].compas[compas[penta]].dura[index]=dura;
	melodia->penta[penta].compas[compas[penta]].armadura=armad;
    melodia->penta[penta].compas[compas[penta]].numsignature=numts;
    melodia->penta[penta].compas[compas[penta]].densignature=dents;
    melodia->penta[penta].compas[compas[penta]].timenota[index].time=-1;
	melodia->penta[penta].compas[compas[penta]].timenota[index].valid=0;
	melodia->penta[penta].compas[compas[penta]].errormarca[index]=-1;
    melodia->penta[penta].compas[compas[penta]].fingering[index]=-1;
	melodia->penta[penta].compas[compas[penta]].color[index]=0;
	++numsilencios;
	return;
}

extern "C" void event_mrest(int n,int m)
{
    if(parsefin==1) return; 
	if(countevent) printf("event_mrest \n");
	return;
}

// type==0 es una barra de separacion entre compases
// type==2 es un signo de inicio de repeticion |:
// type==3 es un signo de final de repeticion :| 
extern "C" void event_bar(int type,char *replist)
{
    if(parsefin==1) return;
	if(startmusicline==1 && type==2){
	   startmusicline=0;
       // signo de repeticion |: en inicio de linea musical
	   // no es fin de compas e inicio de siguiente, no lo tratamos como fin de compas y se 
	   // pasa por alto
	   return;
	}
	if(countevent){ 
	   printf("event_bar \n"); 
	   ++numcompa[penta]; //=numcompases;
	   return; 	
	}
	if(compas[penta]>=numcompases) return;
	melodia->penta[penta].compas[compas[penta]].numnotas=index+1;
    melodia->penta[penta].compas[compas[penta]].numsilencios=numsilencios;
    melodia->penta[penta].compas[compas[penta]].seqcompas=compas[penta];
    melodia->penta[penta].compas[compas[penta]].timecompas.time=-1;
    melodia->penta[penta].compas[compas[penta]].timecompas.valid=0;
	numsilencios=0;
	startmusicline=0;
	index=-1;
	++compas[penta];
	return;
}

extern "C" void event_space()
{
    if(parsefin==1) return;
	if(countevent) printf("event_space \n");
	return;
}

extern "C" void event_graceon()
{
    if(parsefin==1) return;
	if(countevent) printf("event_graceon \n");
	return;
}

extern "C" void event_graceoff()
{
    if(parsefin==1) return;
	if(countevent) printf("event_graceoff \n");
	return;
}

extern "C" void event_rep1()
{
    if(parsefin==1) return;
	if(countevent) printf("event_rep1 \n");
	return;
}

extern "C" void event_rep2()
{
   if(parsefin==1) return;
   if(countevent) printf("event_rep2 \n");
   return;
}

extern "C" void event_playonrep(char *s)
{
    if(parsefin==1) return;
	if(countevent) printf("event_playonrep \n");
	return;

}

extern "C" void event_broken(int type,int n)  // la nota correspondiente tiene n puntos que la siguen (<)
{
	double dura;
    
	if(parsefin==1) return;
	if(countevent){ printf("event_broken \n"); return; }
	if(compas[penta]>=numcompases) return;
	if(type==49){ 
	   broken=49;
	   melodia->penta[penta].compas[compas[penta]].dura[index]*=0.5;
	}else{
	   broken=50;
	   dura=melodia->penta[penta].compas[compas[penta]].dura[index];
	   dura=dura+0.5*dura;
	   melodia->penta[penta].compas[compas[penta]].dura[index]=dura;
	}
	return;
}

// se salta a este evento cuando tenemos una tresillo (n=3) o cualquier n-illo
extern "C" void event_tuple(int n,int q,int r)
{
    if(parsefin==1) return;
	if(countevent) printf("event_tuple \n");
	if(n!=3) n=0; // admitiremos solo tresillos de momento
	istuple=n;
	return;
}

extern "C" void event_startinline()
{
    if(parsefin==1) return;
	if(countevent) printf("event_startinline \n");
	return;
}

extern "C" void event_closeinline()
{
    if(parsefin==1) return;
	if(countevent) printf("event_closeinline \n");
	return;
}

extern "C" void event_chord()
{
    if(parsefin==1) return;
	if(countevent) printf("event_chord \n");
	return;
}

extern "C" void event_chordon(int chorddecorators[])   //inicio de acorde
{
    if(parsefin==1) return;
	if(countevent){ printf("event_chordon \n"); return; }
	if(compas[penta]>=numcompases) return;
	desp=0;
	chordon=1;
	return;
}

extern "C" void event_chordoff(int chord_n, int chord_m)  //fin de acorde
{	
    if(parsefin==1) return;
	if(countevent){ printf("event_chordoff \n"); return; }
	if(compas[penta]>=numcompases) return;
	chordon=0;
	++index;
	return;
}

extern "C" void event_handle_gchord(char *s)
/* deals with an accompaniment (guitar) chord */
/* either copies it straight out or transposes it */
{
    if(parsefin==1) return;
	if(countevent) printf("event_handle_gchord \n");
	if(compas[penta]>=numcompases) return;
	return;
}

extern "C" void event_gchord(char *s)
{
    if(parsefin==1) return;
	if(countevent) printf("event_gchord \n");
	return;
}

extern "C" void event_instruction(char *s)
{
    if(parsefin==1) return;
	if(countevent) printf("event_instruction \n");
	return;
}

extern "C" void event_slur(int t)
{
    if(parsefin==1) return;
	if(countevent) printf("event_slur \n");
	return;
}

extern "C" void event_sluron(int t)
{
    if(parsefin==1) return;
	if(countevent) printf("event_sluron \n");
	return;
}

extern "C" void event_sluroff(int t)
{
    if(parsefin==1) return;
	if(countevent) printf("event_sluroff \n");
	return;
}


extern "C" void event_tie()   // ligarura de union
{
    if(parsefin==1) return;
	if(countevent){ printf("event_tie \n"); return; }
	if(compas[penta]>=numcompases) return;
	if(chordon==1){ 		
		__int64 ttie,temp=1;
		ttie=temp<<(8*(desp-1));
		melodia->penta[penta].compas[compas[penta]].tie[index+1]|=ttie;
	}else melodia->penta[penta].compas[compas[penta]].tie[index]=1;
	return;
}

extern "C" void event_lineend(char ch,int n)
{
    if(parsefin==1) return;
	if(countevent) printf("event_lineend \n");
	return;
}

/* these functions are here to satisfy the linker */
extern "C" void event_microtone(int dir, int a, int b)
{
    if(parsefin==1) return;
	if(countevent) printf("event_micronote \n");
	return;
}

extern "C" void event_normal_tone()
{
    if(parsefin==1) return;
	if(countevent) printf("event_normal_tone \n");
	return;
}

// xaccidental: ^ sostenido = becuadrado  _ bemol
// if (nokey)
extern "C" void event_note(int decorators[DECSIZE],char xaccidental,int xmult,char xnote,int xoctave,int n,int m)
{
	int nota,r,istie;
	__int64 mnota,acci;
	double dura;
	char *tokens="cdefgab";
	static __int64 tnote,tacci,tnotekey;

    if(parsefin==1) return;
	if(countevent) return;
	++index;
    nota=7*xoctave+14;
	for(r=0;r<7;r++) if(tokens[r]==xnote) break;
	nota+=r;
    if(nota<0){
	   nota=-nota;
       mnota=outnotasminfStaff[nota];
	}else
	   mnota=notas00[nota];
	dura=length*n/m;
	if(istuple!=0){
	// adminitmos solo tresillos de momento	   
	   melodia->penta[penta].compas[compas[penta]].tupla[index]=3; // es tresillo
	   dura=dura*2.0/3;     // duracion de una nota en un tresillo
	   --istuple;
	}else melodia->penta[penta].compas[compas[penta]].tupla[index]=1;
	if(broken==49) dura+=0.5*dura;
	else if(broken==50) dura*=0.5;
	broken=0;
    if(chordon==0) tnote=tacci=tnotekey=desp=0;	   
	switch(xaccidental){
	   case '^': acci=1; break;   //sostenido
	   case '=': acci=3; break;   //becuadrado
	   case '_': acci=2; break;   //bemol
       default: acci=0; break;    //sin alterar
	}
	
	if(chordon==1)
	   if(isinchord((int) mnota,melodia->penta[penta].compas[compas[penta]].note[index],0,istie)==1) 
	      --desp;
	
    tnote=mnota<<(8*desp);
	tacci=acci<<(8*desp);
    acci=3-2*acci; if(acci==3) acci=0;
	tnotekey=(mnota+armadura((int) mnota,(int) acci,melodia->armadura))<<(8*desp); ++desp;	
	melodia->penta[penta].compas[compas[penta]].note[index]|=tnote;
	melodia->penta[penta].compas[compas[penta]].clef[index]=penta;
    melodia->penta[penta].compas[compas[penta]].acci[index]|=tacci;	
    melodia->penta[penta].compas[compas[penta]].notekey[index]|=tnotekey;
 	melodia->penta[penta].compas[compas[penta]].dura[index]=dura;
	melodia->penta[penta].compas[compas[penta]].armadura=armad;
    melodia->penta[penta].compas[compas[penta]].numsignature=numts;
    melodia->penta[penta].compas[compas[penta]].densignature=dents;
	melodia->penta[penta].compas[compas[penta]].std.durcompas=0;
    melodia->penta[penta].compas[compas[penta]].std.durcompasqos=0;
    melodia->penta[penta].compas[compas[penta]].std.notasfast=0;
    melodia->penta[penta].compas[compas[penta]].std.notasjust=0;
    melodia->penta[penta].compas[compas[penta]].std.notasslow=0;
    melodia->penta[penta].compas[compas[penta]].std.notasfastqos=0;
    melodia->penta[penta].compas[compas[penta]].std.notasjustqos=0;
    melodia->penta[penta].compas[compas[penta]].std.notasslowqos=0;
	melodia->penta[penta].compas[compas[penta]].std.durnotastotal=0;
    melodia->penta[penta].compas[compas[penta]].std.ritmic=0;
	if(chordon==1) --index;
    return;
}


extern "C" void event_abbreviation(char symbol, char *string, char container)
/* a U: field has been found in the abc */
{
    if(parsefin==1) return;
	if(countevent) printf("event_abbreviation \n");
	return;
}

extern "C" void event_acciaccatura()
{
    if(parsefin==1) return;
	if(countevent) printf("event_acciaccatura \n");
	return;
}

extern "C" void event_split_voice ()
{
   if(parsefin==1) return;
   if(countevent) printf("event_split_voice \n");
   return;
/* code contributed by Frank Meisshaert 2012-05-31 */
}


extern "C" void event_words(char *p,int continuation)
/* a w: field has been encountered */
{
   if(parsefin==1) return;
   if(countevent) printf("event_words \n");
   return;
}


__int64 invert(__int64 a,int &modulo)
{
   __int64 b;
   int r,t,s,buf[8];

   for(r=0;r<8;a>>=8,r++){	  
	  s=(int) a&0xff;
      buf[r]=s;
   }
   for(r=7;r>=0;r--)
      if(buf[r]!=0) break;
   for(t=b=0;t<r+1;t++){
	  b<<=8;
      b=b|buf[t];
   }
   modulo=r+1;
   return b;
}

__int64 invertmodulo(__int64 a,int modul)
{
   __int64 b;
   int r,t,s,buf[8];

   for(r=0;r<8;a>>=8,r++){	  
	  s=(int) a&0xff;
      buf[r]=s;
   }
   for(t=b=0;t<modul;t++){
	  b<<=8;
      b=b|buf[t];
   }
   return b;
}

int loadabc(MELODIA *melod,char *filename)
{
  EJECUCION *eje;
  int v,j,c,res,numnotas,idx,numcfa,numcsol,error=1;
  double dura;

  resetglobalsabctopiano();
  resetglobalsparseabc();
  init_abbreviations();
  melodia=melod;
  melodia->metronomo=90;  // por defecto a 90
  countevent=1; 
  if(_access(filename, 0)==-1) 
     return -2;
  parsefile(filename);

  for(c=voice=0;c<64;c++) if(canvoice[c]!=0) ++voice;
  if(voice==0){
     resetglobalsabctopiano();
     resetglobalsparseabc();
	 countevent=1;
     event_voice(1,NULL,NULL); // si no hay voces simulamos una
	 parsefile(filename); 	
  }

  countevent=0; 
  if(voice==0) numcompases=numcompa[0];
  else{ 
	  numcsol=numcompa[0];
	  numcfa=numcompa[1];
	  numcompases=max(numcsol,numcfa);
  }
  resetglobalsparseabc();
  
  if(voice==0){ 
     event_voice(1,NULL,NULL); // si no hay voces simulamos una
     initvoiceonpenta(melodia,0);
  }else{
     initvoiceonpenta(melodia,0);
     initvoiceonpenta(melodia,1);
  }
  parsefile(filename);

  // añadimos compases de silencio para igualar ambos pentagramas
  if(voice==0){
	 melodia->penta[1].compas=(COMPAS *) new COMPAS[numcompases];
	 melodia->penta[1].allsilencio=1;
     allsilencio(melodia,1);
  }else{
	 if(numcsol<numcfa){
        int dif,r,arma,num,den;
		double dura;

		dif=numcfa-numcsol;
		for(r=0;r<dif;r++){
		   num=melodia->penta[1].compas[numcsol+r].numsignature;
		   den=melodia->penta[1].compas[numcsol+r].densignature;
		   arma=melodia->penta[1].compas[numcsol+r].armadura;
		   dura=num*1.0/den;
	       addcompassilencio(melodia,0,numcsol+r,arma,num,den,dura);
		}
	 }else{
        int dif,r,arma,num,den;
		double dura;

		dif=numcsol-numcfa;
		for(r=0;r<dif;r++){
		   num=melodia->penta[0].compas[numcfa+r].numsignature;
		   den=melodia->penta[0].compas[numcfa+r].densignature;
		   arma=melodia->penta[0].compas[numcfa+r].armadura;
		   dura=num*1.0/den;
	       addcompassilencio(melodia,1,numcfa+r,arma,num,den,dura);
		}
	 }

	 // igualamos las signaturas en ambos pentagras
     for(c=0;c<numcompases;c++){
        melodia->penta[1].compas[c].numsignature=melodia->penta[0].compas[c].numsignature;
		melodia->penta[1].compas[c].densignature=melodia->penta[0].compas[c].densignature;
	 }
  }
  free_abbreviations();

  // verificamos si los compases estan completos y si no lo estan les añadimos el silencio
  // que requieran
  for(j=0;j<2;j++){
	 for(c=0;c<numcompases;c++){
        res=verificacompas(melodia,j,c);
		if(res<0)  // compas mas largo de la cuenta
           error=-3-c;
		if(res>0){
		   dura=res*1.0/64;
		   if(c==0) addnotasilenciohead(melodia,j,c,dura);
		   else addnotasilencio(melodia,j,c,dura);
		}

	 }
  }
  
  // verificamos si una misma nota es pulsada cuando ya lo esta
  for(c=0;c<numcompases;c++){
	  if(c==44)
		  printf("");
     eje=generaejecucion(melodia->penta,c);
	 res=verificaejecucionmidi(eje,idx);
	 if(res==0){
        int pos[2],mlt[2],r,j,can;
		pos[0]=eje->penta[0].runnota[idx].pos;
		pos[1]=eje->penta[1].runnota[idx].pos;
		can=eje->penta[0].hor;
		for(j=0;j<2;j++){
		   mlt[j]=0;
		   for(r=0;r<can;r++)
		      if(eje->penta[j].runnota[r].pos==pos[j]) ++mlt[j];
		}
		if(mlt[0]>=mlt[1]){
		   melodia->penta[0].compas[c].note[pos[0]]=0;
           melodia->penta[0].compas[c].acci[pos[0]]=0;
           melodia->penta[0].compas[c].notekey[pos[0]]=0;
		   melodia->penta[0].compas[c].numsilencios++;
		}else{
		   melodia->penta[1].compas[c].note[pos[1]]=0;
		   melodia->penta[1].compas[c].acci[pos[1]]=0;
           melodia->penta[1].compas[c].notekey[pos[1]]=0;
		   melodia->penta[1].compas[c].numsilencios++;
        }       
		--c;
	 }
	 delete [] eje;
  }
// eliminamos ties con origen o destino en notas de silencio

  for(j=0;j<2;j++){
	 for(c=0;c<numcompases;c++){
        numnotas=melodia->penta[j].compas[c].numnotas;    
        for(v=0;v<numnotas;v++){
		   if(melodia->penta[j].compas[c].note[v]==0)
		      melodia->penta[j].compas[c].tie[v]=0;
		   if(v>0){
              if(melodia->penta[j].compas[c].note[v]==0 && melodia->penta[j].compas[c].tie[v-1]!=0)
              melodia->penta[j].compas[c].tie[v-1]=0;
		   }
		}
	 }
  }

  // comprueba que las alteraciones sean coherentes en todo el compas
  
  __int64 chord,acci,nchord,nacci,nnotekey,temp,temp1;
  int sacci,schord,count,t;
  int snacci,snchord,snnotekey;
  for(j=0;j<2;j++){
     for(c=0;c<numcompases;c++){
        numnotas=melodia->penta[j].compas[c].numnotas;    
        for(v=0;v<numnotas;v++){
           chord=melodia->penta[j].compas[c].note[v];    
           acci=melodia->penta[j].compas[c].acci[v];    
		   for(;chord!=0;chord>>=8,acci>>=8){
		      schord=(int) chord&0xff;
			  sacci=(int) acci&0xff;
              if(sacci==0 || sacci==3) continue;              
              for(t=v+1;t<numnotas;t++){
			     nchord=melodia->penta[j].compas[c].note[t];    
			     nacci=melodia->penta[j].compas[c].acci[t];    
			     nnotekey=melodia->penta[j].compas[c].notekey[t];    
                 for(count=0;nchord!=0;nchord>>=8,nacci>>=8,nnotekey>>=8,++count){                 
		            snchord=(int) nchord&0xff;
			        snacci=(int)  nacci&0xff;
				    snnotekey=(int) nnotekey&0xff;
                    if(snchord!=schord) continue;
				    if(snacci!=0) break;
                    temp=sacci; temp=temp<<(8*count);
				    temp1=0xff; temp1=temp1<<(8*count);
                    nacci=melodia->penta[j].compas[c].acci[t];    
                    nacci=nacci&(~temp1);
				    nacci=nacci|temp;
				    melodia->penta[j].compas[c].acci[t]=nacci;    
				    if(sacci==1) ++snnotekey;
				    if(sacci==2) --snnotekey;
                    temp=snnotekey; temp=temp<<(8*count);
				    temp1=0xff; temp1=temp1<<(8*count);
                    nnotekey=melodia->penta[j].compas[c].notekey[t];    
                    nnotekey=nnotekey&(~temp1);
				    nnotekey=nnotekey|temp;
                    melodia->penta[j].compas[c].notekey[t]=nnotekey;    
				    break;
				 }
				 if(snacci==3) break; // becuadrado
			  }
		   }
		}
	 }
  }

// comprueba que los acordes esten codificados de menor nota a mayor nota y si no lo estan
// entonces giralos  
  int a,b,r,buf[8],modul;
  for(j=0;j<2;j++){
	 for(c=0;c<numcompases;c++){
        numnotas=melodia->penta[j].compas[c].numnotas;    
        for(v=0;v<numnotas;v++){
           chord=melodia->penta[j].compas[c].note[v];    
		   if(chord==0) continue; // es silencio
           a=(int) chord&0xff; 
		   chord=chord>>8;
		   b=(int) chord&0xff; 
           if(b<a) continue;
           melodia->penta[j].compas[c].note[v]=invert(melodia->penta[j].compas[c].note[v],modul);
		   melodia->penta[j].compas[c].notekey[v]=invert(melodia->penta[j].compas[c].notekey[v],modul);
           melodia->penta[j].compas[c].acci[v]=invertmodulo(melodia->penta[j].compas[c].acci[v],modul);
		   melodia->penta[j].compas[c].tie[v]=invertmodulo(melodia->penta[j].compas[c].tie[v],modul);
		}
	 }
  }
  return error;  
}

