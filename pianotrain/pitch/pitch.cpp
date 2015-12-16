#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "dywapitchtrack.h"
/*
** Note that many of the older ISA sound cards on PCs do NOT support
** full duplex audio (simultaneous record and playback).
** And some only support full duplex at lower sample rates.
*/
#define SAMPLE_RATE         (44100)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (1024) //(2048)

typedef float SAMPLE;

extern HWND hwnd;
static int gNumNoInputs = 0;
static double buffin[FRAMES_PER_BUFFER],pitch,pitchb[10];

//FILE *fh;

typedef struct _pitch
{
	int notekey;
	double freq;
} PITCH;

PITCH pitchkey;
static double pitchnota[49]={65.41,69.30,73.42,77.78,82.41,87.31,92.50,98.00,103.83,110.00, 	
                   116.54,123.47,130.81,138.59,146.83,155.56,164.81,174.61,185.00,
				   196.00,207.65,220.00,233.08,246.94,261.63,277.18,293.66,311.13, 	
				   329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88,523.25, 	
				   554.37,587.33,622.25,659.26,698.46,739.99,783.99,830.61,880.00, 	
                   932.33,987.77,1046.50}; 	


int pitchcallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *pitchtracker )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    int i;
    (void) timeInfo; 
    (void) statusFlags;
    double lim=1.029302236643492,liminf,limsup;

    if( inputBuffer == NULL ) return paContinue;
	for( i=0;i<framesPerBuffer;i++)
       buffin[i]=in[i];
    pitch=dywapitch_computepitch((dywapitchtracker*) pitchtracker,buffin,0,FRAMES_PER_BUFFER);
	for(i=9;i>0;i--)    
       pitchb[i]=pitchb[i-1];
	pitchb[0]=pitch;
    for(i=pitch=0;i<10;i++) pitch+=pitchb[i];
	pitch/=10;
    for(i=0;i<49;i++){
	   limsup=pitchnota[i]*lim;
	   liminf=pitchnota[i]/lim;
	   if(liminf<=pitch && pitch<=limsup) break;
	}
	if(i==49){ pitchkey.freq=0; pitchkey.notekey=0; }
    else{
	   pitchkey.freq=pitchnota[i];
	   pitchkey.notekey=36+i;
	}
    SendMessage(hwnd, WM_APP,(WPARAM) &pitchkey, (LPARAM) 0);
//	printf("nota: %d freq: %f \n",res.notekey,res.freq);
//	fprintf(fh,"%f \n",pitch);
    return paContinue;   
}   


int pitchdetection(int offline)
{
    static PaStreamParameters inputParameters;
    static PaStream *stream;
    PaError err;
	static dywapitchtracker pitchtracker;

	if(offline==1){
       if(stream!=NULL){
          err = Pa_CloseStream( stream );
	      stream=NULL;
          if( err != paNoError ){ Pa_Terminate(); return -1;}
       }
       return 1;
	}
    err = Pa_Initialize();
    if( err != paNoError ){ Pa_Terminate(); return -1;}

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
	  Pa_Terminate(); //fprintf(stderr,"Error: No default input device.\n"); 
	  return -1;
    }
    inputParameters.channelCount = 1;       /* 2 stereo input 1 mono input*/
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
	dywapitch_inittracking(&pitchtracker);
    pitchkey.freq=0;
	pitchkey.notekey=0;

    //fh=fopen("donota.txt","w");
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,  // solo leemos entrada de audio &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
              pitchcallback,
              (void *) &pitchtracker);
	if( err != paNoError ){ Pa_Terminate(); stream=NULL; return -1;}
    err = Pa_StartStream( stream );
	if( err != paNoError ){ Pa_Terminate(); stream=NULL; return -1;}
    return 1;
}
