/*
 * midi2abc - program to convert MIDI files to abc notation.
 * Copyright (C) 1998 James Allwright
 * e-mail: J.R.Allwright@westminster.ac.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 */

/* new midi2abc - converts MIDI file to abc format files
 * 
 *
 * re-written to use dynamic data structures 
 *              James Allwright
 *               5th June 1998
 *
 * added output file option -o
 * added summary option -sum
 * added option -u to enter xunit directly
 * fixed computation of xunit using -b option
 * added -obpl (one bar per line) option
 * add check for carriage return embedded inside midi text line
 *                Seymour Shlien  04/March/00
 * made to conform as much as possible to the official version.
 * check for drum track added
 * when midi program channel is command encountered, we ensure that 
 * we are using the correct channel number for the Voice by sending
 * a %%MIDI channel message.
 *
 * Many more changes (see doc/CHANGES) 
 *
 *                Seymour Shlien  2005
 * 
 * based on public domain 'midifilelib' package.
 */

#define VERSION "2.92 August 04 2008"
#define SPLITCODE

/* Microsoft Visual C++ Version 6.0 or higher */
#ifdef _MSC_VER
#define ANSILIBS
#endif

#include <stdio.h>
#include "piano.h"
#ifdef PCCFIX
#define stdout 1
#endif

/* define USE_INDEX if your C libraries have index() instead of strchr() */
#ifdef USE_INDEX
#define strchr index
#endif

#ifdef ANSILIBS
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#else
extern char* malloc();
extern char* strchr();
#endif
#include "midifile.h"
#define BUFFSIZE 200
/* declare MIDDLE C */
#define MIDDLE 72


typedef struct listx LISTX;
typedef struct anote ANOTE;

void initfuncs();
void setupkey(int);
int testtrack(int trackno, int barbeats, int anacrusis);
int open_note(int chan, int pitch, int vol);
int close_note(int chan, int pitch, int *initvol);


/* Global variables and structures */

extern long Mf_toberead;
extern TCHAR currentdir[256];

static FILE *F;
static FILE *outhandle; /* for producing the abc file */

int tracknum=0;  /* track number */
int division;    /* pulses per quarter note defined in MIDI header    */
long tempo = 500000; /* the default tempo is 120 quarter notes/minute */
int unitlen;     /* abc unit length usually defined in L: field       */
int header_unitlen; /* first unitlen set                              */
int unitlen_set =0; /* once unitlen is set don't allow it to change   */
int parts_per_unitlen = 2; /* specifies minimum quantization size */
long laston = 0; /* length of MIDI track in pulses or ticks           */
char textbuff[BUFFSIZE]; /*buffer for handling text output to abc file*/
int trans[256], back[256]; /*translation tables for MIDI pitch to abc note*/
char atog[256]; /* translation tables for MIDI pitch to abc note */
int symbol[256]; /*translation tables for MIDI pitch to abc note */
int key[12];
int sharps;
int trackno, maintrack;
int format; /* MIDI file type                                   */

int karaoke, inkaraoke;
int midline;
int tempocount=0;  /* number of tempo indications in MIDI file */
int gotkeysig=0; /*set to 1 if keysignature found in MIDI file */

/* global parameters that may be set by command line options    */
int xunit;    /* pulses per abc unit length                     */
int tsig_set; /* flag - time signature already set by user      */
int ksig_set; /* flag - key signature already set by user       */
int xunit_set;/* flat - xunit already set by user               */
int extracta; /* flag - get anacrusis from strong beat          */
int guessu;   /* flag - estimate xunit from note durations      */
int guessa;   /* flag - get anacrusis by minimizing tied notes  */
int guessk;   /* flag - guess key signature                     */
int summary;  /* flag - output summary info of MIDI file        */
int keep_short; /*flag - preserve short notes                   */
int swallow_rests; /* flag - absorb short rests                 */
int midiprint; /* flag - run midigram instead of midi2abc       */
#ifdef SPLITCODE
int usesplits; /* flag - split measure into parts if needed     */
#endif
int restsize; /* smallest rest to absorb                        */
int no_triplets; /* flag - suppress triplets or broken rhythm   */
int obpl = 0; /* flag to specify one bar per abc text line      */
int nogr = 0; /* flag to put a space between every note         */
int bars_per_line=4;  /* number of bars per output line         */
int bars_per_staff=4; /* number of bars per music staff         */
int asig, bsig;  /* time signature asig/bsig                    */
int header_asig =0; /* first time signature encountered         */
int header_bsig =0; /* first time signature encountered         */
int header_bb;      /* first ticks/quarter note encountered     */
int active_asig,active_bsig;  /* last time signature declared   */
int last_asig, last_ksig; /* last time signature printed        */
int barsize; /* barsize in parts_per_unitlen units                   */
int Qval;        /* tempo - quarter notes per minute            */
int verbosity=0; /* control amount of detail messages in abcfile*/

/* global arguments dependent on command line options or computed */

int anacrusis=0; 
int bars;
int keysig;
int header_keysig=  -50;  /* header key signature                     */
int active_keysig = -50;  /* last key signature declared        */
int xchannel;  /* channel number to be extracted. -1 means all  */


/* structure for storing music notes */
struct anote {
  int pitch;  /* MIDI pitch    */
  int chan;   /* MIDI channel  */
  int vel;    /* MIDI velocity */
  long time;  /* instante de tiempo donde se inicia el NOTEON */
  long dtnext; /* diferencia de tiempo entre el NOTEON actual y el siguiente */
  long tplay;  /* duracion del evento noteon en pulsos */
  long tinitnoteoff; // instante de tiempo en que llega el noteoff
  long toff;   /* duracion del evento noteoff*/
  int xnum;    /* number of xunits to next note */
  int playnum; /* note duration in number of xunits */
  int posnum; /* note position in xunits */
  int splitnum; /* voice split number */
  int numnotesoverlap; // numero de notas solapantes (en acordes, voces etc)
  int isnote;
  int ischord;
  /* int denom; */
};


/* linked list of notes */
struct listx {
  struct listx* next;
  struct anote* note;
};

/* linked list of text items (strings) */
struct tlistx {
  struct tlistx* next;
  char* text;
  long when; 	/* time in pulses to output */
  int type;     /* 0 - comments, other - field commands */
};


/* a MIDI track */
struct atrack {
  struct listx* head;    /* first note */
  struct listx* tail;    /* last note */
  struct tlistx* texthead; /* first text string */
  struct tlistx* texttail; /* last text string */
  int notes;             /* number of notes in track */
  long tracklen;
  long startwait;
  int startunits;
  int drumtrack;
};

/* can cope with up to 64 track MIDI files */
struct atrack track[64];
int trackcount = 0;
int maxbarcount = 0;
/* maxbarcount  is used to return the numbers of bars created.*/
/* obpl is a flag for one bar per line. */

/* double linked list of notes */
/* used for temporary list of chords while abc is being generated */
struct dlistx {
  struct dlistx* next;
  struct dlistx* last;
  struct anote* note;
};

int notechan[2048],notechanvol[2048]; /*for linking on and off midi
					channel commands            */
int last_tick; /* for getting last pulse number in MIDI file */

char *title = NULL; /* for pasting title from argv[] */
char *origin = NULL; /* for adding O: info from argv[] */


void remove_carriage_returns(char *);
int validnote(int);
void printpitch(struct anote*);
void printfract(int, int);

/*
 int (*Mf_getc)();
 void (*Mf_header)();
 void (*Mf_trackstart)();
 void (*Mf_trackend)();
 void (*Mf_noteon)();
 void (*Mf_noteoff)();
 void (*Mf_pressure)();
 void (*Mf_parameter)();
 void (*Mf_pitchbend)();
 void (*Mf_program)();
 void (*Mf_chanpressure)();
 void (*Mf_sysex)();
 void (*Mf_metamisc)();
 void (*Mf_seqspecific)();
 void (*Mf_seqnum)();
 void (*Mf_text)();
 void (*Mf_eot)();
 void (*Mf_timesig)();
 void (*Mf_smpte)();
 void (*Mf_tempo)();
 void (*Mf_keysig)();
 void (*Mf_arbitrary)();
 void (*Mf_error)();
*/
int (*Mf_getc)();
void (*Mf_header)(int,int,int);
void (*Mf_trackstart)();
void (*Mf_trackend)();
void (*Mf_noteon)(int,int,int);
void (*Mf_noteoff)(int,int,int);
void (*Mf_pressure)(int,int,int);
void (*Mf_parameter)(int,int,int);
void (*Mf_pitchbend)(int,int,int);
void (*Mf_program)(int,int);
void (*Mf_chanpressure)(int,int);
void (*Mf_sysex)(int,char*);
void (*Mf_metamisc)(int,int,char*);
void (*Mf_seqspecific)(int,char*);
void (*Mf_seqnum)(int);
void (*Mf_text)(int,int,char *);
void (*Mf_eot)();
void (*Mf_timesig)(char,char,char,char);
void (*Mf_smpte)(char,char,char,char,char);
void (*Mf_tempo)(long);
void (*Mf_keysig)(char,char);
void (*Mf_arbitrary)(int,char *);
void (*Mf_error)(char *);

/*              Stage 1. Parsing MIDI file                   */

/* Functions called during the reading pass of the MIDI file */

/* The following C routines are required by midifilelib.  */
/* They specify the action to be taken when various items */
/* are encountered in the MIDI.  The mfread function scans*/
/* the MIDI file and calls these functions when needed.   */

extern int ppqn;
extern long mf_sec2ticks(float secs,int division,long tempo);
extern void clear_extern_var_midifile(void);

void clear_extern_var_midi2abc(void)
{
   F=NULL;
   outhandle=NULL;
   tracknum=0;  /* track number */
   division=0;    /* pulses per quarter note defined in MIDI header    */
   tempo = 500000; /* the default tempo is 120 quarter notes/minute */
   unitlen=0;     /* abc unit length usually defined in L: field       */
   header_unitlen=0; /* first unitlen set                              */
   unitlen_set =0; /* once unitlen is set don't allow it to change   */
   parts_per_unitlen = 2; /* specifies minimum quantization size */
   laston = 0; /* length of MIDI track in pulses or ticks           */
   textbuff[0]=0; /*buffer for handling text output to abc file*/
//int trans[256], back[256]; /*translation tables for MIDI pitch to abc note*/
//char atog[256]; /* translation tables for MIDI pitch to abc note */
//int symbol[256]; /*translation tables for MIDI pitch to abc note */
//int key[12];
   sharps=0;
   trackno=maintrack=0;
   format=0; /* MIDI file type                                   */
   karaoke=inkaraoke=0;
   midline=0;
   tempocount=0;  /* number of tempo indications in MIDI file */
   gotkeysig=0; /*set to 1 if keysignature found in MIDI file */
   xunit=0;    /* pulses per abc unit length                     */
   tsig_set=0; /* flag - time signature already set by user      */
   ksig_set=0; /* flag - key signature already set by user       */
   xunit_set=0;/* flat - xunit already set by user               */
   extracta=0; /* flag - get anacrusis from strong beat          */
   guessu=0;   /* flag - estimate xunit from note durations      */
   guessa=0;   /* flag - get anacrusis by minimizing tied notes  */
   guessk=0;   /* flag - guess key signature                     */
   summary=0;  /* flag - output summary info of MIDI file        */
   keep_short=0; /*flag - preserve short notes                   */
   swallow_rests=0; /* flag - absorb short rests                 */
   midiprint=0; /* flag - run midigram instead of midi2abc       */
   #ifdef SPLITCODE
   usesplits=0; /* flag - split measure into parts if needed     */
   #endif
   restsize=0; /* smallest rest to absorb                        */
   no_triplets=0; /* flag - suppress triplets or broken rhythm   */
   obpl = 0; /* flag to specify one bar per abc text line      */
   nogr = 0; /* flag to put a space between every note         */
   bars_per_line=4;  /* number of bars per output line         */
   bars_per_staff=4; /* number of bars per music staff         */
   asig=bsig=0;  /* time signature asig/bsig                    */
   header_asig =0; /* first time signature encountered         */
   header_bsig =0; /* first time signature encountered         */
   header_bb=0;      /* first ticks/quarter note encountered     */
   active_asig=active_bsig=0;  /* last time signature declared   */
   last_asig=last_ksig=0; /* last time signature printed        */
   barsize=0; /* barsize in parts_per_unitlen units                   */
   Qval=0;        /* tempo - quarter notes per minute            */
   verbosity=0; /* control amount of detail messages in abcfile*/
   anacrusis=0; 
   bars=0;
   keysig=0;
   header_keysig=  -50;  /* header key signature                     */
   active_keysig = -50;  /* last key signature declared        */
   xchannel=0;  /* channel number to be extracted. -1 means all  */
   //struct atrack track[64];
   trackcount = 0;
   maxbarcount = 0;
   //int notechan[2048],notechanvol[2048]; /*for linking on and off midi					channel commands            */
   last_tick=0; /* for getting last pulse number in MIDI file */
   title = NULL; /* for pasting title from argv[] */
   origin = NULL; /* for adding O: info from argv[] */
}

int filegetc()
{
    return(getc(F));
}

int filepeekc(int can)
{
	int c[256],d,r;
    
	for(r=0;r<can;r++)
	   c[r]=getc(F);
	d=c[r-1];
	for(r=can-1;r>=0;r--)
	   ungetc(c[r],F);
	return d;
}

void fatal_error(char *s)
/* fatal error encounterd - abort program */
{
  fprintf(stderr, "%s\n", s);
//  exit(1);
}


void event_error(char *s)
/* problem encountered but OK to continue */
{
  char msg[256];

  sprintf(msg, "Error: Time=%ld Track=%d %s\n", Mf_currtime, trackno, s);
  printf("%s",msg);
}


int* checkmalloc(int bytes)
/* malloc with error checking */
{
  int *p;

  p = (int*) malloc(bytes);
  if (p == NULL) {
    fatal_error("Out of memory error - cannot malloc!");
  };
  return (p);
}



char* addstring(char *s)
/* create space for string and store it in memory */
{
  char* p;

  p = (char*) checkmalloc(strlen(s)+1);
  strcpy(p, s);
  return(p);
}

void addtext(char *s, int type)
/* add structure for text */
/* used when parsing MIDI file */
{
  struct tlistx* newx;

  newx = (struct tlistx*) checkmalloc(sizeof(struct tlistx));
  newx->next = NULL;
  newx->text = addstring(s);
  newx->type = type;
  newx->when = Mf_currtime;
  if (track[trackno].texthead == NULL) {
    track[trackno].texthead = newx;
    track[trackno].texttail = newx;
  }
  else {
    track[trackno].texttail->next = newx;
    track[trackno].texttail = newx;
  };
}
  


/* The MIDI file has  separate commands for starting                 */
/* and stopping a note. In order to determine the duration of        */
/* the note it is necessary to find the note_on command associated   */
/* with the note off command. We rely on the note's pitch and channel*/
/* number to find the right note. While we are parsing the MIDI file */
/* we maintain a list of all the notes that are currently on         */
/* head and tail of list of notes still playing.                     */
/* The following doubly linked list is used for this purpose         */

struct dlistx* playinghead;
struct dlistx* playingtail; 


void noteplaying(struct anote* p)
/* This function adds a new note to the playinghead list. */
{
  struct dlistx* newx;

  newx = (struct dlistx*) checkmalloc(sizeof(struct dlistx));
  newx->note = p;
  newx->next = NULL;
  newx->last = playingtail;
  if (playinghead == NULL) {
    playinghead = newx;
  };
  if (playingtail == NULL) {
    playingtail = newx;
  } 
  else {
    playingtail->next = newx;
    playingtail = newx;
  };
}


void addnote(int p, int ch, int v)
/* add structure for note */
/* used when parsing MIDI file */
{
  struct listx* newx;
  struct anote* newnote;

  track[trackno].notes = track[trackno].notes + 1;
  //******************   jordi
  //if(trackno==2)
//	  printf("jordi");
//***********************
  newx = (struct listx*) checkmalloc(sizeof(struct listx));
  newnote = (struct anote*) checkmalloc(sizeof(struct anote));
  newx->next = NULL;
  newx->note = newnote;
  if (track[trackno].head == NULL) {
    track[trackno].head = newx;
    track[trackno].tail = newx;
  } 
  else {
    track[trackno].tail->next = newx;
    track[trackno].tail = newx;
  };
  if (ch == 9) {
    track[trackno].drumtrack = 1;
  };
  newnote->pitch = p;
  newnote->chan = ch;
  newnote->vel = v;
  newnote->time = Mf_currtime;
  laston = Mf_currtime;
  newnote->tplay = Mf_currtime;
  noteplaying(newnote);
}



void notestop(int p, int ch)
/* MIDI note stops */
/* used when parsing MIDI file */
{
  struct dlistx* i;
  int found;
  char msg[80];

  i = playinghead;
  found = 0;
  while ((found == 0) && (i != NULL)) {
    if ((i->note->pitch == p)&&(i->note->chan==ch)) {
      found = 1;
    } 
    else {
      i = i->next;
    };
  };
  if (found == 0) {
    sprintf(msg, "Note terminated when not on - pitch %d", p);
    event_error(msg);
    return;
  };
  /* fill in tplay field */
  i->note->tplay = Mf_currtime - (i->note->tplay);
  i->note->tinitnoteoff=Mf_currtime;  // instante de tiempo en que llega el noteoff
  i->note->toff=0;  // se calculara a porteriori

  // para calcular toff hay que buscar la nota cuyo time es inmediatamente superior al
  // tinitnoteoff, toff sera time-tinitnoteoff

  /* remove note from list */
  if (i->last == NULL) {
    playinghead = i->next;
  } 
  else {
    (i->last)->next = i->next;
  };
  if (i->next == NULL) {
    playingtail = i->last;
  } 
  else {
    (i->next)->last = i->last;
  };
  free(i);
}




FILE *
efopen(char *name,char *mode)
{
    FILE *f;

    if ( (f=fopen(name,mode)) == NULL ) {
      char msg[256];
      sprintf(msg,"Error - Cannot open file %s",name);
      fatal_error(msg);
    }
    return(f);
}


void error(char *s)
{
    fprintf(stderr,"Error: %s\n",s);
}



void txt_header(int xformat,int ntrks,int ldivision)
{
    division = ldivision; 
    format = xformat;
    if (format != 0) {
    /*  fprintf(outhandle,"%% format %d file %d tracks\n", format, ntrks);*/
      if(summary>0) printf("This midi file has %d tracks\n\n",ntrks);
    } 
    else {
/*     fprintf(outhandle,"%% type 0 midi file\n"); */
     if(summary>0) {
	     printf("This is a type 0 midi file.\n");
             printf("All the channels are in one track.\n");
             printf("You may need to process the channels separately\n\n");
            }
     }
     
}


void txt_trackstart()
{
  laston = 0L;
  track[trackno].notes = 0;
  track[trackno].head = NULL;
  track[trackno].tail = NULL;
  track[trackno].texthead = NULL;
  track[trackno].texttail = NULL;
  track[trackno].tracklen = Mf_currtime;
  track[trackno].drumtrack = 0;
}

void txt_trackend()
{
  /* check for unfinished notes */
  if (playinghead != NULL) {
    printf("Error in MIDI file - notes still on at end of track!\n");
  };
  track[trackno].tracklen = Mf_currtime - track[trackno].tracklen;
  trackno = trackno + 1;
  trackcount = trackcount + 1;
}

void txt_noteon(int chan,int pitch,int vol)
{
  if ((xchannel == -1) || (chan == xchannel)) {
    if (vol != 0) {
      addnote(pitch, chan, vol);
    } 
    else {
      notestop(pitch, chan);
    };
  };
}

void txt_noteoff(int chan,int pitch,int vol)
{
  if ((xchannel == -1) || (chan == xchannel)) {
    notestop(pitch, chan);
  };
}

void txt_pressure(int chan,int pitch,int press)
{
}

void txt_parameter(int chan,int control,int value)
{
}

void txt_pitchbend(int chan,int msb,int lsb)
{
}

void txt_program(int chan,int program)
{
/*
  sprintf(textbuff, "%%%%MIDI program %d %d",
         chan+1, program);
*/
  sprintf(textbuff, "%%%%MIDI program %d", program);
  addtext(textbuff,0);
/* abc2midi does not use the same channel number as specified in 
  the original midi file, so we should not specify that channel
  number in the %%MIDI program. If we leave it out the program
  will refer to the current channel assigned to this voice.
*/
}

void txt_chanpressure(int chan,int press)
{
}

void txt_sysex(int leng,char *mess)
{
}

void txt_metamisc(int type,int leng,char *mess)
{
}

void txt_metaspecial(int a,char *mess)
{
}

void txt_metatext(int type,int leng,char *mess)
{ 
    char *ttype[] = {
    NULL,
    "Text Event",        /* type=0x01 */
    "Copyright Notice",    /* type=0x02 */
    "Sequence/Track Name",
    "Instrument Name",    /* ...     */
    "Lyric",
    "Marker",
    "Cue Point",        /* type=0x07 */
    "Unrecognized"
  };
  int unrecognized = (sizeof(ttype)/sizeof(char *)) - 1;
  unsigned char c;
  int n;
  char *p = mess;
  char *buff;
  char buffer2[BUFFSIZE];

  if ((type < 1)||(type > unrecognized))
      type = unrecognized;
  buff = textbuff;
  for (n=0; n<leng; n++) {
    c = *p++;
    if (buff - textbuff < BUFFSIZE - 6) {
      sprintf(buff, 
           (isprint(c)||isspace(c)) ? "%c" : "\\0x%02x" , c);
      buff = buff + strlen(buff);
    };
  }
  if (strncmp(textbuff, "@KMIDI KARAOKE FILE", 14) == 0) {
    karaoke = 1;
  } 
  else {
    if ((karaoke == 1) && (*textbuff != '@')) {
      addtext(textbuff,0);
    } 
    else {
      if (leng < BUFFSIZE - 3) {
        sprintf(buffer2, " %s", textbuff); 
        addtext(buffer2,0);
      };
    };
  };
}

void txt_metaseq(int num)
{  
  sprintf(textbuff, "%%Meta event, sequence number = %d",num);
  addtext(textbuff,0);
}

void txt_metaeot()
/* Meta event, end of track */
{
}

void txt_keysig(char sf,char mi)
{
  int accidentals;
  gotkeysig =1;
  sprintf(textbuff, 
         "%% MIDI Key signature, sharp/flats=%d  minor=%d",
          (int) sf, (int) mi);
  if(verbosity) addtext(textbuff,0);
  sprintf(textbuff,"%d %d\n",sf,mi);
  if (!ksig_set) {
	  addtext(textbuff,1);
	  keysig=sf;
  }
  if (header_keysig == -50) header_keysig = keysig;
  if (summary <= 0) return;
  /* There may be several key signature changes in the midi
     file so that key signature in the mid file does not conform
     with the abc file. Show all key signature changes. 
  */   
  accidentals = (int) sf;
  if (accidentals <0 )
    {
    accidentals = -accidentals;
    printf("Key signature: %d flats", accidentals);
    }
  else
     printf("Key signature : %d sharps", accidentals);
  if (ksig_set) printf(" suppressed\n");
  else printf("\n");
}

void txt_tempo(long ltempo)
{
    if(tempocount>0) return; /* ignore other tempo indications */
    tempo = ltempo;
    tempocount++;
}


void setup_timesig(int nn,int denom,int bb)
{
  asig = nn;
  bsig = denom;
/* we must keep unitlen and xunit fixed for the entire tune */
  if (unitlen_set == 0) {
    unitlen_set = 1; 
    if ((asig*4)/bsig >= 3) {
      unitlen =8;
      } 
    else {
      unitlen = 16;
      };
   }
/* set xunit for this unitlen */
  if(!xunit_set) xunit = (division*bb*4)/(8*unitlen);
  barsize = parts_per_unitlen*asig*unitlen/bsig;
/*  printf("setup_timesig: unitlen=%d xunit=%d barsize=%d\n",unitlen,xunit,barsize); */
  if (header_asig ==0) {header_asig = asig;
	                header_bsig = bsig;
			header_unitlen = unitlen;
			header_bb = bb;
                       }
}


void txt_timesig(char cnn,char cdd,char ccc,char cbb)
{
  int nn,dd,cc,bb;

  nn=cnn; dd=cdd; cc=ccc; bb=cbb;
  int denom = 1;
  while ( dd-- > 0 )
    denom *= 2;
  sprintf(textbuff, 
          "%% Time signature=%d/%d  MIDI-clocks/click=%d  32nd-notes/24-MIDI-clocks=%d", 
    nn,denom,cc,bb);
  if (verbosity) addtext(textbuff,0);
  sprintf(textbuff,"%d %d %d\n",nn,denom,bb);
  if (!tsig_set) {
	  addtext(textbuff,2);
          setup_timesig(nn, denom,bb);
   }
  if (summary>0) {
    if(tsig_set) printf("Time signature = %d/%d suppressed\n",nn,denom);
    else printf("Time signature = %d/%d\n",nn,denom);
    }
}


void txt_smpte(char hr,char mn,char se,char fr,char ff)
{
}

void txt_arbitrary(int leng,char *mess)
{
}



/* Dummy functions for handling MIDI messages.
 *    */
 void no_op0() {}
 void no_op1(int dummy1) {}
 void no_op2(int dummy1, int dummy2) {}
 void no_op3(int dummy1, int dummy2, int dummy3) { }
 void no_op4(int dummy1, int dummy2, int dummy3, int dummy4) { }
 void no_op5(int dummy1, int dummy2, int dummy3, int dummy4, int dummy5) { }

 void no_op2c(int dummy1, char *dummy2) {}
 void no_op3c(int dummy1, int dummy2, char *dummy3) { }
 void no_op4c(char,char, char,char) { }
 void no_op5c(char,char, char,char,char) { }
 void no_op1l(long dummy1) {}
 void no_op2cc(char dummy1, char dummy2) {}

void print_txt_noteon(int chan,int pitch,int vol)
{
int start_time;
int initvol;
if (vol > 0)
open_note(chan, pitch, vol);
else {
  start_time = close_note(chan, pitch,&initvol);
  if (start_time >= 0)
     /* printf("%8.4f %8.4f %d %d %d %d\n",
       (double) start_time/(double) division,
       (double) Mf_currtime/(double) division,
       trackno+1, chan +1, pitch,initvol);
     */
       printf("%d %ld %d %d %d %d\n",
       start_time, Mf_currtime, trackno+1, chan +1, pitch,initvol);

      if(Mf_currtime > last_tick) last_tick = Mf_currtime;
   }
}



void print_txt_noteoff(int chan,int  pitch,int vol)
{
int start_time,initvol;

start_time = close_note(chan, pitch, &initvol);
if (start_time >= 0)
/*
    printf("%8.4f %8.4f %d %d %d %d\n",
     (double) start_time/(double) division,
     (double) Mf_currtime/(double) division,
     trackno+1, chan+1, pitch,initvol);
*/
     printf("%d %ld %d %d %d %d\n",
       start_time, Mf_currtime, trackno+1, chan +1, pitch,initvol);
    if(Mf_currtime > last_tick) last_tick = Mf_currtime;
}



/* In order to associate a channel note off message with its
 * corresponding note on message, we maintain the information
 * the notechan array. When a midi pitch (0-127) is switched
 * on for a particular channel, we record the time that it
 * was turned on in the notechan array. As there are 16 channels
 * and 128 pitches, we initialize an array 128*16 = 2048 elements
 * long.
**/
void init_notechan()
{
/* signal that there are no active notes */
 int i;
 for (i = 0; i < 2048; i++) notechan[i] = -1;
}


/* The next two functions update notechan when a channel note on
   or note off is encountered. The second function close_note,
   returns the time when the note was turned on.
*/
int open_note(int chan, int pitch, int vol)
{
    notechan[128 * chan + pitch] = Mf_currtime;
    notechanvol[128 * chan + pitch] = vol;
    return 0;
}


int close_note(int chan, int pitch, int *initvol)
{
    int index, start_tick;
    index = 128 * chan + pitch;
    if (notechan[index] < 0)
	return -1;
    start_tick = notechan[index];
    *initvol = notechanvol[index];
    notechan[index] = -1;
    return start_tick;
}


/* mftext mode */
int prtime()
{
/*  if(Mf_currtime >= pulses) ignore=0; 
  if (ignore) return 1; 
  linecount++;
  if(linecount > maxlines) {fclose(F); exit(0);}
*/
  int units;
  units = 2;
  if(units==1)
 /*seconds*/
     printf("%6.2f   ",mf_ticks2sec(Mf_currtime,division,tempo));
  else if (units==2)
 /*beats*/
     printf("%6.2f   ",(float) Mf_currtime/(float) division);
  else
 /*pulses*/
    printf("%6ld  ",Mf_currtime);
  return 0;
}

char * pitch2key(int note)
{
static char name[5];
char* s = name;
  switch(note % 12)
  {
  case 0: *s++ = 'c'; break;
  case 1: *s++ = 'c'; *s++ = '#'; break;
  case 2: *s++ = 'd'; break;
  case 3: *s++ = 'd'; *s++ = '#'; break;
  case 4: *s++ = 'e'; break;
  case 5: *s++ = 'f'; break;
  case 6: *s++ = 'f'; *s++ = '#'; break;
  case 7: *s++ = 'g'; break;
  case 8: *s++ = 'g'; *s++ = '#'; break;
  case 9: *s++ = 'a'; break;
  case 10: *s++ = 'a'; *s++ = '#'; break;
  case 11: *s++ = 'b'; break;
  }
  sprintf(s, "%d", (note / 12)-1);  /* octave  (assuming Piano C4 is 60)*/
  return  name;
}


void pitch2drum(int midipitch)
{
static char *drumpatches[] = {
 "Acoustic Bass Drum", "Bass Drum 1", "Side Stick", "Acoustic Snare",
 "Hand Clap", "Electric Snare", "Low Floor Tom", "Closed Hi Hat",
 "High Floor Tom", "Pedal Hi-Hat", "Low Tom", "Open Hi-Hat",
 "Low-Mid Tom", "Hi Mid Tom", "Crash Cymbal 1", "High Tom",		
 "Ride Cymbal 1", "Chinese Cymbal", "Ride Bell", "Tambourine",
 "Splash Cymbal", "Cowbell", "Crash Cymbal 2", "Vibraslap",
 "Ride Cymbal 2", "Hi Bongo", "Low Bongo",	"Mute Hi Conga",
 "Open Hi Conga", "Low Conga", "High Timbale", "Low Timbale",
 "High Agogo", "Low Agogo", "Cabasa", "Maracas",
 "Short Whistle", "Long Whistle", "Short Guiro", "Long Guiro",
 "Claves", "Hi Wood Block", "Low Wood Block", "Mute Cuica",
 "Open Cuica", "Mute Triangle", "Open Triangle" };
if (midipitch >= 35 && midipitch <= 81) {
  printf(" (%s)",drumpatches[midipitch-35]);
  }
}


void mftxt_header (int format, int ntrks, int ldivision)
{
  division = ldivision;
  printf("Header format=%d ntrks=%d division=%d\n",format,ntrks,division);
}

void mftxt_trackstart()
{
  int numbytes;
  tracknum++;
  numbytes = Mf_toberead;
  /*if(track != 0 && tracknum != track) {ignore_bytes(numbytes); return;} */
  printf("Track %d contains %d bytes\n",tracknum,numbytes);
}


void mftxt_noteon(int chan,int pitch,int vol)
{
  char *key;
/*
  if (onlychan >=0 && chan != onlychan) return;
*/
  if (prtime()) return;
  key = pitch2key(pitch);
  printf("Note on  %2d  %3s %3d",chan+1, key,vol);
  if (chan == 9) pitch2drum(pitch);
  printf("\n");
}

void mftxt_noteoff(int chan,int pitch,int vol)
{
  char *key;
/*
  if (onlychan >=0 && chan != onlychan) return;
*/
  if (prtime()) return;
  key = pitch2key(pitch);
  printf("Note off %2d  %3s %3d\n",chan+1,key,vol);
}

void mftxt_pressure(int chan,int pitch,int press)
{
  char *key;
  if (prtime()) return;
  key = pitch2key(pitch);
  printf("Pressure %2d   %3s %3d\n",chan+1,key,press);
}


void mftxt_pitchbend(int chan,int msb,int lsb)
{
/*
  if (onlychan >=0 && chan != onlychan) return;
*/
  if (prtime()) return;
  printf("Pitchbnd %2d msb=%d lsb=%d\n",chan+1,msb,lsb);
}



void mftxt_program(int chan,int program)
{
static char *patches[] = {
 "Acoustic Grand","Bright Acoustic","Electric Grand","Honky-Tonk", 
 "Electric Piano 1","Electric Piano 2","Harpsichord","Clav", 
 "Celesta", "Glockenspiel",  "Music Box",  "Vibraphone", 
 "Marimba", "Xylophone", "Tubular Bells", "Dulcimer", 
 "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ", 
 "Reed Organ", "Accordian", "Harmonica", "Tango Accordian",
 "Acoustic Guitar (nylon)", "Acoustic Guitar (steel)",
 "Electric Guitar (jazz)", "Electric Guitar (clean)", 
 "Electric Guitar (muted)", "Overdriven Guitar",
 "Distortion Guitar", "Guitar Harmonics",
 "Acoustic Bass", "Electric Bass (finger)",
 "Electric Bass (pick)", "Fretless Bass",
 "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2",
 "Violin", "Viola", "Cello", "Contrabass",
 "Tremolo Strings", "Pizzicato Strings",
 "Orchestral Strings", "Timpani",
 "String Ensemble 1", "String Ensemble 2",
 "SynthStrings 1", "SynthStrings 2", 
 "Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit",
 "Trumpet", "Trombone", "Tuba", "Muted Trumpet",
 "French Horn", "Brass Section", "SynthBrass 1", "SynthBrass 2",
 "Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax",
 "Oboe", "English Horn", "Bassoon", "Clarinet",
 "Piccolo", "Flute", "Recorder", "Pan Flute",
 "Blown Bottle", "Skakuhachi", "Whistle", "Ocarina",
 "Lead 1 (square)", "Lead 2 (sawtooth)",
 "Lead 3 (calliope)", "Lead 4 (chiff)", 
 "Lead 5 (charang)", "Lead 6 (voice)",
 "Lead 7 (fifths)", "Lead 8 (bass+lead)",
 "Pad 1 (new age)", "Pad 2 (warm)",
 "Pad 3 (polysynth)", "Pad 4 (choir)",
 "Pad 5 (bowed)", "Pad 6 (metallic)",
 "Pad 7 (halo)", "Pad 8 (sweep)",
 "FX 1 (rain)", "(soundtrack)",
 "FX 3 (crystal)", "FX 4 (atmosphere)",
 "FX 5 (brightness)", "FX 6 (goblins)",
 "FX 7 (echoes)", "FX 8 (sci-fi)",
 "Sitar", "Banjo", "Shamisen", "Koto",
 "Kalimba", "Bagpipe", "Fiddle", "Shanai",
 "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock",
 "Taiko Drum", "Melodic Tom", "Synth Drum", "Reverse Cymbal",
 "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet",
 "Telephone ring", "Helicopter", "Applause", "Gunshot"};
/*
  if (onlychan >=0 && chan != onlychan) return;
*/
  if (prtime()) return;
  printf("Program  %2d %d (%s)\n",chan+1, program,patches[program]);
   }

void mftxt_chanpressure(int chan,int press)
{
  prtime();
  printf("Chanpres %2d pressure=%d\n",chan+1,press);
}


void mftxt_parameter(int chan,int control,int value)
{
  static char *ctype[] = {
 "Bank Select",       "Modulation Wheel",     /*1*/
 "Breath controller", "unknown",              /*3*/
 "Foot Pedal",        "Portamento Time",      /*5*/
 "Data Entry",        "Volume",               /*7*/
 "Balance",           "unknown",              /*9*/
 "Pan position",      "Expression",           /*11*/
 "Effect Control 1",  "Effect Control 2",     /*13*/
 "unknown",           "unknown",              /*15*/
 "Slider 1",          "Slider 2",             /*17*/
 "Slider 3",          "Slider 4",             /*19*/
 "unknown",           "unknown",              /*21*/
 "unknown",           "unknown",              /*23*/
 "unknown",           "unknown",              /*25*/
 "unknown",           "unknown",              /*27*/
 "unknown",           "unknown",              /*29*/
 "unknown",           "unknown",              /*31*/
 "Bank Select (fine)",  "Modulation Wheel (fine)",    /*33*/
 "Breath controller (fine)",  "unknown",              /*35*/
 "Foot Pedal (fine)",   "Portamento Time (fine)",     /*37*/
 "Data Entry (fine)",   "Volume (fine)",              /*39*/
 "Balance (fine)",      "unknown",                    /*41*/
 "Pan position (fine)", "Expression (fine)",          /*43*/
 "Effect Control 1 (fine)",  "Effect Control 2 (fine)", /*45*/
 "unknown",           "unknown",             /*47*/
 "unknown",           "unknown",             /*49*/
 "unknown",           "unknown",             /*51*/
 "unknown",           "unknown",             /*53*/
 "unknown",           "unknown",             /*55*/
 "unknown",           "unknown",             /*57*/
 "unknown",           "unknown",             /*59*/
"unknown",           "unknown",             /*61*/
 "unknown",           "unknown",             /*63*/
 "Hold Pedal",        "Portamento",          /*65*/
 "Susteno Pedal",     "Soft Pedal",          /*67*/
 "Legato Pedal",      "Hold 2 Pedal",        /*69*/
 "Sound Variation",   "Sound Timbre",        /*71*/
 "Sound Release Time",  "Sound Attack Time", /*73*/
 "Sound Brightness",  "Sound Control 6",     /*75*/
 "Sound Control 7",   "Sound Control 8",     /*77*/
 "Sound Control 9",   "Sound Control 10",    /*79*/
 "GP Button 1",       "GP Button 2",         /*81*/
 "GP Button 3",       "GP Button 4",         /*83*/
 "unknown",           "unknown",             /*85*/
 "unknown",           "unknown",             /*87*/
 "unknown",           "unknown",             /*89*/
 "unknown",           "Effects Level",       /*91*/
 "Tremolo Level",     "Chorus Level",        /*93*/
 "Celeste Level",     "Phaser Level",        /*95*/
 "Data button increment",  "Data button decrement", /*97*/
 "NRP (fine)",        "NRP (coarse)",        /*99*/
 "Registered parameter (fine)", "Registered parameter (coarse)", /*101*/
 "unknown",           "unknown",             /*103*/
 "unknown",           "unknown",             /*105*/
 "unknown",           "unknown",             /*107*/
 "unknown",           "unknown",             /*109*/
 "unknown",           "unknown",             /*111*/
 "unknown",           "unknown",             /*113*/
 "unknown",           "unknown",             /*115*/
 "unknown",           "unknown",             /*117*/
 "unknown",           "unknown",             /*119*/
 "All Sound Off",     "All Controllers Off", /*121*/
 "Local Keyboard (on/off)","All Notes Off",  /*123*/
 "Omni Mode Off",     "Omni Mode On",        /*125*/
 "Mono Operation",    "Poly Operation"};

/*  if (onlychan >=0 && chan != onlychan) return; */
  if (prtime()) return;

  printf("CntlParm %2d %s = %d\n",chan+1, ctype[control],value);
}


void mftxt_metatext(int type,int leng,char *mess)
{
  static char *ttype[] = {
    NULL,
    "Text Event",    /* type=0x01 */
    "Copyright Notice",  /* type=0x02 */
    "Seqnce/Track Name",
    "Instrument Name",  /* ...       */
    "Lyric",
    "Marker",
    "Cue Point",    /* type=0x07 */
    "Unrecognized"
  };
  int unrecognized = (sizeof(ttype)/sizeof(char *)) - 1;
  int len;
  register int n, c;
  register char *p = mess;

  if ( type < 1 || type > unrecognized )
    type = unrecognized;
  if (prtime()) return;
  printf("Metatext (%s) ",ttype[type]);
  len = leng;
  if (len > 15) len = 15;
  for ( n=0; n<len; n++ ) {
    c = (*p++) & 0xff;
    if(iscntrl(c)) {printf(" \\0x%02x",c); continue;} /* no <cr> <lf> */
    printf( (isprint(c)||isspace(c)) ? "%c" : "\\0x%02x" , c);
  }
  if (leng>15) printf("...");
  printf("\n");
}

void mftxt_keysig(char csf,char cmi)
{
  int sf,mi;
  sf=csf; mi=cmi;
  static char *major[] = {"Cb", "Gb", "Db", "Ab", "Eb", "Bb", "F",
    "C", "G", "D", "A", "E", "B", "F#", "C#"};
  static char *minor[] = {"Abmin", "Ebmin", "Bbmin", "Fmin", "Cmin",
    "Gmin", "Dmin", "Amin", "Emin", "Bmin", "F#min", "C#min", "G#min"};
  int index;
  index = sf + 7;
  if (prtime()) return;
  if (mi)
    printf("Metatext key signature %s (%d/%d)\n",minor[index],sf,mi);
  else
    printf("Metatext key signature %s (%d/%d)\n",major[index],sf,mi);
}

void mftxt_tempo(long ltempo)
{
  tempo = ltempo;
  if (prtime()) return;
  printf("Metatext tempo = %6.2f bpm\n",60000000.0/tempo);
}

void mftxt_timesig(char cnn,char cdd,char ccc,char cbb)
{
  int nn,dd,cc,bb;
  int denom = 1;

  nn=cnn; dd=cdd; cc=ccc; bb=cbb;
  while ( dd-- > 0 )
    denom *= 2;
  if (prtime()) return;
  printf("Metatext time signature=%d/%d\n",nn,denom);
/*  printf("Time signature=%d/%d  MIDI-clocks/click=%d \
  32nd-notes/24-MIDI-clocks=%d\n", nn,denom,cc,bb); */
}

void mftxt_smpte(char chr,char cmn,char cse,char cfr,char cff)
{
  int hr,mn,se,fr,ff;
  hr=chr; mn=cmn; se=cse; fr=cfr; ff=cff;
  if (prtime()) return;
  printf("Metatext SMPTE, %d:%d:%d  %d=%d\n", hr,mn,se,fr,ff);
}

void mftxt_metaeot()
{
  if (prtime()) return;
  printf("Meta event, end of track\n");
}


void initfunc_for_midinotes()
{
    Mf_error =error;
    Mf_header =txt_header;
    Mf_trackstart =txt_trackstart;
    Mf_trackend =txt_trackend;
    Mf_noteon =print_txt_noteon;
    Mf_noteoff =print_txt_noteoff;
    Mf_pressure = no_op3;
    Mf_parameter = no_op3;
    Mf_pitchbend = no_op3;
    Mf_program = no_op2;
    Mf_chanpressure = no_op2;
    Mf_sysex = no_op2c;
    Mf_metamisc =no_op3c;
    Mf_seqnum = no_op1;
    Mf_eot = no_op0;
    Mf_timesig = no_op4c;
    Mf_smpte =  no_op5c;
    Mf_tempo = no_op1l;
    Mf_keysig = no_op2cc;
    Mf_seqspecific = no_op2c;
    Mf_text = no_op3c;
    Mf_arbitrary = no_op2c;
}


void initfunc_for_mftext()
{
    Mf_error =error;
    Mf_header = mftxt_header;
    Mf_trackstart = mftxt_trackstart;
    Mf_trackend = txt_trackend;
    Mf_noteon = mftxt_noteon;
    Mf_noteoff = mftxt_noteoff;
    Mf_pressure = mftxt_pressure;
    Mf_parameter =  mftxt_parameter;
    Mf_pitchbend =  mftxt_pitchbend;
    Mf_program =  mftxt_program;
    Mf_chanpressure =  mftxt_chanpressure;
    Mf_sysex =  no_op2c;
    Mf_metamisc =  no_op3c;
    Mf_seqnum =  no_op1;
    Mf_eot =  mftxt_metaeot;
    Mf_timesig =  mftxt_timesig;
    Mf_smpte =  mftxt_smpte;
    Mf_tempo =  mftxt_tempo;
    Mf_keysig =  mftxt_keysig;
    Mf_seqspecific =  no_op2c;
    Mf_text =  mftxt_metatext;
    Mf_arbitrary =  no_op2c;
}




void initfuncs()
{
    Mf_error =  error;
    Mf_header =   txt_header;
    Mf_trackstart =   txt_trackstart;
    Mf_trackend =   txt_trackend;
    Mf_noteon =   txt_noteon;
    Mf_noteoff =   txt_noteoff;
    Mf_pressure =   txt_pressure;
    Mf_parameter =   txt_parameter;
    Mf_pitchbend =   txt_pitchbend;
    Mf_program =   txt_program;
    Mf_chanpressure =   txt_chanpressure;
    Mf_sysex =   txt_sysex;
    Mf_metamisc =   txt_metamisc;
    Mf_seqnum =   txt_metaseq;
    Mf_eot =   txt_metaeot;
    Mf_timesig =   txt_timesig;
    Mf_smpte =   txt_smpte;
    Mf_tempo =   txt_tempo;
    Mf_keysig =   txt_keysig;
    Mf_seqspecific =   txt_metaspecial;
    Mf_text =   txt_metatext;
    Mf_arbitrary =   txt_arbitrary;
}


/*  Stage 2 Quantize MIDI tracks. Get key signature, time signature...   */ 



/* This routine calculates the time interval before the next note */
/* called after the MIDI file has been read in */
void postprocess(int trackno)
{
  struct listx* i,*j;
  int h;
  int overlap;

  i = track[trackno].head;
  if (i != NULL) {
    track[trackno].startwait = i->note->time;
  } 
  else {
    track[trackno].startwait = 0;
  };
  while (i != NULL) {
    if (i->next != NULL) {
      i->note->dtnext = i->next->note->time - i->note->time;
    } 
    else {
      i->note->dtnext = i->note->tplay;
    };
    i = i->next;
  }
  i = track[trackno].head;
  //***************************
  // jordi seguir aqui

  while(i!=NULL){
     h=i->note->tinitnoteoff;
	 j=i->next;
	 if(j==NULL) break;
     if(j->note->time<h){
	 // si llegamos aqui es que hay solapamiento de noteon,
	 // tenemos dos noteon consecutivos sin un noteoff intermedio
        i->note->toff=0;
	 }else i->note->toff=j->note->time-h;
/*******************************************
	 overlap=0;
     while(j!=NULL){
		if(j->note->time<h){
		   ++overlap;
		   j=j->next;
		   continue;
		}
        break;
	 }
     i->note->numnotesoverlap=overlap;
/**********************************************
     double per;
	 ANOTE *thiss;
	 LISTX *v; 

	 thiss=i->note;
	 per=(thiss->tplay-thiss->dtnext)*100.0/thiss->tplay;
	 if(thiss->dtnext!=0){
		if(per<30){ 
	       thiss->isnote=1;
		   thiss->ischord=0;
		}else{
		   thiss->isnote=0;
		   thiss->ischord=1;
		   v=i->next;			  
		   if(v->note->dtnext==0){
			  // multiples voces formando acorde con silencio inicial				 
			  // las alargamos hasta que coincidan con la nota raiz del acorde
			  thiss->dtnext=0;
		      while(v->note->dtnext==0){
		         v->note->tplay+=v->note->time-thiss->time;
			     v->note->time=thiss->time;
				 v->note->dtnext=v->next->note->time - v->note->time;
				 v=v->next;
			  }
		   }else{
			  // la nota despues del silencio no forma acorte con la que sigue
			  thiss->dtnext=0;
	          v->note->tplay+=v->note->time-thiss->time;
		      v->note->time=thiss->time;
			  v->note->dtnext=v->next->note->time - v->note->time;
		   }
		}
	 }else{
		thiss->isnote=0;
        thiss->ischord=1;
	 }

**********************************************/        

     i=i->next;
  }
}


void scannotes(int trackno)
/* diagnostic routine to output notes in a track */
{
  struct listx* i;

  i = track[trackno].head;
  while (i != NULL) {
    printf("Pitch %d chan %d vel %d time %ld  %ld xnum %d playnum %d\n",
            i->note->pitch, i->note->chan, 
            i->note->vel, i->note->dtnext, i->note->tplay,
            i->note->xnum, i->note->playnum);
    i = i->next;
  };
}


int xnum_to_next_nonchordal_note(struct listx* fromitem,int spare,int quantum)
{
struct anote* jnote;
struct listx* nextitem;
int i,xxnum;
jnote = fromitem->note;
if (jnote->xnum > 0) return jnote->xnum;
i = 0;
nextitem = fromitem->next;
while (nextitem != NULL && i < 5) {
  jnote = nextitem->note;
  xxnum = (2*(jnote->dtnext + spare + (quantum/4)))/quantum;
  if (xxnum > 0) return xxnum;
  i++;
  nextitem = nextitem->next;
  }
return 0;
}

int quant(int val)
{
   int initq;
   int r,temp,temp0,tempp,error,errorp,error0,min,valq;

   initq=xunit*unitlen/8;
   if(initq%2!=0) ++initq;
   temp0=initq;
   while(temp0%2==0)
      temp0=temp0/2;
   tempp=tempp=temp0+temp0/2;
   for(r=0;r<64;r++,temp0*=2,tempp=temp0+temp0/2){
      error0=abs(val-temp0);
	  errorp=abs(val-tempp);
	  error=errorp; temp=tempp;
      if(error0<=errorp){ error=error0; temp=temp0; }
      if(error<=min || r==0){ min=error; valq=temp; }  
   }
   return valq;
}

//****************************************************
//****************************************************

int quant1(ANOTE *j,int &mr,int &md)
{
   int pq[14]={48,32,24,16,12,8,6,4,3,2,1,0};
   //int pq[14]={0,1,2,3,4,6,8,12,16,24,32,48};
   int tt,tr,r,d,fi,fj,minr,mind;
   double err,minerr;

   // la variable pq puede que dependa de quantum ???

   // para el caso toff=0
   // tplay=240 y dtnext=60 con tiempo de negra 120 ticks
   // significa que hay una blanca (240) y durante su duracion ha
   // aparecido otro NOTEON a 60 ticks del inicio de la blanca.
   // esto puede indicar que hay dos voces o mas simultaneas, por ejemplo
   // compas 4 empezando desde 1 pentagrama de FA en el midi wuthering.mid
   // como proceder ????
   minerr=10000;
   tt=j->tplay+j->toff;
   for(r=0;r<12;r++){
      fi=ppqn*pq[r]/8;
	  for(d=r;d<12;d++){
		 fj=ppqn*pq[d]/8;
	     err=tt-fi-fj; 
		 if(err<0) err=-err;
         if(err<minerr){ 
		    minerr=err;
            minr=r; mind=d;
		 }         
	  }
   }
   mr=pq[minr]; md=pq[mind];
   return 1;
}

void addrest(struct listx* j,int trest,int rest)
{
   LISTX *ap,*ne;

   ap=(LISTX *) new LISTX;
   ap->note=(ANOTE *) new ANOTE;
   ap->note->pitch=-1;
   ap->note->chan=j->note->chan;
   ap->note->vel=j->note->vel;
   ap->note->time=j->note->time+j->note->tplay+j->note->toff;
   ap->note->dtnext=j->next->note->time-ap->note->time;
   ap->note->tplay=rest;
   ap->note->toff=0;
   ap->note->playnum=trest;
   ap->note->xnum=trest;
   ne=j->next;
   j->next=ap;
   ap->next=ne;
   return;
}

int splitnotaysilencio(LISTX *j,int  xunit)
{
  ANOTE* thiss;
  int playnum,tplay,toff,duratick,quantum,tplayrest,rest;

  quantum = (int) (2.*xunit/parts_per_unitlen);
  thiss = j->note;
  playnum=thiss->playnum;
  tplay=thiss->tplay;
  toff=thiss->toff;
  duratick=ppqn*playnum/8;
  // toff es cero si hay solapamiento
  // programar lo que sigue segun ello
  rest=tplay+toff-duratick;
  if(rest>0){
     tplayrest=2*quant(rest)/quantum;
     j->note->toff-=rest;
	 j->note->dtnext=j->note->tplay+j->note->toff;
	 addrest(j,tplayrest,rest);
  }
  return 1;
}


int quantize_prueba(int trackno,int  xunit)
{
  LISTX* j; //,*jant;
  LISTX* v;
  ANOTE* thiss,*cho[10];
  double per;
  int mr,md,quantum,toterror;
  int posnum,xxnum,index,l1,l2;
     
  if (xunit == 0) {
    return(10000);
  };
  quantum = (int) (2.*xunit/parts_per_unitlen); 
  track[trackno].startunits = (2*(track[trackno].startwait + (quantum/4)))/quantum;
  toterror = 0;
  j = track[trackno].head;
  j = track[trackno].head;
  posnum = 0;
  index=0;  // jordi
  while (j != NULL) {
	thiss = j->note;
    if(thiss->dtnext==0){ 
	   mr=0; md=0; 
	}else{
	   quant1(thiss,mr,md);
	   if(thiss->numnotesoverlap>0) md=0;
	}
	thiss->xnum=mr; //2*quant(thiss->dtnext+spare)/quantum;  // jordi
	thiss->playnum=thiss->xnum;   // si activamos esta linea no se reconocen los acordes
	if(thiss->dtnext==0){
	   cho[index++]=thiss;
    }else{
		// do lineas siguientes en pruebas
	   if(md!=0)
	      splitnotaysilencio(j,xunit);
	   if(index!=0){
          int r;
		  for(r=0;r<index;r++){
             quant1(cho[r],mr,md);
             cho[r]->playnum=mr;
		  }
		  quant1(thiss,mr,md);
		  thiss->playnum=mr;
		  index=0;
	   }
	}
    if ((thiss->playnum == 0) && (keep_short)) {
      thiss->playnum = 1;
    };
    xxnum =  xnum_to_next_nonchordal_note(j,0,quantum);
	    if ((swallow_rests>=0) && (xxnum - thiss->playnum <= restsize)
		        && xxnum > 0) {
      thiss->playnum = xxnum;
    };
    thiss->posnum = posnum;
    posnum += thiss->xnum;
    j = j->next;
  };
  return(0);
}

void printdebug(int trackno)
{
  struct listx* j; //,*jant;
  struct anote* thiss;
  int a[10],r;
  FILE *fp;

  j = track[trackno].head;
  fp=fopen("debug.txt","w");
  while (j != NULL) {
	thiss = j->note;
    a[1]=thiss->pitch;
	a[2]=thiss->dtnext;
	a[3]=thiss->playnum;
	a[4]=thiss->posnum;
	a[5]=thiss->splitnum;
	a[6]=thiss->toff;
	a[7]=thiss->tplay;
	a[8]=thiss->vel;
	for(r=1;r<9;r++)
	   fprintf(fp,"%d ",a[r]);
	fprintf(fp,"\n");
	j=j->next;
  }
  fclose(fp);
  return;
}

//*************************************************************
//*************************************************************

int quantize(int trackno,int  xunit)
/* Work out how long each note is in musical time units.
 * The results are placed in note.playnum              */
{
  struct listx* j; //,*jant;
  struct anote* thiss,*cho[10];
  int spare;
  int toterror;
  int quantum,total;
  int posnum,xxnum,index;

  /* fix to avoid division by zero errors in strange MIDI */
  if (xunit == 0) {
    return(10000);
  };
  quantum = (int) (2.*xunit/parts_per_unitlen); /* xunit assume 2 parts_per_unit */
  track[trackno].startunits = (2*(track[trackno].startwait + (quantum/4)))/quantum;
  spare = 0;
  toterror = 0;
  j = track[trackno].head;
  posnum = 0;
  index=0;  // jordi
  while (j != NULL) {
    thiss = j->note;
    /* this->xnum is the quantized inter onset time */
    /* this->playnum is the quantized note length   */
    // jordi
	// playnum es la duracion de la nota en xunit, este valor dividido por dos
	// playnum tambien decide si es una acorde o una nota simple
	// nos dara el numero de veces que dura la nota en unidades de 1/8 (corchea)

	thiss->xnum=2*quant(thiss->dtnext+spare)/quantum;  // jordi
	//thiss->playnum=2*quant(thiss->tplay)/quantum;   // jordi
	thiss->playnum=thiss->xnum;   // si activamos esta linea no se reconocen los acordes
	if(thiss->dtnext==0){
	   cho[index++]=thiss;
    }else{
	   if(index!=0){
          int r;
		  for(r=0;r<index;r++){
             total=cho[r]->tplay+cho[r]->toff;	
             cho[r]->playnum=2*quant(total)/quantum;
		  }
		  total=thiss->tplay+thiss->toff;	
		  thiss->playnum=2*quant(total)/quantum;
		  index=0;
	   }
	}
	// thiss->xnum = (2*(thiss->dtnext + spare + (quantum/4)))/quantum;
    // thiss->playnum = (2*(thiss->tplay + (quantum/4)))/quantum;

    if ((thiss->playnum == 0) && (keep_short)) {
      thiss->playnum = 1;
    };
    /* In the event of short rests, the inter onset time
     * will be larger than the note length. However, for
     * chords the inter onset time can be zero.          */
    xxnum =  xnum_to_next_nonchordal_note(j,spare,quantum);
    if ((swallow_rests>=0) && (xxnum - thiss->playnum <= restsize)
		        && xxnum > 0) {
      thiss->playnum = xxnum;
    };
   /* this->denom = parts_per_unitlen;  this variable is never used ! */
    spare = spare + thiss->dtnext - (thiss->xnum*xunit/parts_per_unitlen);
    if (spare > 0) {
      toterror = toterror + spare;
    } 
    else {
      toterror = toterror - spare;
    };
    /* gradually forget old errors so that if xunit is slightly off,
       errors don't accumulate over several bars */
    spare = (spare * 96)/100;
    thiss->posnum = posnum;
    posnum += thiss->xnum;
	//jant=j; // jordi
	//****************************
    j = j->next;
  };
  return(toterror);
}


int quantize1(int trackno,int  xunit)
/* Work out how long each note is in musical time units.
 * The results are placed in note.playnum              */
{
  struct listx* j; //,*jant;
  struct anote* thiss,*cho[10];
  int spare,total;
  int toterror;
  int quantum;
  int posnum,xxnum,index;

  /* fix to avoid division by zero errors in strange MIDI */
  if (xunit == 0) {
    return(10000);
  };
  quantum = (int) (2.*xunit/parts_per_unitlen); /* xunit assume 2 parts_per_unit */
  track[trackno].startunits = (2*(track[trackno].startwait + (quantum/4)))/quantum;
  spare = 0;
  toterror = 0;
  j = track[trackno].head;
  posnum = 0;
  index=0;  // jordi
  while (j != NULL) {
    thiss = j->note;
    /* this->xnum is the quantized inter onset time */
    /* this->playnum is the quantized note length   */
    // jordi
	// playnum es la duracion de la nota en xunit, este valor dividido por dos
	// playnum tambien decide si es una acorde o una nota simple
	// nos dara el numero de veces que dura la nota en unidades de 1/8 (corchea)

	//thiss->xnum=2*quant(thiss->dtnext+spare)/quantum;  // jordi
/*    total=thiss->tplay+thiss->toff;	
	thiss->xnum=2*quant(total)/quantum;
	thiss->playnum=thiss->xnum;
*/
   total=thiss->tplay+thiss->toff;	
   thiss->playnum=2*quant(total)/quantum;
   if(thiss->dtnext==0) thiss->xnum=0;
   else thiss->xnum=thiss->playnum;
	

	//thiss->playnum=2*quant(thiss->tplay)/quantum;   // jordi
	//thiss->playnum=thiss->xnum;   // si activamos esta linea no se reconocen los acordes
/*
	if(thiss->dtnext==0){
	   cho[index++]=thiss;
    }else{
	   if(index!=0){
          int r;
		  for(r=0;r<index;r++){
		     cho[r]->playnum=2*quant(thiss->dtnext+spare)/quantum;
		  }
		  index=0;
	   }
	}
*/	
	 //thiss->xnum = (2*(thiss->dtnext + spare + (quantum/4)))/quantum;
     //thiss->playnum = (2*(thiss->tplay + (quantum/4)))/quantum;

    if ((thiss->playnum == 0) && (keep_short)) {
      thiss->playnum = 1;
    };
    /* In the event of short rests, the inter onset time
     * will be larger than the note length. However, for
     * chords the inter onset time can be zero.          */
    xxnum =  xnum_to_next_nonchordal_note(j,spare,quantum);
    if ((swallow_rests>=0) && (xxnum - thiss->playnum <= restsize)
		        && xxnum > 0) {
      thiss->playnum = xxnum;
    };
   /* this->denom = parts_per_unitlen;  this variable is never used ! */
/***************
// jordi
    spare = spare + thiss->dtnext - (thiss->xnum*xunit/parts_per_unitlen);
    if (spare > 0) {
      toterror = toterror + spare;
    } 
    else {
      toterror = toterror - spare;
    };
    // gradually forget old errors so that if xunit is slightly off,
    //   errors don't accumulate over several bars 
    spare = (spare * 96)/100;
***********************/
    thiss->posnum = posnum;
    posnum += thiss->xnum;
	//jant=j; // jordi
    j = j->next;
  };
  return(toterror);
}


void guesslengths(int trackno)
/* work out most appropriate value for a unit of musical time */
{
  int i;
  int trial[100];
  float avlen, factor, tryx;
  long min;

  min = track[trackno].tracklen;
  if (track[trackno].notes == 0) {
    return;
  };
  avlen = ((float)(min))/((float)(track[trackno].notes));
  tryx = avlen * (float) 0.75;
  factor = tryx/100;
  for (i=0; i<100; i++) {
    trial[i] = quantize(trackno, (int) tryx);
    if ((long) trial[i] < min) {
      min = (long) trial[i];
      xunit = (int) tryx;
    };
    tryx = tryx + factor;
  };
xunit_set = 1;
}


int findana(int maintrack,int barsize)
/* work out anacrusis from MIDI */
/* look for a strong beat marking the start of a bar */
{
  int min, mincount;
  int place;
  struct listx* p;

  min = 0;
  mincount = 0;
  place = 0;
  p = track[maintrack].head;
  while ((p != NULL) && (place < barsize)) {
    if ((p->note->vel > min) && (place > 0)) {
      min = p->note->vel;
      mincount = place;
    };
    place = place + (p->note->xnum);
    p = p->next;
  };
  return(mincount);
}



int guessana(int barbeats)
/* try to guess length of anacrusis */
{
  int score[64];
  int min, minplace;
  int i,j;

  if (barbeats > 64) {
    fatal_error("Bar size exceeds static limit of 64 units!");
  };
  for (j=0; j<barbeats; j++) {
    score[j] = 0;
    for (i=0; i<trackcount; i++) {
      score[j] = score[j] + testtrack(i, barbeats, j);
      /* restore values to num */
      quantize(i, xunit);
    };
  };
  min = score[0];
  minplace = 0;
  for (i=0; i<barbeats; i++) {
    if (score[i] < min) {
      min = score[i];
      minplace = i;
    };
  };
  return(minplace);
}


int findkey(int maintrack)
/* work out what key MIDI file is in */
/* algorithm is simply to minimize the number of accidentals needed. */
{
  int j;
  int max, min, n[12], key_score[12];
  int minkey, minblacks;
  static int keysharps[12] = {
	  0, -5, 2, -3, 4, -1, 6, 1, -4, 3, -2, 5};
  struct listx* p;
  int thispitch;
  int lastpitch;
  int totalnotes;

  /* analyse pitches */
  /* find key */
  for (j=0; j<12; j++) {
    n[j] = 0;
  };
  min = track[maintrack].tail->note->pitch;
  max = min;
  totalnotes = 0;
  for (j=0; j<trackcount; j++) {
    totalnotes = totalnotes + track[j].notes;
    p = track[j].head;
    while (p != NULL) {
      thispitch = p->note->pitch;
      if (thispitch > max) {
        max = thispitch;
      } 
      else {
        if (thispitch < min) {
          min = thispitch;
        };
      };
      n[thispitch % 12] = n[thispitch % 12] + 1;
      p = p->next;
    };
  };
  /* count black notes for each key */
  /* assume pitch = 0 is C */
  minkey = 0;
  minblacks = totalnotes;
  for (j=0; j<12; j++) {
    key_score[j] = n[(j+1)%12] + n[(j+3)%12] + n[(j+6)%12] +
                   n[(j+8)%12] + n[(j+10)%12];
    /* printf("Score for key %d is %d\n", j, key_score[j]); */
    if (key_score[j] < minblacks) {
      minkey = j;
      minblacks = key_score[j];
    };
  };
  /* do conversion to abc pitches */
  /* Code changed to use absolute rather than */
  /* relative choice of pitch for 'c' */
  /* MIDDLE = (min + (max - min)/2 + 6)/12 * 12; */
  /* Do last note analysis */
  lastpitch = track[maintrack].tail->note->pitch;
  if (minkey != (lastpitch%12)) {
    fprintf(outhandle,"%% Last note suggests ");
    switch((lastpitch+12-minkey)%12) {
    case(2):
      fprintf(outhandle,"Dorian ");
      break;
    case(4):
      fprintf(outhandle,"Phrygian ");
      break;
    case(5):
      fprintf(outhandle,"Lydian ");
      break;
    case(7):
      fprintf(outhandle,"Mixolydian ");
      break;
    case(9):
      fprintf(outhandle,"minor ");
      break;
    case(11):
      fprintf(outhandle,"Locrian ");
      break;
    default:
      fprintf(outhandle,"unknown ");
      break;
    };
    fprintf(outhandle,"mode tune\n");
  };
  /* switch to minor mode if it gives same number of accidentals */
  if ((minkey != ((lastpitch+3)%12)) && 
      (key_score[minkey] == key_score[(lastpitch+3)%12])) {
         minkey = (lastpitch+3)%12;
  };
  /* switch to major mode if it gives same number of accidentals */
  if ((minkey != (lastpitch%12)) && 
      (key_score[minkey] == key_score[lastpitch%12])) {
         minkey = lastpitch%12;
  };
  sharps = keysharps[minkey];
  return(sharps);
}




/* Stage 3  output MIDI tracks in abc format                        */


/* head and tail of list of notes in current chord playing */
/* used while abc is being generated */
struct dlistx* chordhead;
struct dlistx* chordtail;


void printchordlist()
/* diagnostic routine */
{
  struct dlistx* i;

  i = chordhead;
  printf("----CHORD LIST------\n");
  while(i != NULL) {
    printf("pitch %d len %d\n", i->note->pitch, i->note->playnum);
    if (i->next == i) {
      fatal_error("Loopback problem!");
    };
    i = i->next;
  };
}

void checkchordlist()
/* diagnostic routine */
/* validates data structure */
{
  struct dlistx* i;
  int n;

  if ((chordhead == NULL) && (chordtail == NULL)) {
    return;
  };
  if ((chordhead == NULL) && (chordtail != NULL)) {
    fatal_error("chordhead == NULL and chordtail != NULL");
  };
  if ((chordhead != NULL) && (chordtail == NULL)) {
    fatal_error("chordhead != NULL and chordtail == NULL");
  };
  if (chordhead->last != NULL) {
    fatal_error("chordhead->last != NULL");
  };
  if (chordtail->next != NULL) {
    fatal_error("chordtail->next != NULL");
  };
  i = chordhead;
  n = 0;
  while((i != NULL) && (i->next != NULL)) {
    if (i->next->last != i) {
      char msg[80];

      sprintf(msg, "chordlist item %d : i->next->last!", n);
      fatal_error(msg);
    };
    i = i->next;
    n = n + 1;
  };
  /* checkchordlist(); */
}

void addtochord(struct anote* p)
/* used when printing out abc */
{
  struct dlistx* newx;
  struct dlistx* place;

  newx = (struct dlistx*) checkmalloc(sizeof(struct dlistx));
  newx->note = p;
  newx->next = NULL;
  newx->last = NULL;

  if (chordhead == NULL) {
    chordhead = newx;
    chordtail = newx;
    checkchordlist();
    return;
  };
  place = chordhead;
  while ((place != NULL) && (place->note->pitch > p->pitch)) {
    place = place->next;
  };
  if (place == chordhead) {
    newx->next = chordhead;
    chordhead->last = newx;
    chordhead = newx;
    checkchordlist();
    return;
  };
  if (place == NULL) {
    newx->last = chordtail;
    chordtail->next = newx;
    chordtail = newx;
    checkchordlist();
    return;
  };
  newx->next = place;
  newx->last = place->last;
  place->last = newx;
  newx->last->next = newx;
  checkchordlist();
}

struct dlistx* removefromchord(struct dlistx* i)
/* used when printing out abc */
{
  struct dlistx* newi;

  /* remove note from list */
  if (i->last == NULL) {
    chordhead = i->next;
  } 
  else {
    (i->last)->next = i->next;
  };
  if (i->next == NULL) {
    chordtail = i->last;
  } 
  else {
    (i->next)->last = i->last;
  };
  newi = i->next;
  free(i);
  checkchordlist();
  return(newi);
}

int findshortest(int gap)
/* find the first note in the chord to terminate */
{
  int min, v;
  struct dlistx* p;

  p = chordhead;
  min = gap;
  while (p != NULL) {
    v = p->note->playnum;
    if (v < min) {
      min = v;
    };
    p = p->next;
  };
  return(min);
}

void advancechord(int len)
/* adjust note lengths for all notes in the chord */
{
  struct dlistx* p;

  p = chordhead;
  while (p != NULL) {
    if (p->note->playnum <= len) {
      if (p->note->playnum < len) {
        fatal_error("Error - note too short!");
      };
      /* remove note */
      checkchordlist();
      p = removefromchord(p);
    } 
    else {
      /* shorten note */
      p->note->playnum = p->note->playnum - len;
      p = p->next;
    };
  };
}

void freshline()
/* if the current line of abc or text is non-empty, start a new line */
{
  if (midline == 1) {
    fprintf(outhandle,"\n");
    midline = 0;
  };
}


void printnote (struct listx *i)
{
      printf("%ld ",i->note->time);
      printpitch(i->note);
      printfract(i->note->playnum, parts_per_unitlen);
      printf(" %d %d %d %d\n",i->note->xnum, i->note->playnum,
       i->note->posnum,i->note->splitnum); 
}

void listnotes(int trackno, int start, int end)
/* A diagnostic like scannotes. I usually call it when
   I am in the debugger (for example in printtrack).
*/
{
struct listx* i;
int k;
i = track[trackno].head;
k = 0;
printf("ticks pitch xnum,playnum,posnum,splitnum\n");
while (i != NULL && k < end)
  {
  if (k >= start) 
    printnote(i);
  k++;
  i = i->next;
  }
}



int testtrack(int trackno,int barbeats,int anacrusis)
/* print out one track as abc */
{
  struct listx* i;
  int step, gap;
  int barnotes;
  int barcount;
  int breakcount;

  breakcount = 0;
  chordhead = NULL;
  chordtail = NULL;
  i = track[trackno].head;
  gap = 0;
  if (anacrusis > 0) {
    barnotes = anacrusis;
  } 
  else {
    barnotes = barbeats;
  };
  barcount = 0;
  while((i != NULL)||(gap != 0)) {
    if (gap == 0) {
      /* add notes to chord */
      addtochord(i->note);
      gap = i->note->xnum;
      i = i->next;
      advancechord(0); /* get rid of any zero length notes */
    } 
    else {
      step = findshortest(gap);
      if (step > barnotes) {
        step = barnotes;
      };
      if (step == 0) {
        fatal_error("Advancing by 0 in testtrack!");
      };
      advancechord(step);
      gap = gap - step;
      barnotes = barnotes - step;
      if (barnotes == 0) {
        if (chordhead != NULL) {
          breakcount = breakcount + 1;
        };
        barnotes = barbeats;
        barcount = barcount + 1;
        if (barcount>0  && barcount%4 ==0) {
        /* can't zero barcount because I use it for computing maxbarcount */
          freshline();
          barcount = 0;
        };
      };
    };
  };
  return(breakcount);
}

//****************************************************
// TRANSFORMA NOTAS NUMERICAS A VALORES ABC
//****************************************************
void printpitch(struct anote* j)
/* convert numerical value to abc pitch */
{
  int p, po,i;

  p = j->pitch;
  if (p == -1) {
    fprintf(outhandle,"z");
  } 
  else {
    po = p % 12;
    if ((back[trans[p]] != p) || (key[po] == 1)) {
      fprintf(outhandle,"%c%c", symbol[po], atog[p]);
      for (i=p%12; i<256; i += 12) /* apply accidental to all octaves */
         back[trans[i]] = i;
    } 
    else {
      fprintf(outhandle,"%c", atog[p]);
    };
    while (p >= MIDDLE + 12) {
      fprintf(outhandle,"'");
      p = p - 12;
    };
    while (p < MIDDLE - 12) {
      fprintf(outhandle,",");
      p = p + 12;
    };
  };
}

static void reduce(int *a, int *b)
{
  int t, n, m;

    /* find HCF using Euclid's algorithm */
    if (*a > *b) {
        n = *a;
        m = *b;
      }
    else {
        n = *b;
        m = *a;
       };
while (m != 0) {
      t = n % m;
      n = m;
      m = t;
    };
*a = *a/n;
*b = *b/n;
}



void printfract(int a,int b)
/* print fraction */
/* used when printing abc */
{
  int c, d;

  c = a;
  d = b;
  reduce(&c,&d);
  /* print out length */
  if (c != 1) {
    fprintf(outhandle,"%d", c);
  };
  if (d != 1) {
    fprintf(outhandle,"/%d", d);
  };
}

void printchord(int len)
/* Print out the current chord. Any notes that haven't            */
/* finished at the end of the chord are tied into the next chord. */
{
  struct dlistx* i;

  i = chordhead;
  if (i == NULL) {
    /* no notes in chord */
#ifdef SPLITCODE
    fprintf(outhandle,"z"); //"x"
#else
    fprintf(outhandle,"z");
#endif
    printfract(len, parts_per_unitlen);
    midline = 1;
  } 
  else {
    if (i->next == NULL) {
      /* only one note in chord */
      printpitch(i->note);
      printfract(len, parts_per_unitlen);
      midline = 1;
      if (len < i->note->playnum) {
        fprintf(outhandle,"-");    // jordi imprime nota ligada
      };
    } 
    else {
      fprintf(outhandle,"[");
      while (i != NULL) {
        printpitch(i->note);
        printfract(len, parts_per_unitlen);
        if (len < i->note->playnum) {
          fprintf(outhandle,"-");    // jordi imprime nota ligada
        };
      if (nogr && i->next != NULL) fprintf(outhandle," ");
        i = i->next;
      };
      fprintf(outhandle,"]");
      midline = 1;
    };
  };
}

char dospecial(struct listx* i, int *barnotes, int *featurecount)
/* identify and print out triplets and broken rhythm */
{
  int v1, v2, v3, vt;
  int xa, xb;
  int pnum;
  long total, t1, t2, t3;

  
  if ((chordhead != NULL) || (i == NULL) || (i->next == NULL)
  /* || (asig%3 == 0) || (asig%2 != 0) 2004/may/09 SS*/) {
    return(' ');
  };
  // si silencio volver, jordi
  if(i->note->pitch==-1) return(' ');
  t1 = i->note->dtnext;
  v1 = i->note->xnum;
  pnum = i->note->playnum;
  if ((v1 < pnum) || (v1 > 1 + pnum) || (pnum == 0)) {
    return(' ');
  };
  t2 = i->next->note->dtnext;
  v2 = i->next->note->xnum;
  pnum = i->next->note->playnum;
  if (/*(v2 < pnum) ||*/ (v2 > 1 + pnum) || (pnum == 0) || (v1+v2 > *barnotes)) {
    return(' ');
  };
  /* look for broken rhythm */
  total = t1 + t2;
  if (total == 0L) {
    /* shouldn't happen, but avoids possible divide by zero */
    return(' ');
  };
  if (((v1+v2)%2 == 0) && ((v1+v2)%3 != 0)) {
    vt = (v1+v2)/2;
      if (vt == validnote(vt)) {
      /* do not try to break a note which cannot be legally expressed */
      switch ((int) ((t1*6+(total/2))/total)) {
        case 2:
          *featurecount = 2;
          i->note->xnum  = vt;
          i->note->playnum = vt;
          i->next->note->xnum  = vt;
          i->next->note->playnum = vt;
          return('<');
          break;
        case 4:
          *featurecount = 2;
          i->note->xnum  = vt;
          i->note->playnum = vt;
          i->next->note->xnum  = vt;
          i->next->note->playnum = vt;
          return('>');
          break;
        default:
          break;
      };
    };
  };
  /* look for triplet */
  if (i->next->next != NULL) {
    t3 = i->next->next->note->dtnext;
    v3 = i->next->next->note->xnum;
    pnum = i->next->next->note->playnum;
    if ((v3 < pnum) || (v3 > 1 + pnum) || (pnum == 0) || 
        (v1+v2+v3 > *barnotes)) {
      return(' ');
    };
    if ((v1+v2+v3)%2 != 0) {
      return(' ');
    };
    vt = (v1+v2+v3)/2;
    if ((vt%2 == 1) && (vt > 1)) {
      /* don't want strange fractions in triplet */
      return(' ');
    };
    total = t1+t2+t3;
    xa = (int) ((t1*6+(total/2))/total); 
    xb = (int) (((t1+t2)*6+(total/2))/total);
    if ((xa == 2) && (xb == 4) && (vt%3 != 0) ) {
      *featurecount = 3;
      *barnotes = *barnotes + vt;
      i->note->xnum = vt;
      i->note->playnum = vt;
      i->next->note->xnum = vt;
      i->next->note->playnum = vt;
      i->next->next->note->xnum = vt;
      i->next->next->note->playnum = vt;
    };
  };
  return(' ');
}

int validnote(int n)
/* work out a step which can be expressed as a musical time */
{
  int v;

  if (n <= 4) {
    v = n;
  } 
  else {
    v = 4;
    while (v*2 <= n) {
      v = v*2;
    };
    if (v + v/2 <= n) {
      v = v + v/2;
    };
  };
  return(v);
}

void handletext(long t,struct tlistx** textplace,int trackno)
/* print out text occuring in the body of the track */
/* The text is printed out at the appropriate place within the track */
/* In addition the function handles key signature and time */
/* signature changes that can occur in the middle of the tune. */
{
  char* str;
  char ch;
  int type,sf,mi,nn,denom,bb;

  while (((*textplace) != NULL) && ((*textplace)->when <= t)) {
    str = (*textplace)->text;
    ch = *str;
    type = (*textplace)->type;
    remove_carriage_returns(str);
    if (((int)ch == '\\') || ((int)ch == '/')) {
      inkaraoke = 1;
    };
    if ((inkaraoke == 1) && (karaoke == 1)) {
      switch(ch) {
        case ' ':
          fprintf(outhandle,"%s", str);
          midline = 1;
          break;
        case '\\':
          freshline();
          fprintf(outhandle,"w:%s", str + 1);
          midline = 1;
          break;
        case '/':
          freshline();
          fprintf(outhandle,"w:%s", str + 1);
          midline = 1;
          break;
        default :
          if (midline == 0) {
            fprintf(outhandle,"%%%s", str);
          } 
	  else {
            fprintf(outhandle,"-%s", str);
          };
          break;
      };
    } 
    else {
      freshline();
      ch=*(str+1);
      switch (type) {
      case 0:
      if (ch != '%') 
      fprintf(outhandle,"%%%s\n", str);
      else 
      fprintf(outhandle,"%s\n", str);
      break;
      case 1: /* key signature change */
      sscanf(str,"%d %d",&sf,&mi);
      if((trackno != 0 || trackcount==1) &&
	 (active_keysig != sf)) {
	      setupkey(sf);
	      active_keysig=sf;
              }
      break;
      case 2: /* time signature change */
      sscanf(str,"%d %d %d",&nn,&denom,&bb);
      if ((trackno != 0 || trackcount ==1) &&
	  (active_asig != nn || active_bsig != denom))
        {
  	setup_timesig(nn,denom,bb);
	fprintf(outhandle,"M: %d/%d\n",nn,denom);
        fprintf(outhandle,"L: 1/%d\n",unitlen);
	active_asig=nn;
	active_bsig=denom;
	}
      break;
     default:
      break;
      }
  }
  *textplace = (*textplace)->next;
 }
}


#ifdef SPLITCODE

/* This function identifies irregular chords, (notes which
   do not exactly overlap in time). The notes in the
   chords are split into separate lines (split numbers).
   The xnum (delay) to next note is updated.
*/

int splitstart[10],splitend[10]; /* used in forming chords from notes*/
int lastposnum[10]; /* posnum of previous note in linked list */
int endposnum; /* posnum at last note in linked list */
struct anote* prevnote[10]; /*previous note in linked list */
struct listx*  last_i[10];  /*note after finishing processing bar*/
int existingsplits[10]; /* existing splits in active bar */
struct dlistx* splitchordhead[10]; /* chordhead list for splitnum */
struct dlistx* splitchordtail[10]; /* chordtail list for splitnum */
int splitgap[10]; /* gap to next note at end of split measure */

void label_split(struct anote *note, int activesplit)
{
/* The function assigns a split number (activesplit), to
   a specific note, (*note). We also update splitstart
   and splitend which specifies the region in time where
   the another note must occur if it forms a proper chord.
   After assigning a split number to the note we need to
   update note->xnum as this indicates the gap to the
   next note in the same split number. The function uses
   a greedy algorithm. It assigns a note to the first
   splitnumber code which satisfies the above constraint.
   If it cannot find a splitnumber, a new one (voice or track)
   is created. It would be nice if the voices kept the
   high and low notes (in pitch) separate.
*/
     note->splitnum = activesplit;
     splitstart[activesplit] = note->posnum;
     splitend[activesplit] = splitstart[activesplit] + note->playnum;
     if (prevnote[activesplit]) 
         prevnote[activesplit]->xnum = note->posnum - lastposnum[activesplit];
     lastposnum[activesplit] = note->posnum;
     prevnote[activesplit] = note;
     /* in case this is the last activesplit note make sure it
        xnum points to end of track. Otherwise it will be changed
        when the next activesplit note is labeled.
     */
     note->xnum = endposnum - note->posnum;
     existingsplits[activesplit]++;
}


void label_split_voices (int trackno)
{
/* This function sorts all the notes in the track into
   separate split part. A note is placed into a separate
   part if it forms a chord in the current part but
   does not have the same onset time and same end time.
   If this occurs, we search for another part where
   this does not happen. If we can't find such a part
   a new part (split) is created.
   The heuristic used needs to be improved, so
   that split number 0 always contains notes and so
   that notes in the same pitch range or duration are
   given the same split number.
*/ 
int activesplit,nsplits;
int done;
struct listx* i;
int k;
int firstposnum;
/* initializations */
activesplit = 0;
nsplits = 0;
for (k=0;k<10;k++) {
       splitstart[k]=splitend[k]=lastposnum[k]=0;
       prevnote[k] = NULL;
       existingsplits[k] = 0;
       splitgap[k]=0;
       }
i = track[trackno].head;
if (track[trackno].tail == 0x0) return;
endposnum =track[trackno].tail->note->posnum +
           track[trackno].tail->note->playnum;

if (i != NULL) label_split(i->note, activesplit);
/* now label all the notes in the track */
while (i != NULL)
  {
  done =0;
  if (nsplits == 0) { /*no splits exist, create split number 0 */
     activesplit = 0;
     nsplits++;
     i->note->splitnum = activesplit;
     splitstart[activesplit] = i->note->posnum;
     splitend[activesplit] = splitstart[activesplit] + i->note->playnum;
     firstposnum = splitstart[activesplit];
     } else {  /* do a compatibility check with the last split number */
  if (  (   i->note->posnum == splitstart[activesplit] 
         && i->note->playnum == (splitend[activesplit] - splitstart[activesplit]))
      || i->note->posnum >= splitend[activesplit])
     {
     if (existingsplits[activesplit] == 0) {
         last_i[activesplit] = i;
         splitgap[activesplit] = i->note->posnum - firstposnum;
         }
     label_split(i->note, activesplit);
     done = 1;
     }
/* need to search for any other compatible split numbers  */
  if (done == 0) for (activesplit=0;activesplit<nsplits;activesplit++) {
      if (  (   i->note->posnum == splitstart[activesplit] 
             && i->note->playnum == splitend[activesplit] - splitstart[activesplit])
          || i->note->posnum >= splitend[activesplit])
       {
        if (existingsplits[activesplit] == 0) {
           last_i[activesplit] = i;
           splitgap[activesplit] = i->note->posnum - firstposnum;
           }
        label_split(i->note,activesplit);
        done = 1;
        break;
        }
      }
     
/* No compatible split number found. Create new split */
   if (done == 0) {
     if(nsplits < 10) {nsplits++; activesplit = nsplits-1;}  
     if (existingsplits[activesplit] == 0) {
         last_i[activesplit] = i;
         splitgap[activesplit] = i->note->posnum - firstposnum;
         }
     label_split(i->note,activesplit);
     }
   } 
/*  printf("note %d links to %d  %d (%d %d)\n",i->note->pitch,activesplit,
i->note->posnum,splitstart[activesplit],splitend[activesplit]);
*/
  i = i->next;
  } /* end while loop */
}


int nextsplitnum(int splitnum)
  {
  while (splitnum < 9) {
      splitnum++;
      if (existingsplits[splitnum]) return splitnum;
      } 
     return -1;
}


int count_splits()
{
int i,n;
n = 0;
for (i=0;i<10;i++)
  if (existingsplits[i]) n++;
return n;
}

void printtrack_with_splits(int trackno,int anacrusis)
/* This function is an adaption of printtrack so that
   notes with separate split numbers are in separated
   regions in the measure. (Separated with &'s).
   To do this we must make multiple passes through
   each bar and maintain separate chordlists (in
   event that some chords overlap over more than
   one measure).
*/
{
  struct listx* i;
  struct tlistx* textplace;
  struct tlistx* textplace0; /* track 0 text storage */
  int step, gap;
  int barnotes;
  int barcount;
  int bars_on_line;
  long now;
  char broken;
  int featurecount;
  int last_barsize,barnotes_correction;
  int splitnum = 0;
  int j;
  int nlines;
  int done;

  nlines= 0;
  label_split_voices (trackno);

  midline = 0;
  featurecount = 0;
  inkaraoke = 0;
  now = 0L;
  broken = ' ';
  for (j=0;j<10;j++) {
      splitchordhead[j] = NULL;
      splitchordtail[j] = NULL;
      }
  i = track[trackno].head;
  textplace = track[trackno].texthead;
  textplace0 = track[0].texthead;
  /*gap = track[trackno].startunits;*/ gap = 0;
  if (anacrusis > 0) {
    barnotes = anacrusis;
    barcount = -1;
  } 
  else {
    barnotes = barsize;
    barcount = 0;
  };
  bars_on_line = 0;
  last_barsize = barsize;
  active_asig = header_asig;
  active_bsig = header_bsig;
  setup_timesig(header_asig,header_bsig,header_bb);
  active_keysig = header_keysig;
  handletext(now, &textplace, trackno);
  splitnum = 0;
  chordhead = splitchordhead[splitnum]; 
  chordtail = splitchordtail[splitnum]; 
  gap = splitgap[splitnum];

  while((i != NULL)||(gap != 0)) {
    if (gap == 0) {
      /* do triplet here */
      if (featurecount == 0) {
	if (!no_triplets) {
	  broken = dospecial(i, &barnotes, &featurecount);
	};
      };

/* ignore any notes that are not in the current splitnum */
      if (i->note->splitnum == splitnum) {
               /*printf("\nadding ");
                 printnote(i); */
	addtochord(i->note);
	gap = i->note->xnum;
	now = i->note->time;
	}

      i = i->next;
      advancechord(0); /* get rid of any zero length notes */
      if (trackcount > 1 && trackno !=0)
	      handletext(now, &textplace0, trackno);
      handletext(now, &textplace,trackno);
      barnotes_correction = barsize - last_barsize;
      barnotes += barnotes_correction;
      last_barsize = barsize;
    } 
    else {
      step = findshortest(gap);
      if (step > barnotes) {
	step = barnotes;
      };
      step = validnote(step);
      if (step == 0) {
	fatal_error("Advancing by 0 in printtrack!");
      };
      if (featurecount == 3)
        {
        fprintf(outhandle," (3");
        };
      printchord(step); if ( featurecount > 0) { featurecount = featurecount - 1; };
      if ((featurecount == 1) && (broken != ' ')) {
        fprintf(outhandle,"%c", broken);
      };
      advancechord(step);
      gap = gap - step;
      barnotes = barnotes - step;

/* at the end of the bar we must decide whether to place
   a | or &. If we place a & then we have to return to
   the beginning of the bar and process the next split number.
*/  
      if (barnotes == 0) { /* end of bar  ? */
        nlines++;
        if (nlines > 5000) {
            printf("\nProbably infinite loop: aborting\n");
            fprintf(outhandle,"\n\nProbably infinite loop: aborting\n");
            return;
            }
/* save state for the last splitnum before going to the next */
        last_i[splitnum] = i;
        splitchordhead[splitnum] = chordhead;
        splitchordtail[splitnum] = chordtail;
        splitgap[splitnum] = gap;

/*     look for the next splitnum which contains notes in
       the current measure. If not, end the measure.
*/
        done = 0;
        while (done != 1) {
           splitnum = nextsplitnum(splitnum);

           if (splitnum == -1) {
              fprintf(outhandle," | ");
              splitnum = nextsplitnum(splitnum);
              done = 1;
              break;
              }

           if (splitgap[splitnum] >= barsize)
               {
               splitgap[splitnum] -= barsize;
               continue; /* look for other splits */
               }
 
           fprintf(outhandle, " & ");
           i = last_i[splitnum]; 
           done = 1;
           }
/* restore state for the next splitnum */
       chordhead = splitchordhead[splitnum];
       chordtail = splitchordtail[splitnum];
       checkchordlist();
       gap = splitgap[splitnum];
       i = last_i[splitnum];

       /*
        printf("returning to %ld ",i->note->time);
        printpitch(i->note);
        printf("\n");
       */
     
        barnotes = barsize;
        barcount = barcount + 1;
	bars_on_line++;
        if (barcount >0 && barcount%bars_per_staff == 0)  {
		freshline();
		bars_on_line=0;
	}
     /* can't zero barcount because I use it for computing maxbarcount */
        else if(bars_on_line >= bars_per_line && i != NULL) {
		fprintf(outhandle," \\");
	       	freshline();
	        bars_on_line=0;}
      }
      else if (featurecount == 0) {
          /* note grouping algorithm */
          if ((barsize/parts_per_unitlen) % 3 == 0) {
            if ( (barnotes/parts_per_unitlen) % 3 == 0
               &&(barnotes%parts_per_unitlen) == 0) {
              fprintf(outhandle," ");
            };
          } 
	  else {
            if (((barsize/parts_per_unitlen) % 2 == 0)
                && (barnotes % parts_per_unitlen) == 0
                && ((barnotes/parts_per_unitlen) % 2 == 0)) {
              fprintf(outhandle," ");
            };
          };
      }
      if (nogr) fprintf(outhandle," ");
    };
   if (i == NULL) /* end of track before end of measure ? */
    {
     last_i[splitnum] = i;
     splitchordhead[splitnum] = chordhead;
     splitchordtail[splitnum] = chordtail;
     splitgap[splitnum] = gap;
     splitnum = nextsplitnum(splitnum);
     if (splitnum == -1) break;
     chordhead = splitchordhead[splitnum];
     chordtail = splitchordtail[splitnum];
     gap = splitgap[splitnum];
     i = last_i[splitnum];
     if (barnotes != barsize) fprintf(outhandle, " & ");
     barnotes = barsize;
    }
 
  };
  /* print out all extra text */
  while (textplace != NULL) {
    handletext(textplace->when, &textplace, trackno);
  };
  freshline();
  if (barcount > maxbarcount) maxbarcount = barcount;
}



void printtrack_split_voice(int trackno,int anacrusis)
/* print out one track as abc */
{
  struct listx* i;
  struct tlistx* textplace;
  struct tlistx* textplace0; /* track 0 text storage */
  int step, gap;
  int barnotes;
  int barcount;
  int bars_on_line;
  long now;
  char broken;
  int featurecount;
  int last_barsize,barnotes_correction;
  int nlines;
  int splitnum;
  int lastnote_in_split;

  nlines= 0;
  lastnote_in_split = 0;
  label_split_voices (trackno);
  midline = 0;
  featurecount = 0;
  inkaraoke = 0;
  now = 0L;
  broken = ' ';
  chordhead = NULL;
  chordtail = NULL;
  i = track[trackno].head;
  textplace = track[trackno].texthead;
  textplace0 = track[0].texthead;
  gap = track[trackno].startunits;
  if (anacrusis > 0) {
    barnotes = anacrusis;
    barcount = -1;
  } 
  else {
    barnotes = barsize;
    barcount = 0;
  };
  bars_on_line = 0;
  last_barsize = barsize;
  active_asig = header_asig;
  active_bsig = header_bsig;
  setup_timesig(header_asig,header_bsig,header_bb);
  active_keysig = header_keysig;
  handletext(now, &textplace, trackno);
  splitnum = 0;
  gap = splitgap[splitnum];
  if (count_splits() > 1) fprintf(outhandle,"V: split%d%c\n",trackno+1,'A'+splitnum);

  while((i != NULL)||(gap != 0)) {
    if (gap == 0) {
      if (i->note->posnum + i->note->xnum == endposnum) lastnote_in_split = 1;
      /* do triplet here */
      if (featurecount == 0) {
        if (!no_triplets) {
          broken = dospecial(i, &barnotes, &featurecount);
        };
      };
/* ignore any notes that are not in the current splitnum */
      if (i->note->splitnum == splitnum) {
               /*printf("\nadding ");
                 printnote(i); */
	addtochord(i->note);
	gap = i->note->xnum;
	now = i->note->time;
	}
      i = i->next;
      advancechord(0); /* get rid of any zero length notes */
      if (trackcount > 1 && trackno !=0)
	      handletext(now, &textplace0, trackno);
      handletext(now, &textplace,trackno);
      barnotes_correction = barsize - last_barsize;
      barnotes += barnotes_correction;
      last_barsize = barsize;
    } 
    else {
      step = findshortest(gap);
      if (step > barnotes) {
        step = barnotes;
      };
      step = validnote(step);
      if (step == 0) {
        fatal_error("Advancing by 0 in printtrack!");
      };
      if (featurecount == 3)
        {
        fprintf(outhandle," (3");
        };
      printchord(step);
      if ( featurecount > 0) {
        featurecount = featurecount - 1;
      };
      if ((featurecount == 1) && (broken != ' ')) {
        fprintf(outhandle,"%c", broken);
      };
      advancechord(step);
      gap = gap - step;
      barnotes = barnotes - step;
      if (barnotes == 0) {
        nlines++;
        if (nlines > 5000) {
            printf("\nProbably infinite loop: aborting\n");
            fprintf(outhandle,"\n\nProbably infinite loop: aborting\n");
            return;
            }
        fprintf(outhandle,"|");
        barnotes = barsize;
        barcount = barcount + 1;
	bars_on_line++;
        if (barcount >0 && barcount%bars_per_staff == 0)  {
		freshline();
		bars_on_line=0;
	}
     /* can't zero barcount because I use it for computing maxbarcount */
        else if(bars_on_line >= bars_per_line && i != NULL) {
		if (!lastnote_in_split) fprintf(outhandle," \\");
	       	freshline();
	        bars_on_line=0;}
      }
      else if (featurecount == 0) {
          /* note grouping algorithm */
          if ((barsize/parts_per_unitlen) % 3 == 0) {
            if ( (barnotes/parts_per_unitlen) % 3 == 0
               &&(barnotes%parts_per_unitlen) == 0) {
              fprintf(outhandle," ");
            };
          } 
	  else {
            if (((barsize/parts_per_unitlen) % 2 == 0)
                && (barnotes % parts_per_unitlen) == 0
                && ((barnotes/parts_per_unitlen) % 2 == 0)) {
              fprintf(outhandle," ");
            };
          };
      }
      if (nogr) fprintf(outhandle," ");
    };
  if (i == NULL && gap == 0) 
    {
    i = track[trackno].head;
    splitnum = nextsplitnum(splitnum);
    lastnote_in_split = 0;
    if (splitnum == -1) break;
    gap = splitgap[splitnum];
    if(barnotes != barsize) freshline();
    fprintf(outhandle,"V:split%d%c\n",trackno+1,'A'+splitnum);
    if (anacrusis > 0) {
      barnotes = anacrusis;
      barcount = -1;
      } 
    else {
      barnotes = barsize;
      barcount = 0;
      };
    }
  };
  /* print out all extra text */
  while (textplace != NULL) {
    handletext(textplace->when, &textplace, trackno);
  };
  freshline();
  if (barcount > maxbarcount) maxbarcount = barcount;
}

#endif


// jordi se imprime todo track en el fichero abc 

void printtrack(int trackno,int anacrusis)
/* print out one track as abc */
{
  struct listx* i; //,*iant;
  struct tlistx* textplace;
  struct tlistx* textplace0; /* track 0 text storage */
  int step, gap,haybars,isbarnotes;
  int barnotes;
  int barcount;
  int bars_on_line;
  long now;
  char broken;
  int featurecount;
  int last_barsize,barnotes_correction;

  midline = 0;
  featurecount = 0;
  inkaraoke = 0;
  now = 0L;
  broken = ' ';
  chordhead = NULL;
  chordtail = NULL;
  i = track[trackno].head;
  textplace = track[trackno].texthead;
  textplace0 = track[0].texthead;
  gap = track[trackno].startunits;
  if (anacrusis > 0) {
    barnotes = anacrusis;
    barcount = -1;
  } 
  else {
    barnotes = barsize;
    barcount = 0;
  };
  bars_on_line = 0;
  haybars=isbarnotes=0;
  last_barsize = barsize;
  active_asig = header_asig;
  active_bsig = header_bsig;
  setup_timesig(header_asig,header_bsig,header_bb);
  active_keysig = header_keysig;
  handletext(now, &textplace, trackno);

  while((i != NULL)||(gap != 0)) {
	//**************************
//	if((int) i==0x45afe0) // || (int) i==0x4590c0)
//			 printf("hola");
	//**************************
    if (gap == 0) {
      /* do triplet here */
	  if (featurecount == 0) {
         if (!no_triplets) {
          broken = dospecial(i, &barnotes, &featurecount);      
		 }
	  }
      /* add notes to chord, en la anacrusa no se añaden notas */
      addtochord(i->note);
      gap = i->note->xnum;     // no hay gap--> la nota siguiente es del mismo acorde
      now = i->note->time;
//	  iant=i;
      i = i->next;
      advancechord(0); /* get rid of any zero length notes */
      if (trackcount > 1 && trackno !=0)
	      handletext(now, &textplace0, trackno);
      handletext(now, &textplace,trackno);
      barnotes_correction = barsize - last_barsize;
      barnotes += barnotes_correction;
      last_barsize = barsize;
    } 
    else {       // jordi chordhead contiene un apuntador al acorde cuando llegamos a este else
      step = findshortest(gap);
      if (step > barnotes) {
        step = barnotes;
      };
      step = validnote(step);
      if (step == 0) {
        fatal_error("Advancing by 0 in printtrack!");
      };
      if (featurecount == 3)
        {
        fprintf(outhandle," (3");
        };
      printchord(step);      // jordi impreme el acorde
      if ( featurecount > 0) {
        featurecount = featurecount - 1;
      };
      if ((featurecount == 1) && (broken != ' ')) {
        fprintf(outhandle,"%c", broken);
      };
      advancechord(step);
      gap = gap - step;
      barnotes = barnotes - step;
	  isbarnotes=1;
      if (barnotes == 0) {
		++haybars; isbarnotes=0;
        fprintf(outhandle,"|");          // imprime barra de compas
        barnotes = barsize;
        barcount = barcount + 1;
		bars_on_line++;
        if (barcount >0 && barcount%bars_per_staff == 0)  {
		freshline();
		bars_on_line=0;
	}
     /* can't zero barcount because I use it for computing maxbarcount */
        else if(bars_on_line >= bars_per_line && i != NULL) {
		fprintf(outhandle," \\");
	       	freshline();
	        bars_on_line=0;}
      }
      else if (featurecount == 0) {
          /* note grouping algorithm */
          if ((barsize/parts_per_unitlen) % 3 == 0) {
            if ( (barnotes/parts_per_unitlen) % 3 == 0
               &&(barnotes%parts_per_unitlen) == 0) {
              fprintf(outhandle," ");
            };
          } 
	  else {
            if (((barsize/parts_per_unitlen) % 2 == 0)
                && (barnotes % parts_per_unitlen) == 0
                && ((barnotes/parts_per_unitlen) % 2 == 0)) {
              fprintf(outhandle," ");
            };
          };
      }
      if (nogr) fprintf(outhandle," ");
    };
  };
  /* print out all extra text */
  while (textplace != NULL) {
    handletext(textplace->when, &textplace, trackno);
  };
  //******************************************************************
  // jordi añadida 8-3-2014
  // a veces el ultimo compas no esta cerrado con un bar porque no llega al valor total
  // del compas y se supone que hay silencios. añadimos una barra de cierre y pianotrain
  // se encarga de añadir los silencios que hacen falta
  if(haybars!=0 && isbarnotes!=0) 
     fprintf(outhandle,"|");
  //******************************************************************
  freshline();
  if (barcount > maxbarcount) maxbarcount = barcount; 
}




void remove_carriage_returns(char *str)
{
/* a carriage return might be embedded in a midi text meta-event.
   do not output this in the abc file or this would make a nonsyntactic
   abc file.
*/
char * loc;
while (loc  = (char *) strchr(str,'\r')) *loc = ' ';
while (loc  = (char *) strchr(str,'\n')) *loc = ' ';
}



void printQ()
/* print out tempo for abc */
{
  float Tnote, freq;
  Tnote = mf_ticks2sec((long)((xunit*unitlen)/4), division, tempo);
  freq = (float) 60.0/Tnote;
  fprintf(outhandle,"Q:1/4=%d\n", (int) (freq+0.5));
  if (summary>0) printf("Tempo: %d quarter notes per minute\n",
    (int) (freq + 0.5));
}




void setupkey(int sharps)
/* set up variables related to key signature */
{
  char sharp[13], flat[13], shsymbol[13], flsymbol[13];
  int j, t, issharp;
  int minkey;

  for (j=0; j<12; j++) 
    key[j] = 0;
  minkey = (sharps+12)%12;
  if (minkey%2 != 0) {
    minkey = (minkey+6)%12;
  };
  strcpy(sharp,    "ccddeffggaab");
  strcpy(shsymbol, "=^=^==^=^=^=");
  if (sharps == 6) {
    sharp[6] = 'e';
    shsymbol[6] = '^';
  };
  strcpy(flat, "cddeefggaabb");
  strcpy(flsymbol, "=_=_==_=_=_=");
  /* Print out key */

  if (sharps >= 0) {
    if (sharps == 6) {
      fprintf(outhandle,"K:F#");
    } 
    else {
      fprintf(outhandle,"K:%c", sharp[minkey] + 'A' - 'a');
    };
    issharp = 1;
  } 
  else {
    if (sharps == -1) {
      fprintf(outhandle,"K:%c", flat[minkey] + 'A' - 'a');
    } 
    else {
      fprintf(outhandle,"K:%cb", flat[minkey] + 'A' - 'a');
    };
    issharp = 0;
  };
  if (sharps >= 0) {
    fprintf(outhandle," %% %d sharps\n", sharps);
  }
  else {
    fprintf(outhandle," %% %d flats\n", -sharps);
  };
  key[(minkey+1)%12] = 1;
  key[(minkey+3)%12] = 1;
  key[(minkey+6)%12] = 1;
  key[(minkey+8)%12] = 1;
  key[(minkey+10)%12] = 1;
  for (j=0; j<256; j++) {
    t = j%12;
    if (issharp) {
      atog[j] = sharp[t];
      symbol[j] = shsymbol[t];
    } 
    else {
      atog[j] = flat[t];
      symbol[j] = flsymbol[t];
    };
    trans[j] = 7*(j/12)+((int) atog[j] - 'a');
    if (j < MIDDLE) {
      atog[j] = (char) (int) atog[j] + 'A' - 'a';
    };
    if (key[t] == 0) {
      back[trans[j]] = j;
    };
  };
}




/*  Functions for supporting the command line user interface to midi2abc.     */


int readnum(char *num) 
/* read a number from a string */
/* used for processing command line */
{
  int t;
  char *p;
  int neg;
  
  t = 0;
  neg = 1;
  p = num;
  if (*p == '-') {
    p = p + 1;
    neg = -1;
  };
  while (((int)*p >= '0') && ((int)*p <= '9')) {
    t = t * 10 + (int) *p - '0';
    p = p + 1;
  };
  return neg*t;
}


int readnump(char **p) 
/* read a number from a string (subtly different) */
/* used for processing command line */
{
  int t;
  
  t = 0;
  while (((int)**p >= '0') && ((int)**p <= '9')) {
    t = t * 10 + (int) **p - '0';
    *p = *p + 1;
  };
  return t;
}


void readsig(int *a,int *b,char *sig)
/* read time signature */
/* used for processing command line */
{
  char *p;
  int t;

  p = sig;
  if ((int)*p == 'C') {
    *a = 4;
    *b = 4;
    return;
  };
  *a = readnump(&p);
  if ((int)*p != '/') {
    char msg[80];

    sprintf(msg, "Expecting / in time signature found %c!", *p);
    fatal_error(msg);
  };
  p = p + 1;
  *b = readnump(&p);
  if ((*a == 0) || (*b == 0)) {
    char msg[80];

    sprintf(msg, "%d/%d is not a valid time signature!", *a, *b);
    fatal_error(msg);
  };
  t = *b;
  while (t > 1) {
    if (t%2 != 0) {
      fatal_error("Bad key signature, divisor must be a power of 2!");
    }
    else {
      t = t/2;
    };
  };
}

int is_power_of_two(int numb)
/* checks whether numb is a power of 2 less than 256 */
{
int i,k;
k = 1;
for (i= 0;i<8;i++) {
  if(numb == k) return(1);
  k *= 2;
  }
return(0);
}

int getarg(char *option,int argc,char *argv[])
/* extract arguments from command line */
{
  int j, place;

  place = -1;
  for (j=0; j<argc; j++) {
    if (strcmp(option, argv[j]) == 0) {
      place = j + 1;
    };
  };
  return (place);
}

int huntfilename(int argc,char *argv[])
/* look for filename argument if -f option is missing */
/* assumes filename does not begin with '-'           */
{
  int j, place;

  place = -1;
  j = 1;
  while ((place == -1) && (j < argc)) {
    if (strncmp("-", argv[j], 1) != 0) {
      place = j;
    } 
    else {
     if (strchr("ambQkcou", *(argv[j]+1)) == NULL) {
       j = j + 1;
     }
     else {
       j = j + 2;
     };
    };
  };
  return(place);
}

int process_command_line_arguments(int argc,char *argv[])
{
  int val;
  int arg;
  arg = getarg("-ver",argc,argv);
//  if (arg != -1) {printf("%s\n",VERSION); exit(0);}
  midiprint = 0;
  arg = getarg("-midigram",argc,argv);
  if (arg != -1) 
   {
   midiprint = 1;
   }

#ifdef SPLITCODE
  usesplits = 0;
  arg = getarg("-splitbars",argc,argv);
  if (arg != -1) 
   usesplits = 1;
  arg = getarg("-splitvoices",argc,argv);
  if (arg != -1) 
   usesplits = 2;
#endif 

  arg = getarg("-mftext",argc,argv);
  if (arg != -1) 
   {
   midiprint = 2;
   }

  arg = getarg("-a", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    anacrusis = readnum(argv[arg]);
  } 
  else {
    anacrusis = 0;
  };
  arg = getarg("-m", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    readsig(&asig, &bsig, argv[arg]);
    tsig_set = 1;
  }
  else {
    asig = 4;
    bsig = 4;
    tsig_set = 0;
  };
  arg = getarg("-Q", argc, argv);
  if (arg != -1) {
    Qval = readnum(argv[arg]);
  }
  else {
    Qval = 0;
  };
  arg = getarg("-u", argc,argv);
  if (arg != -1) {
    xunit = readnum(argv[arg]);
    xunit_set = 1;
  }
  else {
	xunit = 0;
	xunit_set = 0;
       };
  arg = getarg("-ppu",argc,argv);
  if (arg != -1) {
     val = readnum(argv[arg]);
     if (is_power_of_two(val)) parts_per_unitlen = val;
     else {
	   printf("*error* -ppu parameter must be a power of 2\n");
           parts_per_unitlen = 2;
          }
  }
  else
     parts_per_unitlen = 2;
  arg = getarg("-aul",argc,argv);
  if (arg != -1) {
     val = readnum(argv[arg]);
     if (is_power_of_two(val)) {
	    unitlen = val;
	    unitlen_set = 1;}
        else 
           printf("*error* -aul parameter must be a power of 2\n");
      }
  arg = getarg("-bps",argc,argv);
  if (arg != -1)
   bars_per_staff = readnum(argv[arg]);
  else {
       bars_per_staff=4;
   };
  arg = getarg("-bpl",argc,argv);
  if (arg != -1)
   bars_per_line = readnum(argv[arg]);
  else {
       bars_per_line=1;
   };


  extracta = (getarg("-xa", argc, argv) != -1);
  guessa = (getarg("-ga", argc, argv) != -1);
  guessu = (getarg("-gu", argc, argv) != -1);
  guessk = (getarg("-gk", argc, argv) != -1);
  keep_short = (getarg("-s", argc, argv) != -1);
  summary = getarg("-sum",argc,argv); 
  swallow_rests = getarg("-sr",argc,argv);
  if (swallow_rests != -1) {
	 restsize = readnum(argv[swallow_rests]);
         if(restsize <1) restsize=1;
        }
  obpl = getarg("-obpl",argc,argv);
  if (obpl>= 0) bars_per_line=1;
  if (!unitlen_set) {
    if ((asig*4)/bsig >= 3) {
      unitlen =8;
     }
    else {
      unitlen = 16;
     };
   }
  arg = getarg("-b", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    bars = readnum(argv[arg]);
  }
  else {
    bars = 0;
  };
  arg = getarg("-c", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    xchannel = readnum(argv[arg]) - 1;
  }
  else {
    xchannel = -1;
  };
  arg = getarg("-k", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    keysig = readnum(argv[arg]);
    if (keysig<-6) keysig = 12 - ((-keysig)%12);
    if (keysig>6)  keysig = keysig%12;
    if (keysig>6)  keysig = keysig - 12;
    ksig_set = 1;
  } 
  else {
    keysig = -50;
    ksig_set = 0;
  };

  if(guessk) ksig_set=1;

  arg = getarg("-o",argc,argv);
  if ((arg != -1) && (arg < argc))  {
    outhandle = efopen(argv[arg],"w");  /* open output abc file */
  } 
  else {
    outhandle = stdout;
  };
  arg = getarg("-nt", argc, argv);
  if (arg == -1) {
    no_triplets = 0;
  } 
  else {
    no_triplets = 1;
  };
  arg = getarg("-nogr",argc,argv);
  if (arg != -1)
     nogr=1;
  else nogr = 0;

  arg = getarg("-title",argc,argv);
  if (arg != -1) {
       title = addstring(argv[arg]);
       }

  arg = getarg("-origin",argc,argv);
  if (arg != -1) {
       origin = addstring(argv[arg]);
       }
  

  arg = getarg("-f", argc, argv);
  if (arg == -1) {
    arg = huntfilename(argc, argv);
  };

  if ((arg != -1) && (arg < argc)) {
    F = efopen(argv[arg],"rb");
/*    fprintf(outhandle,"%% input file %s\n", argv[arg]); */
  }
  else {
    printf("midi2abc version %s\n  usage :\n",VERSION);
    printf("midi2abc filename <options>\n");
    printf("         -a <beats in anacrusis>\n");
    printf("         -xa  Extract anacrusis from file ");
    printf("(find first strong note)\n");
    printf("         -ga  Guess anacrusis (minimize ties across bars)\n");
    printf("         -gk  Guess key signature \n");
    printf("         -gu  Guess xunit from note duration statistics\n");
    printf("         -m <time signature>\n");
    printf("         -b <bars wanted in output>\n");
    printf("         -Q <tempo in quarter-notes per minute>\n");
    printf("         -k <key signature> -6 to 6 sharps\n");
    printf("         -c <channel>\n");
    printf("         -u <number of midi pulses in abc time unit>\n");
    printf("         -ppu <number of parts in abc time unit>\n");
    printf("         -aul <denominator of L: unit length>\n");
    printf("         [-f] <input file>\n");
    printf("         -o <output file>\n");
    printf("         -s Do not discard very short notes\n");
    printf("         -sr <number> Absorb short rests following note\n");
    printf("           where <number> specifies its size\n");
    printf("         -sum summary\n");
    printf("         -nt Do not look for triplets or broken rhythm\n");
    printf("         -bpl <number> of bars printed on one line\n");
    printf("         -bps <number> of bars to be printed on staff\n");
    printf("         -obpl One bar per line\n");
    printf("         -nogr No note grouping. Space between all notes\n");
#ifdef SPLITCODE
    printf("         -splitbars  splits bars to avoid nonhomophonic chords\n");
    printf("         -splitvoices  splits voices to avoid nonhomophonic chords\n");
#endif
    printf("         -title <string> Pastes title following\n");
    printf("         -origin <string> Adds O: field containing string\n");
    printf("         -midigram   Prints midigram instead of abc file\n");
    printf("         -mftext mftext output\n"); 
    printf("         -ver version number\n");
    printf(" None or only one of the options -gu, -b, -Q -u should\n");
    printf(" be specified. If none are present, midi2abc will uses the\n");
    printf(" the PPQN information in the MIDI file header to determine\n");
    printf(" the suitable note length. This is the recommended method.\n");
    printf(" The input filename is assumed to be any string not\n");
    printf(" beginning with a - (hyphen). It may be placed anywhere.\n");

    exit(0);
  };
  return arg;
}



void midi2abc (int arg,char *argv[])
{
int voiceno;
int accidentals; /* used for printing summary */
int j;
int argc;

/* initialization */
  trackno = 0;
  track[trackno].texthead = NULL;
  track[trackno].texttail = NULL;
  initfuncs();
  playinghead = NULL;
  playingtail = NULL;
  karaoke = 0;
  Mf_getc = filegetc;

/* parse MIDI file */
	  mfread();

  fclose(F);

/* count MIDI tracks with notes */
  maintrack = 0;
  while ((track[maintrack].notes == 0) && (maintrack < trackcount)) {
    maintrack = maintrack + 1;
  };
  if (track[maintrack].notes == 0) {
    fatal_error("MIDI file has no notes!");
  };

/* compute dtnext for each note */
  for (j=0; j<trackcount; j++) {
    postprocess(j);
  };

  if (tsig_set == 1){  /* for -m parameter set up time signature*/
       header_asig = asig; 
       header_unitlen = unitlen;
       header_bsig = bsig;
  };

/* print abc header block */
  argc = huntfilename(arg, argv);
  fprintf(outhandle,"X: 1\n"); 

  if (title != NULL) 
  fprintf(outhandle,"T: %s\n",title);
  else
  fprintf(outhandle,"T: from %s\n",argv[argc]); 

  if (origin != NULL)
  fprintf(outhandle,"O: %s\n",origin);

  if (header_bsig == 0) {
     fprintf(outhandle,"%%***Missing time signature meta command in MIDI file\n");
     setup_timesig(4,  4,  8);
     }
  fprintf(outhandle,"M: %d/%d\n", header_asig, header_bsig);
  fprintf(outhandle,"L: 1/%d\n", header_unitlen); 

  barsize = parts_per_unitlen*header_asig*header_unitlen/header_bsig;

/* compute xunit size for -Q -b options */
  if (Qval != 0) {
    xunit = mf_sec2ticks((60.0*4.0)/((float)(Qval*unitlen)), division, tempo);
  };
  if (bars > 0) {
    xunit = (int) ((float) track[maintrack].tracklen*2/(float) (bars*barsize));
    /* we multiply by 2 because barsize is in half unitlen. */
  };

/* estimate xunit if not set or if -gu runtime option */
  if (xunit == 0 || guessu) {
    guesslengths(maintrack);
  };

/* output Q: (tempo) to abc file */
  printQ();

  
/* Quantize note lengths of all tracks */
/* llena el campo playnum que es la duracion de la nota en unidades xunit */
  // break aqui en j=2 para ver como se calcula la duracion de la primera nota
  // jordi
  for (j=0; j<trackcount; j++) {
    quantize(j, xunit);
  };

//  printdebug(3);

  /* Estimate anacrusis if requested. Otherwise it is set by       */
/* user or set to 0.                                             */
  if (extracta) {
    anacrusis = findana(maintrack, barsize);
    fprintf(outhandle,"%%beats in anacrusis = %d\n", anacrusis);
  };
  if (guessa) {
    anacrusis = guessana(barsize);
    fprintf(outhandle,"%%beats in anacrusis = %d\n", anacrusis);
  };

/* If key signature is not known find the best one.              */
  if ((keysig == -50 && gotkeysig ==0) || guessk) {
       keysig = findkey(maintrack);
       if (summary>0) printf("Best key signature = %d flats/sharps\n",keysig);
       }
       header_keysig = keysig;
       setupkey(header_keysig);

/* scannotes(maintrack); For debugging */

/* Convert each track to abc format and print */
// jordi 15-2-2014, original trackcount > 1
  if (trackcount >= 1) {
    voiceno = 1;
    for (j=0; j<trackcount; j++) {
	  //******************************************************************
	  // jordi el tamaño del compas no puede variar de un track a otro
	  barsize = parts_per_unitlen*header_asig*header_unitlen/header_bsig;

	  //******************************************************************
      freshline();
      if (track[j].notes > 0) {
        fprintf(outhandle,"V:%d\n", voiceno);
	if (track[j].drumtrack) fprintf(outhandle,"%%%%MIDI channel 10\n");
        voiceno = voiceno + 1;
      };
#ifdef SPLITCODE
      if (usesplits==1) printtrack_with_splits(j, anacrusis); 
      else if (usesplits==2) printtrack_split_voice(j, anacrusis);
      else printtrack(j,anacrusis);
#else
      printtrack(j,anacrusis); 
#endif
    };
  }
  else {
#ifdef SPLITCODE
     if (usesplits==1) printtrack_with_splits(maintrack, anacrusis);
     else if (usesplits==2) printtrack_split_voice(maintrack, anacrusis);
     else printtrack(maintrack,anacrusis);
#else
     printtrack(maintrack, anacrusis);  
#endif
  };

  /* scannotes(maintrack); for debugging */

/* output report if requested */
  if(summary>0) {
   accidentals = keysig;
   if (accidentals <0 )
     {
     accidentals = -accidentals;
     printf("Using key signature: %d flats\n", accidentals);
     }
   else
     printf("Using key signature : %d sharps\n", accidentals);
   printf("Using an anacrusis of %d beats\n",anacrusis);
   printf("Using unit length : 1/%d\n",unitlen);
   printf("Using %d pulses per unit length (xunit).\n",xunit);
   printf("Producing %d bars of output.\n",maxbarcount);
   }


/* free up data structures */
  for (j=0; j< trackcount; j++) {
    struct listx* thiss;
    struct listx* x;
    struct tlistx* tthis;
    struct tlistx* tx;

    thiss = track[j].head;
    while (thiss != NULL) {
      free(thiss->note);
      x = thiss->next ;
      free(thiss);
      thiss = x;
    };
    tthis = track[j].texthead;
    while (tthis != NULL) {
      free(tthis->text);
      tx = tthis->next;
      free(tthis);
      tthis = tx;
    };
  };
  fclose(outhandle);
}

void midigram(int argc,char *argv[])
{
initfunc_for_midinotes();
init_notechan();
last_tick=0;
/*F = efopen(argv[argc -1],"rb");*/
Mf_getc = filegetc;
mfread();
printf("%d\n",last_tick);
}

void mftext(int argc,char *argv[])
{
initfunc_for_mftext();
init_notechan();
last_tick=0;
/*F = efopen(argv[argc -1],"rb");*/
Mf_getc = filegetc;
mfread();
/*printf("%d\n",last_tick);*/
}


int miditoabc(char *file)
{
  FILE *efopen();
  int r;
  char *ar[6],buff[300];

  clear_extern_var_midi2abc();
  clear_extern_var_midifile();
  for(r=0;r<6;r++) ar[r]=(char *) new char[300];
  strcpy(ar[1],file);
  strcpy(ar[2],"-ppu");
  strcpy(ar[3],"4");
  strcpy(ar[4],"-o");
  //strcpy(ar[5],"g.abc");
  strcpy(buff,currentdir);
  strcat(buff,"\\g.abc");
  strcpy(ar[5],buff);
  process_command_line_arguments(6,ar);
  if(midiprint ==1) midigram(6,ar);
  else if(midiprint ==2) mftext(6,ar);
  else midi2abc(6,ar); 
  return 0;
}


/*
LISTX *addsilenciotohead(atrack *j,int rest,int can,int xunit)
{
  LISTX *ap,*ne;
  int quantum,trest;
  int r;

  quantum = (int) (2.*xunit/parts_per_unitlen);
  for(r=0;r<can;r++){
     trest=2*quant(rest)/quantum;
	 if(r==0){
        ap=(LISTX *) new LISTX;
        ap->note=(ANOTE *) new ANOTE;
        ap->note->pitch=-1;     // para pruebas
        ap->note->chan=j->head->note->chan;
        ap->note->vel=j->head->note->vel;
        ap->note->time=0;
        ap->note->dtnext=rest;
        ap->note->tplay=rest;
        ap->note->toff=0;
        ap->note->playnum=trest;
        ap->note->xnum=trest;
	    ne=j->head;
	    j->head=ap;
        ap->next=ne;     
	 }else{
        addrest(ap,trest,rest);
		ap=ap->next;
	 }
  }
  return ap;
}
*/