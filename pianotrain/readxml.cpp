#include "piano.h"
#include <string.h>
#include <stdio.h>
#include <io.h>


#define PARTEXTRACT 1

int parsexml(char *filename);
int partextract;
int canpart;

static MELODIA *melodia;
static int nummeasures[64]; // numero maximo de partes
static int countevent;
static int part;
static int maxstaves[64];
static int minstaves[64];

void resetglobalsxml(void)
{
   int r;

   countevent=0;
   canpart=-1;
   for(r=0;r<64;r++){ 
      nummeasures[r]=maxstaves[r]=0;
      minstaves[r]=10000;
   }
   return;
}

void partstart(char *id)
{
	part=atoi(id+1);
	++canpart;
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void partend(char *id)
{
	return;
}
void measurestart(char *id)
{
    ++nummeasures[canpart];
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void measureend(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void notestart(void)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void noteend(void)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}

void chord(void)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void pitch(char *note,char *alteracion,char *octave)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void tie(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void figura(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}

void staves(char *id)
{
	int st;
	st=atoi(id);
	if(st>maxstaves[canpart]) maxstaves[canpart]=st;
	if(st<minstaves[canpart]) minstaves[canpart]=st;
    if(part!=partextract) return;
	if(countevent==1) return;
	return;
}

void staff(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void armadura(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	melodia->armadura=atoi(id);
	return;
}
void numerador(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	melodia->numsignature=atoi(id);  	
	return; 
}
void denominador(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	melodia->densignature=atoi(id);
	return;
}

void initvoiceonpenta(MELODIA *melodia,int penta,int numcompases)
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

int loadmusicxml(MELODIA *melod,char *filename)
{
  int r;

  melodia=melod;
  melodia->metronomo=90;  // por defecto a 90
  if(_access(filename, 0)==-1) 
     return -2;
  resetglobalsxml();
  countevent=1; 
  parsexml(filename);
  partextract=PARTEXTRACT;
  if(maxstaves[partextract]!=minstaves[partextract])
	  return -2;
  if(maxstaves[partextract]>2) 
	  return -3;
  for(r=0;r<maxstaves[partextract];r++)
     initvoiceonpenta(melodia,r,nummeasures[partextract]);  
  return 1;
}
  
/*

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
}


void notevalue(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}
void alteracion(char *id)
{
	if(part!=partextract) return;
	if(countevent==1) return;
	return;
}

*/