#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "piano.h"
#include <math.h>
#include <direct.h>

EJECUCION *generaejecucion(PENTA *penta,int compas);
int verificaejecucion(EJECUCION *eje);
int verificanumcambios(MELODIA *melodia,int penta,int compas);
int armadura(int nota,int,int ar);
int calcnoteindex(NOTA *n1);
int calcnoteindex(int n1);
void aplicarepeticiones(MELODIA *&melodia);
int fmarkov(char *fil,int ord, int let,char *res);
int loadabc(MELODIA *melod,char *filename);
int isinchord(int notekey,__int64 chord,__int64 tie,int &istie);
void initializefingering(NOTA *listanotas,int can);
int loadmusicxml(MELODIA *melod,char *filename);

extern CONFIG concfg;
extern int BARTOBARSPACE;
extern int NumOfStaffs;
extern TCHAR currentdir[256];
extern HWND hwnd;

static int no[44]={36,38,40,41,43,45,47,         \
	                    48,50,52,53,55,57,59,    \
   	 		            60,62,64,65,67,69,71,    \
				        72,74,76,77,79,81,83,    \
						84,86,88,89,91,93,95,    \
						96,98,100,101,103,105,107,108 \
};

double uniform_deviate(int);

double round(double doValue, int nPrecision,int ro=1)
{
    static const double doBase = 10.0;
    double doComplete5, doComplete5i;
    
    doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));   
	if(ro==1){
       if(doValue < 0.0) doComplete5 -= 5.0;
       else doComplete5 += 5.0;    
	}
    doComplete5 /= doBase;
    modf(doComplete5, &doComplete5i);    
    doValue=doComplete5i / pow(doBase, (double) nPrecision);
	return doValue;
}

void split(char *buf,char *cad1,char *cad2)
{
   char *token;

   token=strtok(buf,"=");
   strcpy(cad1,buf);
   token=strtok(NULL,"=");
   strcpy(cad2,token);
   return;   
}

void saveconfiguracion(void)
{
     FILE *fp;

	 fp=fopen("piano.ini","w");
	 if(fp==NULL) exit(0);
	 fprintf(fp,"%s=%s\n","compas",concfg.compas);
     fprintf(fp,"%s=%d\n","maxintervaloG",concfg.maxintervalog);
	 fprintf(fp,"%s=%d\n","maxintervaloF",concfg.maxintervalof);
	 fprintf(fp,"%s=%d\n","minintervaloG",concfg.minintervalog);
     fprintf(fp,"%s=%d\n","minintervaloF",concfg.minintervalof);
	 fprintf(fp,"%s=%s\n","armadura",concfg.armadura);
	 fprintf(fp,"%s=%s\n","compases",concfg.numcompases);
	 fprintf(fp,"%s=%d\n","fingering",concfg.fingering);
	 fprintf(fp,"%s=%d\n","silencios",concfg.silencios);
	 fprintf(fp,"%s=%d\n","followme",concfg.followme);
	 fprintf(fp,"%s=%d\n","sigueme",concfg.sigueme);
	 fprintf(fp,"%s=%d\n","playmode",concfg.playmode);
	 fprintf(fp,"%s=%d\n","randommusicqosritmo",concfg.randommusicqosritmo);
	 fprintf(fp,"%s=%d\n","notereadqosritmo",concfg.notereadqosritmo);
	 fprintf(fp,"%s=%d\n","randommusicqoscompas",concfg.randommusicqoscompas);
	 fprintf(fp,"%s=%d\n","randommusicqoslegato",concfg.randommusicqoslegato);
	 fprintf(fp,"%s=%d\n","notereadqoslegato",concfg.notereadqoslegato);
	 fprintf(fp,"%s=%s\n","bloquesritmicosG",concfg.bloquesritmicosg);
	 fprintf(fp,"%s=%s\n","bloquesritmicosF",concfg.bloquesritmicosf);
	 fprintf(fp,"%s=%d\n","repeatG",concfg.repeatg);
     fprintf(fp,"%s=%d\n","repeatf",concfg.repeatf);
	 fprintf(fp,"%s=%d\n","notereadlearning",concfg.notereadlearning);
     fprintf(fp,"%s=%d\n","metronoteread",concfg.metronoteread);
	 fprintf(fp,"%s=%d\n","metronoteinterval",concfg.metronoteinterval);
	 fprintf(fp,"%s=%d\n","InDevice",concfg.iInDevice);
	 fprintf(fp,"%s=%d\n","OutDevice",concfg.iOutDevice);
	 fprintf(fp,"%s=%d\n","Velocity",concfg.DefaultVelocity);
	 fprintf(fp,"%s=%d\n","Metronomo",concfg.metronomo);
	 fprintf(fp,"%s=%d\n","Ritmo",concfg.ritmo);
	 fprintf(fp,"%s=%d\n","soloritmo",concfg.soloritmo);
	 fprintf(fp,"%s=%d\n","maxvariacion",concfg.maxvariacion);
	 fprintf(fp,"%s=%d\n","compascreen",concfg.maxcompascreen);
	 fprintf(fp,"%s=%d\n","RandomModeGenerationMusic",concfg.randommodegenerationmusic);
     fprintf(fp,"%s=%d\n","Ocultanota",concfg.ocultanota);
     fprintf(fp,"%s=%d\n","maxintervalog_noteinterval",concfg.maxintervalog_noteinterval);
	 fprintf(fp,"%s=%d\n","maxintervalof_noteinterval",concfg.maxintervalof_noteinterval);
	 fprintf(fp,"%s=%d\n","minintervalog_noteinterval",concfg.minintervalog_noteinterval);
     fprintf(fp,"%s=%d\n","minintervalof_noteinterval",concfg.minintervalof_noteinterval);
	 fprintf(fp,"%s=%d\n","maxintervalo_flashnoteinterval",concfg.maxintervalo_flashnoteinterval);
     fprintf(fp,"%s=%d\n","minintervalo_flashnoteinterval",concfg.minintervalo_flashnoteinterval);
	 fprintf(fp,"%s=%.1f\n","maxtono_earnoteread",concfg.maxtono_earnoteread);
     fprintf(fp,"%s=%.1f\n","mintono_earnoteread",concfg.mintono_earnoteread);
	 fprintf(fp,"%s=%d\n","accuracynotes",concfg.accuracynotes);
     fprintf(fp,"%s=%d\n","accuracytempo",concfg.accuracytempo);
	 fprintf(fp,"%s=%d\n","armaduranotereading",concfg.armaduranotereading);
     fprintf(fp,"%s=%d\n","armaduranoteinterval",concfg.armaduranoteinterval);
	 fprintf(fp,"%s=%d\n","singoctava",concfg.singoctava);
	 fprintf(fp,"%s=%d\n","earoctava",concfg.earoctava);
	 fprintf(fp,"%s=%d\n","stoponerror",concfg.stoponerror);
	 fprintf(fp,"%s=%d\n","notereadingacci",concfg.notereadingacci);
	 fprintf(fp,"%s=%d\n","notereadingchord",concfg.notereadingchord);
	 fprintf(fp,"%s=%d\n","notereadingdeep",concfg.notereadingdeep);
	 fprintf(fp,"%s=%d\n","notereadinglead",concfg.notereadinglead);
	 fprintf(fp,"%s=%d\n","notereadingfiguras",concfg.notereadingfiguras);
	 fprintf(fp,"%s=%d\n","notereadingfingering",concfg.notereadingfingering);
	 fprintf(fp,"%s=%d\n","randommusicacci",concfg.randommusicacci);
	 fprintf(fp,"%s=%d\n","notereadingmemo",concfg.notereadingmemo);
     fprintf(fp,"%s=%d\n","maxintervalog_noteread",concfg.maxintervalog_noteread);
	 fprintf(fp,"%s=%d\n","maxintervalof_noteread",concfg.maxintervalof_noteread);
	 fprintf(fp,"%s=%d\n","minintervalog_noteread",concfg.minintervalog_noteread);
     fprintf(fp,"%s=%d\n","minintervalof_noteread",concfg.minintervalof_noteread);
	 fprintf(fp,"%s=%d\n","oidoabrel",concfg.oidoabrel);
	 fprintf(fp,"%s=%d\n","tiposolfeo",concfg.tiposolfeo);
	 fprintf(fp,"%s=%d\n","memoespacial",concfg.memoespacial);
     fprintf(fp,"%s=%d\n","repsierrritmo",concfg.repsierrritmo);
	 fprintf(fp,"%s=%s\n","midfile",concfg.midfile);
	 fprintf(fp,"%s=%s\n","xmlfile",concfg.xmlfile);
     fclose(fp);
     return;
}

void cargarconfiguracion(MELODIA *melodia)
{    
		// Bloques rítmicos a utilizar
     FILE *fp;
	 char buf[256],*token;
	 char var[256],value[256];
	 int j;

	 fp=fopen("piano.ini","r+");
	 if(fp==NULL) exit(0);
	 while(fgets(buf,256,fp)!=NULL){
		if(buf[0]=='#' || buf[0]==0x0a) continue;
		split(buf,var,value);
        if(!stricmp(var,"compas")){
			if(melodia!=NULL){ 
			   melodia->numsignature=value[0]-0x30;
			   melodia->densignature=value[2]-0x30;
			}
			sscanf(value,"%s",concfg.compas);
            continue;
		}
		if(!stricmp(var,"maxintervaloG")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalog=atoi(buf);
			if(melodia!=NULL) melodia->penta[0].maxintervalo=concfg.maxintervalog;
			continue;
		}
		if(!stricmp(var,"maxintervaloF")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalof=atoi(buf);
			if(melodia!=NULL) melodia->penta[1].maxintervalo=concfg.maxintervalof;
			continue;
		}
		if(!stricmp(var,"minintervaloG")){
		    sscanf(value,"%s",buf);
			concfg.minintervalog=atoi(buf);
			if(melodia!=NULL) melodia->penta[0].minintervalo=concfg.minintervalog;
			continue;
		}
		if(!stricmp(var,"minintervaloF")){
		    sscanf(value,"%s",buf);
			concfg.minintervalof=atoi(buf);
			if(melodia!=NULL) melodia->penta[1].minintervalo=concfg.minintervalof;
			continue;
		}
		if(!stricmp(var,"maxintervalog_noteinterval")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalog_noteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"maxintervalof_noteinterval")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalof_noteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"minintervalog_noteinterval")){
		    sscanf(value,"%s",buf);
			concfg.minintervalog_noteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"minintervalof_noteinterval")){
		    sscanf(value,"%s",buf);
			concfg.minintervalof_noteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadingmemo")){
		    sscanf(value,"%s",buf);
			concfg.notereadingmemo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"maxintervalog_noteread")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalog_noteread=atoi(buf);
			continue;
		}
		if(!stricmp(var,"maxintervalof_noteread")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalof_noteread=atoi(buf);
			continue;
		}
		if(!stricmp(var,"minintervalog_noteread")){
		    sscanf(value,"%s",buf);
			concfg.minintervalog_noteread=atoi(buf);
			continue;
		}
		if(!stricmp(var,"minintervalof_noteread")){
		    sscanf(value,"%s",buf);
			concfg.minintervalof_noteread=atoi(buf);
			continue;
		}
		if(!stricmp(var,"maxintervalo_flashnoteinterval")){
		    sscanf(value,"%s",buf);
			concfg.maxintervalo_flashnoteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"minintervalo_flashnoteinterval")){
		    sscanf(value,"%s",buf);
			concfg.minintervalo_flashnoteinterval=atoi(buf);
			continue;
		}		
		if(!stricmp(var,"maxtono_earnoteread")){
		    sscanf(value,"%s",buf);
			concfg.maxtono_earnoteread=atof(buf);
			continue;
		}
		if(!stricmp(var,"mintono_earnoteread")){
		    sscanf(value,"%s",buf);
			concfg.mintono_earnoteread=atof(buf);
			continue;
		}		
		if(!stricmp(var,"notereadlearning")){
		    sscanf(value,"%s",buf);
			concfg.notereadlearning=atoi(buf);
			continue;
		}
		if(!stricmp(var,"metronoteread")){
		    sscanf(value,"%s",buf);
			concfg.metronoteread=atoi(buf);
			continue;
		}
		if(!stricmp(var,"metronoteinterval")){
		    sscanf(value,"%s",buf);
			concfg.metronoteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadingacci")){
		    sscanf(value,"%s",buf);
			concfg.notereadingacci=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadingchord")){
		    sscanf(value,"%s",buf);
			concfg.notereadingchord=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadingdeep")){
		    sscanf(value,"%s",buf);
			concfg.notereadingdeep=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadinglead")){
		    sscanf(value,"%s",buf);
			concfg.notereadinglead=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadingfiguras")){
		    sscanf(value,"%s",buf);
			concfg.notereadingfiguras=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadingfingering")){
		    sscanf(value,"%s",buf);
			concfg.notereadingfingering=atoi(buf);
			continue;
		}
		if(!stricmp(var,"randommusicacci")){
		    sscanf(value,"%s",buf);
			concfg.randommusicacci=atoi(buf);
			continue;
		}
		if(!stricmp(var,"singoctava")){
		    sscanf(value,"%s",buf);
			concfg.singoctava=atoi(buf);
			continue;
		}
		if(!stricmp(var,"earoctava")){
		    sscanf(value,"%s",buf);
			concfg.earoctava=atoi(buf);
			continue;
		}
		if(!stricmp(var,"oidoabrel")){
		    sscanf(value,"%s",buf);
			concfg.oidoabrel=atoi(buf);
			continue;
		}
		if(!stricmp(var,"tiposolfeo")){
		    sscanf(value,"%s",buf);
			concfg.tiposolfeo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"stoponerror")){
		    sscanf(value,"%s",buf);
			concfg.stoponerror=atoi(buf);
			continue;
		}
		if(!stricmp(var,"memoespacial")){
		    sscanf(value,"%s",buf);
			concfg.memoespacial=atoi(buf);
			continue;
		}
		if(!stricmp(var,"maxvariacion")){
		    sscanf(value,"%s",buf);
			concfg.maxvariacion=atoi(buf);
			continue;
		}
		if(!stricmp(var,"armadura")){
            if(melodia!=NULL) melodia->armadura=atoi(value);
			sscanf(value,"%s",concfg.armadura);
			continue;
		}
		if(!stricmp(var,"compases")){
            if(melodia!=NULL) melodia->numcompases=atoi(value);
			sscanf(value,"%s",concfg.numcompases);
			continue;
		}
		if(!stricmp(var,"fingering")){
            if(melodia!=NULL) melodia->fingering=atoi(value);
		    sscanf(value,"%s",buf);
			concfg.fingering=atoi(buf);
			continue;
		}

		if(!stricmp(var,"silencios")){
            if(melodia!=NULL) melodia->silencios=atoi(value);
		    sscanf(value,"%s",buf);
			concfg.silencios=atoi(buf);
			continue;
		}
		if(!stricmp(var,"followme")){
		    sscanf(value,"%s",buf);
			concfg.followme=atoi(buf);
			continue;
		}
		if(!stricmp(var,"sigueme")){
		    sscanf(value,"%s",buf);
			concfg.sigueme=atoi(buf);
			continue;
		}
		if(!stricmp(var,"playmode")){
		    sscanf(value,"%s",buf);
			concfg.playmode=atoi(buf);
			continue;
		}
		if(!stricmp(var,"randommusicqosritmo")){
		    sscanf(value,"%s",buf);
			concfg.randommusicqosritmo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadqosritmo")){
		    sscanf(value,"%s",buf);
			concfg.notereadqosritmo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"randommusicqoslegato")){
		    sscanf(value,"%s",buf);
			concfg.randommusicqoslegato=atoi(buf);
			continue;
		}
		if(!stricmp(var,"notereadqoslegato")){
		    sscanf(value,"%s",buf);
			concfg.notereadqoslegato=atoi(buf);
			continue;
		}
		if(!stricmp(var,"randommusicqoscompas")){
		    sscanf(value,"%s",buf);
			concfg.randommusicqoscompas=atoi(buf);
			continue;
		}
		if(!stricmp(var,"accuracynotes")){
		    sscanf(value,"%s",buf);
			concfg.accuracynotes=atoi(buf);
			continue;
		}
		if(!stricmp(var,"accuracytempo")){
		    sscanf(value,"%s",buf);
			concfg.accuracytempo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"repsierrritmo")){
		    sscanf(value,"%s",buf);
			concfg.repsierrritmo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"bloquesritmicosG")){			
			j=0;
			sscanf(value,"%s",concfg.bloquesritmicosg);
			if(melodia!=NULL){
			   token=strtok(value,",");
			   while(token!=NULL){         
			      strcpy(melodia->ritmo.bloqueritmico[0].bloques[j++],token);			   
			      token=strtok(NULL,",");
			   }
			   melodia->ritmo.bloqueritmico[0].numbloques=j;
			}
			continue;
		}
		if(!stricmp(var,"bloquesritmicosF")){			
			j=0;
			sscanf(value,"%s",concfg.bloquesritmicosf);
			if(melodia!=NULL){
			   token=strtok(value,",");
			   while(token!=NULL){         
			      strcpy(melodia->ritmo.bloqueritmico[1].bloques[j++],token);			   
			      token=strtok(NULL,",");
			   }			
			   melodia->ritmo.bloqueritmico[1].numbloques=j;
			}
			continue;
		}
		if(!stricmp(var,"repeatG")){
			sscanf(value,"%s",buf);
			concfg.repeatg=atoi(buf);
			if(melodia!=NULL) melodia->repeatg=concfg.repeatg;			
			continue;
		}
		if(!stricmp(var,"repeatF")){
			sscanf(value,"%s",buf);
			concfg.repeatf=atoi(buf);
			if(melodia!=NULL) melodia->repeatf=concfg.repeatf;
			continue;
		}
		if(!stricmp(var,"InDevice")){
			sscanf(value,"%s",buf);
			concfg.iInDevice=atoi(buf);
			continue;
		}
		if(!stricmp(var,"OutDevice")){
			sscanf(value,"%s",buf);
			concfg.iOutDevice=atoi(buf);
			continue;
		}
		if(!stricmp(var,"Velocity")){
			sscanf(value,"%s",buf);
			concfg.DefaultVelocity=atoi(buf);
			continue;
		}
		if(!stricmp(var,"Metronomo")){
			if(melodia!=NULL) melodia->metronomo=atoi(value);
			sscanf(value,"%s",buf);
			concfg.metronomo=atoi(buf);
			continue;
		}		
		if(!stricmp(var,"Ritmo")){
			sscanf(value,"%s",buf);
			concfg.ritmo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"soloritmo")){
			sscanf(value,"%s",buf);
			concfg.soloritmo=atoi(buf);
			continue;
		}
		if(!stricmp(var,"RandomModeGenerationMusic")){
			sscanf(value,"%s",buf);
			concfg.randommodegenerationmusic=atoi(buf);
			if(melodia!=NULL) melodia->randommodegenerationmusic=concfg.randommodegenerationmusic;
			continue;
		}
		if(!stricmp(var,"ocultanota")){
			sscanf(value,"%s",buf);
			concfg.ocultanota=atoi(buf);
			continue;
		}
		if(!stricmp(var,"compascreen")){
			sscanf(value,"%s",buf);
			concfg.maxcompascreen=atoi(buf);
			BARTOBARSPACE=(NumOfStaffs-OFFSETPENTA)/concfg.maxcompascreen;  //42 jordi
			if(concfg.maxcompascreen==1) 
				BARTOBARSPACE=(NumOfStaffs-OFFSETPENTA)/2; //si un compas en pantalla lo muestra hasta la mitad de la pantalla
			continue;
		}
		if(!stricmp(var,"armaduranotereading")){
			sscanf(value,"%s",buf);
			concfg.armaduranotereading=atoi(buf);
			continue;
		}
        if(!stricmp(var,"armaduranoteinterval")){
			sscanf(value,"%s",buf);
			concfg.armaduranoteinterval=atoi(buf);
			continue;
		}
		if(!stricmp(var,"midfile")){
			int r=0;
			while(value[r]!=0){
			   if(value[r]==10) value[r]=0;
			   ++r;
			}
			strcpy(concfg.midfile,value);            
			continue;
		}
		if(!stricmp(var,"xmlfile")){
			int r=0;
			while(value[r]!=0){
			   if(value[r]==10) value[r]=0;
			   ++r;
			}
			strcpy(concfg.xmlfile,value);            
			continue;
		}
	 }
	 fclose(fp);
	 // limite de notas con que trabaja el programas, 0 en clave de SOL y 1 en clave de FA
	 // los valores numericos son los codigos de exploracion del teclado MIDI
	 // mirar array no[29]
	 // concfg.noteinf[0]=57; concfg.notesup[0]=88; // de momento el SPAn es FIJO
	 // concfg.noteinf[1]=35; concfg.notesup[1]=64; 
	 concfg.noteinfgeneric[0]=57;     concfg.notesupgeneric[0]=85; // de momento el SPAn es FIJO
	 concfg.noteinfgeneric[1]=36;     concfg.notesupgeneric[1]=64;
	 concfg.noteinfpaintkey[0]=57;    concfg.notesuppaintkey[0]=85; // de momento el SPAn es FIJO
	 concfg.noteinfpaintkey[1]=36;    concfg.notesuppaintkey[1]=64;
	 concfg.noteinfrandommusic[0]=50; concfg.notesuprandommusic[0]=100;
	 concfg.noteinfrandommusic[1]=12; concfg.notesuprandommusic[1]=64;
	 if(concfg.tiposolfeo==1) concfg.singoctava=1; // si solfeo hablado se mapea a una octava
	 if(melodia!=NULL){ 
	    melodia->noteinf[0]=55; melodia->notesup[0]=85; // de momento el SPAn es FIJO antes 57 en lugar de 55
	    melodia->noteinf[1]=36; melodia->notesup[1]=64;
		melodia->penta[0].compas=(COMPAS *) new COMPAS[melodia->numcompases];
        melodia->penta[1].compas=(COMPAS *) new COMPAS[melodia->numcompases];
		melodia->penta[0].allsilencio=0;
		melodia->penta[1].allsilencio=0;
		melodia->puneje.eje=NULL;
	 }
	 return;
}

int interval(NOTA *n1,NOTA *n2)
{
   	int inter;
   	int dist0,dist1;
    	
   	dist0=calcnoteindex(n1);
   	dist1=calcnoteindex(n2);
   	inter=dist0-dist1;
   	inter=abs(inter)+1;
   	return inter;
}
// intervalo esta en semitonos
NOTA addinterval(NOTA *nta,int interval,int mi,int ma)
{
	NOTA nota;
	int d0,d1,sin,v;

	d0=nta->notakey;
	sin=(int)(uniform_deviate(rand()) * 2000);
    if(sin<1000) d1=d0+interval;
	else d1=d0-interval;
	if(d1<mi) d1=d0+interval;
	if(d1>ma) d1=d0-interval;
    nota.notakey=d1;
	v=calcnoteindex(d1);
	if(v==-1){ --d1; nota.nota=d1; nota.acci=1;}
	else{ nota.nota=no[v]; nota.acci=0; }
	if(concfg.noteinfgeneric[1]<=d1 && d1<=concfg.notesupgeneric[1]) nota.penta=1;
    else nota.penta=0;
    return nota;
}


int semitonos(int nota,int acci)
{
	int s;

	switch(nota){
	   case 0: s=0; break;
	   case 1: s=2; break;
	   case 2: s=4; break;
	   case 3: s=5; break;
	   case 4: s=7; break;
	   case 5: s=9; break;
	   case 6: s=11; break;
	}
	s+=acci;
	return s;
}

int randomnotenatural(int inf,int sup)
{
    int j;

	while(1){
       j=(int)(uniform_deviate(rand()) * 29);
	   if(no[j]>=inf && no[j]<=sup) return no[j];
	}
}

NOTA *genrndnote(MELODIA *melodia,int penta,double duracion)
{
    int j,silen;
	static NOTA h;
   
	h.penta=j=penta;
    h.nota = randomnotenatural(melodia->noteinf[j],melodia->notesup[j]); 
    h.acci = (int)(uniform_deviate(rand()) * 2000);//for Notes 36 thru 84 (from Julienne Walker's Eternally Confuzzled website)		  
	if(h.acci<100) h.acci=1;
	if(h.acci>=100 && h.acci<200) h.acci=2;
	if(h.acci>=200) h.acci=0;
	h.notakey=-1; // no asignamos notekey, lo hacemos mas tarde con la armadura
	h.color=0;
    h.dura = duracion;
	if(melodia->silencios==1){
       silen=(int)(uniform_deviate(rand()) * 2000);
	   if(silen<50) h.nota=h.acci=h.notakey=0;
	}
	return &h;
}

NOTA *genmarkovnote(MELODIA *melodia,int penta,double duracion,char **res)
{
    int j,acci;
	static NOTA h;
	static int mindex[2];
   
	if(res==NULL){
	   mindex[0]=mindex[1]=0;
	   return NULL;
	}
	h.penta=j=penta;
    h.nota=res[penta][mindex[penta]++];
	if(h.nota<0)
		h.nota=res[penta][mindex[penta]++]&127;; // se trata de una nota formando parte de un acorde, la codificamos nota|=128 en origen
    h.acci = (int)(uniform_deviate(rand()) * 2000);//for Notes 36 thru 84 (from Julienne Walker's Eternally Confuzzled website)		  
	if(h.acci<100) h.acci=1;
	if(h.acci>=100 && h.acci<200) h.acci=2;
	if(h.acci>=200) h.acci=0;
    acci=h.acci;
	acci=3-2*h.acci; if(acci==3) acci=0;
	h.notakey=h.nota+acci;
	h.color=0;
    h.dura = duracion;
	return &h;
}

NOTA *genrndnotestadistical(HITSPROB *hp,int can)
{
	static NOTA h;
	double prob;
	int r,i,j,v,acci,den,fig;
	int tot=0;

    fig=(int)(uniform_deviate(rand())*5);
	for(r=0;r<can;r++){
	   for(j=0;j<3;j++){
          den=hp[r].numhits[j]+hp[r].numfails[j];
		  if(den>=5) prob=hp[r].numhits[j]*100.0/den;
		  else prob=0;
          hp[r].masa[j]=-0.95*prob+100;
	      tot+=hp[r].masa[j];
	   }
	}
    i=(int)(uniform_deviate(rand()) * tot)+1;    
	for(r=v=0;r<can;r++){
	   for(j=0;j<3;j++){
	      v+=hp[r].masa[j];
	      if(v>=i) break;
	   }
	   if(v>=i) break;
	} 
	h.nota=hp[r].note;
	h.penta=hp[r].penta;
	acci=h.acci=j;
	acci=3-2*h.acci; if(acci==3) acci=0;
    h.notakey=h.nota+acci;
    h.color=0;
	h.dura=fig;
    return &h;
}

NOTA *genrndnote(void)
{
    int j,acci,fig;
	static NOTA h;

	j=(int)(uniform_deviate(rand())*2);
	fig=(int)(uniform_deviate(rand())*5);
	h.penta=j;
    h.nota = randomnotenatural(concfg.noteinfgeneric[j],concfg.notesupgeneric[j]); 
    h.acci = (int)(uniform_deviate(rand()) * 3000);//for Notes 36 thru 84 (from Julienne Walker's Eternally Confuzzled website)		  
	if(h.acci<1000) h.acci=1;
	if(h.acci>=1000 && h.acci<2000) h.acci=2;
	if(h.acci>=2000) h.acci=0;
	if(concfg.notereadingacci==0) h.acci=0;		  
	acci=h.acci;
	acci=3-2*h.acci; if(acci==3) acci=0;
    h.notakey=h.nota+armadura(h.nota,acci,concfg.armaduranotereading);
	h.color=0;
    h.dura = fig;
	return &h;
}

NOTA *genrndfingernote(NOTA *listanotas,int can)
{
	static NOTA h;
    static int count;

	if(count==1024){
	   count=0;
	   initializefingering(listanotas,can);
	}
	h=listanotas[count++];
	return &h;
}

CHORD getchord(int ChordType,int Inversion,int root,int acciroot,int penta,int &mn,int &mx)
{
	int Note[10],r,can,nt,v,j,a,acci;
	CHORD ch;

	ch.nota=ch.acci=ch.notakey=0;
	mn=10000; mx=-mn;
	Note[0]=root;
	if (ChordType == 1)//Major Triad
	{
	   Note[1] = root + 4;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 7;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   can=3;
	}
	if(ChordType == 2)//Minor Triad
	{
	   Note[1] = root + 3;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 7;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   can=3;
	}
	if(ChordType == 3)//Diminished Triad
	{
	   Note[1] = root + 3;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 6;
	   if ((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   can=3;
	}
	if(ChordType == 4)//Augmented Triad
	{
	   Note[1] = root + 4;
	   if(Inversion == 1) Note[1] -= 12;
 	   Note[2] = root + 8;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   can=3;
	}
	if(ChordType == 5)//Dominant 7th
	{
	   Note[1] = root + 4;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 7;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   Note[3] = root + 10;
	   if((Inversion == 1) || (Inversion == 2) || (Inversion == 3)) Note[3] -= 12;
	   can=4;
	}
	if(ChordType == 6)//Major 7th
	{
	   Note[1] = root + 4;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 7;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   Note[3]= root + 11;
	   if((Inversion == 1) || (Inversion == 2) || (Inversion == 3)) Note[3] -= 12;
	   can=4;
	}
	if(ChordType == 7)//Minor 7th
	{
	   Note[1] = root + 3;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 7;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   Note[3] = root + 10;
	   if((Inversion == 1) || (Inversion == 2) || (Inversion == 3)) Note[3] -= 12;
	   can=4;
	}
    if(ChordType == 8)//Diminished 7th
	{
	   Note[1] = root + 3;
	   if(Inversion == 1) Note[1] -= 12;
	   Note[2] = root + 6;
	   if((Inversion == 1) || (Inversion == 2)) Note[2] -= 12;
	   Note[3] = root + 9;
	   if((Inversion == 1) || (Inversion == 2) || (Inversion == 3)) Note[3] -= 12;
	   can=4;
	}
	for(r=0;r<can;r++){
	   nt=Note[r]; 
	   for(j=0;j<44;j++){
	      for(a=0;a<3;a++){
			 if(acciroot==1 && a==2) continue;
		     if(acciroot==2 && a==1) continue;
			 if(acciroot==0 && a==1) continue;
		     acci=3-2*a; 
			 if(acci==3) acci=0;
	         v=nt-armadura(no[j],acci,concfg.armaduranotereading);
			 if(v==no[j]) break; 
		  }
		  if(v==no[j]) break; 
	   }
	   if(v<=mn) mn=v;
	   if(v>mx) mx=v;
       ch.nota<<=8;
	   ch.nota|=v;
	   ch.acci<<=8;
	   ch.acci|=a;
	   ch.notakey<<=8;
	   ch.notakey|=nt;
	   ch.color=0;
	   ch.compas=0;
	   ch.dura=1;
	   ch.penta=penta;
	}   	   
	return ch;
}

CHORD *genrndchord(void)
{
	NOTA *h;
	int type,inver,mn,mx,fig;
    static CHORD ch;
	CHORD tp;
	
	while(1){
	   type=(int)(uniform_deviate(rand())*7)+1;
	   inver=(int)(uniform_deviate(rand())*3);
	   // si *4 permitimos hasta la corchea, si *5 hasta la semicorchea
	   fig=(int)(uniform_deviate(rand())*4);	
       h=genrndnote();
       ch=getchord(type,inver,h->notakey,h->acci,h->penta,mn,mx); 
	   tp=ch;
	   if(h->penta==0){
          if(mn<concfg.noteinfgeneric[0] || mn>concfg.notesupgeneric[0]) continue;
          if(mx<concfg.noteinfgeneric[0] || mx>concfg.notesupgeneric[0]) continue;
	   }else{
          if(mn<concfg.noteinfgeneric[1] || mn>concfg.notesupgeneric[1]) continue;
		  if(mx<concfg.noteinfgeneric[1] || mx>concfg.notesupgeneric[1]) continue;
	   }
	   if(concfg.notereadingacci==0 && ch.acci!=0) continue;	   
	   break;
   	}
	ch.dura=fig;
	return &ch;
}

double *getduraciones(char *ap)
{
   int r;
   char *token,value[256];
   double tmp,*result;

   r=0;
   strcpy(value,ap);
   result=(double *) new double[256];
   token=strtok(value,"+");
   while(token!=NULL){         
      tmp=1.0/(token[0]-0x30);
      if(token[1]=='.') tmp+=tmp/2.0;
      result[r++]=tmp;
	  token=strtok(NULL,"+");
   }
   result[r]=-1;
   return result;
}

double calcduracion(char *ap)
{
   double longitud,tmp;
   char *token,value[256];

   longitud=0;
   strcpy(value,ap);
   token=strtok(value,"+");
   while(token!=NULL){         
      tmp=1.0/(token[0]-0x30);
      if(token[1]=='.') tmp+=tmp/2.0;
      longitud+=tmp;
      token=strtok(NULL,"+");
   }
   return longitud;
}

void arrayadd(int *ap,int value)
{
	int r;
    
	for(r=0;ap[r]>=0;r++);
	ap[r++]=value;
	ap[r++]=-1;
}

void arrayremove(int *ap,int index)
{
	int r;

	for(r=index;ap[r]>=0;r++)
	   ap[r]=ap[r+1];
	return;
}

int arraysize(int *ap)
{
	int r;

    for(r=0;ap[r]>=0;r++);
	return r;
}

double generarBloquesRitmicos(MELODIA *melodia,int *bloques,int penta,double longitudActual, double longitudObjetivo) 
{
	RITMO *rit;
	double tmp;
	char *ap;
	int i,idxBloque,bloque;
    int *bloquesCandidatos,numbloques;

	rit=&melodia->ritmo;
    numbloques=rit->bloqueritmico[penta].numbloques;
	bloquesCandidatos = (int *) new int[numbloques+1];
	for (i=0;i<numbloques;i++) bloquesCandidatos[i]=i;
    bloquesCandidatos[i]=-1;
    i=0;
	while(i<arraysize(bloquesCandidatos)) {
	   ap=rit->bloqueritmico[penta].bloques[i];
	   tmp=calcduracion(ap);
	   if(tmp>(longitudObjetivo-longitudActual))
	      arrayremove(bloquesCandidatos,i);
	   else
		  i++;
	}
	while(longitudActual < longitudObjetivo && arraysize(bloquesCandidatos) > 0) {
	   idxBloque=(int)(uniform_deviate(rand()) * arraysize(bloquesCandidatos));				
	   bloque=bloquesCandidatos[idxBloque];
	   arrayadd(bloques,bloque);
	   ap=rit->bloqueritmico[penta].bloques[bloque];
	   tmp=calcduracion(ap);
	   longitudActual+=tmp;
	   if(longitudActual<longitudObjetivo){
	      longitudActual=generarBloquesRitmicos(melodia,bloques,penta,longitudActual,longitudObjetivo);
		  if(longitudActual<longitudObjetivo){
		     arrayremove(bloquesCandidatos,idxBloque);
			 arrayremove(bloques,arraysize(bloques)-1);
             ap=rit->bloqueritmico[penta].bloques[bloque];
			 tmp=calcduracion(ap);
			 longitudActual-=tmp; 
		  }
	   }
	}
	delete [] bloquesCandidatos;
	return longitudActual;
}
    		    
int generarCompas(MELODIA *melodia,int penta,int compas)
{			
   char *apbloq;
   double longitudcompas,*duraciones,longitud;
   int *listabloques,j,s,dist,count,csilen,acci;
   NOTA *nota;
   static NOTA *Noteant;
   static int iniciof[2];
   
   if(compas==-1){ 
	   iniciof[0]=iniciof[1]=0; 
	   if(Noteant!=NULL) delete [] Noteant;
	   Noteant=(NOTA *) new NOTA[2];
	   return 1; 
   }
   longitudcompas=melodia->numsignature*1.0/melodia->densignature;	   
   listabloques=(int *) new int[1024];
   listabloques[0]=-1;
   longitud=0;
   longitud=generarBloquesRitmicos(melodia,listabloques,penta,longitud,longitudcompas); 
   if(longitud<longitudcompas){
		delete [] listabloques;
		return -1;
   }
   for(j=csilen=count=0;listabloques[j]!=-1;j++){      
      apbloq=melodia->ritmo.bloqueritmico[penta].bloques[listabloques[j]];
      duraciones=getduraciones(apbloq);
	  for(s=0;duraciones[s]>0;s++){
		 while(1){													
			nota=genrndnote(melodia,penta,duraciones[s]);					
			if(nota->nota==0) break;      // si es silencio sal y solo anota valores
		    if(iniciof[penta]==0){
			   iniciof[penta]=1;
   			   break;
			}		
			dist=interval(nota,&Noteant[penta]);
			if(dist>=melodia->penta[penta].minintervalo && 
			   dist<=melodia->penta[penta].maxintervalo) break;
		 }
	  	 if(nota->nota!=0) Noteant[penta]=*nota;   // si es silencio no la trates como nota anterior
		 else ++csilen;
		 if(concfg.randommusicacci==0) nota->acci=0; // si 0 no hay accidentes
	     acci=nota->acci;
	     acci=3-2*acci; if(acci==3) acci=0;
		 melodia->penta[penta].compas[compas].note[count]=nota->nota;
		 melodia->penta[penta].compas[compas].clef[count]=penta;
		 melodia->penta[penta].compas[compas].tupla[count]=1;
		 melodia->penta[penta].compas[compas].tie[count]=0;
		 melodia->penta[penta].compas[compas].notekey[count]=nota->nota+armadura(nota->nota,acci,melodia->armadura);
		 melodia->penta[penta].compas[compas].acci[count]=nota->acci;
	     melodia->penta[penta].compas[compas].dura[count]=nota->dura;
		 melodia->penta[penta].compas[compas].timenota[count].time=-1;
		 melodia->penta[penta].compas[compas].timenota[count].valid=0;
		 melodia->penta[penta].compas[compas].errormarca[count]=-1;
         melodia->penta[penta].compas[compas].fingering[count]=-1;
		 melodia->penta[penta].compas[compas].color[count++]=0;		          
	  }
	  delete [] duraciones;
   }
   melodia->penta[penta].compas[compas].numnotas=count;
   melodia->penta[penta].compas[compas].numsilencios=csilen;
   melodia->penta[penta].compas[compas].seqcompas=compas;
   melodia->penta[penta].compas[compas].timecompas.time=-1;
   melodia->penta[penta].compas[compas].timecompas.valid=0;
   melodia->penta[penta].compas[compas].armadura=melodia->armadura;
   melodia->penta[penta].compas[compas].numsignature=melodia->numsignature;
   melodia->penta[penta].compas[compas].densignature=melodia->densignature;
   melodia->penta[penta].compas[compas].std.durcompas=0;
   melodia->penta[penta].compas[compas].std.durcompasqos=0;
   melodia->penta[penta].compas[compas].std.notasfast=0;
   melodia->penta[penta].compas[compas].std.notasjust=0;
   melodia->penta[penta].compas[compas].std.notasslow=0;
   melodia->penta[penta].compas[compas].std.notasfastqos=0;
   melodia->penta[penta].compas[compas].std.notasjustqos=0;
   melodia->penta[penta].compas[compas].std.notasslowqos=0;
   melodia->penta[penta].compas[compas].std.durnotastotal=0;
   melodia->penta[penta].compas[compas].std.ritmic=0;
   delete [] listabloques;
   return 1;
}	


int generarCompasmarkov(MELODIA *melodia,int penta,int compas,char **res)
{			
   char *apbloq;
   double longitudcompas,*duraciones,longitud;
   int *listabloques,j,s,count,csilen,acci;
   NOTA *nota;
   
   if(compas==-1){ 
	   genmarkovnote(melodia,0,0,NULL);					
	   return 1; 
   }
   longitudcompas=melodia->numsignature*1.0/melodia->densignature;	   
   listabloques=(int *) new int[1024];
   listabloques[0]=-1;
   longitud=0;
   longitud=generarBloquesRitmicos(melodia,listabloques,penta,longitud,longitudcompas); 
   if(longitud<longitudcompas){
		delete [] listabloques;
		return -1;
   }
   for(j=csilen=count=0;listabloques[j]!=-1;j++){      
      apbloq=melodia->ritmo.bloqueritmico[penta].bloques[listabloques[j]];
      duraciones=getduraciones(apbloq);
	  for(s=0;duraciones[s]>0;s++){
		 nota=genmarkovnote(melodia,penta,duraciones[s],res);					
	  	 if(nota->nota==0) ++csilen;
		 if(concfg.randommusicacci==0) nota->acci=0; // si 0 no hay accidentes
	     acci=nota->acci;
	     acci=3-2*acci; if(acci==3) acci=0;
		 melodia->penta[penta].compas[compas].note[count]=nota->nota;
		 melodia->penta[penta].compas[compas].clef[count]=penta;
		 melodia->penta[penta].compas[compas].tupla[count]=1;
		 melodia->penta[penta].compas[compas].tie[count]=0;
		 melodia->penta[penta].compas[compas].notekey[count]=nota->nota+armadura(nota->nota,acci,melodia->armadura);
		 melodia->penta[penta].compas[compas].acci[count]=nota->acci;
	     melodia->penta[penta].compas[compas].dura[count]=nota->dura;
		 melodia->penta[penta].compas[compas].timenota[count].time=-1;
		 melodia->penta[penta].compas[compas].timenota[count].valid=0;
		 melodia->penta[penta].compas[compas].errormarca[count]=-1;
         melodia->penta[penta].compas[compas].fingering[count]=-1;
		 melodia->penta[penta].compas[compas].color[count++]=0;		          
	  }
	  delete [] duraciones;
   }
   melodia->penta[penta].compas[compas].numnotas=count;
   melodia->penta[penta].compas[compas].numsilencios=csilen;
   melodia->penta[penta].compas[compas].seqcompas=compas;
   melodia->penta[penta].compas[compas].timecompas.time=-1;
   melodia->penta[penta].compas[compas].timecompas.valid=0;
   melodia->penta[penta].compas[compas].armadura=melodia->armadura;
   melodia->penta[penta].compas[compas].numsignature=melodia->numsignature;
   melodia->penta[penta].compas[compas].densignature=melodia->densignature;
   melodia->penta[penta].compas[compas].std.durcompas=0;
   melodia->penta[penta].compas[compas].std.durcompasqos=0;
   melodia->penta[penta].compas[compas].std.notasfast=0;
   melodia->penta[penta].compas[compas].std.notasjust=0;
   melodia->penta[penta].compas[compas].std.notasslow=0;
   melodia->penta[penta].compas[compas].std.notasfastqos=0;
   melodia->penta[penta].compas[compas].std.notasjustqos=0;
   melodia->penta[penta].compas[compas].std.notasslowqos=0;
   melodia->penta[penta].compas[compas].std.durnotastotal=0;
   melodia->penta[penta].compas[compas].std.ritmic=0;
   delete [] listabloques;
   return 1;
}	


int *generabloque(MELODIA *melodia,int penta)
{
   double longitudcompas,longitud;
   int *listabloques;

   longitudcompas=melodia->numsignature*1.0/melodia->densignature;	   
   listabloques=(int *) new int[1024];
   listabloques[0]=-1;
   longitud=0;
   longitud=generarBloquesRitmicos(melodia,listabloques,penta,longitud,longitudcompas); 
   if(longitud<longitudcompas){      	
		delete [] listabloques;
		return NULL;
   }
   return listabloques;
}

void generarCompasProgressive(MELODIA *melodia,int penta,int compas,int *listabloques,int maxvariacion)
{			
   char *apbloq;
   double *duraciones;
   int j,s,dist,count,csilen,acci,h,vari,maxiter;
   NOTA *nota;
   static NOTA *Noteant,notadum;
   static int iniciof[2];
   
   if(compas==-1){ 
	   iniciof[0]=iniciof[1]=0; 
	   if(Noteant!=NULL) delete [] Noteant;
	   Noteant=(NOTA *) new NOTA[2];
	   return; 
   }   
   for(j=csilen=count=h=0;listabloques[j]!=-1;j++){      
      apbloq=melodia->ritmo.bloqueritmico[penta].bloques[listabloques[j]];
      duraciones=getduraciones(apbloq);
	  maxiter=0;
      for(s=0;duraciones[s]>0;s++){
		 while(1){								
            vari=(int)(uniform_deviate(rand())*2);
			if((vari==1 && h<maxvariacion) || maxvariacion==0){
				nota=genrndnote(melodia,penta,duraciones[s]);					
                if(compas!=0){
	               acci=nota->acci;
	               acci=3-2*acci; if(acci==3) acci=0;
				   if(nota->nota+armadura(nota->nota,acci,melodia->armadura)==melodia->penta[penta].compas[compas-1].notekey[count])
				      continue;
				}
			}else{
			   nota=&notadum;
		       nota->nota=melodia->penta[penta].compas[compas-1].note[count];		       
		       nota->acci=melodia->penta[penta].compas[compas-1].acci[count];
	           nota->dura=melodia->penta[penta].compas[compas-1].dura[count];		       
			}				
			if(nota->nota==0) break;      // si es silencio sal y solo anota valores
		    if(iniciof[penta]==0){
			   iniciof[penta]=1;
   			   break;
			}		
			dist=interval(nota,&Noteant[penta]);
			if(dist>=melodia->penta[penta].minintervalo && 
			   dist<=melodia->penta[penta].maxintervalo) break;
			++maxiter;
			if(maxiter>100){ 
			   --h; maxiter=0; 
			}
		 }
		 if(vari==1) ++h;
	  	 if(nota->nota!=0) Noteant[penta]=*nota;   // si es silencio no la trates como nota anterior
		 else ++csilen;
		 if(concfg.randommusicacci==0) nota->acci=0; // si 0 no hay accidentes
	     acci=nota->acci;
	     acci=3-2*acci; if(acci==3) acci=0;
		 melodia->penta[penta].compas[compas].note[count]=nota->nota;
		 melodia->penta[penta].compas[compas].clef[count]=penta;
		 melodia->penta[penta].compas[compas].tupla[count]=1;
		 melodia->penta[penta].compas[compas].tie[count]=0;
		 melodia->penta[penta].compas[compas].notekey[count]=nota->nota+armadura(nota->nota,acci,melodia->armadura);
		 melodia->penta[penta].compas[compas].acci[count]=nota->acci;
	     melodia->penta[penta].compas[compas].dura[count]=nota->dura;
		 melodia->penta[penta].compas[compas].timenota[count].time=-1;
		 melodia->penta[penta].compas[compas].timenota[count].valid=0;
		 melodia->penta[penta].compas[compas].errormarca[count]=-1;
         melodia->penta[penta].compas[compas].fingering[count]=-1;
		 melodia->penta[penta].compas[compas].color[count++]=0;		          
	  }
	  delete [] duraciones;
   }
   melodia->penta[penta].compas[compas].numnotas=count;
   melodia->penta[penta].compas[compas].numsilencios=csilen;
   melodia->penta[penta].compas[compas].seqcompas=compas;
   melodia->penta[penta].compas[compas].timecompas.time=-1;
   melodia->penta[penta].compas[compas].timecompas.valid=0;
   melodia->penta[penta].compas[compas].armadura=melodia->armadura;
   melodia->penta[penta].compas[compas].numsignature=melodia->numsignature;
   melodia->penta[penta].compas[compas].densignature=melodia->densignature;
   melodia->penta[penta].compas[compas].std.durcompas=0;
   melodia->penta[penta].compas[compas].std.durcompasqos=0;
   melodia->penta[penta].compas[compas].std.notasfast=0;
   melodia->penta[penta].compas[compas].std.notasjust=0;
   melodia->penta[penta].compas[compas].std.notasslow=0;
   melodia->penta[penta].compas[compas].std.notasfastqos=0;
   melodia->penta[penta].compas[compas].std.notasjustqos=0;
   melodia->penta[penta].compas[compas].std.notasslowqos=0;
   melodia->penta[penta].compas[compas].std.durnotastotal=0;
   melodia->penta[penta].compas[compas].std.ritmic=0;
   return;
}	

int generarmelodia(MELODIA *&melodia)
{
   int i,j,sel;
   EJECUCION *eje;
   int *listabl[2];
   
   sel=melodia->randommodegenerationmusic;
   if(sel==0){      // total aleatoria
      generarCompas(melodia,0,-1);
      for(i=0;i<melodia->numcompases;i++){
         for(j=0;j<2;j++)
            if(generarCompas(melodia,j,i)==-1) return -1;
	     eje=generaejecucion(melodia->penta,i);
         if(verificaejecucion(eje)==0) 
	        --i;
	     delete eje;
	  }
	  aplicarepeticiones(melodia);   // solo aplicamos repeticiones en fullrandom
   }
   if(sel==1){           // progresiva
      generarCompasProgressive(melodia,0,-1,NULL,0);
	  listabl[0]=generabloque(melodia,0);
	  listabl[1]=generabloque(melodia,1);
	  if(listabl[0]==NULL || listabl[1]==NULL) return -1;
      for(i=0;i<melodia->numcompases;i++){
		 for(j=0;j<2;j++){
            if(i==0) generarCompasProgressive(melodia,j,i,listabl[j],0);
            else{ 
			   generarCompasProgressive(melodia,j,i,listabl[j],concfg.maxvariacion);
		       if(verificanumcambios(melodia,j,i)!=concfg.maxvariacion){ 
			      --j; continue; 
			   }
			}
		 }
		 eje=generaejecucion(melodia->penta,i);
         if(verificaejecucion(eje)==0) 
	        --i;
	     delete eje;
	  }
	  delete [] listabl[0];
	  delete [] listabl[1];
   }
   if(sel==2){      // markov beyer
	  char *res[2];

	  res[0]= (char *) new char[melodia->numcompases*32+1];
	  res[1]=(char *) new char[melodia->numcompases*32+1];
      fmarkov("beyer0.mrk",7,melodia->numcompases*32,res[0]);
	  fmarkov("beyer1.mrk",7,melodia->numcompases*32,res[1]);
      generarCompasmarkov(melodia,0,-1,NULL);
      for(i=0;i<melodia->numcompases;i++){
         for(j=0;j<2;j++)
            if(generarCompasmarkov(melodia,j,i,res)==-1) return -1;
	     eje=generaejecucion(melodia->penta,i);
         if(verificaejecucion(eje)==0) 
	        --i;
	     delete eje;		 
	  }
	  delete [] res[0];
	  delete [] res[1];
   }
   if(sel==3){      // markov bach
	  char *res[2];

	  res[0]= (char *) new char[melodia->numcompases*32+1];
	  res[1]=(char *) new char[melodia->numcompases*32+1];
      fmarkov("bach0.mrk",4,melodia->numcompases*32,res[0]);
	  fmarkov("bach1.mrk",4,melodia->numcompases*32,res[1]);
      generarCompasmarkov(melodia,0,-1,NULL);
      for(i=0;i<melodia->numcompases;i++){
         for(j=0;j<2;j++)
            if(generarCompasmarkov(melodia,j,i,res)==-1) return -1;
	     eje=generaejecucion(melodia->penta,i);
         if(verificaejecucion(eje)==0) 
	        --i;
	     delete eje;		 
	  }
	  delete [] res[0];
	  delete [] res[1];
   }
   // jordi en pruebas 8-1-2014, carga de fichero abc
   if(sel==4){
	  TCHAR buff[300];
	  int nua,nub,err;

	  strcpy(buff,currentdir);
	  strcat(buff,"\\g.abc");
	  err=loadabc(melodia,buff);
	  if(err>=-2 && err<0) return err;
	  if(err<=-3){
         err+=3; err=-err; // err contiene el compas erroneo
		 sprintf(buff,"El compas %d es erroneo en la partitura.\nQuizas la melodia cambie.\nSigo de todos modos ?\n",err);
		 if(MessageBox(hwnd,buff,ERROR, MB_YESNO)==IDNO) return -3;
	  }
	  sprintf(concfg.armadura,"%d",melodia->armadura);
	  nua=melodia->numsignature;
	  nub=melodia->densignature;
	  sprintf(concfg.compas,"%d/%d",nua,nub);
   }
   if(sel==5){
	  TCHAR buff[300];
	  int nua,nub,err;

	  err=loadmusicxml(melodia,concfg.xmlfile);
	  if(err>=-2 && err<0) return err;
	  if(err<=-3){
         err+=3; err=-err; // err contiene el compas erroneo
		 sprintf(buff,"El compas %d es erroneo en la partitura.\nQuizas la melodia cambie.\nSigo de todos modos ?\n",err);
		 if(MessageBox(hwnd,buff,ERROR, MB_YESNO)==IDNO) return -3;
	  }
	  sprintf(concfg.armadura,"%d",melodia->armadura);
	  nua=melodia->numsignature;
	  nub=melodia->densignature;
	  sprintf(concfg.compas,"%d/%d",nua,nub);
   }
   return 1;
}

int verificaejecucion(EJECUCION *eje)
{
	int r,s,can,nota[2];

	can=eje->penta[0].hor;
	for(r=0;r<can;r++){
	   for(s=0;s<2;s++) nota[s]=eje->penta[s].runnota[r].notakey;	   
	   if(nota[0]==nota[1]) 
	      return 0;
	}
	return 1;
}

int verificaejecucionmidi(EJECUCION *eje,int &pos)
{
	int r,s,can,nt,istie;
	__int64 nota[2],chord;

	can=eje->penta[0].hor; 
	for(r=0;r<can;r++){
	   for(s=0;s<2;s++) 
	      nota[s]=eje->penta[s].runnota[r].notakey;	   
	   if(nota[0]==0 || nota[1]==0) continue;
       chord=nota[0];
       for(;chord!=0;chord>>=8){
		  nt=(int) chord&0xff;
		  if(isinchord(nt,nota[1],0,istie)){ pos=r; return 0; }
	   }
	}
	return 1;  // no esta duplicado en los dos pentagramas
}

int verificanumcambios(MELODIA *melodia,int penta,int compas)
{
    int numnotas,r,can;

	numnotas=melodia->penta[penta].compas[compas].numnotas;
    for(r=can=0;r<numnotas;r++){
	   if(melodia->penta[penta].compas[compas].notekey[r]!=melodia->penta[penta].compas[compas-1].notekey[r])
		   ++can;
    }
	return can;
}

int verificacompas(MELODIA *melodia,int penta,int compas)
{    
	int j=penta,r,k,s,nmaxsemifusa;
    int numnotas,count,res,num,den;
	double tmp;

	int q;
	double rs,acc;

	nmaxsemifusa=64;
	num=melodia->penta[penta].compas[compas].numsignature;
	den=melodia->penta[penta].compas[compas].densignature;
	nmaxsemifusa=nmaxsemifusa*num*1.0/den;
    numnotas=melodia->penta[j].compas[compas].numnotas;
	for(r=acc=count=0;r<numnotas;r++){
       tmp=melodia->penta[j].compas[compas].dura[r];
	   tmp=tmp*64;      // 64 duracion de una semifusa
	   k=tmp;
       q=floor(tmp);
	   rs=tmp-q;
       acc=acc+rs;

	   if((int) ceil(acc)==1){ 
		   ++k; acc-=1; 
	   }
	   for(s=0;s<k;s++)
	      count++;       
	}
	res=nmaxsemifusa-count;
	return res;
}

EJECUCION *generaejecucion(PENTA *penta,int compas)
{
	int numnotas,count,dif,li,lu;
	int r,j,s,k,w,pos[2],posa[2];
	double tmp;
	EJECUCION *eje;
	int indextotempo[64][16];     //suponemos como maximo compases de 8 tiempos
	int beat[64][64];
	int pospen[2][64];
	int q,dur[64],num,tickbeat;
	double rs,acc;

    eje=(EJECUCION *) new EJECUCION;
	eje->penta[0].ver=eje->penta[1].ver=1;		
	for(j=0;j<2;j++){
	   numnotas=penta[j].compas[compas].numnotas;
	   for(r=acc=count=0;r<numnotas;r++){
          tmp=penta[j].compas[compas].dura[r];
	      tmp=tmp*64;      // 64 duracion una semifusa
		  k=tmp;
          q=floor(tmp);
	      rs=tmp-q;
          acc=acc+rs;
	      if((int) ceil(acc)==1){ 
		     ++k; acc-=1;  // para tresillos accedemos aqui
		  }
		  for(s=0;s<k;s++)
		     eje->penta[j].runnota[count++].pos=r;
	   }
	   eje->penta[j].hor=count;
	}
    posa[0]=eje->penta[0].runnota[0].pos;
	posa[1]=eje->penta[1].runnota[0].pos;
	pospen[0][0]=posa[0];
    pospen[1][0]=posa[1];
    for(k=1,r=1;r<count;r++){
	   pos[0]=eje->penta[0].runnota[r].pos;
	   pos[1]=eje->penta[1].runnota[r].pos;	   	   
	   if(pos[0]==posa[0] && pos[1]==posa[1]) continue;
	   pospen[0][k]=pos[0];
	   pospen[1][k++]=pos[1];
	   posa[0]=pos[0];
	   posa[1]=pos[1];
	}
	eje->penta[0].hor=eje->penta[1].hor=k;
	for(r=0;r<k;r++){
       s=pospen[0][r];
	   w=pospen[1][r];
       for(j=0;j<count;j++){
	      pos[0]=eje->penta[0].runnota[j].pos;
	      pos[1]=eje->penta[1].runnota[j].pos;	   	   
          if(pos[0]==s && pos[1]==w) break;
	   }
	   dur[r]=j;
	}

	num=penta[0].compas[compas].numsignature;
	tickbeat=count/num;
	dur[k]=count;
    for(s=0;s<num;s++){
	   li=s*tickbeat; lu=(s+1)*tickbeat-1;	   
	   for(r=j=0;r<k;r++){
	      if(dur[r]<=li && li<=dur[r+1]-1)
		     beat[s][j++]=r;
	      if(dur[r]<=lu && lu<=dur[r+1]-1)
		     beat[s][j++]=r;
	   }
	   beat[s][j]=-1;
	}
    for(r=0;r<k;r++){
	   for(s=w=0;s<num;s++){
		  for(j=0;beat[s][j]!=-1;j++){
			 if(beat[s][j]==r){
		        indextotempo[r][w++]=s;
				break;
			 }
		  }
	   }
	   indextotempo[r][w]=-1;
	}

	for(r=0;r<2;r++){
       for(j=0;j<k;j++){
	      w=pospen[r][j];
		  eje->penta[r].runnota[j].nota=penta[r].compas[compas].note[w];
          eje->penta[r].runnota[j].tupla=penta[r].compas[compas].tupla[w];
          eje->penta[r].runnota[j].clef=penta[r].compas[compas].clef[w];
		  eje->penta[r].runnota[j].tie=penta[r].compas[compas].tie[w];
		  eje->penta[r].runnota[j].notakey=penta[r].compas[compas].notekey[w];
          eje->penta[r].runnota[j].dura=penta[r].compas[compas].dura[w];
		  eje->penta[r].runnota[j].acci=penta[r].compas[compas].acci[w];		  
		  eje->penta[r].runnota[j].color=penta[r].compas[compas].color[w];		  
		  eje->penta[r].runnota[j].pos=w;		  		  
          if(j==k-1) dif=-1;
		  else{
	         dif=pospen[r][j+1]-w;		  
			 if(dif!=0) dif=1;
		  }
          eje->penta[r].runnota[j].dif=dif;
		  for(w=0;indextotempo[j][w]!=-1;w++)
		     eje->penta[r].runnota[j].beat[w]=indextotempo[j][w];          
		  eje->penta[r].runnota[j].beat[w]=-1;
	   }
	}
	return eje;
}

EJECUCION *generaejecucion1(PENTA *penta,int compas)
{
	int numnotas,count,dif,li,lu;
	int r,j,s,k,w,pos[2],posa[2];
	double tmp;
	EJECUCION *eje;
	int indextotempo[64][16];     //suponemos como maximo compases de 8 tiempos
	int beat[64][64];
	int pospen[2][64];
	int q,dur[64],num,tickbeat;
	double rs,acc;

    eje=(EJECUCION *) new EJECUCION;
	eje->penta[0].ver=eje->penta[1].ver=1;		
	for(j=0;j<2;j++){
	   numnotas=penta[j].compas[compas].numnotas;
	   for(r=acc=count=0;r<numnotas;r++){
          tmp=penta[j].compas[compas].dura[r];
	      tmp=tmp*64;      // 64 duracion una semifusa
		  k=tmp;
          q=floor(tmp);
	      rs=tmp-q;
          acc=acc+rs;
	      if((int) ceil(acc)==1){ 
		     ++k; acc-=1;  // para tresillos accedemos aqui
		  }
		  for(s=0;s<k;s++)
		     eje->penta[j].runnota[count++].pos=r;
	   }
	   eje->penta[j].hor=count;
	}
    posa[0]=eje->penta[0].runnota[0].pos;
	posa[1]=eje->penta[1].runnota[0].pos;
	pospen[0][0]=posa[0];
    pospen[1][0]=posa[1];
    for(k=1,r=1;r<count;r++){
	   pos[0]=eje->penta[0].runnota[r].pos;
	   pos[1]=eje->penta[1].runnota[r].pos;	   	   
	   if(pos[0]==posa[0] && pos[1]==posa[1]) continue;
	   pospen[0][k]=pos[0];
	   pospen[1][k++]=pos[1];
	   posa[0]=pos[0];
	   posa[1]=pos[1];
	}
	eje->penta[0].hor=eje->penta[1].hor=k;
	for(r=0;r<k;r++){
       s=pospen[0][r];
	   w=pospen[1][r];
       for(j=0;j<count;j++){
	      pos[0]=eje->penta[0].runnota[j].pos;
	      pos[1]=eje->penta[1].runnota[j].pos;	   	   
          if(pos[0]==s && pos[1]==w) break;
	   }
	   dur[r]=j;
	}

//*****************************************

	num=penta[0].compas[compas].numsignature;
	tickbeat=count/num;
	dur[k]=count;
    for(s=0;s<num;s++){
	   li=s*tickbeat; lu=(s+1)*tickbeat-1;	   
	   for(r=j=0;r<k;r++){
	      if(dur[r]<=li && li<=dur[r+1]-1)
		     beat[s][j++]=r;
	      if(dur[r]<=lu && lu<=dur[r+1]-1)
		     beat[s][j++]=r;
	   }
	   beat[s][j]=-1;
	}
    for(r=0;r<k;r++){
	   for(s=w=0;s<num;s++){
		  for(j=0;beat[s][j]!=-1;j++){
			 if(beat[s][j]==r){
		        indextotempo[r][w++]=s;
				break;
			 }
		  }
	   }
	   indextotempo[r][w]=-1;
	}

//*****************************************

	for(r=0;r<2;r++){
       for(j=0;j<k;j++){
	      w=pospen[r][j];
		  eje->penta[r].runnota[j].nota=penta[r].compas[compas].note[w];
          eje->penta[r].runnota[j].tupla=penta[r].compas[compas].tupla[w];
          eje->penta[r].runnota[j].clef=penta[r].compas[compas].clef[w];
		  eje->penta[r].runnota[j].tie=penta[r].compas[compas].tie[w];
		  eje->penta[r].runnota[j].notakey=penta[r].compas[compas].notekey[w];
          eje->penta[r].runnota[j].dura=penta[r].compas[compas].dura[w];
		  eje->penta[r].runnota[j].acci=penta[r].compas[compas].acci[w];		  
		  eje->penta[r].runnota[j].color=penta[r].compas[compas].color[w];		  
		  eje->penta[r].runnota[j].pos=w;		  		  
          if(j==k-1) dif=-1;
		  else{
	         dif=pospen[r][j+1]-w;		  
			 if(dif!=0) dif=1;
		  }
          eje->penta[r].runnota[j].dif=dif;
		  for(w=0;indextotempo[j][w]!=-1;w++)
		     eje->penta[r].runnota[j].beat[w]=indextotempo[j][w];          
		  eje->penta[r].runnota[j].beat[w]=-1;
	   }
	}
	return eje;
}

/*
EJECUCION *generaejecucion1(PENTA *penta,int compas)
{
	int numnotas,count;
	int r,j,s,k,w,pos[2],posa[2];
	double tmp;
	EJECUCION *eje;
	int pospen[2][32];

	int q;
	double rs,acc;

    eje=(EJECUCION *) new EJECUCION;
	eje->penta[0].ver=eje->penta[1].ver=1;		
	for(j=0;j<2;j++){
	   numnotas=penta[j].compas[compas].numnotas;
	   for(r=acc=count=0;r<numnotas;r++){
          tmp=penta[j].compas[compas].dura[r];
	      tmp=tmp*32;      // 32 duracion una fusa
		  k=tmp;
          q=floor(tmp);
	      rs=tmp-q;
          acc=acc+rs;
	      if((int) ceil(acc)==1){ 
		     ++k; acc-=1; 
		  }
		  for(s=0;s<k;s++)
		     eje->penta[j].runnota[count++].pos=r;
	   }
	   eje->penta[j].hor=count;
	}
    posa[0]=eje->penta[0].runnota[0].pos;
	posa[1]=eje->penta[1].runnota[0].pos;
	pospen[0][0]=posa[0];
    pospen[1][0]=posa[1];
    for(k=1,r=1;r<count;r++){
	   pos[0]=eje->penta[0].runnota[r].pos;
	   pos[1]=eje->penta[1].runnota[r].pos;	   	   
	   if(pos[0]==posa[0] && pos[1]==posa[1]) continue;
	   pospen[0][k]=pos[0];
	   pospen[1][k++]=pos[1];
	   posa[0]=pos[0];
	   posa[1]=pos[1];
	}
	eje->penta[0].hor=eje->penta[1].hor=k;
	for(r=0;r<2;r++){
       for(j=0;j<k;j++){
	      w=pospen[r][j];
		  eje->penta[r].runnota[j].nota=penta[r].compas[compas].note[w];
		  eje->penta[r].runnota[j].tie=penta[r].compas[compas].tie[w];
		  eje->penta[r].runnota[j].tupla=penta[r].compas[compas].tupla[w];
		  eje->penta[r].runnota[j].notakey=penta[r].compas[compas].notekey[w];
          eje->penta[r].runnota[j].dura=penta[r].compas[compas].dura[w];
		  eje->penta[r].runnota[j].acci=penta[r].compas[compas].acci[w];		  
		  eje->penta[r].runnota[j].color=penta[r].compas[compas].color[w];		  
		  eje->penta[r].runnota[j].pos=w;		  
	   }
	}
	return eje;
}

*/
/*
EJECUCION *generaejecucion(PENTA *penta,int compas)
{
	int numnotas,count,dif;
	int r,j,s,k,w,pos[2],posa[2];
	double tmp;
	EJECUCION *eje;
	int indextotempo[32];
	int pospen[2][32];

	int q;
	double rs,acc;

    eje=(EJECUCION *) new EJECUCION;
	eje->penta[0].ver=eje->penta[1].ver=1;		
	for(j=0;j<2;j++){
	   numnotas=penta[j].compas[compas].numnotas;
	   for(r=acc=count=0;r<numnotas;r++){
          tmp=penta[j].compas[compas].dura[r];
	      tmp=tmp*32;      // 32 duracion una fusa
		  k=tmp;
          q=floor(tmp);
	      rs=tmp-q;
          acc=acc+rs;
	      if((int) ceil(acc)==1){ 
		     ++k; acc-=1;  // para tresillos accedemos aqui
		  }
		  for(s=0;s<k;s++)
		     eje->penta[j].runnota[count++].pos=r;
	   }
	   eje->penta[j].hor=count;
	}
    posa[0]=eje->penta[0].runnota[0].pos;
	posa[1]=eje->penta[1].runnota[0].pos;
	pospen[0][0]=posa[0];
    pospen[1][0]=posa[1];
    for(k=1,r=1;r<count;r++){
	   pos[0]=eje->penta[0].runnota[r].pos;
	   pos[1]=eje->penta[1].runnota[r].pos;	   	   
	   if(pos[0]==posa[0] && pos[1]==posa[1]) continue;
	   pospen[0][k]=pos[0];
	   pospen[1][k++]=pos[1];
	   posa[0]=pos[0];
	   posa[1]=pos[1];
	}
	eje->penta[0].hor=eje->penta[1].hor=k;
	for(r=0;r<k;r++){
       s=pospen[0][r];
	   w=pospen[1][r];
       for(j=0;j<count;j++){
	      pos[0]=eje->penta[0].runnota[j].pos;
	      pos[1]=eje->penta[1].runnota[j].pos;	   	   
          if(pos[0]==s && pos[1]==w) break;
	   }
       indextotempo[r]=(j*(concfg.compas[2]-0x30))/32;
	}
	for(r=0;r<2;r++){
       for(j=0;j<k;j++){
	      w=pospen[r][j];
		  eje->penta[r].runnota[j].nota=penta[r].compas[compas].note[w];
          eje->penta[r].runnota[j].tupla=penta[r].compas[compas].tupla[w];
		  eje->penta[r].runnota[j].tie=penta[r].compas[compas].tie[w];
		  eje->penta[r].runnota[j].notakey=penta[r].compas[compas].notekey[w];
          eje->penta[r].runnota[j].dura=penta[r].compas[compas].dura[w];
		  eje->penta[r].runnota[j].acci=penta[r].compas[compas].acci[w];		  
		  eje->penta[r].runnota[j].color=penta[r].compas[compas].color[w];		  
		  eje->penta[r].runnota[j].pos=w;		  		  
          if(j==k-1) dif=-1;
		  else{
	         dif=pospen[r][j+1]-w;		  
			 if(dif!=0) dif=1;
		  }
          eje->penta[r].runnota[j].dif=dif;
		  eje->penta[r].runnota[j].beat=indextotempo[j];
	   }
	}
	return eje;
}


	for(r=0;r<k;r++){
	   if(r==k-1){ indextotempo[r]=num-1; break; }
	   for(w=s=0;w<num;w++,s+=tickbeat){	   
		  if(dur[r+1]<=s+tickbeat){
             indextotempo[r]=w;
		     break;
		  }
	   }
	}

*/