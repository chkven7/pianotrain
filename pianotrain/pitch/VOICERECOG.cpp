#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "portaudio.h"

#define VERY_BIG  (1e30)
#define M_PI 3.14159265358979323846
#define SAMPLE_RATE         16000

#define PA_SAMPLE_TYPE      paFloat32 //paUInt8
#define FRAMES_PER_BUFFER   128 //(2048)
//#define DTWWINDOW 350         // 256 mejora el numero de aciertos a 8000
#define DTWWINDOW 2
#define FL 125 //125
#define FH 6400 //3400

typedef float SAMPLE;

typedef struct knn_
{
   double dist;
   int index;
} KNN;

typedef struct hnotas_
{
   char name[32];
   char magic[32];
   int **ref;
   int fil,col;
} HNOTAS;

static int gNumNoInputs = 0;
static double *coef;
static double *x_buffer; 
static int NB_COEF; 
static char *notas[]={"do","re","mi","fa","sol","la","si"};
double buffin[FRAMES_PER_BUFFER],CPb[10]; 
char *palabra; 
int pindex;
int learning;
HNOTAS hnotas[256];
int canhnotas;
extern HWND hwnd;

static int Callback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData );

double dtw(int **x,int **y,unsigned int xsize,unsigned int ysize,unsigned int params);
double DTWDistanceFun(int **A,int **B,int I,int J,int r);
double dtw2(int **x,int **y,unsigned int xsize,unsigned int ysize,unsigned int params);

int notatoindex(char *name)
{
   int r;

   for(r=0;r<7;r++)
      if(strcmp(name,notas[r])==0)
         break;
   return r;
}

BOOL IsDots(const TCHAR* str) {
   if(strcmp(str,".") && strcmp(str,"..")) return FALSE;
   return TRUE;
}

BOOL DeleteDirectory(const TCHAR* sPath) 
{
    HANDLE hFind; // file handle
    WIN32_FIND_DATA FindFileData;
     
    TCHAR DirPath[MAX_PATH];
    TCHAR FileName[MAX_PATH];
     
    strcpy(DirPath,sPath);
    strcat(DirPath,"\\*"); // searching all files
    strcpy(FileName,sPath);
    strcat(FileName,"\\");
     
    // find the first file
    hFind = FindFirstFile(DirPath,&FindFileData);
    if(hFind == INVALID_HANDLE_VALUE) return FALSE;
    strcpy(DirPath,FileName);
     
    bool bSearch = true;
    while(bSearch){ // until we find an entry
       if(FindNextFile(hFind,&FindFileData)) {
          if(IsDots(FindFileData.cFileName)) continue;
          strcat(FileName,FindFileData.cFileName);
          if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
    // we have found a directory, recurse
             if(!DeleteDirectory(FileName)){
                FindClose(hFind);
                return FALSE; // directory couldn't be deleted
			 }
    // remove the empty directory
             RemoveDirectory(FileName);
             strcpy(FileName,DirPath);
		  }else{
             if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    // change read-only file mode
                _chmod(FileName, _S_IWRITE);
             if(!DeleteFile(FileName)) { // delete the file
                FindClose(hFind);
                return FALSE;
			 }
             strcpy(FileName,DirPath);
		  }
	   }else{
    // no more files there
          if(GetLastError() == ERROR_NO_MORE_FILES)
             bSearch = false;
          else {
    // some error occurred; close the handle and return FALSE
             FindClose(hFind);
             return FALSE;
		  }
	   }     
    }
    FindClose(hFind); // close the file handle     
    return RemoveDirectory(sPath); // remove the empty directory     
}

char **retrievedirfiles(int &numfil,char *pwdc)
{
    int can,r; 
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    char **base;
    TCHAR pwd[256];
    
    can=0;
    strcpy(pwd,pwdc);
    strcat(pwd,"\\speech\\");
    if(SetCurrentDirectory(pwd)==0){ numfil=-1; return NULL;}
    strcat(pwd,"*.txt");
    hFind = FindFirstFile(pwd,&FindData); ++can;
    if(hFind==INVALID_HANDLE_VALUE){ 
		SetCurrentDirectory(pwdc);
		numfil=-1; 
		return NULL;
	}
    while (FindNextFile(hFind, &FindData))
       ++can;
    FindClose(hFind);
    base=(char **) new char*[can]; // maximo numero de ficheros en directorio
    for(r=0;r<can;r++)
       base[r]=(char *) new char[32];
    hFind = FindFirstFile(pwd,&FindData);
    can=0;
    strcpy(base[can++],FindData.cFileName);
    while (FindNextFile(hFind, &FindData))
       strcpy(base[can++],FindData.cFileName);
    FindClose(hFind);
    numfil=can;
    return base;
}

void deletedirfilesdatabase(char **dirfiles,int canf)
{
   int r;

   for(r=0;r<canf;r++)
      delete [] dirfiles[r];
   delete [] dirfiles;
   return;
}

int loadfeaturedatabase(void)
{
   int f,c,r,s,canf;
   char a[256],name[32],magic[32],*file,**dirfiles;
   int **x,fil,col;
   HNOTAS *hn;
   FILE *fh;
   TCHAR pwd[256],pwdc[256];
    
   GetCurrentDirectory(256,pwdc);
   strcpy(pwd,pwdc);
   strcat(pwd,"\\speech\\");
   dirfiles=retrievedirfiles(canf,pwdc);
   if(dirfiles==NULL){ 
	   SetCurrentDirectory(pwdc);
	   return 0;
   }
   if(canf>256) canf=28; // maximo numero de ficheros en la base de datos
   if(canf<28){ 
	   SetCurrentDirectory(pwdc);
	   return canf; // deben de haber 28 muestras de voz en la base de datos 
   }
   for(s=0;s<canf;s++){      
      hn=&hnotas[s];
      file=dirfiles[s];   
      fh=fopen(file,"r");
      fgets(a,256,fh);
      sscanf(a,"%d %d",&fil,&col); 
      x=(int **) new int*[fil];
      for(r=0;r<fil;r++)
         x[r]=(int *) new int[col]; 
      for(f=0;f<fil;f++)
         for(c=0;c<col;c++)
	        fscanf(fh,"%d",&x[f][c]);
	  for(r=0;r<strlen(file);r++){
        if(file[r]=='-') file[r]=' ';
        if(file[r]=='.') file[r]=' ';
	  }
      sscanf(file,"%s %s",name,magic);
      strcpy(hn->name,name);
      strcpy(hn->magic,magic);
      hn->ref=x;
      hn->fil=fil;
      hn->col=col;
      fclose(fh);
   }
   SetCurrentDirectory(pwdc);
   deletedirfilesdatabase(dirfiles,canf);
   return canf;
}

void deletedatabasedirectory(void)
{
      char pwd[256],pwdc[256];

      GetCurrentDirectory(256,pwdc);	 
      strcpy(pwd,pwdc);
      strcat(pwd,"\\speech\\");
      if(SetCurrentDirectory(pwd)==0) return;
      DeleteDirectory(pwd); 
	  SetCurrentDirectory(pwdc);
      return;
}

void deletefeature(int **x,int fil)
{
	int j;

    for(j=0;j<fil;j++) delete [] x[j];
	delete [] x;
	return;
}

void deletefeaturedatabase(void)
{
   int s;

   for(s=0;s<canhnotas;s++)
      deletefeature(hnotas[s].ref,hnotas[s].fil);
   return;
}

void savefeature(int **x,int xfil,int xcol,char *file)
{
	  int r,j,ma,mx;
	  char temp[64];
      FILE *fh;
      char pwd[256],pwdc[256];

      GetCurrentDirectory(256,pwdc);	 
      strcpy(pwd,pwdc);
      strcat(pwd,"\\speech\\");
      if(SetCurrentDirectory(pwd)==0){
         CreateDirectory(pwd,NULL);   
		 SetCurrentDirectory(pwd);
	  }
      mx=-1;	 
      for(r=0;r<canhnotas;r++){
		 if(strcmp(hnotas[r].name,file)==0){
            ma=atoi(hnotas[r].magic);
            if(ma>mx) mx=ma;
         }     
      }
      ++mx;
      sprintf(temp,"%s-%d",file,mx);
      strcpy(hnotas[canhnotas].name,file);
      sprintf(hnotas[canhnotas].magic,"%d",mx);
      hnotas[canhnotas].ref=x;
      hnotas[canhnotas].fil=xfil;
      hnotas[canhnotas++].col=xcol;
      strcat(temp,".txt"); 
	  fh=fopen(temp,"w");
	  fprintf(fh,"%d %d \n",xfil,xcol);
	  for(r=0;r<xfil;r++){
	    for(j=0;j<xcol;j++)
	      fprintf(fh,"%d ",x[r][j]);
	    fprintf(fh,"\n");
	  }
      SetCurrentDirectory(pwdc);
      fclose(fh);
}

int **getfeature(char *palabra,int can,int &fil,int &col)
{
   int r,j,w;
   int *z,countz,M,**x,cansamples;
   int dtwwindow;

   dtwwindow=DTWWINDOW;
   cansamples=can/dtwwindow;
   M=SAMPLE_RATE/(2*FL);  // valor maximo de muestras entre pasos por cero
   x=(int **) new int*[dtwwindow]; //x=(int **) new int*[canwin];
   for(r=0;r<dtwwindow;r++){
      x[r]=(int *) new int[M]; 
      for(j=0;j<M;j++) x[r][j]=0;
   }
   z=(int *) new int[cansamples];
   w=0; // prueba
   for(r=0;r<cansamples*dtwwindow;r+=cansamples){
      for(j=r+1,countz=0;j<r+cansamples;j++)
        if(palabra[j]*palabra[j-1]<0) 
			z[countz++]=j-r;
     for(j=countz-1;j>0;j--) z[j]-=z[j-1];      
	 for(j=0;j<countz;j++){
        if(z[j]-1>=M) continue;
	    ++x[w][z[j]-1];
	 }
	 ++w;
   }
   delete [] z;
   fil=dtwwindow;
   col=M;
   return x;
}

void preenfasis(double* fx, double* fxp,int Size)
{
	// fx	: Vector sin normalizar.
	// fxp	: Vector normalizado y con preénfasis.
	// np	: Tamaño de los vectores.
	
	double maximo=0;
	
	fxp[0]=float(fx[0]);
		
	for(int i=1;i<Size;i++)
	{	
		fxp[i]=fx[i]-0.95*fx[i-1];	//Preenfasis
	
		if(fabs(fxp[i])>maximo)		//Buscando el máximo
			maximo=fabs(fxp[i]);
	}
	
	for(i=0;i<Size;i++)
		fxp[i]=fxp[i]/maximo;
	
}
// frec inferior de corte fc1
// frec superior corte fc2
// nc numero de coeficientes
     
void passbandfilter(double fc1,double fc2,int nc)
{
  int nz=0;
  int i;
  double d1, d2;
  double h;


  d1 = ((double)nc - 1.0)/2.0;
  
  for( i = 0; i < nc; ++i )
  {
    d2 = (double)i - d1;
    h = d2 == 0 ? (fc2 - fc1) / M_PI : (sin(fc2 * d2) - sin(fc1 * d2)) / (M_PI * d2);
	coef[i]=h;
  }
  return;
}

// if the size of NB_COEF = 2^n use a bit mask instead of the modulo (%)
// %=NB_COEF => &=(NB_COEF-1)
// pipe is a circular buffer
// x: muestra entrada
// y: muestra de salida

double FilterFIR(float x)
{
  int n;
  double y;
  static int ptr_x_buffer;

  x_buffer[ptr_x_buffer++] = x;
  ptr_x_buffer %= NB_COEF;

  y=0;
  for( n = (NB_COEF-1) ; n >= 0 ; n-- )
  {
    y += coef[n] * x_buffer[ptr_x_buffer++];
    ptr_x_buffer %= NB_COEF;
  }
  return(y);
}

void initfiltercoeff(void)
{
   double fc1=2.0*FL/SAMPLE_RATE;   //fl=125 hz
   double fc2=2.0*FH/SAMPLE_RATE;  //fh=3400 hz  fs=8000 downsampling
   int nc=32,r;                      // 32 coeficientes

   coef=(double *) new double[nc];
   x_buffer=(double *) new double[nc];
   NB_COEF=nc;
   for(r=0;r<nc;r++) x_buffer[r]=0;
   passbandfilter(fc1,fc2,nc);
   return;
} 

void deletefilter(void)
{
	delete [] coef;
	delete [] x_buffer;
    return;
}

int fsign(double y)
{
	if(y>=0)
		return (1);
	else
		return (0);
}
double Coper(double * dato, int n, int L)
{
	int i;
	double CP=0;

	for(i=n;i<n+L;i++)
		if(i>0)
			CP=CP+fabs(dato[i]*fabs(dato[i])-dato[i-1]*fabs(dato[i-1]));
		else
			CP=CP+fabs(dato[0]*dato[0]*fsign(dato[0]));

	return double(CP);
	
}

int comparevoicerecog( const void *arg1, const void *arg2 )
{
   KNN *a,*b;
   
   a=(KNN *) arg1;
   b=(KNN *) arg2;
   
   if(a->dist>b->dist) return 1;
   if(a->dist<b->dist) return -1;
   return 0;
}

static int VoiceCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *trnotaparam )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    int i;
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
	double CP,temp;      // 256 maxima cantidad de distancias en la base de datos
	int CPUI,CPUF;
	int **x,xfil,xcol;
    KNN knn[256];
	char *lnota=(char *) trnotaparam;
	static int estado;

	CPUI=8;       // valores que hay que ajustar
	CPUF=4;  
    if( inputBuffer == NULL ) return paContinue;    
    for(i=0;i<framesPerBuffer;++i){       
	   buffin[i]=FilterFIR(in[i]);
	}

	if(estado==0){
       CP=Coper(buffin,0,framesPerBuffer);
	   // printf("%f detectando inicio\n",CP);
       if(CP*10000<CPUI){ 
	      return paContinue;
	   }
	   estado=1;
	   //for(i=0;i<framesPerBuffer;i++)
	   //   palabra[i]=(buffin[i]>=0) ? 1 : -1;
	   //**************
	   // prueba
	   for(i=1;i<framesPerBuffer;i++)
	      palabra[i-1]=((buffin[i]-buffin[i-1])*SAMPLE_RATE>=0) ? 1 : -1;
       palabra[i-1]=palabra[i-2]; // prueba
	   //********************
	   pindex=i;
	   return paContinue;
	}
	if(estado==1){
       CP=Coper(buffin,0,framesPerBuffer);
	   for(i=3;i>=0;i--)    // media movil ultimos 4 valores
          CPb[i]=CPb[i-1];
	   CPb[0]=CP;
       for(i=temp=0;i<4;i++) temp+=CPb[i];
  	   temp/=4; CP=temp;
	   // printf("%f detectando fin  \n",CP);
       for(i=0;i<framesPerBuffer;i++){
		  if(pindex+i<SAMPLE_RATE*2){         // SAMPLE_RATE*2 es el tamaño maximo del buffer que contiene la palabra, 2 (seg) es la maxima duracion de una palabra
	         palabra[pindex+i]=(buffin[i]>=0) ? 1 : -1;
          }else{ 
			 pindex=SAMPLE_RATE*2; i=0;
		     break;
		  }
       }
	   pindex+=i;
	   if(CP*10000>CPUF) return paContinue;
	   estado=2;
	}
    if(estado==2){
       int knnnota[7],maxv,vv,hh;
       
	   estado=0;
	   if(pindex<2000) return paContinue;
	   x=getfeature(palabra,pindex,xfil,xcol);
       if(learning==0){ 
          for(i=0;i<canhnotas;i++){ 
             knn[i].dist=dtw2(x,hnotas[i].ref,(unsigned int) xfil,
			          (unsigned int) hnotas[i].fil,SAMPLE_RATE/(2*FL));
		  //printf("Distancia a patron: %s es %f\n",hnotas[i].name,knn[i].dist);
             knn[i].index=notatoindex(hnotas[i].name);
		  }

          maxv=-1;
          for(i=0;i<7;i++) knnnota[i]=0;
          qsort((void *) knn,(size_t) canhnotas,sizeof(KNN),comparevoicerecog);
		  hh=(canhnotas>=1) ? 1: canhnotas;  
          for(i=0;i<hh;i++)  // knn de 5  
             ++knnnota[knn[i].index];
          vv=0;
          for(i=0;i<7;i++)    // comprobamos el mas votado de entre las 7 notas
             if(knnnota[i]>maxv){ vv=i; maxv=knnnota[i]; }
		  if(maxv==1) vv=knn[0].index;   // minima distancia si no hay voto de mayoria
//          printf("MINIMA DISTANCIA DE %f PARA NOTA: %s \n",knn[vv].dist,notas[knn[vv].index]);
		  SendMessage(hwnd, WM_APP+1,(WPARAM) vv, (LPARAM) 0);
		  //printf("Has dicho la NOTA: %s (y/n) ?",notas[vv]);	
       }else{  // learning==1  //
		  LRESULT nx;
		  nx=(int) SendMessage(hwnd, WM_APP+2,(WPARAM) 1, (LPARAM) 0);
          if(nx>=0 && nx<28){
             trnotaparam=notas[abs(nx)];
		     savefeature(x,xfil,xcol,(char *) trnotaparam);     
			 return paContinue;
		  }
		  if(nx==28){			 
             trnotaparam=notas[6];
		     savefeature(x,xfil,xcol,(char *) trnotaparam);     
             PostMessage(hwnd, WM_APP+2,(WPARAM) 2, (LPARAM) 0);
             return paComplete;
		  }
	   }
       deletefeature(x,xfil);
       return paContinue;
    }
    return paContinue;
}


int solfeohablado(int offline,int learn,char *learnnota)
{
    static PaStreamParameters inputParameters; //,outputParameters;
    static PaStream *stream;
    PaError err;
    
	if(offline==-1){
	   canhnotas=loadfeaturedatabase();
       if(canhnotas!=28) deletedatabasedirectory(); // si base de datos incompleta la eleminamos del directorio
	   return canhnotas;
	}
	if(offline==1){
       if(stream!=NULL){
          err = Pa_CloseStream( stream );
		  stream=NULL;
	      deletefeaturedatabase();
	      deletefilter();
		  delete [] palabra;
          if( err != paNoError ){ Pa_Terminate(); return -1;}
       }
       return 1;
	}
    learning=learn;
    err = Pa_Initialize();
    if( err != paNoError ){ Pa_Terminate(); return -1;}

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
	  Pa_Terminate(); //fprintf(stderr,"Error: No default input device.\n"); 
	  return -1;
    }    
    inputParameters.channelCount = 1;       
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;    
    initfiltercoeff();	
    palabra=(char *) new char[SAMPLE_RATE*2];  // 2 seg maxima duracion de una palabra
	err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL, //&outputParameters,  // solo leemos entrada de audio &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              0, 
              VoiceCallback,
              (void *) learnnota);
		   
	if( err != paNoError ){ Pa_Terminate(); stream=NULL; return -1;}
    err = Pa_StartStream( stream );
	if( err != paNoError ){ Pa_Terminate(); stream=NULL; return -1;}
    return 1;
}
/*
void main(void)
{
  	int learn;

	learn=1;
      solfeohablado(0,learn); 
      while(1);
      solfeohablado(1,0);   // libera todo
      return;

	if(learn==1){
	   for(i=0;i<7;i++){	      
	      printf("PRONUNCIA LA NOTA %s despues de pulsar enter\n",notas[i]);
		  getchar();
	      solfeohablado(0,learn,i);  // online,learning,nota i
		  getchar();
		  solfeohablado(1,0,0);  // offline libera todo
		  printf("Quieres repetir (y/n)? ");
		  scanf("%c",&r);
		  if(r=='y'){ --i; continue; }
	   }
	}else{
	   solfeohablado(0,learn,0);     // online, sin learning
	   getchar();
	   solfeohablado(1,0,0);     // liberatodo
	}
	return;
}
*/


/*******************************
   for( m=0; m<128; m++ ){	 
	   *out++ = palabra[m]; 
   }
***********************************/

/*****************************************************************
	// prueba
     outputParameters.device = Pa_GetDefaultOutputDevice(); 
     outputParameters.channelCount = 1; 
     outputParameters.sampleFormat = PA_SAMPLE_TYPE;
     outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
     outputParameters.hostApiSpecificStreamInfo = NULL;


//             dist[i]=DTWDistanceFun(x,hnotas[i].ref,xfil,hnotas[i].fil,SAMPLE_RATE/(2*FL));

*******************************************************************/
