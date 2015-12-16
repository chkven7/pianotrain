//compiled with Microsoft's Visual C/C++ 6.0 (with the Feb 2003 MSDN library of API's)
//MidiPlyr.c in the MSDN Samples got me going,
//Petzold's Programming Windows showed how simple it could be,
//and MIDImon.c in the MSDN Samples showed how to read MIDI keyboard data.
#include <windows.h>
#include <mmsystem.h>//add winmm.lib to Project -Settings -Link
#include <math.h>//for sin and 
#include <stdio.h>
#include <commctrl.h>//add comctl32.lib
#include "resource.h"
#include "piano.h"
#include "pianoabout.h"
#include "pianoconf.h"
#include "pianoinfo.h"

#define BETA 0               // si 1 muestra BETA en la title bar de la ventana
#define FAST 1               // si 0 mostrara reloj de arena y rectangulos con gradiente para mostrar los retardos en el ejercicio6
#define PI 3.141592653589793
#define IDM_EXIT 0x100
#define IDM_INSTRUMENT 0x200
#define IDM_INPUT 0X300
#define IDM_DEVICE 0x400
#define IDM_VELOCITY 0x500
#define IDM_KEYS 0x600
#define IDM_SHOWNAMEKEYS 0x601
#define IDM_SHOWVIRSTAFF 0x602
#define IDM_CHORDS 0x700
#define IDM_ACCIDENTAL 0x800
#define IDM_TEST 0x900
#define IDM_TEST1 0xC00
#define IDM_TEST2 0xD00
#define IDM_TEST4 0xD10
#define IDM_TEST5 0xD20
#define IDM_TEST6 0xD30
#define IDM_HELPEXERCISE 0xD40
#define IDM_UNSTICK 0xA00
#define IDM_ABOUT 0xB00
#define IDM_WEBPIANOTRAIN 0xB10
#define NEITHER 0
#define LEFT 1
#define RIGHT 2
#define BOTH 3
#define ERROR_MSG_SIZE 1024
#define ID_METRONOMO 30000
#define ID_ACCNOTES 30001
#define ID_ACCTEMPO 30002

#define BACKGROUND RGB(244,238,217)   // fondo de la ventana
#define OFFSETPENTA 5                 // offset previo al primer compas
#define RED RGB(255,0,0)
#define ORANGE RGB(255,128,0)
#define REDBLACK RGB(210,33,37)
#define BLACK RGB(0,0,0)
#define GRAY RGB(110,110,110)
#define GREEN RGB(0,255,0)
#define YELLOW RGB(255,255,0)
#define BROWN RGB(165,42,42)
#define GREENBOLD RGB(0,128,0)
#define BLUE RGB(0,0,255)
#define BLUEBOLD RGB(0,0,153)
#define PURPLE RGB(128,0,128)
#define WHITE RGB(255,255,255)

char About[] = "Version 1.0       \n\n27 Nov 2012         \nJordi Tomas       \n";

int BlackKeyNotes[25] = {37,39,42,44,46,49,51,54,56,58,61,63,66,68,70,73,75,78,80,82,85,87,90,92,94};
int saveAccidental;

//keys                       2   3       5   6   7   8   9   0       =           Q   W   E   R   T   Y   U   I   O   P   [   ]           A   S   D       G   H       K   L  ;                    Z   X   C   V   B   N   M   <   >   /
//scan codes     0,  1,  2,  3,  4, -1,  6,  7,  8, -1, 10, 11, -1, 13, -1, -1, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, -1, 30, 31, 32, -1, 34, 35, -1, 37, 38, 39, -1, -1, -1, -1, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53  -1  55  56  57  58  -1  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80
//notes                     C#  D#      F#  G#  A#      C#  D#      F#           C   D   E   F   G   A   B   C   D   E   F   G          F#  G#  A#      C#  D#      F#  G#  A#                   G   A   B   C   D   E   F   G   A   B       C   D   E   F       A   B   C   D   E   F   G   A   B  C#  D#      G#  A#  C#  D#  F#  G#  A#  B#   C
int Notes[] = { -1, -1, -1, 61, 63, -1, 66, 68, 70, -1, 73, 75, -1, 78, -1, -1, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, -1, -1, 42, 44, 46, -1, 49, 51, -1, 54, 56, 58, -1, -1, -1, -1, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, -1, 36, 38, 40, 41, -1, 81, 83, 84, 86, 88, 89, 91, 93, 95, 37, 39, -1, 80, 82, 85, 87, 90, 92, 94, 96, 97}; 
int xKey[] =  { -1, -1, -1, 15, 16, -1, 18, 19, 20, -1, 22, 23, -1, 25, -1, -1, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,  4,  5,  6, -1,  8,  9, -1, 11, 12, 13, -1, -1, -1, -1,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, -1,  0,  1,  2,  3, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34,  1,  2, -1, 26, 27, 29, 30, 32, 33, 34};
int yKey[] =  { -1, -1, -1,  0,  0, -1,  0,  0,  0, -1,  0,  0, -1,  0, -1, -1,  2,  3,  1,  2,  3,  3,  1,  2,  3,  1,  2,  3, -1, -1,  0,  0,  0, -1,  0,  0, -1,  0,  0,  0, -1, -1, -1, -1,  3,  3,  1,  2,  3,  1,  2,  3,  3,  1, -1,  2,  3,  1,  2, -1,  3,  1,  2,  3,  1,  2,  3,  3,  1,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0};
//t yStaff[] ={ -1, -1, -1,382,368, -1,356,350,344,338,332,326, -1,314, -1, -1,382,368,362,356,350,344,338,332,326,320,314,308, -1, -1,458,452,446, -1,434,428, -1,416,410,404, -1, -1, -1, -1,452,446,440,434,428,422,416,410,404,398, -1,476,470,464,458, -1,302,296,290,284,278,272,266,260,254,476,470, -1,308,302,290,284,272,266,260,254,248};
int yStaff[200];//36 is the lowest note
int gStaff[200]; //jordi, antes 61, con este valor en WM_SIZE el indice del array llega a 80
int fStaff[200]; // lo que provoca un fallo de memoria
int outminfStaff[200]; // notas por debajo de 36
int outmingStaff[200];
//               C   D   E   F   G   A   B   C   D   E   F   G   A   B   C   D   E   F   G   A   B   C   D   E   F   G   A   B   C   D   E   F   G   A   B
int LKeys[] = { 55, 69, 70, 58, 30, 31, 32, 47, 34, 35, 50, 37, 38, 39, 16,  3,  4, 19,  6,  7,  8, 23, 10, 11, 26, 13, 72, 73, 62, 74, 75, 65, 76, 77, 78};
int RKeys[] = { 69, 70, 57, 30, 31, 32, 46, 34, 35, 49, 37, 38, 39, 53,  3,  4, 18,  6,  7,  8, 22, 10, 11, 25, 13, 72, 73, 61, 74, 75, 64, 76, 77, 78, 68};
int MouseKeys[] = { -1, -1, -1, -1, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 55, 56, 57, 58, 60, 61, 62, 63, 64, 65, 66, 67, 68};

//              36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95  96
int Codes[] = { 55, 69, 56, 70, 57, 58, 30, 44, 31, 45, 32, 46, 47, 34, 48, 35, 49, 50, 37, 51, 38, 52, 39, 53, 16,  3, 17,  4, 18, 19,  6, 20,  7, 21,  8, 22, 23, 10, 24, 11, 25, 26, 13, 27, 72, 60, 73, 61, 62, 74, 63, 75, 64, 65, 76, 66, 77, 67, 78, 68, 80};

char CDEFGAB[] = "CDEFGAB";
char ComputerKeys1[] = "ZXCVBNM,./QWERTYUIOP[]";
char ComputerKeys2[] = "ASDFGHJKL;'234567890-=";
char Staff[128];
int StaffWidth[2];
int NoteWidth[2];
int GClefWidth[2];
int FClefWidth[2];
int Widths[7];
int Widths1[22];
int Widths2[22];
int ScanCodes[40];

int x,y, z, left, saveleft, top, top260, top260min30, top260min30div2, xPos, yPos, accidental = 1, index, TitleAndMenu;
int topvirstaff;
int iNumDevs,currenthor,currentver;
//, iInDevice = 0, iOutDevice = MIDIMAPPER;//-1 (pointer to driver for default Windows MIDI sounds)
int WhiteKeyWidth, BlackKeyWidth, ExtraSpace, noteloc, ChordType = 0, KeyName = 0, Inversion = 0;
int ScanCode, Note, PreviousNote = -1, SavedNote, BracketingKeys, Velocity;
//DefaultVelocity = 127;
DWORD dwInstance, dwParam1, dwParam2;
DWORD fileSize, dwBytesRead, dwBytesWritten;
double d1, d2, d3, d4;
char szAppName[64] = "Pianotrain ";
char oculto[]={-67,-70,-85,-66,-33,-87,-70,-83,-84,-74,-80,-79,0};
char Ini[512];
char InDev[] = "Input Device=";
char OutDev[] = "Output Device=";
char Vel[] = "Default Velocity=";
char ShowChords[] = "Toca &Acorde";
char HideChords[] = "Oculta &Acorde";
char showvirtualstaff[]="Ver &Sistema";
char shownamkeys[]="Ver &Notas";
char ShowKeys[] = "Ver Escala";
char HideKeys[] = "Oculta Escala";
char temp[16];
char VelocityChoice[8][4] = {"128","112"," 96"," 80"," 64"," 48"," 32"," 16"};
//char Keys[12][3] = {"C ","F ","Bb","Eb","Ab","Db","Gb","B ","E ","A ","D ","G "};
  char Keys[12][3] = {"C ","G ","D ","A ","E ","B ","Gb","Db","Ab","Eb","Bb","F "};
//int Scale[12] = {60,65,70,63,68,61,66,71,64,69,62,67};
  int Scale[12] = {60,67,62,69,64,71,66,61,68,63,70,65};
int Interval[] = {0,2,4,5,7,9,11,12};

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

static int noteindex[49];

#define NOTEREADRILE 5   // longitud del array de notas para el calculo del ritmo en notereading (antes 10)
NOTA RandomNote[64];     // maximo numero de notas en acorde 
int randomcannote; // numero de notas en randomnote	
int notereadingdeep;
int notereadingmemo; // numero de notas que se presentaran (no acordes) en color negro y que deberan tocarse
int notereadinglead; // lectura adelantada
NOTEREADINGLIST notereadinglist[NOTEREADRILE];
int notereadinglisthits[NOTEREADRILE];
double notereadingritmo;
double notereadinglegato;
int xKeyLoc[12];
int yKeyLoc[12];
int micentry;
int pernoteread;
int earnoteread;
int earinterval;

// semaforos
int semapaintclocksand1;

char Chords[10][25] = {"Sin Acorde       ","Triada Mayor    ","Triad Menor    ","Triada Dismin. ","Triada Aument.","7ma Dominante   ","7ma Mayor      ","7ma Menor      ","7ma Dismin. ","All Scale Notes"};
char Inversions[4][25] = {"Sin Inversion ","1ra Inversion","2da Inversion","3ra Inversion"};
char PlayNote[] =  "Localizacion de notas";
char PlayNote1[] =  "Musica Aleatoria";
char PlayNote2[] = "Localizacion de Intervalos";
char PlayNote4[] = "FlashCard de Intervalos";
char PlayNote5[] = "Location Inversa de Notas";
char PlayNote6[] = "Memoria Espacial del teclado";
char helpexer[]="Descripcion del Ejercicio";
char pianocfg[]="Opciones";
char About1[]="&Acerca de";
char coments[]="&Preguntas y Comentarios";
char Arial[] = "Times New Roman"; //"Arial";
char Maestro[] ="Maestro";
char ArialRounded[] = "Arial Rounded MT Bold";
char ErrorMsg[1024];
BOOL first = TRUE, fromkeydown, midi_in = FALSE, showchords = FALSE, showkeys = FALSE, showtest = FALSE, showscale = FALSE, showinginstruments = FALSE;
BOOL shownamekeys=FALSE,showvirstaff=TRUE;
BOOL showtest1=FALSE;
BOOL showtest2=FALSE;
BOOL showtest3=FALSE;
BOOL showtest4=FALSE;
BOOL showtest5=FALSE;
BOOL showtest6=FALSE;
BOOL helpexercise=FALSE;
HWND hwnd, hwndChords[9], hwndInversions[4], hwndKeys[12], hwndShowScale, hwndList;
HWND hwndnumeric[21],hwndaccnotes,hwndacctempo,hwndaccnotesvalue,hwndacctempovalue;
HWND hwndmetronomo,hwndmetronomovalue,hwndaccnotestitle,hwndacctempotitle,hwndmetronomotitle; 
HWND hwndvolume,hwndvolumetitle,hwndvolumevalue;
HWND hwndmetronoteread,hwndmetronotereadvalue,hwndmetronotereadtitle;
HWND hwndmetronoteinterval,hwndmetronoteintervalvalue,hwndmetronoteintervaltitle;
HWND buttonear,buttonrepe,buttonmic,buttonritmo,buttonmetro,buttoncfg,buttongame1,buttongame2,buttongame3,buttongame4,buttongame5;
HWND buttongame6,buttonplay,buttongoto;
HINSTANCE hInst;
HANDLE hFile;
HMENU hMenu, hMenuPopup;
HFONT hFont, hSmallFont,hSmallFontItalic, hMaestroFont,hMaestroFontSmall,hMaestroFontSmall1,hMaestroFontSmall2;
HFONT hMaestroFontSmall3;
HFONT hfontsystem,hfontsystemg;
HPEN hPen;
HBRUSH hWhiteBrush, hBlackBrush, hBlueBrush,hlBlueBrush,hPinkBrush,hHelpBrush,hGreenBrush,hRedBrush;
HGDIOBJ hOldFont, hOldPen, hOldBrush;
RECT rect, testRect;
HDC hdc, hdcMem1,hdccompathelp;
HBITMAP hMemBitmap1;
PAINTSTRUCT ps;
LOGFONT lf, lf1, lf2,lf3,lf4,lf5,lf6,lf7;
HMIDIOUT hMidiOut;
MIDIOUTCAPS moc;
HMIDIIN hMidiIn;
MIDIINCAPS mic;
SYSTEMTIME st;
FILETIME ft;
ULARGE_INTEGER ul;
char hlpvalue[64];
RECT hlprect;
HWND hlpwnd;
MEMOESPACIAL kiz,ref;

//TRACKMOUSEEVENT tme;

//******************************************************************
// numero de caracteres pentagrama (=) qur forma un compas en la pantalla
int BARTOBARSPACE=7;

// tamaño del pentagrama y notas (24 por defecto) 
// debe de ser un numero divisible por 8 con resto 0 (32 es el siguiente)
int STAFFSIZEINTERVAL=24;
int STAFFFAILSIZEINTERVAL=-16;
int ARIALKEYSIZE=15;

//numero de caracteres pentagrama(=) que llenan el ancho de pantalla
int NumOfStaffs;
//***************************************************************

int interline,ntinterline;
int offsetdispinterval;
int offsetdispintervalbck;

char Piano[] = "000 Acoustic Grand Piano";
char Piano2[] = "001 Bright Acoustic Piano";
char Piano3[] = "002 Electric Grand Piano";
char Piano4[] = "003 Honky Tonk Piano";
char Piano5[] = "004 Electric Piano 1";
char Piano6[] = "005 Electric Piano 2";
char Piano7[] = "006 Harpsichord";
char Piano8[] = "007 Clavinet";
char ChromaticPercussion[] = "008 Celesta";
char ChromaticPercussion2[] = "009 Glockenspiel";
char ChromaticPercussion3[] = "010 Music Box";
char ChromaticPercussion4[] = "011 Vibraphone";
char ChromaticPercussion5[] = "012 Marimba";
char ChromaticPercussion6[] = "013 Xylophone";
char ChromaticPercussion7[] = "014 Tubular Bells";
char ChromaticPercussion8[] = "015 Dulcimer";
char Organ[] = "016 Drawbar Organ";
char Organ2[] = "017 Percussive Organ";
char Organ3[] = "018 Rock Organ";
char Organ4[] = "019 Church Organ";
char Organ5[] = "020 Reed Organ";
char Organ6[] = "021 Accoridan";
char Organ7[] = "022 Harmonica";
char Organ8[] = "023 Tango Accordian";
char Guitar[] = "024 Nylon Acoustic Guitar";
char Guitar2[] = "025 Steel Acoustic Guitar";
char Guitar3[] = "026 Jazz Electric Guitar";
char Guitar4[] = "027 Clean Electric Guitar";
char Guitar5[] = "028 Muted Electric Guitar";
char Guitar6[] = "029 Overdrive Guitar";
char Guitar7[] = "030 Distorted Guitar";
char Guitar8[] = "031 Harmonica Guitar";
char Bass[] = "032 Acoustic Bass";
char Bass2[] = "033 Electric Fingered Bass";
char Bass3[] = "034 Electric Picked Bass";
char Bass4[] = "035 Fretless Bass";
char Bass5[] = "036 Slap Bass 1";
char Bass6[] = "037 Slap Bass 2";
char Bass7[] = "038 Syn Bass 1";
char Bass8[] = "039 Syn Bass 2";
char Strings[] = "040 Violin";
char Strings2[] = "041 Viola";
char Strings3[] = "042 Cello";
char Strings4[] = "043 Contrabass";
char Strings5[] = "044 Tremolo Strings";
char Strings6[] = "045 Pizzicato Strings";
char Strings7[] = "046 Orchestral Harp";
char Strings8[] = "047 Timpani";
char Ensemble[] = "048 String Ensemble 1";
char Ensemble2[] = "049 String Ensemble 2 (Slow)";
char Ensemble3[] = "050 Syn Strings 1";
char Ensemble4[] = "051 Syn Strings 2";
char Ensemble5[] = "052 Choir Aahs";
char Ensemble6[] = "053 Voice Olhs";
char Ensemble7[] = "054 Syn Choir";
char Ensemble8[] = "055 Orchestral Hit";
char Brass[] = "056 Trumpet";
char Brass2[] = "057 Trombone";
char Brass3[] = "058 Tuba";
char Brass4[] = "059 Muted Trumpet";
char Brass5[] = "060 French Horn";
char Brass6[] = "061 Brass Section";
char Brass7[] = "062 Syn Brass 1";
char Brass8[] = "063 Syn Brass 2";
char Reed[] = "064 Soprano Sax";
char Reed2[] = "065 Alto Sax";
char Reed3[] = "066 Tenor Sax";
char Reed4[] = "067 Baritone Sax";
char Reed5[] = "068 Oboe";
char Reed6[] = "069 English Horn";
char Reed7[] = "070 Bassoon";
char Reed8[] = "071 Clarinet";
char Pipe[] = "072 Piccolo";
char Pipe2[] = "073 Flute";
char Pipe3[] = "074 Recorder";
char Pipe4[] = "075 Pan Flute";
char Pipe5[] = "076 Bottle Blow";
char Pipe6[] = "077 Shakuhachi";
char Pipe7[] = "078 Whistle";
char Pipe8[] = "079 Ocarina";
char SynthLead[] = "080 Syn Square Wave";
char SynthLead2[] = "081 Syn Sawtooth Wave";
char SynthLead3[] = "082 Syn Calliope";
char SynthLead4[] = "083 Syn Chiff";
char SynthLead5[] = "084 Syn Chrang";
char SynthLead6[] = "085 Syn Voice";
char SynthLead7[] = "086 Syn Fifths Sawtooth Wave";
char SynthLead8[] = "087 Syn Brass & Lead";
char SynthPad[] = "088 New Age Syn Pad";
char SynthPad2[] = "089 Warm Syn Pad";
char SynthPad3[] = "090 Polysynth Syn Pad";
char SynthPad4[] = "091 Choir Syn Pad";
char SynthPad5[] = "092 Bowed Syn Pad";
char SynthPad6[] = "093 Metal Syn Pad";
char SynthPad7[] = "094 Halo Syn Pad";
char SynthPad8[] = "095 Sweep Syn Pad";
char SynthEffects[] = "096 SFX Rain";
char SynthEffects2[] = "097 SFX Soundtrack";
char SynthEffects3[] = "098 SFX Crystal";
char SynthEffects4[] = "099 SFX Atmosphere";
char SynthEffects5[] = "100 SFX Brightness";
char SynthEffects6[] = "101 SFX Goblins";
char SynthEffects7[] = "102 SFX Echoes";
char SynthEffects8[] = "103 SFX Sci-Fi";
char Ethnic[] = "104 Sitar";
char Ethnic2[] = "105 Banjo";
char Ethnic3[] = "106 Shamisen";
char Ethnic4[] = "107 Koto";
char Ethnic5[] = "108 Kalimba";
char Ethnic6[] = "109 Bag Pipe";
char Ethnic7[] = "110 Fiddle";
char Ethnic8[] = "111 Shanai";
char Percussive[] = "112 Tinkle Bell";
char Percussive2[] = "113 Agogo";
char Percussive3[] = "114 Steel Drum";
char Percussive4[] = "115 Woodblock";
char Percussive5[] = "116 Taiko Drum";
char Percussive6[] = "117 Melodic Tom";
char Percussive7[] = "118 Syn Drum";
char Percussive8[] = "119 Reverse Cymbal";
char SoundEffects[] = "120 Guitar Fret Noise";
char SoundEffects2[] = "121 Breath Noise";
char SoundEffects3[] = "122 Seashore";
char SoundEffects4[] = "123 Bird Tweet";
char SoundEffects5[] = "124 Telephone Ring";
char SoundEffects6[] = "125 Helicopter";
char SoundEffects7[] = "126 Applause";
char SoundEffects8[] = "127 Gun Shot";

DWORD *Instruments[128] = {\
(DWORD*)Piano,
(DWORD*)Piano2,
(DWORD*)Piano3,
(DWORD*)Piano4,
(DWORD*)Piano5,
(DWORD*)Piano6,
(DWORD*)Piano7,
(DWORD*)Piano8,
(DWORD*)ChromaticPercussion,
(DWORD*)ChromaticPercussion2,
(DWORD*)ChromaticPercussion3,
(DWORD*)ChromaticPercussion4,
(DWORD*)ChromaticPercussion5,
(DWORD*)ChromaticPercussion6,
(DWORD*)ChromaticPercussion7,
(DWORD*)ChromaticPercussion8,
(DWORD*)Organ,
(DWORD*)Organ2,
(DWORD*)Organ3,
(DWORD*)Organ4,
(DWORD*)Organ5,
(DWORD*)Organ6,
(DWORD*)Organ7,
(DWORD*)Organ8,
(DWORD*)Guitar,
(DWORD*)Guitar2,
(DWORD*)Guitar3,
(DWORD*)Guitar4,
(DWORD*)Guitar5,
(DWORD*)Guitar6,
(DWORD*)Guitar7,
(DWORD*)Guitar8,
(DWORD*)Bass,
(DWORD*)Bass2,
(DWORD*)Bass3,
(DWORD*)Bass4,
(DWORD*)Bass5,
(DWORD*)Bass6,
(DWORD*)Bass7,
(DWORD*)Bass8,
(DWORD*)Strings,
(DWORD*)Strings2,
(DWORD*)Strings3,
(DWORD*)Strings4,
(DWORD*)Strings5,
(DWORD*)Strings6,
(DWORD*)Strings7,
(DWORD*)Strings8,
(DWORD*)Ensemble,
(DWORD*)Ensemble2,
(DWORD*)Ensemble3,
(DWORD*)Ensemble4,
(DWORD*)Ensemble5,
(DWORD*)Ensemble6,
(DWORD*)Ensemble7,
(DWORD*)Ensemble8,
(DWORD*)Brass,
(DWORD*)Brass2,
(DWORD*)Brass3,
(DWORD*)Brass4,
(DWORD*)Brass5,
(DWORD*)Brass6,
(DWORD*)Brass7,
(DWORD*)Brass8,
(DWORD*)Reed,
(DWORD*)Reed2,
(DWORD*)Reed3,
(DWORD*)Reed4,
(DWORD*)Reed5,
(DWORD*)Reed6,
(DWORD*)Reed7,
(DWORD*)Reed8,
(DWORD*)Pipe,
(DWORD*)Pipe2,
(DWORD*)Pipe3,
(DWORD*)Pipe4,
(DWORD*)Pipe5,
(DWORD*)Pipe6,
(DWORD*)Pipe7,
(DWORD*)Pipe8,
(DWORD*)SynthLead,
(DWORD*)SynthLead2,
(DWORD*)SynthLead3,
(DWORD*)SynthLead4,
(DWORD*)SynthLead5,
(DWORD*)SynthLead6,
(DWORD*)SynthLead7,
(DWORD*)SynthLead8,
(DWORD*)SynthPad,
(DWORD*)SynthPad2,
(DWORD*)SynthPad3,
(DWORD*)SynthPad4,
(DWORD*)SynthPad5,
(DWORD*)SynthPad6,
(DWORD*)SynthPad7,
(DWORD*)SynthPad8,
(DWORD*)SynthEffects,
(DWORD*)SynthEffects2,
(DWORD*)SynthEffects3,
(DWORD*)SynthEffects4,
(DWORD*)SynthEffects5,
(DWORD*)SynthEffects6,
(DWORD*)SynthEffects7,
(DWORD*)SynthEffects8,
(DWORD*)Ethnic,
(DWORD*)Ethnic2,
(DWORD*)Ethnic3,
(DWORD*)Ethnic4,
(DWORD*)Ethnic5,
(DWORD*)Ethnic6,
(DWORD*)Ethnic7,
(DWORD*)Ethnic8,
(DWORD*)Percussive,
(DWORD*)Percussive2,
(DWORD*)Percussive3,
(DWORD*)Percussive4,
(DWORD*)Percussive5,
(DWORD*)Percussive6,
(DWORD*)Percussive7,
(DWORD*)Percussive8,
(DWORD*)SoundEffects,
(DWORD*)SoundEffects2,
(DWORD*)SoundEffects3,
(DWORD*)SoundEffects4,
(DWORD*)SoundEffects5,
(DWORD*)SoundEffects6,
(DWORD*)SoundEffects7,
(DWORD*)SoundEffects8};

int notereadtimersand;
int noteintervaltimersand;

//***********************************
// variable de estado para sigueme y followme

HANDLE hplaymelodia;
int playmelodiastatus;
int playmelodiastopv;
HANDLE htwoplay;
int twoplaystop;             
int siguemestatus;
int metronomostatus;
int metronomostop;
int metronomobeat;
int metrodivisor=1;
HANDLE hmetronomo;
int followmestatus;
int followmestop;
HANDLE hfollowme;
int posyminpenta;
int posymaxpenta;

HWND    autobutton[17];
HBITMAP bmpauto[17];
HBITMAP bmpsand,bmpcheck,bmpkey,bmpwoodu,bmpwoodd,bmpmicro,bmpmicroon,bmpritmon,bmpritmoy,bmpmetron,bmpmetroy;
HBITMAP bmpokan,bmpokay,bmpcfg,bmpcfgy,bmpgame1,bmpgame2,bmpgame3,bmpgame4,bmpgame5,bmpgame6,bmpsplash,bmpearn,bmpeary,bmprepe;
HBITMAP bmphandl,bmphandr,bmpplayn,bmpplayy,bmpgoto;
HBITMAP hbitmaphelp;
BITMAP  bmphelp;
MELODIA *melodia,*bufmelodia;
int gfstat[2];    // estado del conmutador de una mano o ambas
TCHAR currentdir[256],titlebar[256];
SCORE notereadingscore;
SCORE notereadinversescore;
SCORE memoespacialscore;
CONFIG concfg;
int Instrument = 0;

void DrawKey(int source);
void StartNote(int source);
void EndNote(int source);
void CALLBACK MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
int Atoi(char*);
void WriteIni(void);
void notereading(int,int);
void notereadingupdate(void);
void notereadingfail(int,int,int);
void notereadingchords(int gen,int hitfail);
int InitRandomMusicTest(void);
void paintrandommusic(MELODIA *melodia);
WNDPROC pListProc;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int generarmelodia(MELODIA *&melodia);
void cargarconfiguracion(MELODIA *melodia);
double round(double doValue, int nPrecision,int ro=1);
void scrollrandommusic(MELODIA *melodia,int scroll);
void LiberaMelodia(MELODIA *&);
void liberathreads(void);
void endthreads(void);
EJECUCION *generaejecucion(PENTA *penta,int compas);
void intervaltestfail(MELODIA *melodia,int r,int c);
void IntervalTestUpdate(int Note,int, int init);
void genetic(MELODIA *melodia,int penta);
int viterbi(MELODIA *melodia,int penta);
int viterbi(NOTA *listanotas,int can,int penta);
int calcnoteindex(NOTA *n1);
int calcnoteindex(int n1);
int onspace(int n1);
int istogether(int n1,int n2);
NOTA *genrndnote(void);
CHORD *genrndchord(void);
NOTA *genrndfingernote(NOTA *listanotas,int can);
void Paintnotereading(NOTA *nt,int can,int onestaff,int armadura,int pintanota);
void NoteInterval(void);
void NoteIntervalCreateSlidingControls(void);
void NoteIntervalCreateButtonsremove(void);
NOTA *scrollnoteinterval(MELODIA *melodia,int init);
void IntervalTestUpdate1(MELODIA *melodia);
void IntervalTestUpdate2(MELODIA *melodia);
void PaintNoteInterval(NOTA *nt,int can,int,int);
void paintscores(SCORE *);
void paintrandommusicscores(SCORE *);
void paintrandommusicsolorithm(MELODIA *melodia,int penta,int compas);
void resetscores(MELODIA *melodia);
void resetmelodia(MELODIA *melodia,int initcompas=0);
NOTA *genrndnotestadistical(HITSPROB *hp,int);
void saveconfiguracion(void);
void calctempomedio(MELODIA *melodia,int penta,int compas);
void mydrawtext(HWND hw,COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposy,int init);
void mydrawtext1(COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposx,int sposy,int init);
void painttemposcore(MELODIA *melodia);
void painttemposcoretext(MELODIA *melodia);
void painttempobymeasure(MELODIA *melodia,int penta,int compas);
void FlashCardButtonshideshow(int hs) ;
void FlashCardNoteInterval(void);
void FlashCardCreateButtons(void);
void FlashCardCreateButtonsremove(void);
int interval(NOTA *n1,NOTA *n2);
void IntervalTestUpdate3(MELODIA *melodia);
void paintclocksand(int posx,int posy,int color,int color1,int color2,int per);
COLORREF paintclocksand1(int posx,int posy,double umbral,double per);
void paintrectangle(int posx,int posy,int color,int per);
MELODIA *copiamelodia(MELODIA *melodia);
void allsilencio(MELODIA *melodia,int penta);
void RandomMusicButtonsScoreshideshow(int hs);
void RandomMusicSlidingControlshideshow(int hs);
void RandomMusicButtonsScoresremove(void);
void RandomMusicSlidingControlsremove(void);
void liberabitmaps(void);
void calcarmaduranotereading(int **&posarm,int armadura);
void NoteReadingButtonshideshow(int hs);
void NoteReadingCreateButtonsremove(void);
void NoteReadingCreateButtons(void);
void NotereadInverseCreateButtons(void);
int armadura(int minota,int,int ar);
void loadbitmaps(void);
void paintkey(int posx,int posy);
void paintwoodu(int posx,int posy);
void paintwoodd(int posx,int posy);
void paintok(int posx,int posy,int yn,int score);
void paintrandommusicqosmeasure(MELODIA *);
void paintsplash(int posx,int posy,int upd);
int semitonos(int nota,int acci);
void updatesliders(HWND hwndScrollBar);
void GenericCreateControls(HWND);
void GenericControlsRemove(void);
void GetDesktopResolution(int& horizontal, int& vertical);
int pitchdetection(int);  // la primera llamada activa pitch la siguiente lo desactiva
int solfeohablado(int offline,int learn,char *learnnota); 
void PlayNotaMidi(__int64 nota,int push=1);
void removemousemsgqueue(void); 
void NotereadInverseButtonshideshow(int); 
void NotereadInverse(HBITMAP respkiz,int);
void NotereadInverseCreateButtonsremove(void);
int paintstaff(HDC hdc,int x,int y,int can);
NOTA addinterval(NOTA *nta,int interval,int mi,int ma);
double uniform_deviate(int seed);
void paintkeypiano(int notekey,int estado,int oneoctava,HBRUSH hbr);
void paintkeypiano1(int notekey,int estado,HBRUSH hbr,HDC hdcext);
void painthand(int posx,int posy,int lr);
void initmemoespacial(int);
void memoespacialupdate(int);
void genericbuttonhideshow(int hs);
void ajustafuentesrandommusic(int);
void RandomMusicCreateButtonsScores(void);
void RandomMusicCreateSlidingControls(void);
void paintpentamemoespacial(int posx,int posy,HBITMAP hbmp);
void liberamemoespacialbmp(MEMOESPACIAL *me);
char *tablatempo(int tempo);
void playmelodiastop(void);
void disenabuttongeneric(BOOL ena);
void paintbomba(int posx0,int posy0,int posx1,int posy1,COLORREF col);
void runfollowme(void);
void endfollowme(void);
void runsigueme(void);
void endsigueme(void);
COLORREF paintritmicexecution(MELODIA *melodia,int penta,int compas,int mode);
void paintrandommusiccursor1(MELODIA *melodia, COLORREF color,HDC exhdc,int init);
void mydrawtext2(COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposx,int sposy);
EJECUCION *generaejecucion1(PENTA *penta,int compas);
RECT *paintmetronomostaff(MELODIA *melodia,int gpos);
double similaridad(double *x,double *y,int n);
DWORD WINAPI playmelodia(void);
DWORD WINAPI twoplaymelodia(void);
DWORD WINAPI paintfollowme(void);
DWORD WINAPI paintmetronomo(void);
void paintrect(RECT *rec);
void soloritmo(void);
char *getnumtonum(int num);
char *gettiponotasingle(int dura);
void ajustatiempoenties(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	MSG          msg;
	WNDCLASS     wndclass;
    HBRUSH Blue = CreateSolidBrush(BACKGROUND);

	// desencriptamos el tipo de version del programa
	if(BETA==1){
	   int r;
	   for(r=0;r<strlen(oculto);r++){
	      oculto[r]=oculto[r]^0xff;
	   }
	   strcat(szAppName,oculto);
	}
	hInst = hInstance;
	wndclass.style         = CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) Blue; // (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
		return 0;

	hwnd = CreateWindow(szAppName, szAppName,
		//WS_OVERLAPPEDWINDOW,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |  WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

// fijamos el volumen del dispositivo de ondas para que el metronomo este mas bajo en
// volumen que el del midi
int setwavevolume(void)
{
  HWAVEOUT WaveOutDevice=NULL;
  DWORD pdwVolume;
  MMRESULT result;
  WAVEFORMATEX wf;
  unsigned long Left;
  unsigned long Right;

  wf.wFormatTag = WAVE_FORMAT_PCM;
  wf.nChannels = 1;
  wf.nSamplesPerSec = 8000;
  wf.wBitsPerSample = 8;
  wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
  wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
  wf.cbSize = 0;

  for (UINT id = 0; id < waveOutGetNumDevs(); id++)
  {
    if (waveOutOpen(&WaveOutDevice, id, &wf, 0, 0, CALLBACK_NULL)== MMSYSERR_NOERROR)
    {
      break; //Wave Device found!
    }
  }
  if (WaveOutDevice==NULL) return -1;
  result = waveOutGetVolume(WaveOutDevice,&pdwVolume);
  if(result != MMSYSERR_NOERROR)
     pdwVolume=0; // e have a problem!

  Left=10922;
  Right=10922;
  pdwVolume=Left+Right * 65536;
  waveOutSetVolume(WaveOutDevice,pdwVolume);
  waveOutClose(WaveOutDevice);
  return 1;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
 
   const HWND hDesktop = GetDesktopWindow();
   GetWindowRect(hDesktop, &desktop);
   horizontal = desktop.right;
   vertical = desktop.bottom;
   return;
}

//sub-class procedure
LRESULT ListProc(HWND hwnd2, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONUP)
	{
		index = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
		Instrument = index;
		midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));
		DestroyWindow(hwndList);
	}
	else if (message == WM_KEYUP)
	{
		if ((wParam == VK_DOWN) && (index < 127))
			index++;
		else if ((wParam == VK_UP) && (index != 0))
			index--;

		else if (wParam == VK_ESCAPE) {
			DestroyWindow(hwndList);
		}
		else if (wParam == VK_RETURN) {
			index = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			if (index < 128) {
				DestroyWindow(hwndList);
				Instrument = index;
				midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));
			}
		}
	}
	else if (message == WM_DESTROY)
		showinginstruments = FALSE;

	return CallWindowProc(pListProc, hwnd2, message, wParam, lParam);
}

void calccoordfgmystaff(int noteloc)
{
	int x,z,j,noteloc1,noteloc64;
	if(currenthor==1024){
	   if(noteloc==top-98)
          gStaff[19]=390;
	   if(noteloc==top-98+100)
          gStaff[19]=454;
	}
	
	//****************************
	// 1-3-14
	noteloc1=noteloc;
	for(x = 0; x < 60; x += 12){
	   for (z = 0; z < 12; ){
	      outminfStaff[x+(z++)] = noteloc;//36  -0
		  noteloc += 0.5*interline;		  
		  outminfStaff[x+z++] = noteloc;//35    -1
		  noteloc += 0.5*interline;
		  outminfStaff[x+z++] = noteloc;//33    -2
		  outminfStaff[x+z++] = noteloc;//33    -3
          noteloc += 0.5*interline;
		  outminfStaff[x+z++] = noteloc;//31    -4
		  outminfStaff[x+z++] = noteloc;//31    -5
          noteloc += 0.5*interline;
		  outminfStaff[x+z++] = noteloc;//29    -6
		  outminfStaff[x+z++] = noteloc;//29    -7
          noteloc += 0.5*interline;
          outminfStaff[x+z++] = noteloc;//28    -8
          noteloc += 0.5*interline;
		  outminfStaff[x+z++] = noteloc;//26    -9
		  outminfStaff[x+z++] = noteloc;//26    -10
          noteloc += 0.5*interline;
	   }
	}
    noteloc=noteloc1;
	//****************************************

		for (x = 0; x < 60; x += 12)
		{
			for (z = 0; z < 12; )
			{
				fStaff[x+(z++)] = noteloc;//36 C
				fStaff[x+z++] = noteloc;//37 C#
				noteloc -= 0.5*interline;
				fStaff[x+z++] = noteloc;//38 D
				fStaff[x+z++] = noteloc;//39 D#
				noteloc -= 0.5*interline;
				fStaff[x+z++] = noteloc;//40 E
				noteloc -= 0.5*interline;
//				if ((x+z-1) == (64-36))//E above middle C
//				   break;
				if ((x+z-1) == (64-36)){//E above middle C
				   noteloc64=noteloc;
				}
				fStaff[x+z++] = noteloc;//41 F
				fStaff[x+z++] = noteloc;//42 F#
				noteloc -= 0.5*interline;
				fStaff[x+z++] = noteloc;//43 G
				fStaff[x+z++] = noteloc;//44 G#
				noteloc -= 0.5*interline;
				fStaff[x+z++] = noteloc;//45 A
				fStaff[x+z++] = noteloc;//46 A#
				noteloc -= 0.5*interline;
				fStaff[x+z++] = noteloc;//47 B
				noteloc -= 0.5*interline;
			}
//			if ((x+z-1) == (64-36))//E above middle C
//		      break;
		}
		noteloc=noteloc64;
		noteloc-=interline;
		//***********************************************
		// mejorar las dos lineas siguiente
		// el valor siguiente es gstaff[55-36 (=19)]
		// se calculan las siguiente lineas adicionales en sol sumando 5 a 449 o
		// 5 a 385
	//	if(concfg.maxcompascreen>=5) gStaff[19]=390;   // para maxcompasscreen>=5  gStaff[19]=449;
	//	else gStaff[19]=454; 
		///*******************************************************
        noteloc1=noteloc;
		for (x=20;x < 69; x += 12){
			for (z = 0; z < 12; ){
				gStaff[x+(z++)] = noteloc;//36 C
				gStaff[x+z++] = noteloc;//37 C#
				noteloc -= 0.5*interline;
				gStaff[x+z++] = noteloc;//38 D
				gStaff[x+z++] = noteloc;//39 D#
				noteloc -= 0.5*interline;
				gStaff[x+z++] = noteloc;//40 E
				noteloc -= 0.5*interline;
				gStaff[x+z++] = noteloc;//41 F
				gStaff[x+z++] = noteloc;//42 F#
				noteloc -= 0.5*interline;
				gStaff[x+z++] = noteloc;//43 G
				gStaff[x+z++] = noteloc;//44 G#
				noteloc -= 0.5*interline;
				gStaff[x+z++] = noteloc;//45 A
				gStaff[x+z++] = noteloc;//46 A#
				noteloc -= 0.5*interline;
				gStaff[x+z++] = noteloc;//47 B
				noteloc -= 0.5*interline;
			}
		}
//*************************************************
		noteloc=noteloc1;
		for (x=20;x < 69; x += 12){
			for (z = 0; z < 12; ){
				outmingStaff[x+(z++)] = noteloc;//36 C
				outmingStaff[x+z++] = noteloc;//37 C#
				noteloc += 0.5*interline;
				outmingStaff[x+z++] = noteloc;//38 D
				outmingStaff[x+z++] = noteloc;//39 D#
				noteloc += 0.5*interline;
				outmingStaff[x+z++] = noteloc;//40 E
				noteloc += 0.5*interline;
				outmingStaff[x+z++] = noteloc;//41 F
				outmingStaff[x+z++] = noteloc;//42 F#
				noteloc += 0.5*interline;
				outmingStaff[x+z++] = noteloc;//43 G
				outmingStaff[x+z++] = noteloc;//44 G#
				noteloc += 0.5*interline;
				outmingStaff[x+z++] = noteloc;//45 A
				outmingStaff[x+z++] = noteloc;//46 A#
				noteloc += 0.5*interline;
				outmingStaff[x+z++] = noteloc;//47 B
				noteloc += 0.5*interline;
			}
		}
		for(j=0;j<=19;j++)
		   gStaff[19-j]=outmingStaff[22+j];
		return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL fBlt;            // TRUE if BitBlt occurred 
    static BOOL fScroll;         // TRUE if scrolling occurred 
    static BOOL fSize;           // TRUE if fBlt & WM_SIZE 
    static int xMinScroll;       // minimum horizontal scroll value 
    static int xCurrentScroll;   // current horizontal scroll value 
    static int xMaxScroll;       // maximum horizontal scroll value 
    static int yMinScroll;       // minimum vertical scroll value 
    static int yCurrentScroll;   // current vertical scroll value 
    static int yMaxScroll;       // maximum vertical scroll value 
    static int ismousedrag;
    static int currentscancode;

	switch(message)
	{
	case WM_CREATE:
        int hor,ver;

		INITCOMMONCONTROLSEX icex;	//for common controls
	    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	    icex.dwICC = ICC_BAR_CLASSES;
	    InitCommonControlsEx(&icex);	//load the trackbar class

		if (0 == AddFontResource("MAESTRO_.TTF"))
			MessageBox(hwnd, "Couldn't load the MAESTRO_.TTF font!\nMake sure it's in the Simple Piano folder,\nor the folder that Windows7 moves data files to.", ERROR, MB_OK);
		GetDesktopResolution(hor,ver);
		currenthor=hor;
        currentver=ver;
        if(!(currenthor==1024 && currentver==768)){// || currenthor==1280 && currentver==1024)){
		   //MessageBox(hwnd,"La resolucion no es 1024x768 o 1280x1024","PIANOTRAIN",MB_OK|MB_ICONWARNING);
		   MessageBox(hwnd,"La resolucion no es 1024x768","PIANOTRAIN",MB_OK|MB_ICONWARNING);
		   exit(1);
		}
		GetCurrentDirectory((DWORD) 256,(LPTSTR) currentdir);
		loadbitmaps();
		SetTimer(hwnd,3,2000,NULL);  // temporizador para splash
 		GetLocalTime(&st);
		SystemTimeToFileTime(&st, &ft);
		ul.LowPart = ft.dwLowDateTime;
		ul.HighPart = ft.dwHighDateTime;
		ul.QuadPart /= 10000;//because low 4 digits are 0's
		srand(ul.LowPart);

		hwndKeys[0] = hwndChords[0] = NULL;
		cargarconfiguracion(NULL);		
		hMenu = CreateMenu();
		AppendMenu(hMenu, MF_STRING, IDM_EXIT, "&Salir");
		AppendMenu(hMenu, MF_STRING, IDM_INSTRUMENT, "&Instrumento");
		iNumDevs = midiOutGetNumDevs();
		z = midiInGetNumDevs();
		setwavevolume();
		if (z) {
			hMenuPopup = CreateMenu();
			AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT)hMenuPopup, "Input &Device");
			for (x = 0; x < z; x++) {
				if (MMSYSERR_NOERROR == midiInGetDevCaps(x, &mic, sizeof(mic)))
					AppendMenu(hMenuPopup, MF_STRING, IDM_INPUT + x, mic.szPname);
			}
			CheckMenuItem(hMenuPopup, IDM_INPUT + concfg.iInDevice, MF_CHECKED);
		}
		if (MMSYSERR_NOERROR == midiOutGetDevCaps(MIDIMAPPER, &moc, sizeof(moc)))//Microsoft MIDI Mapper
		{
			hMenuPopup = CreateMenu();
			AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT)hMenuPopup, "&Dispositivos");
			AppendMenu(hMenuPopup, MF_STRING, IDM_DEVICE + (int)MIDIMAPPER, moc.szPname);
			for (x = 0; x < iNumDevs; x++)
			{
				midiOutGetDevCaps(x, &moc, sizeof(moc));
				AppendMenu(hMenuPopup, MF_STRING, IDM_DEVICE + x, moc.szPname);
			}
			CheckMenuItem(hMenuPopup, IDM_DEVICE + concfg.iOutDevice, MF_CHECKED);
		}
		hMenuPopup = CreateMenu();
		AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT)hMenuPopup, "&Utilidades");
		AppendMenu(hMenuPopup, MF_CHECKED, IDM_SHOWVIRSTAFF, showvirtualstaff);
		AppendMenu(hMenuPopup, MF_STRING, IDM_SHOWNAMEKEYS, shownamkeys);
		AppendMenu(hMenuPopup, MF_STRING, IDM_KEYS, ShowKeys);
		AppendMenu(hMenuPopup, MF_STRING, IDM_CHORDS, ShowChords);

        hMenuPopup = CreateMenu();
        AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT)hMenuPopup, "&Ejercicios");
		AppendMenu(hMenuPopup, MF_STRING, IDM_TEST, PlayNote);
		AppendMenu(hMenuPopup, MF_STRING, IDM_TEST5, PlayNote5);
		AppendMenu(hMenuPopup, MF_STRING, IDM_TEST4, PlayNote4);
		AppendMenu(hMenuPopup, MF_STRING, IDM_TEST2, PlayNote2);
        AppendMenu(hMenuPopup, MF_STRING, IDM_TEST6, PlayNote6);
        AppendMenu(hMenuPopup, MF_STRING, IDM_TEST1, PlayNote1);       
        AppendMenu(hMenuPopup, MF_STRING, DIAL_APPR, pianocfg);
    
		hMenuPopup = CreateMenu();
		AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT)hMenuPopup, "&Ayuda");
        AppendMenu(hMenuPopup, MF_STRING, IDM_HELPEXERCISE, "&Descripcion del Ejercicio");
        AppendMenu(hMenuPopup, MF_STRING, IDM_WEBPIANOTRAIN,coments);
		AppendMenu(hMenuPopup, MF_STRING, IDM_ABOUT, About1);
		SetMenu(hwnd, hMenu);

		if (MMSYSERR_NOERROR == midiOutOpen(&hMidiOut, concfg.iOutDevice, 0, 0, 0)) {
			midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));//channel 0 and grand piano (instrument 0)
			midiConnect((HMIDI)hMidiIn, (HMIDIOUT)hMidiOut, NULL);//THRU MIDI
		}

		if (MMSYSERR_NOERROR == midiInOpen((LPHMIDIIN)&hMidiIn, concfg.iInDevice, (DWORD)MidiInProc, 0, CALLBACK_FUNCTION))
		{//if a MIDI keyboard is attached
			midi_in = TRUE;
			midiInStart(hMidiIn);
		}

		lf.lfHeight = -ARIALKEYSIZE; //-29;
		lf.lfWeight = 700;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;
		lf.lfStrikeOut = 0;
		lf.lfCharSet = 0;
		lf.lfOutPrecision = 3;
		lf.lfClipPrecision = 2;
		lf.lfQuality = 1;
		lf.lfPitchAndFamily = 0x22;
		for (x = 0; Arial[x] != 0; x++)
			lf.lfFaceName[x] = Arial[x];
		lf.lfFaceName[x] = 0;
		hFont = CreateFontIndirect(&lf);

		lf1.lfHeight = -13;
		lf1.lfWeight = 400;
		lf1.lfItalic = 0;
		lf1.lfUnderline = 0;
		lf1.lfStrikeOut = 0;
		lf1.lfCharSet = 0;
		lf1.lfOutPrecision = 3;
		lf1.lfClipPrecision = 2;
		lf1.lfQuality = 1;
		lf1.lfPitchAndFamily = 0x22;
		for (x = 0; Arial[x] != 0; x++)
			lf1.lfFaceName[x] = Arial[x];
		lf1.lfFaceName[x] = 0;
		hSmallFont = CreateFontIndirect(&lf1);

		lf6.lfHeight = -13;
		lf6.lfWeight = 400;
		lf6.lfItalic = 1;
		lf6.lfUnderline = 0;
		lf6.lfStrikeOut = 0;
		lf6.lfCharSet = 0;
		lf6.lfOutPrecision = 3;
		lf6.lfClipPrecision = 2;
		lf6.lfQuality = 1;
		lf6.lfPitchAndFamily = 0x22;
		for (x = 0; Arial[x] != 0; x++)
			lf6.lfFaceName[x] = Arial[x];
		lf6.lfFaceName[x] = 0;
		hSmallFontItalic = CreateFontIndirect(&lf6);

		lf2.lfHeight = -48;
		lf2.lfWeight = 400;
		lf2.lfItalic = 0;
		lf2.lfUnderline = 0;
		lf2.lfStrikeOut = 0;
		lf2.lfCharSet = 2;
		lf2.lfOutPrecision = 3;
		lf2.lfClipPrecision = 2;
		lf2.lfQuality = 1;
		lf2.lfPitchAndFamily = 0x02;
		for (x = 0; Maestro[x] != 0; x++)
			lf2.lfFaceName[x] = Maestro[x];
		lf2.lfFaceName[x] = 0;
		hMaestroFont = CreateFontIndirect(&lf2);
		ntinterline=-lf2.lfHeight/4;

		lf3.lfHeight = -25;
		lf3.lfWeight = 350;
		lf3.lfItalic = 0;
		lf3.lfUnderline = 0;
		lf3.lfStrikeOut = 0;
		lf3.lfCharSet = 2;
		lf3.lfOutPrecision = 3;
		lf3.lfClipPrecision = 2;
		lf3.lfQuality = 1;
		lf3.lfPitchAndFamily = 0x02;
		for (x = 0; Maestro[x] != 0; x++)
			lf3.lfFaceName[x] = Maestro[x];
		lf3.lfFaceName[x] = 0;
		hMaestroFontSmall = CreateFontIndirect(&lf3);

		lf5.lfHeight = -STAFFFAILSIZEINTERVAL;  //14
		lf5.lfWeight = 350;
		lf5.lfItalic = 0;
		lf5.lfUnderline = 0;
		lf5.lfStrikeOut = 0;
		lf5.lfCharSet = 2;
		lf5.lfOutPrecision = 3;
		lf5.lfClipPrecision = 2;
		lf5.lfQuality = 1;
		lf5.lfPitchAndFamily = 0x02;
		for (x = 0; Maestro[x] != 0; x++)
			lf5.lfFaceName[x] = Maestro[x];
		lf5.lfFaceName[x] = 0;
		hMaestroFontSmall2 = CreateFontIndirect(&lf5);

		lf4.lfHeight = -STAFFSIZEINTERVAL;
		lf4.lfWeight = 500;
		lf4.lfItalic = 0;
		lf4.lfUnderline = 0;
		lf4.lfStrikeOut = 0;
		lf4.lfCharSet = 2;
		lf4.lfOutPrecision = 3;
		lf4.lfClipPrecision = 2;
		lf4.lfQuality = DRAFT_QUALITY;
		lf4.lfPitchAndFamily = 0x02;
		for (x = 0; Maestro[x] != 0; x++)
			lf4.lfFaceName[x] = Maestro[x];
		lf4.lfFaceName[x] = 0;
		hMaestroFontSmall1 = CreateFontIndirect(&lf4);
        interline=-lf4.lfHeight/4;

		lf7.lfHeight = -STAFFSIZEINTERVAL;
		lf7.lfWeight = 500;
		lf7.lfItalic = 0;
		lf7.lfUnderline = 0;
		lf7.lfStrikeOut = 0;
		lf7.lfCharSet = 2;
		lf7.lfOutPrecision = 3;
		lf7.lfClipPrecision = 2;
		lf7.lfQuality =NONANTIALIASED_QUALITY;
		lf7.lfPitchAndFamily = 0x02;
		for (x = 0; Maestro[x] != 0; x++)
			lf7.lfFaceName[x] = Maestro[x];
		lf7.lfFaceName[x] = 0;
		hMaestroFontSmall3 = CreateFontIndirect(&lf7);


        hfontsystem=CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH
                         || FF_DONTCARE, "Times New Roman");
        hfontsystemg=CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH
                         || FF_DONTCARE, "Times New Roman");
		hPen = CreatePen(PS_SOLID, 2, 0x808080);
		hWhiteBrush = CreateSolidBrush(BACKGROUND); 
		hBlackBrush = CreateSolidBrush(0);
		hBlueBrush = CreateSolidBrush(0xFF0000);
		hlBlueBrush = CreateSolidBrush(0xDCDCED);
		hPinkBrush = CreateSolidBrush(0xFF50FF);
		hHelpBrush = CreateSolidBrush(0xFFFFE1);
		hGreenBrush = CreateSolidBrush(GREEN);
		hRedBrush = CreateSolidBrush(RED);
		for (x = 0; x < 40; x++)
			ScanCodes[x] = 0;
		TitleAndMenu = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
        GenericCreateControls(hwnd);		
		return 0;

	case WM_SIZE:
		//int hor,ver;

		rect.left = rect.top = 0;
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		top = rect.bottom - 180;
        top260 = top - 215; //260  
		top260min30 = top260 - 30;
		if (top260min30 > 252)//top of screen in 1024x768
			top260min30 = 252;
		top260min30div2 = top260min30/2;
		noteloc = top-98; //+100;//low C, note 36
		if(showvirstaff==0) topvirstaff=top-25;
		else topvirstaff=top260-45;
		for (x = 0; x < 60; x += 12)
		{
			for (z = 0; z < 12; )
			{
				if ((x+z) == (60-36))//middle C
					noteloc -= 10;
				yStaff[x+(z++)] = noteloc;//36 C
				yStaff[x+z++] = noteloc;//37 C#
				noteloc -= 6;
				if ((x+z) == (62-36))//D above middle C
					noteloc -= 8;
				yStaff[x+z++] = noteloc;//38 D
				yStaff[x+z++] = noteloc;//39 D#
				noteloc -= 6;
				yStaff[x+z++] = noteloc;//40 E
				noteloc -= 6;
				yStaff[x+z++] = noteloc;//41 F
				yStaff[x+z++] = noteloc;//42 F#
				noteloc -= 6;
				yStaff[x+z++] = noteloc;//43 G
				yStaff[x+z++] = noteloc;//44 G#
				noteloc -= 6;
				yStaff[x+z++] = noteloc;//45 A
				yStaff[x+z++] = noteloc;//46 A#
				noteloc -= 6;
				yStaff[x+z++] = noteloc;//47 B
				noteloc -= 6;
			}
		}
		// tamaño fuente
        if(currenthor==1024 && currentver==768){
		   STAFFSIZEINTERVAL=24;
           STAFFFAILSIZEINTERVAL=16;
		   ARIALKEYSIZE=15;
		   noteloc = top-98;
		   if(showtest1 && concfg.maxcompascreen<=4) noteloc = top-98+100; 
		}
		
		if(currenthor==1280 && currentver==1024){
		   STAFFSIZEINTERVAL=40;    
		   STAFFFAILSIZEINTERVAL=24;
		   ARIALKEYSIZE=29;
           noteloc = top-98-100;     
		}
        calccoordfgmystaff(noteloc);
		d1 = (double)top260min30div2 * cos(60.0 * PI / 180.0);//for circle of fifths
		d2 = (double)top260min30div2 * sin(60.0 * PI / 180.0);
		d3 = (double)top260min30div2 * cos(30.0 * PI / 180.0);
		d4 = (double)top260min30div2 * sin(30.0 * PI / 180.0);
		xKeyLoc[0] = top260min30div2;
		yKeyLoc[0] = 0;
		xKeyLoc[1] = (int)d1 + top260min30div2;
	  	yKeyLoc[1] = top260min30div2 - (int)d2;
		xKeyLoc[2] = (int)d3 + top260min30div2;
		yKeyLoc[2] = top260min30div2 - (int)d4;
		xKeyLoc[3] = top260min30;
		yKeyLoc[3] = top260min30div2;
		xKeyLoc[4] = xKeyLoc[2];
		yKeyLoc[4] = top260min30div2 + (int)d4;
		xKeyLoc[5] = xKeyLoc[1];
		yKeyLoc[5] = top260min30div2 + (int)d2;
		xKeyLoc[6] = top260min30div2;
		yKeyLoc[6] = top260min30;
		xKeyLoc[7] = top260min30div2 - (int)d1;
		yKeyLoc[7] = top260min30 - top260min30div2 + (int)d2;
		xKeyLoc[8] = top260min30div2 - (int)d3;
		yKeyLoc[8] = top260min30 - top260min30div2 + (int)d4;
		xKeyLoc[9] = 0;
		yKeyLoc[9] = top260min30div2;
		xKeyLoc[10] = top260min30div2 - (int)d3;
		yKeyLoc[10] = top260min30 - top260min30div2 - (int)d4;
		xKeyLoc[11] = top260min30div2 - (int)d1;
		yKeyLoc[11] = top260min30 - top260min30div2 - (int)d2;
		if (hwndKeys[0]!= NULL)
		{
			for (x = 0; x < 12; x++)
			{
				DestroyWindow(hwndKeys[x]);
				hwndKeys[x] = NULL;
			}
			for (x = 0, z = top260-270; x < 12; x++)
			{
				hwndKeys[x] = CreateWindow("BUTTON", Keys[x],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					xKeyLoc[x], yKeyLoc[x], 45, 30,
					hwnd, NULL, hInst, NULL);
			}
			DestroyWindow(hwndShowScale);
			hwndShowScale = CreateWindow("BUTTON", "Ver Escala",
				WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
				top260min30-(top260min30/2)-35, top260min30-(top260min30/2), 110, 30,
				hwnd, NULL, hInst, NULL);
		}

		if (hwndChords[0] != NULL)
		{
			for (x = 0; x < 9; x++)
			{
				DestroyWindow(hwndChords[x]);
				hwndChords[x] = NULL;
			}
			for (x = 0, z = 0; x < 9; x++, z += 30)
			{
				hwndChords[x] = CreateWindow("BUTTON", Chords[x],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 + 50, z, 135, 30,
					hwnd, NULL, hInst, NULL);
			}
			SendMessage(hwndChords[ChordType], BM_SETCHECK, BST_CHECKED, 0);
			for (x = 0; x < 4; x++)
				DestroyWindow(hwndInversions[x]);
			for (x = 0, z = 30; x < 3; x++, z += 30)
			{
				hwndInversions[x] = CreateWindow("BUTTON", Inversions[x],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 + 50 + 135, z, 120, 30,
					hwnd, NULL, hInst, NULL);
			}
			hwndInversions[3] = CreateWindow("BUTTON", Inversions[3],
				WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
				top260min30 + 50 + 135, 150, 120, 30,
				hwnd, NULL, hInst, NULL);
			SendMessage(hwndInversions[Inversion], BM_SETCHECK, BST_CHECKED, 0);
		}		
		WhiteKeyWidth = rect.right / 35;//5 octaves = 60 keys = 35 white keys
		BlackKeyWidth = WhiteKeyWidth*2/3;
		ExtraSpace = (rect.right % 35) / 2;
		if(helpexercise==TRUE){
            int xNewSize; 
            int yNewSize; 
            SCROLLINFO si; 

            xNewSize = LOWORD(lParam); 
            yNewSize = HIWORD(lParam); 
            if(fBlt) fSize = TRUE;      
            xMaxScroll = max(bmphelp.bmWidth-xNewSize, 0); 
            xCurrentScroll = min(xCurrentScroll, xMaxScroll); 
            si.cbSize = sizeof(si); 
            si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
            si.nMin   = xMinScroll; 
            si.nMax   = bmphelp.bmWidth; 
            si.nPage  = xNewSize; 
            si.nPos   = xCurrentScroll; 
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
            yMaxScroll = max(bmphelp.bmHeight - yNewSize, 0); 
            yCurrentScroll = min(yCurrentScroll, yMaxScroll); 
            si.cbSize = sizeof(si); 
            si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
            si.nMin   = yMinScroll; 
            si.nMax   = bmphelp.bmHeight; 
            si.nPage  = yNewSize; 
            si.nPos   = yCurrentScroll; 
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
        } 
		return 0;

	case WM_ACTIVATE:
		break;   
	
	case WM_CAPTURECHANGED:
		break;     
		for (x = 0; x < 40; x++)
			ScanCodes[x] = 0;
		break;

    case WM_KILLFOCUS :
		 if (hwnd == GetParent ((HWND) wParam)){
			char buff[64];

			GetClassName((HWND) wParam,buff,64);
			if(strcmp(buff,"ListBox")==0) break;
			GetWindowText((HWND) wParam,buff,64);
			if(strcmp(buff,"Procesando")==0) break;
			SendMessage((HWND) wParam,BM_CLICK,0,0);
			SetFocus(hwnd);
          //  SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(0,BN_CLICKED),wParam);
		 }
        break;

	case WM_COMMAND:
		if(LOWORD(wParam) == IDM_EXIT){
	       DestroyWindow(hwnd);
	       return 0;
		}else if (LOWORD(wParam) == IDM_INSTRUMENT){
			if (showinginstruments == FALSE)
			{
				showinginstruments = TRUE;
				InvalidateRect(hwnd, &rect, FALSE);
				hwndList = CreateWindow("LISTBOX", "Instruments",
					WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_VSCROLL,// | LBS_NOTIFY,
					0, TitleAndMenu, 240, rect.bottom,
					hwnd, NULL, hInst, NULL);
				pListProc = (WNDPROC)SetWindowLong(hwndList, GWL_WNDPROC, (LONG)ListProc);
				for (x = 0; x < 128; x++)
					SendMessage(hwndList, LB_ADDSTRING, 0, *(DWORD*)&Instruments[x]);
				index = Instrument;
				SendMessage(hwndList, LB_SETCURSEL, index, 0);
				SetFocus(hwndList);
			}
		}

		else if ((LOWORD(wParam) >= IDM_INPUT-1) && (LOWORD(wParam) < (IDM_DEVICE-1)))
		{
			CheckMenuItem(hMenu, IDM_INPUT + concfg.iInDevice, MF_UNCHECKED);
			concfg.iInDevice = LOWORD(wParam) - IDM_INPUT;
			saveconfiguracion();
			CheckMenuItem(hMenu, IDM_INPUT + concfg.iInDevice, MF_CHECKED);

			midiInClose(hMidiIn);
			if (MMSYSERR_NOERROR == midiInOpen((LPHMIDIIN)&hMidiIn, concfg.iInDevice, (DWORD)MidiInProc, 0, CALLBACK_FUNCTION))
			{//if a MIDI keyboard is attached
				midi_in = TRUE;
				midiInStart(hMidiIn);
			}
		}

		else if ((LOWORD(wParam) >= IDM_DEVICE-1) && (LOWORD(wParam) < (IDM_VELOCITY-1)))
		{
			CheckMenuItem(hMenu, IDM_DEVICE + concfg.iOutDevice, MF_UNCHECKED);
			concfg.iOutDevice = LOWORD(wParam) - IDM_DEVICE;
			saveconfiguracion();
			CheckMenuItem(hMenu, IDM_DEVICE + concfg.iOutDevice, MF_CHECKED);

			midiOutClose(hMidiOut);
			if (MMSYSERR_NOERROR == midiOutOpen(&hMidiOut, concfg.iOutDevice, 0, 0, 0))
				midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));
		}

		else if ((LOWORD(wParam) >= (IDM_VELOCITY-1)) && (LOWORD(wParam) < (IDM_KEYS-1)))
		{
			CheckMenuItem(hMenu, 7 - (concfg.DefaultVelocity / 16) + IDM_VELOCITY, MF_UNCHECKED);
			concfg.DefaultVelocity = ((8 - (LOWORD(wParam)-IDM_VELOCITY)) * 16) - 1;
			saveconfiguracion();
			CheckMenuItem(hMenu, LOWORD(wParam), MF_CHECKED);
		}

		else if ((LOWORD(wParam) >= (IDM_ACCIDENTAL-1)) && (LOWORD(wParam) < IDM_TEST))
		{
			CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_UNCHECKED);
			accidental = LOWORD(wParam) - IDM_ACCIDENTAL;
			CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_CHECKED);
			if (showkeys)
			{
				if (accidental == 1)
					*(WORD*)&Keys[6][0] = '#F';
				else//if (accidental == 2)
					*(WORD*)&Keys[6][0] = 'bG';
				DestroyWindow(hwndKeys[6]);
				hwndKeys[6] = CreateWindow("BUTTON", Keys[6],
					WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DLGFRAME,
					xKeyLoc[6], yKeyLoc[6], 45, 30,
					hwnd, NULL, hInst, NULL);
			}
		}

		else if (LOWORD(wParam) == IDM_KEYS)
		{
			if (showkeys == FALSE)
			{
				showkeys = TRUE;
				ModifyMenu(hMenu, IDM_KEYS, MF_BYCOMMAND|MF_CHECKED, IDM_KEYS,ShowKeys);
				if(showchords) SendMessage(hwnd, WM_COMMAND,IDM_CHORDS, 0);				
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);				
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				DrawMenuBar(hwnd);
				if (accidental == 1)
					*(WORD*)&Keys[6][0] = '#F';
				else//if (accidental == 2)
					*(WORD*)&Keys[6][0] = 'bG';
				for (x = 0, z = top260-270; x < 12; x++)
				{
					hwndKeys[x] = CreateWindow("BUTTON", Keys[x],
						WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
						xKeyLoc[x], yKeyLoc[x], 45, 30,
						hwnd, NULL, hInst, NULL);
				}
				SendMessage(hwndKeys[KeyName], BM_SETCHECK, BST_CHECKED, 0);
				hwndShowScale = CreateWindow("BUTTON", "Ver escala",
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30-(top260min30/2)-35, top260min30-(top260min30/2), 110, 30,
					hwnd, NULL, hInst, NULL);
				SendMessage(hwndShowScale, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else
			{
				showkeys = FALSE;
				showscale = FALSE;
				ChordType = 0;
				ModifyMenu(hMenu, IDM_KEYS, MF_BYCOMMAND|MF_STRING, IDM_KEYS, ShowKeys);
				DrawMenuBar(hwnd);
				for (x = 0; x < 12; x++)
				{
					DestroyWindow(hwndKeys[x]);
					hwndKeys[x] = NULL;
				}
				DestroyWindow(hwndShowScale);
				hwndKeys[0] = NULL;//flag
			}
			InvalidateRect(hwnd, &rect, FALSE);
			UpdateWindow(hwnd);
		}
		else if(LOWORD(wParam) == IDM_SHOWVIRSTAFF)
		{
			if(showvirstaff==TRUE){				
				showvirstaff=FALSE;
				topvirstaff=top-25;
				ModifyMenu(hMenu, IDM_SHOWVIRSTAFF,MF_BYCOMMAND|MF_STRING,IDM_SHOWVIRSTAFF,showvirtualstaff);								
				DrawMenuBar(hwnd);
			}else{
				showvirstaff=TRUE;
				topvirstaff=top260-45;
				ModifyMenu(hMenu, IDM_SHOWVIRSTAFF,MF_BYCOMMAND|MF_CHECKED,IDM_SHOWVIRSTAFF,showvirtualstaff);								
				DrawMenuBar(hwnd);
			}
			InvalidateRect(hwnd,NULL,TRUE);
			UpdateWindow(hwnd);
		}

		else if(LOWORD(wParam) == IDM_SHOWNAMEKEYS)
		{
			if(shownamekeys==TRUE){				
				shownamekeys=FALSE;
				ModifyMenu(hMenu, IDM_SHOWNAMEKEYS,MF_BYCOMMAND|MF_STRING, IDM_SHOWNAMEKEYS,shownamkeys);								
				DrawMenuBar(hwnd);
			}else{
				shownamekeys=TRUE;
				ModifyMenu(hMenu, IDM_SHOWNAMEKEYS,MF_BYCOMMAND|MF_CHECKED, IDM_SHOWNAMEKEYS,shownamkeys);								
				DrawMenuBar(hwnd);
			}
			InvalidateRect(hwnd, &rect, FALSE);
			UpdateWindow(hwnd);
		}

		else if (LOWORD(wParam) == IDM_CHORDS)
		{
			if (showchords == FALSE)
			{
				showchords = TRUE;
				ModifyMenu(hMenu, IDM_CHORDS, MF_BYCOMMAND|MF_CHECKED, IDM_CHORDS,ShowChords);
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);				
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				DrawMenuBar(hwnd);
				for (x = 0, z = 0; x < 9; x++, z += 20)
				{
					hwndChords[x] = CreateWindow("BUTTON", Chords[x],
						WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
						top260min30 + 50, z, 135, 20,
						hwnd, NULL, hInst, NULL);
				}
				SendMessage(hwndChords[ChordType], BM_SETCHECK, BST_CHECKED, 0);
				for (x = 0, z = 30; x < 3; x++, z += 20)
				{
					hwndInversions[x] = CreateWindow("BUTTON", Inversions[x],
						WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
						top260min30 + 50 + 135, z, 120, 20,
						hwnd, NULL, hInst, NULL);
				}
				hwndInversions[3] = CreateWindow("BUTTON", Inversions[3],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 + 50 + 135, 150, 120, 20,
					hwnd, NULL, hInst, NULL);
				SendMessage(hwndInversions[Inversion], BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				showchords = FALSE;
				ChordType=0;
				ModifyMenu(hMenu, IDM_CHORDS, MF_BYCOMMAND|MF_STRING, IDM_CHORDS, ShowChords);
				DrawMenuBar(hwnd);
				for (x = 0; x < 9; x++)
					DestroyWindow(hwndChords[x]);
				hwndChords[0] = NULL;//flag
				for (x = 0; x < 4; x++)
					DestroyWindow(hwndInversions[x]);
			}
			InvalidateRect(hwnd, &rect, FALSE);
			UpdateWindow(hwnd);
		}

		else if (LOWORD(wParam) == IDM_TEST)     // note reading juego1
		{
			if (showtest3 == FALSE)
			{
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);								
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);				
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showchords) SendMessage(hwnd, WM_COMMAND, IDM_CHORDS, 0);				
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				showtest3 = TRUE;				
				ModifyMenu(hMenu, IDM_TEST, MF_BYCOMMAND|MF_CHECKED, IDM_TEST, PlayNote);								
				DrawMenuBar(hwnd);				
				testRect.left = top260min30; //+125;
				testRect.right = testRect.left + (8* StaffWidth[0]);
				testRect.top = top-462-59;
				testRect.bottom = top260;								
				hdc = GetDC(hwnd);
				SelectObject(hdc, hMaestroFont);
				cargarconfiguracion(NULL);
				SetTimer(hwnd,10,15000,NULL);
				SendMessage(hwnd,WM_TIMER,(WPARAM) 10,(LPARAM) NULL);
				earnoteread=earinterval=0;				
                if(concfg.notereadingchord==0) earinterval=1-concfg.oidoabrel;
				if(concfg.notereadingdeep==1) notereadingdeep=7; // si se pide notas futuras establecemos maximo a 7
				else notereadingdeep=1; // en caso contrario mostramos solo la nota actual
				notereadinglead=concfg.notereadinglead;
				notereadingmemo=concfg.notereadingmemo;
				if(notereadingmemo!=0){
				   if(notereadingmemo>6) notereadingmemo=6;
				   if(notereadingmemo<0) notereadingmemo=0;
                   if(notereadingdeep==1) 
				      notereadingdeep=notereadingmemo+1;
				}
				if(notereadinglead==1 && notereadingmemo>=1){
				   notereadingdeep=7;
				   --notereadingmemo;
				}else notereadinglead=0;
				notereading(-1,0);
				notereading(0,0);								
				ReleaseDC(hwnd, hdc);				
			}
			else
			{
				showtest3 = FALSE;
				ModifyMenu(hMenu, IDM_TEST, MF_BYCOMMAND|MF_STRING, IDM_TEST, PlayNote);
				DrawMenuBar(hwnd);
				hdc = GetDC(hwnd);
				FillRect(hdc, &rect, hWhiteBrush);
			    notereading(-2,0);
				ReleaseDC(hwnd, hdc);
				gfstat[0]=gfstat[1]=0;
				NoteReadingCreateButtonsremove();
				KillTimer(hwnd,10);
				KillTimer(hwnd,12);
				notereadtimersand=0;
				saveconfiguracion();
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}

		else if (LOWORD(wParam) == IDM_TEST1 )    // random music
		{
			// juego6
			static int shvs=showvirstaff;
		    //static HANDLE htwoplay;

			if (showtest1 == FALSE)
			{
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);
				if(showchords) SendMessage(hwnd, WM_COMMAND, IDM_CHORDS, 0);			
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				if(InitRandomMusicTest()==-1){
				   InvalidateRect(hwnd, &rect, FALSE);
				   UpdateWindow(hwnd);
				   break;
				}
				showtest1 = TRUE; 
				ModifyMenu(hMenu, IDM_TEST1, MF_BYCOMMAND|MF_CHECKED, IDM_TEST1,PlayNote1);
				DrawMenuBar(hwnd);
				testRect.left = ExtraSpace;
				testRect.right = testRect.left + (NumOfStaffs* StaffWidth[1]); //42 jordi
				testRect.top = top-462-59;				
				if(concfg.maxcompascreen<=4 && currenthor==1024){
				   ajustafuentesrandommusic(1);
				   SendMessage(hwnd, WM_COMMAND, IDM_SHOWVIRSTAFF, 0);
				   ModifyMenu(hMenu, IDM_SHOWVIRSTAFF,MF_BYCOMMAND|MF_DISABLED,IDM_SHOWVIRSTAFF,showvirtualstaff);								
				   DrawMenuBar(hwnd);
				}				
				RandomMusicCreateButtonsScores();
				RandomMusicCreateSlidingControls();
				liberathreads();
				if(concfg.followme==1) runfollowme();				
				if(concfg.sigueme==1) runsigueme();				
			}
			else
			{
				showtest1 = FALSE;
				ModifyMenu(hMenu, IDM_TEST1, MF_BYCOMMAND|MF_STRING, IDM_TEST1,PlayNote1);
				if(currenthor==1024){
			       ModifyMenu(hMenu, IDM_SHOWVIRSTAFF,MF_BYCOMMAND|MF_ENABLED,IDM_SHOWVIRSTAFF,showvirtualstaff);								
				   if(shvs==TRUE)
				      ModifyMenu(hMenu,IDM_SHOWVIRSTAFF, MF_BYCOMMAND|MF_CHECKED,IDM_SHOWVIRSTAFF,showvirtualstaff);
				   else 
				      ModifyMenu(hMenu,IDM_SHOWVIRSTAFF, MF_BYCOMMAND|MF_STRING,IDM_SHOWVIRSTAFF,showvirtualstaff);
				}
				DrawMenuBar(hwnd);
				endthreads();
				hdc = GetDC(hwnd);				
				FillRect(hdc, &rect, hWhiteBrush);
				if(currenthor==1024)
				   ajustafuentesrandommusic(0);
				ReleaseDC(hwnd, hdc);
				LiberaMelodia(melodia);
				RandomMusicButtonsScoresremove(); 
				RandomMusicSlidingControlsremove();
				if(bufmelodia!=NULL){
					LiberaMelodia(bufmelodia);
					gfstat[0]=gfstat[1]=0;
					bufmelodia=NULL;
				}
				showvirstaff=shvs;
				SetWindowText(hwnd,"Pianotrain");
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}
		else if (LOWORD(wParam) == IDM_TEST2)     // note interval
		{
			// juego3 
			if (showtest2 == FALSE)
			{
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);
				if(showchords) SendMessage(hwnd, WM_COMMAND, IDM_CHORDS, 0);			
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				showtest2 = TRUE;
				//ModifyMenu(hMenu, IDM_TEST2, MF_BYCOMMAND|MF_STRING, IDM_TEST2, StopNotes2);
				ModifyMenu(hMenu, IDM_TEST2, MF_BYCOMMAND|MF_CHECKED, IDM_TEST2, PlayNote2);
				DrawMenuBar(hwnd);
				testRect.left = top260min30+ 125;
				testRect.right = testRect.left + (8* StaffWidth[0]);
				testRect.top = top-462-59;
				testRect.bottom = top260;
				hdc = GetDC(hwnd);
				SelectObject(hdc, hMaestroFont);
	            NoteIntervalCreateSlidingControls();
				NoteInterval();
				ReleaseDC(hwnd, hdc);
			}
			else
			{
				showtest2 = FALSE;
				ModifyMenu(hMenu, IDM_TEST2, MF_BYCOMMAND|MF_STRING, IDM_TEST2, PlayNote2);
				DrawMenuBar(hwnd);
				hdc = GetDC(hwnd);
				FillRect(hdc, &rect, hWhiteBrush);
				ReleaseDC(hwnd, hdc);
				if(bufmelodia!=NULL){
					LiberaMelodia(bufmelodia);
					gfstat[0]=gfstat[1]=0;
					bufmelodia=NULL;
				}				
				NoteIntervalCreateButtonsremove();
				KillTimer(hwnd,14);
				noteintervaltimersand=0;
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}
		else if (LOWORD(wParam) == IDM_TEST4)     // flash card note interval
		{
			// juego4 
			if (showtest4 == FALSE)
			{
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);
				if(showchords) SendMessage(hwnd, WM_COMMAND, IDM_CHORDS, 0);			
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				showtest4 = TRUE;
				//ModifyMenu(hMenu, IDM_TEST4, MF_BYCOMMAND|MF_STRING, IDM_TEST4, StopNotes4);
				ModifyMenu(hMenu, IDM_TEST4, MF_BYCOMMAND|MF_CHECKED, IDM_TEST4, PlayNote4);
				DrawMenuBar(hwnd);
				testRect.left = top260min30 + 125;
				testRect.right = testRect.left + (8* StaffWidth[0]);
				testRect.top = top-462-59;
				testRect.bottom = top260;
				hdc = GetDC(hwnd);
				SelectObject(hdc, hMaestroFont);
				FlashCardNoteInterval();
				ReleaseDC(hwnd, hdc);
			}
			else
			{
				showtest4 = FALSE;
				ModifyMenu(hMenu, IDM_TEST4, MF_BYCOMMAND|MF_STRING, IDM_TEST4, PlayNote4);
				DrawMenuBar(hwnd);
				hdc = GetDC(hwnd);
				FillRect(hdc, &rect, hWhiteBrush);
				ReleaseDC(hwnd, hdc);
				FlashCardCreateButtonsremove();
				LiberaMelodia(melodia);
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}
		else if(LOWORD(wParam) == IDM_TEST5)       // localizacion de notas
		{
			if (showtest5 == FALSE)
			{
								
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);				
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);				
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showtest6) SendMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				if(showchords) SendMessage(hwnd, WM_COMMAND, IDM_CHORDS, 0);				
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				showtest5 = TRUE;				
				ModifyMenu(hMenu, IDM_TEST5, MF_BYCOMMAND|MF_CHECKED, IDM_TEST5, PlayNote5);								
				DrawMenuBar(hwnd);								
			    testRect.left =ExtraSpace;
	            testRect.right=testRect.top=testRect.bottom=0; 
		        NotereadInverse(NULL,0);
			}
			else
			{
				showtest5 = FALSE;
				ModifyMenu(hMenu, IDM_TEST5, MF_BYCOMMAND|MF_STRING, IDM_TEST5, PlayNote5);
				DrawMenuBar(hwnd);
				hdc = GetDC(hwnd);
				FillRect(hdc, &rect, hWhiteBrush);
				ReleaseDC(hwnd, hdc);
				gfstat[0]=gfstat[1]=0;
				NotereadInverse(NULL,-1);
				NotereadInverseCreateButtonsremove(); 
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}
		else if(LOWORD(wParam) == IDM_TEST6)
		{
			if (showtest6 == FALSE) // memoria espacial
			{
								
				if(showtest1) SendMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);
				if(showtest2) SendMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);				
				if(showtest3) SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);				
				if(showtest4) SendMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				if(showtest5) SendMessage(hwnd, WM_COMMAND, IDM_TEST5, 0);
				if(showchords) SendMessage(hwnd, WM_COMMAND, IDM_CHORDS, 0);				
				if(showkeys) SendMessage(hwnd, WM_COMMAND, IDM_KEYS, 0);				
				showtest6 = TRUE;								
				ModifyMenu(hMenu, IDM_TEST6, MF_BYCOMMAND|MF_CHECKED, IDM_TEST6, PlayNote6);								
				DrawMenuBar(hwnd);								
				testRect.left = top260min30; //+125;
				testRect.right = testRect.left + (8* StaffWidth[0]);
				testRect.top = top-462-59;
				testRect.bottom = top260;				
				hdc = GetDC(hwnd);
		        initmemoespacial(0);
				initmemoespacial(1);
				ReleaseDC(hwnd,hdc);				
			}
			else
			{
				showtest6 = FALSE;
				ModifyMenu(hMenu, IDM_TEST6, MF_BYCOMMAND|MF_STRING, IDM_TEST6, PlayNote6);
				DrawMenuBar(hwnd);
				hdc = GetDC(hwnd);
				liberamemoespacialbmp(&ref);
				liberamemoespacialbmp(&kiz);
				FillRect(hdc, &rect, hWhiteBrush);
				ReleaseDC(hwnd, hdc);
				gfstat[0]=gfstat[1]=0;
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
		    }			
		}
		else if (LOWORD(wParam) == IDM_HELPEXERCISE){
		    LONG es;
			static LONG bk;
			static RECT bkrect;

            if(helpexercise==FALSE){
			   char helpfile[64];
			   if(!(showtest1||showtest2||showtest3||showtest4||showtest5||showtest6)){
		          break;
			   }
			   genericbuttonhideshow(1);  // random music
			   if(showtest1==TRUE){ 
				   strcpy(helpfile,"exercise6.bmp");
			       RandomMusicButtonsScoreshideshow(1);
				   RandomMusicSlidingControlshideshow(1); 
			   }
			   if(showtest2==TRUE) strcpy(helpfile,"exercise4.bmp");  // note interval
			   if(showtest3==TRUE){ // localizacion de notas 
				   strcpy(helpfile,"exercise1.bmp");
				   NoteReadingButtonshideshow(1);
			   }
			   if(showtest4==TRUE){ 
				  strcpy(helpfile,"exercise3.bmp"); // flash card
			      FlashCardButtonshideshow(1);
			   }
			   if(showtest5==TRUE){  // localizacion inversa
				   strcpy(helpfile,"exercise2.bmp");
				   NotereadInverseButtonshideshow(1);
			   }
			   if(showtest6==TRUE) strcpy(helpfile,"exercise5.bmp"); // memoria espacial

			   helpexercise=TRUE;
			   genericbuttonhideshow(1);
               ModifyMenu(hMenu,IDM_HELPEXERCISE, MF_BYCOMMAND|MF_CHECKED,IDM_HELPEXERCISE,helpexer);								
			   DrawMenuBar(hwnd);	
			   es=bk=GetWindowLong(hwnd, GWL_STYLE);
			   es=es| WS_HSCROLL | WS_VSCROLL;
			   GetClientRect(hwnd,&bkrect);			   
			   SetWindowLong(hwnd,GWL_STYLE,es);
			   hdc = GetDC(hwnd);			   
               hdccompathelp=CreateCompatibleDC(hdc); 			
               hbitmaphelp = (HBITMAP)LoadImage(hInst,helpfile, IMAGE_BITMAP, 0, 0,
                       LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);             
   		       GetObject(hbitmaphelp,sizeof(BITMAP),&bmphelp);
			   SelectObject(hdccompathelp,hbitmaphelp); 
			   ReleaseDC(hwnd,hdc);			   
               fBlt = TRUE; //FALSE; 
               fScroll = FALSE; 
               fSize = FALSE;  
               xMinScroll = 0; 
               xCurrentScroll = 0; 
               xMaxScroll = 0; 
               yMinScroll = 0; 
               yCurrentScroll = 0; 
               yMaxScroll = 0; 			   
			}else{
			   helpexercise=FALSE;
			   genericbuttonhideshow(0);
			   if(showtest1==TRUE){ 
			      RandomMusicButtonsScoreshideshow(0);
				  RandomMusicSlidingControlshideshow(0); 
			   }
			   if(showtest3==TRUE) NoteReadingButtonshideshow(0);
			   if(showtest4==TRUE) FlashCardButtonshideshow(0);
			   if(showtest5==TRUE) NotereadInverseButtonshideshow(0);
			   genericbuttonhideshow(0);
			   DeleteDC(hdccompathelp);
			   DeleteObject(hbitmaphelp);
               ModifyMenu(hMenu,IDM_HELPEXERCISE, MF_BYCOMMAND|MF_STRING,IDM_HELPEXERCISE,helpexer);								
			   DrawMenuBar(hwnd);				   
               SetWindowLong(hwnd,GWL_STYLE,bk);
			   SetWindowPos(hwnd,HWND_TOP,0,0,bkrect.right,bkrect.bottom,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED); 
			}
		    InvalidateRgn(hwnd,NULL,TRUE);
		    UpdateWindow(hwnd);
		}

		else if (LOWORD(wParam) == IDM_WEBPIANOTRAIN){
			ShellExecute(NULL,"open","iexplore","http://www.pianotrain.es/contacto.html",NULL,SW_SHOWNORMAL);
		}
		else if (LOWORD(wParam) == IDM_ABOUT){
			pianoAbout pianoab;
			pianoab.Show(TRUE);
		}

        else if (LOWORD(wParam)==DIAL_APPR || LOWORD(wParam)==DIAL_EJE1 || 
			     LOWORD(wParam)==DIAL_EJE3 || LOWORD(wParam)==DIAL_EJE4 ||
				 LOWORD(wParam)==DIAL_EJE6){
			SendMessage(buttoncfg, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpcfgy);
			pianoconf pianofc(LOWORD(wParam));
			//IMPORTANTE, si no dehabilitamos la ventana padre, 
			// la caja de dialogo pianoconf seria modeless 
			EnableWindow(hwnd,FALSE);  
            if(pianofc.Show(TRUE)==TRUE){
			   BARTOBARSPACE=(NumOfStaffs-OFFSETPENTA)/concfg.maxcompascreen;  // 42 jordi
			   if(concfg.maxcompascreen==1) 
				   BARTOBARSPACE=(NumOfStaffs-OFFSETPENTA)*3.0/4; ///2; //si un compas en pantalla lo muestra hasta la mitad de la pantalla
			   if(showtest1){ 
			      PostMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);							
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST1, 0);							
			   }
			   if(showtest2){
			      PostMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST2, 0);
			   }
			   if(showtest3){ 
			      PostMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
			   }
			   if(showtest4){ 
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST4, 0);
			   }
			   if(showtest6){
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
				  PostMessage(hwnd, WM_COMMAND, IDM_TEST6, 0);
			   }
			}
			// rehabilitamos la ventana padre
			EnableWindow(hwnd,TRUE);
			SendMessage(buttoncfg, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpcfg);			
		}
		
		else if (HIWORD(wParam) == BN_CLICKED)
		{
			{
			   HWND hwndCtl = (HWND) lParam;
			   HBITMAP bmp;
			   SetFocus(hwnd);

			   if(hwndCtl==buttonear){
				  if(concfg.notereadingchord==1) break;
				  bmp=(HBITMAP) SendMessage(buttonear, BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
				  if(bmp==bmpearn){
			         SendMessage(buttonear, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpeary);
					 if(micentry==1){
					    SendMessage(buttonmic, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmicro);
					    pitchdetection(1);        // deshabilitando micro modo cantado 
						solfeohablado(1,0,NULL);  // deshabilitando micro modo hablado
					    micentry=0;
					 }
					 earnoteread=1;	
					 notereadingdeep=1;
					 notereadingmemo=0;
					 if(earinterval==1){ 						 
						 KillTimer(hwnd,12);
						 notereadtimersand=0;
						 notereading(0,0); 
						 InvalidateRect(hwnd, NULL, TRUE);				  
					     break;
					 }else PlayNotaMidi((__int64) RandomNote[0].notakey);
				  }else{ 
					 SendMessage(buttonear, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpearn);
					 SendMessage(hwnd,WM_HSCROLL,0,(LPARAM) hwndmetronoteread);
					 earnoteread=0;  // deshabilitando
					 if(concfg.notereadingdeep==1) notereadingdeep=7;
				  }		
				  InvalidateRect(hwnd, NULL, TRUE);				  
				  break;
			   }
			   if(hwndCtl==buttonrepe){
				  if(earnoteread==1){
				     if(earinterval==0) PlayNotaMidi((__int64) RandomNote[0].notakey);
				     else PlayNotaMidi((__int64) RandomNote[1].notakey,0);
				  }else{
					 int r;

					 if(concfg.notereadingchord==1 || notereadingmemo==0)
   					    for(r=0;r<randomcannote;r++)
			               PlayNotaMidi((__int64) RandomNote[r].notakey,0);
				  }
				  break;
			   }
			   if(hwndCtl==buttonmic){
				  if(playmelodiastatus==1) break;
				  if(concfg.notereadingchord==1) break;
				  bmp=(HBITMAP) SendMessage(buttonmic, BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
				  if(bmp==bmpmicro){					 
			         SendMessage(buttonmic, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmicroon);
					 micentry=1;
					 if(concfg.tiposolfeo==0) pitchdetection(0);  // habilitando deteccion de pitch
					 else{  // habilitando solfeo hablado
				        if(solfeohablado(-1,0,NULL)!=28){  // 28 numero de muestras en la base de datos
                           SendMessage(hwnd, WM_APP+2,(WPARAM) 0, (LPARAM) 0); // creamos base de datos hablada, modo learning
						}
						else solfeohablado(0,0,NULL); //base de datos ok, activamos reconocimiento
					 }
					 if(earnoteread==1){
					    SendMessage(buttonear, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpearn);
					    earnoteread=0;  // deshabilitando ear training
						InvalidateRect(hwnd, NULL, TRUE);				  
					 }
				  }else{ 
					 SendMessage(buttonmic, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmicro);
					 pitchdetection(1);  // deshabilitando
					 solfeohablado(1,0,NULL); // deshabilitando
					 micentry=0;
				  }		
				  break;
			   }
			   if(hwndCtl==buttonritmo){
				  bmp=(HBITMAP) SendMessage(buttonritmo, BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
				  if(bmp==bmpritmon){					 
  			         SendMessage(buttonritmo, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpritmoy);
                     concfg.ritmo=1;
				  }else{ 
					 SendMessage(buttonritmo, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpritmon);
                     concfg.ritmo=0;
				  }		
				  saveconfiguracion();
				  InvalidateRect(hwnd, NULL, TRUE);				  
				  break;
			   }
			   if(hwndCtl==buttonmetro){
				  bmp=(HBITMAP) SendMessage(buttonmetro, BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
				  if(bmp==bmpmetron){					 
  			         SendMessage(buttonmetro, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmetroy);
                     metronomostatus=1;
					 if(hmetronomo!=NULL){ 
						 CloseHandle(hmetronomo); 
						 hmetronomo=NULL;
					 }
                     hmetronomo=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) paintmetronomo,NULL, 0, NULL);  
				  }else{ 
					 SendMessage(buttonmetro, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmetron);
					 metronomostatus=0;
				     metronomostop=1;					 
				     //while(metronomostop==1 && hmetronomo!=NULL);
				     //if(hmetronomo!=NULL){ CloseHandle(hmetronomo); hmetronomo=NULL;}
				  }		
				  InvalidateRect(hwnd, NULL, TRUE);				  
				  break;
			   }
			   //aqui1
			   if(hwndCtl==buttonplay){ 
				  if(micentry==1) break;
				  bmp=(HBITMAP) SendMessage(buttonplay, BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
				  if(bmp==bmpplayn){
			         SendMessage(buttonplay, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpplayy);
					 if(followmestatus==1) followmestop=1; //endfollowme();
                     if(siguemestatus==1) endsigueme(); //prueba
					 if(melodia!=NULL){
						 if(melodia->metronomo!=0){
						   playmelodiastopv=0;
                           hplaymelodia=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) playmelodia,NULL, 0, NULL);  
						   break;
						 }						
					 }
					 break;
				  }else{	
			         SendMessage(buttonplay, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpplayn);
					 if(metronomostatus==1){
					    metronomostatus=0;
				        metronomostop=1;					 
						SendMessage(buttonmetro, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmetron);
						Sleep(1000); // tiempo de espera a que finalize paintmetronomo
					 }
					 if(playmelodiastatus==1){
					    playmelodiastopv=1;
					    while(playmelodiastopv==1);
					 }
					 if(hplaymelodia!=NULL){ CloseHandle(hplaymelodia); hplaymelodia=NULL;}
					 if(followmestatus==1) runfollowme();
					 if(siguemestatus==1){ 
					    endsigueme();
						runsigueme();
					 }
				     InvalidateRect(hwnd, NULL, TRUE);				  
				     break;
				  }
			   }
               if(hwndCtl==buttongoto){ 
                  bmp=(HBITMAP) SendMessage(buttonplay, BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
			      if((followmestatus==1 || playmelodiastatus==1) && bmp==bmpplayy)
			         PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(0,BN_CLICKED),(LPARAM) buttonplay);
				  if(siguemestatus==1) endsigueme();
				  if(followmestatus==1) followmestop=1;
				  if(LOWORD(wParam)==1){
					    int initcompas=melodia->puneje.compas;
						if(initcompas>melodia->numcompases-1)
					       initcompas=melodia->numcompases-1;					    
						offsetdispinterval=(initcompas/concfg.maxcompascreen)*concfg.maxcompascreen;					    
						if(melodia->puneje.eje!=NULL)
						   delete melodia->puneje.eje;
						melodia->puneje.eje=generaejecucion(melodia->penta,initcompas);
						melodia->puneje.index=0;
						melodia->puneje.compas=initcompas;
						melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
						resetmelodia(melodia,melodia->puneje.compas);
						IntervalTestUpdate(0,0,-1);
						IntervalTestUpdate(0,0,-2);						
						scrollrandommusic(melodia,offsetdispinterval);
				  }else{
					 pianogoto pianofc;
					 pianofc.initcompas=melodia->puneje.compas;
			         EnableWindow(hwnd,FALSE);  
					 // deshabilitamos la ventana padre para que el dialog sea modal
                     if(pianofc.Show(TRUE)==TRUE){				
						if(pianofc.initcompas>melodia->numcompases-1)
					       pianofc.initcompas=melodia->numcompases-1;					    
						offsetdispinterval=(pianofc.initcompas/concfg.maxcompascreen)*concfg.maxcompascreen;					    
						if(melodia->puneje.eje!=NULL)
						   delete melodia->puneje.eje;
						melodia->puneje.eje=generaejecucion(melodia->penta,pianofc.initcompas);
						melodia->puneje.index=0;
						melodia->puneje.compas=pianofc.initcompas;
						melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
						resetmelodia(melodia,melodia->puneje.compas);
						IntervalTestUpdate(0,0,-1);
						IntervalTestUpdate(0,0,-2);
						scrollrandommusic(melodia,offsetdispinterval);
					 }
				  }
				  // rehabilitamos la ventana padre
				  EnableWindow(hwnd,TRUE);
			      if(siguemestatus==1) runsigueme();
				  if(followmestatus==1) runfollowme();
				  InvalidateRect(hwnd, NULL, TRUE);				  
                  break;                  
			   }
			   if(hwndCtl==buttoncfg){
				  if(playmelodiastatus==1) break;
				  if(showtest3==1){ SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_EJE1,0),(LPARAM) 0); break; }
                  //if(showtest5==1){ SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_EJE2,0),(LPARAM) 0); break; }
				  if(showtest4==1){ SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_EJE3,0),(LPARAM) 0); break; }
				  if(showtest2==1){ SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_EJE4,0),(LPARAM) 0); break; }
				  //if(showtest6==1){ SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_EJE5,0),(LPARAM) 0); break; }
				  if(showtest1==1){ SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_EJE6,0),(LPARAM) 0); break; }
				  SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(DIAL_APPR,0),(LPARAM) 0);
				  break;
			   }
			   if(hwndCtl==buttongame1){  // localizacion de notas
				  SendMessage(hwnd, WM_COMMAND, IDM_TEST,0);
				  break;
			   }
			   if(hwndCtl==buttongame2){   // localizacion inversa de notas 
				  SendMessage(hwnd, WM_COMMAND, IDM_TEST5,0);
				  break;
			   }
			   if(hwndCtl==buttongame3){   // memorizacion de intervalos
				  SendMessage(hwnd, WM_COMMAND, IDM_TEST4,0);
				  break;
			   }
			   if(hwndCtl==buttongame4){        // localizacion de intervalos
				  SendMessage(hwnd, WM_COMMAND, IDM_TEST2,0);
				  break;
			   }
			   if(hwndCtl==buttongame5){        // memoria espacial
				  SendMessage(hwnd, WM_COMMAND, IDM_TEST6,0);
				  break;
			   }
			   if(hwndCtl==buttongame6){        //  music aleatoria
				  SendMessage(hwnd, WM_COMMAND, IDM_TEST1,0);
				  break;
			   }
			}

			if (showkeys)
			{
				for (x = 0; x < 12; x++)
				{
					if (lParam == (LONG)hwndShowScale)
					{
						if (showscale)
						{
							SendMessage(hwndShowScale, BM_SETCHECK, BST_UNCHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote-36];//36 is the lowest MIDI note number in this program
							EndNote(0);
							showscale = FALSE;
						}
						else
						{
							showscale = TRUE;
							SendMessage(hwndShowScale, BM_SETCHECK, BST_CHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote-36];//36 is the lowest MIDI note number in this program
							StartNote(0);
						}
						break;
					}
					else if (lParam == (LONG)hwndKeys[x])
					{
						SendMessage(hwndKeys[KeyName], BM_SETCHECK, BST_UNCHECKED, 0);
						KeyName = x;
						SendMessage(hwndKeys[KeyName], BM_SETCHECK, BST_CHECKED, 0);
//						if ((KeyName >= 1) && (KeyName <= 5))
						if ((KeyName >= 7) && (KeyName <= 11))
						{
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_UNCHECKED);
							accidental = 2;
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_CHECKED);
							*(WORD*)&Keys[6][0] = 'bG';
							DestroyWindow(hwndKeys[6]);
							hwndKeys[6] = CreateWindow("BUTTON", Keys[6],
								WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DLGFRAME,
								xKeyLoc[6], yKeyLoc[6], 45, 30,
								hwnd, NULL, hInst, NULL);
						}
//						else if ((KeyName >= 7) && (KeyName <= 11))
						else if ((KeyName >= 1) && (KeyName <= 5))
						{
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_UNCHECKED);
							accidental = 1;
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_CHECKED);
							*(WORD*)&Keys[6][0] = '#F';
							DestroyWindow(hwndKeys[6]);
							hwndKeys[6] = CreateWindow("BUTTON", Keys[6],
								WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DLGFRAME,
								xKeyLoc[6], yKeyLoc[6], 45, 30,
								hwnd, NULL, hInst, NULL);
						}
						InvalidateRect(hwnd, &rect, FALSE);
						UpdateWindow(hwnd);
						if (showscale)
						{
							SendMessage(hwndShowScale, BM_SETCHECK, BST_UNCHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote-36];//36 is the lowest MIDI note number in this program
							EndNote(0);
							SendMessage(hwndShowScale, BM_SETCHECK, BST_CHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote-36];//36 is the lowest MIDI note number in this program
							StartNote(0);
						}
						break;
					}
				}
			}
			if (showchords)
			{
				for (x = 0; x < 9; x++)
				{
					if (lParam == (LONG)hwndChords[x])
					{
						SendMessage(hwndChords[ChordType], BM_SETCHECK, BST_UNCHECKED, 0);
						ChordType = x;
						SendMessage(hwndChords[ChordType], BM_SETCHECK, BST_CHECKED, 0);
						break;
					}
				}
				for (x = 0; x < 4; x++)
				{
					if (lParam == (LONG)hwndInversions[x])
					{
						SendMessage(hwndInversions[Inversion], BM_SETCHECK, BST_UNCHECKED, 0);
						Inversion = x;
						SendMessage(hwndInversions[Inversion], BM_SETCHECK, BST_CHECKED, 0);
						break;
					}
				}
			}

			if(showtest3){			
				int nota,acci,nota1,acci1;

		        if(concfg.notereadingchord==1) break;
			    if(concfg.metronoteread!=0){
			       concfg.metronoteread=0;
				   SendMessage(hwndmetronoteread,TBM_SETPOS,true,0);
				   SendMessage(hwnd,WM_HSCROLL,0,(LPARAM) hwndmetronoteread);
				   return 0;
				}
				for (x=0;x<21;x++)
					if (lParam == (LONG)hwndnumeric[x]) break;				
                acci=x/7; nota=x%7;
	            acci=3-2*acci; if(acci==3) acci=0;
                nota1=calcnoteindex(&RandomNote[0]);  
                nota1=nota1%7;
				acci1=RandomNote[0].acci;
	            acci1=3-2*acci1; if(acci1==3) acci1=0;
				acci1=armadura(RandomNote[0].nota,acci1,concfg.armaduranotereading);
                if(semitonos(nota1,acci1)==semitonos(nota,acci)){
				   notereading(-3,1);
			       notereading(0,0);
				}else{ 
					notereading(-3,0);
					if(concfg.stoponerror==1) notereading(1,0);
					else notereading(0,0);
				}
				InvalidateRect(hwnd, &rect, FALSE);				
				UpdateWindow(hwnd);
		        break;
			}
			
			if(showtest4){			 // localizacion de intervalos
				int compas,dist,pass;
				NOTA nota[2];

				compas=melodia->puneje.compas;
				nota[1].nota=(int) melodia->penta[0].compas[compas].note[0];
				nota[0].nota=(int) melodia->penta[0].compas[compas-1].note[0];
				dist=interval(&nota[0],&nota[1]);
				for (x = 0; x < 12; x++)
					if (lParam == (LONG)hwndnumeric[x]) break;				
				pass=0;
				if(x+1==dist){
			       melodia->penta[0].compas[compas].color[0]=GREENBOLD;
				   pass=1;
				}else
				   melodia->penta[0].compas[compas].color[0]=RED;
				if(concfg.stoponerror==1 && pass==1)
				   ++melodia->puneje.compas;
				if(concfg.stoponerror==0)
                   ++melodia->puneje.compas;
                IntervalTestUpdate3(melodia);							
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
				break;
			}
			if(showtest5){
			   HBITMAP bmp;			
			   int r;

			   SetFocus(hwnd);
			   HWND hwndCtl = (HWND) lParam;
			   for(r=0;r<16;r++){
			      if(hwndCtl==autobutton[r]){
				     bmp=(HBITMAP) SendMessage(autobutton[r], BM_GETIMAGE, IMAGE_BITMAP,(LPARAM) 0);
					 break;
				  }
			   }
			   if(r<16) NotereadInverse(bmp,0);
			   break;
			}

			SetFocus(hwnd);
			for (x = 0; x < 40; x++)
				ScanCodes[x] = 0;//just in case
		}
		return 0;

	case WM_KEYUP:		
		ScanCode = HIWORD(lParam) & 0x0FF;
		if(concfg.soloritmo==1){
		   if(ScanCode==42) ScanCode=48;
		   if(ScanCode==54) ScanCode=22;
		}
		if (ScanCode <= 53)
		{
			if(playmelodiastatus==1) return 0;
			SavedNote = Notes[ScanCode];
			if (SavedNote != -1)
				EndNote(1);
		}
		return 0;

	case WM_KEYDOWN:
		if (0x40000000 & lParam) return 0;//ignore typematics
		ScanCode = HIWORD(lParam) & 0x0FF;
		if(concfg.soloritmo==1){
		   if(ScanCode==42) ScanCode=48;
		   if(ScanCode==54) ScanCode=22;
		}
		if (ScanCode <= 53)
		{
			if(playmelodiastatus==1) return 0;
			SavedNote = Notes[ScanCode];			
			if (SavedNote != -1)
			{
				Velocity = concfg.DefaultVelocity;
				StartNote(1);
			}
		}
		return 0;

	case WM_NCLBUTTONDBLCLK:
		return 0;//because the WM_LBUTTONUP message is also sent with the wrong lParam valuse

	case WM_RBUTTONUP:
           if(showtest6==TRUE){
              int orgs=gStaff[64-36]-295;
              int offset=testRect.right*0.5-50;
			  POINT p0,p1;

	          p0.x=offset;
			  if(currenthor==1280){ 
                 p0.y=orgs-100;
			     p1.x=p0.x+318;
				 p1.y=p0.y+427;
			  }else{
                 p0.y=orgs;
			     p1.x=p0.x+178;
				 p1.y=p0.y+240;
			  }
              if(xPos>=p0.x && xPos<=p1.x && yPos>=p0.y && yPos<=p1.y){
				 liberamemoespacialbmp(&ref);
				 liberamemoespacialbmp(&kiz);
				 hdc = GetDC(hwnd);
				 if(concfg.memoespacial==1) concfg.memoespacial=2;
				 else concfg.memoespacial=1;
		         initmemoespacial(0);
				 initmemoespacial(1);
				 saveconfiguracion();
				 ReleaseDC(hwnd,hdc);
  			     InvalidateRect(hwnd, &rect, FALSE);
			     UpdateWindow(hwnd);
			     return 0; 
			  }
		   }
		   return 0;

	case WM_LBUTTONUP:    // mano   
		{
		   int currentscan;
		   xPos = LOWORD(lParam);  
           yPos = HIWORD(lParam);  

		   currentscan=-1;
		   if(yPos>=top && yPos<(top+150)){
		      x = (xPos-ExtraSpace) / WhiteKeyWidth;
		      if ((x >= 4) && (x <= 25))
			     ScanCode = MouseKeys[x];
		      else if (x < 4)
		 	     ScanCode = MouseKeys[x+26];
		      else if (x > 25)
			     ScanCode = MouseKeys[x+4];
		      if (yPos > (top+100)){
			     SavedNote = Notes[ScanCode];
		         currentscan=SavedNote;
				 if (SavedNote != -1)
			        EndNote(2);
			  }else//if black key, get it from table
			  {
			     if (((xPos-ExtraSpace)%WhiteKeyWidth) > (WhiteKeyWidth*2/3))
				    ScanCode = RKeys[x];
			     else if (((xPos-ExtraSpace)%WhiteKeyWidth) < (WhiteKeyWidth/3))
				    ScanCode = LKeys[x];
			     if (ScanCode != -1){
				    SavedNote = Notes[ScanCode];
					currentscan=SavedNote;
				    if (SavedNote != -1)
					   EndNote(2);
				 }
			  }
		   }
		   if(showtest1==TRUE){
		      RECT rct[2],*recmetro;
		      int penta=-1,densignature,cps;		  
              int orgs=gStaff[64-36]-250;
	          int orgf=fStaff[43-36]-250;	

			  if(ismousedrag==1){
			     if(currentscan!=currentscancode)	 
			        PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(1,BN_CLICKED),(LPARAM) buttongoto);
				 ismousedrag=0;
			  }
			  if(playmelodiastatus==1) return 0;
		      rct[0].left=ExtraSpace;
		      rct[0].top=orgs;
		      rct[0].right=ExtraSpace+GClefWidth[1];
		      rct[0].bottom=orgs+8*interline;			   
		      rct[1].left=ExtraSpace;
		      rct[1].top=orgf+interline;
		      rct[1].right=ExtraSpace+FClefWidth[1];
		      rct[1].bottom=orgf+5*interline;	
              cps=melodia->puneje.compas;
			  densignature=melodia->penta[0].compas[cps].densignature;
			  recmetro=paintmetronomostaff(melodia,1);
              if(xPos>recmetro->left && xPos<recmetro->right && 
				  yPos<recmetro->bottom && yPos>recmetro->top){
                  metrodivisor*=2;
				  if(metrodivisor*densignature>16) metrodivisor=1;
 			      InvalidateRect(hwnd, &rect, FALSE);
			      UpdateWindow(hwnd);
                  return 0;			  
			  }				 
              if(xPos>rct[0].left && xPos<rct[0].right && 
			     yPos<rct[0].bottom && yPos>rct[0].top && gfstat[1]==0)
		         penta=0;
              if(xPos>rct[1].left && xPos<rct[1].right && 
			     yPos<rct[1].bottom && yPos>rct[1].top && gfstat[0]==0)
			     penta=1;
		      if(penta!=-1){
				 if(melodia->penta[0].allsilencio==1 || melodia->penta[1].allsilencio==1)
			        return 0;
		         if(gfstat[penta]==0){
		            gfstat[penta]=1;
			        bufmelodia=melodia;
                    offsetdispintervalbck=offsetdispinterval;
					if(siguemestatus==1) endsigueme();				   
					if(followmestatus==1) followmestop=1; //endfollowme();			        					
					while(followmestop==1);
					melodia=copiamelodia(melodia);
			        allsilencio(melodia,penta);
				    melodia->oth=penta+1;
	                melodia->puneje.compas=bufmelodia->puneje.compas;
		            melodia->puneje.index=0;
			        melodia->puneje.eje=generaejecucion(melodia->penta,bufmelodia->puneje.compas);
	                melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
	                melodia->metronomoreal=-1;
	                IntervalTestUpdate(0,0,-1);
					IntervalTestUpdate(0,0,-2);
	 		        hdc = GetDC(hwnd);
                    scrollrandommusic(melodia,offsetdispintervalbck);	
                    paintrandommusic(melodia);
				    ReleaseDC(hwnd, hdc);
					if(followmestatus==1) runfollowme();
					if(siguemestatus==1) runsigueme();				   
  			        InvalidateRect(hwnd, &rect, FALSE);
			        UpdateWindow(hwnd);
                    return 0;			  
				 }else{
					if(followmestatus==1) followmestop=1; //endfollowme();
					while(followmestop==1);
					if(siguemestatus==1) endsigueme();				   
			        gfstat[penta]=0;
			        LiberaMelodia(melodia);
			        melodia=bufmelodia;
					offsetdispinterval=offsetdispintervalbck;
				    bufmelodia=NULL;
			        InvalidateRect(hwnd, &rect, FALSE);
					if(followmestatus==1) runfollowme();
					if(siguemestatus==1) runsigueme();				   
			        UpdateWindow(hwnd);
                    return 0;
				 }
			  }
		   }
		   
		   if(showtest2==TRUE){
		      RECT rct[2];
		      int penta=-1;		  
	          int orgs=gStaff[64-36]-295;
	          int orgf=fStaff[43-36]-240;	

		      rct[0].left=top260min30;
		      rct[0].top=orgs;
		      rct[0].right=top260min30+GClefWidth[0];
		      rct[0].bottom=orgs+8*ntinterline;			   
		      rct[1].left=top260min30;
		      rct[1].top=orgf;
		      rct[1].right=top260min30+FClefWidth[0];
		      rct[1].bottom=orgf+5*ntinterline;		   
              if(xPos>rct[0].left && xPos<rct[0].right && 
			     yPos<rct[0].bottom && yPos>rct[0].top && gfstat[1]==0)
		         penta=0;
              if(xPos>rct[1].left && xPos<rct[1].right && 
			     yPos<rct[1].bottom && yPos>rct[1].top && gfstat[0]==0)
			     penta=1;
		      if(penta!=-1){
		         if(gfstat[penta]==0){
		            gfstat[penta]=1;
			        bufmelodia=melodia;
			        melodia=copiamelodia(melodia);
			        allsilencio(melodia,penta);
				    melodia->oth=penta+1;
	                melodia->puneje.compas=bufmelodia->puneje.compas;
		            melodia->puneje.index=0;
			        melodia->puneje.eje=generaejecucion(melodia->penta,bufmelodia->puneje.compas);
	                melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
	                melodia->metronomoreal=-1;
	                IntervalTestUpdate(0,0,-1);
	 		        hdc = GetDC(hwnd);                     
                    PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas),6, 
						0,concfg.armaduranoteinterval);
				    ReleaseDC(hwnd, hdc);
  			        InvalidateRect(hwnd, &rect, FALSE);
			        UpdateWindow(hwnd);
                    return 0;			  
				 }else{
			        gfstat[penta]=0;
			        LiberaMelodia(melodia);
			        melodia=bufmelodia;
				    bufmelodia=NULL;
			        InvalidateRect(hwnd, &rect, FALSE);
			        UpdateWindow(hwnd);
                    return 0;
				 }
			  }
		   }

           if(showtest3==TRUE){
		      RECT rct[2];
		      int penta=-1;		  
	          int orgs=gStaff[64-36]-295;
	          int orgf=fStaff[43-36]-240;	

		      rct[0].left=top260min30;
		      rct[0].top=orgs;
		      rct[0].right=top260min30+GClefWidth[0];
		      rct[0].bottom=orgs+8*ntinterline;			   
		      rct[1].left=top260min30;
		      rct[1].top=orgf;
		      rct[1].right=top260min30+FClefWidth[0];
		      rct[1].bottom=orgf+5*ntinterline;		   
              if(xPos>rct[0].left && xPos<rct[0].right && 
			     yPos<rct[0].bottom && yPos>rct[0].top && gfstat[1]==0)
		         penta=0;
              if(xPos>rct[1].left && xPos<rct[1].right && 
			     yPos<rct[1].bottom && yPos>rct[1].top && gfstat[0]==0)
			     penta=1;
		      if(penta!=-1){
		         if(gfstat[penta]==0){
		            gfstat[penta]=1;
					notereading(-4,0);
		            notereading(0,0);						
				    InvalidateRect(hwnd, &rect, FALSE);
			        UpdateWindow(hwnd);
					return 0;
				 }else{
					gfstat[penta]=0;
					notereading(-4,0);
				    notereading(0,0);
  			        InvalidateRect(hwnd, &rect, FALSE);
			        UpdateWindow(hwnd);
                    return 0;
				 }
			  }
		   }
           if(showtest6==TRUE){
              int orgs=gStaff[64-36]-295;
              int offset=testRect.right*0.5-50;
			  POINT p0,p1;

	          p0.x=offset;
			  if(currenthor==1280){ 
                 p0.y=orgs-100;
			     p1.x=p0.x+318;
				 p1.y=p0.y+427;
			  }else{
                 p0.y=orgs;
			     p1.x=p0.x+178;
				 p1.y=p0.y+240;
			  }
              if(xPos>=p0.x && xPos<=p1.x && yPos>=p0.y && yPos<=p1.y){
		         if(gfstat[0]==0) gfstat[0]=1;
			     else gfstat[0]=0;
				 liberamemoespacialbmp(&ref);
				 liberamemoespacialbmp(&kiz);
				 hdc = GetDC(hwnd);
		         initmemoespacial(0);
				 initmemoespacial(1);
				 ReleaseDC(hwnd,hdc);
  			     InvalidateRect(hwnd, &rect, FALSE);
			     UpdateWindow(hwnd);
			     return 0; 
			  }
		   }		      
           return 0;
		}
    
	case WM_LBUTTONDOWN:
		xPos = LOWORD(lParam);  
        yPos = HIWORD(lParam);  

		currentscancode=-1;
		if(yPos<top) return 0;
		if ((yPos > (top)) && (yPos < (top+150)) && (xPos > ExtraSpace) && (xPos < (WhiteKeyWidth*35)))
		{
			x = (xPos-ExtraSpace) / WhiteKeyWidth;
			if ((x >= 4) && (x <= 25))
				ScanCode = MouseKeys[x];
			else if (x < 4)
				ScanCode = MouseKeys[x+26];
			else if (x > 25)
				ScanCode = MouseKeys[x+4];
			if (yPos > (top+100))
			{
				SavedNote = Notes[ScanCode];
				currentscancode=SavedNote;
				if (SavedNote != -1)
				{
					Velocity = concfg.DefaultVelocity;
					StartNote(2);
				}
			}
			else//if black key, get it from table
			{
				if (((xPos-ExtraSpace)%WhiteKeyWidth) > (WhiteKeyWidth*2/3))
					ScanCode = RKeys[x];
				else if (((xPos-ExtraSpace)%WhiteKeyWidth) < (WhiteKeyWidth/3))
					ScanCode = LKeys[x];
				if (ScanCode != -1)
				{
					SavedNote = Notes[ScanCode];
					currentscancode=SavedNote;
					if (SavedNote != -1)
					{
						Velocity = concfg.DefaultVelocity;
						StartNote(2);
					}
				}
			}
		}
		return 0;

	case WM_SETCURSOR:
		{
		   POINT a;
		   HWND tmp;
           		   
		   tmp=(HWND) wParam;
		   if(hlpwnd!=NULL && tmp!=hwnd) 
			   break;
		   GetWindowRect(tmp,&hlprect);			  
		   a.x=a.y=0;
		   ClientToScreen(hwnd,&a);
		   hlprect.left-=a.x; hlprect.right-=a.x;
		   hlprect.top-=a.y; hlprect.bottom-=a.y;
		   if(tmp==buttonplay){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Play");
			  break;
		   }
		   if(tmp==buttongoto){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Ir a compas");
			  break;
		   }
		   if(tmp==buttonmic){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Entrada de Micro");
			  break;
		   }
		   if(tmp==buttonear){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Ear Training");
			  break;
		   }
		   if(tmp==buttonrepe){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Repetir");
			  break;
		   }
           if(tmp==buttoncfg){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Opciones");
			  break;
		   }
           if(tmp==buttongame1){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Localizacion de Notas");
			  break;
		   }
           if(tmp==buttongame2){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Localizacion Inversa de Notas");
			  break;
		   }
           if(tmp==buttongame3){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"FlashCard de Intervalos");
			  break;
		   }
           if(tmp==buttongame4){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Localizacion de Intervalos");
			  break;
		   }
           if(tmp==buttongame5){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Mem. Espacial");
			  break;
		   }
           if(tmp==buttongame6){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Musica Aleatoria");
			  break;
		   }
           if(tmp==buttonritmo){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Analisis Ritmico");
			  break;
		   }
           if(tmp==buttonmetro){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Metronomo  ");
			  break;
		   }
           if(tmp==hwndaccnotes){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Tolerancia en tiempo de Notas");
			  break;
		   }		   
           if(tmp==hwndacctempo){   // colocar aqui la ventana de ayuda asociada al cursor
		      SetTimer(hwnd,4,400,NULL);
			  strcpy(hlpvalue,"Tolerancia en tiempo de Compas");
			  break;
		   }		   
	       KillTimer(hwnd,4);
		   if(hlpwnd!=NULL) 
			   DestroyWindow(hlpwnd);				 
		   hlpwnd=NULL;		   
		   break;
		}

	case WM_MOUSEMOVE:    
		// se usa para el desplazamiento del cursor sobre el teclado mientras se mantiene
		// pulsada una tecla virtual del raton, inhabilitamos esta posibilidad cuando estamos
		// en modo eartraining en noteread para evitar las falsas entradas que produce
		// si arrastramos el raton sobre el teclado del piano con una tecla pulsada del rator
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);

	    if(showtest1==TRUE){
	       RECT rct[2],*recmetro;		
           int orgs=gStaff[64-36]-250;
	       int orgf=fStaff[43-36]-250;	
		  
		   // si estamos ejecutando la melodia eliminamos la deteccion del
		   // movimiento del raton con lbutton pulsado para evitar entradas erroneas
		   if(wParam == MK_LBUTTON){
			   if(currentscancode!=-1) 
				   ismousedrag=1;			   
			   return 0;
		   }
		   rct[0].left=ExtraSpace;
		   rct[0].top=orgs;
		   rct[0].right=ExtraSpace+GClefWidth[1];
		   rct[0].bottom=orgs+8*interline;			   
		   rct[1].left=ExtraSpace;
		   rct[1].top=orgf+interline;
		   rct[1].right=ExtraSpace+FClefWidth[1];
		   rct[1].bottom=orgf+5*interline;			
		   recmetro=paintmetronomostaff(melodia,1);
           if(xPos>recmetro->left && xPos<recmetro->right && 
			  yPos<recmetro->bottom && yPos>recmetro->top){
			  SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
              return 0;			  
		   }				 
           if(xPos>rct[0].left && xPos<rct[0].right && 
			  yPos<rct[0].bottom && yPos>rct[0].top && gfstat[1]==0)
	          SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
           if(xPos>rct[1].left && xPos<rct[1].right && 
		      yPos<rct[1].bottom && yPos>rct[1].top && gfstat[0]==0)
		      SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		}	  
  	    if(showtest2==TRUE || showtest3==TRUE){
		   RECT rct[2];
	       int orgs=gStaff[64-36]-295;
	       int orgf=fStaff[43-36]-240;	

		   rct[0].left=top260min30;
		   rct[0].top=orgs;
		   rct[0].right=top260min30+GClefWidth[0];
		   rct[0].bottom=orgs+8*ntinterline;			   
		   rct[1].left=top260min30;
		   rct[1].top=orgf;
		   rct[1].right=top260min30+FClefWidth[0];
		   rct[1].bottom=orgf+5*ntinterline;		   
           if(xPos>rct[0].left && xPos<rct[0].right && 
		      yPos<rct[0].bottom && yPos>rct[0].top && gfstat[1]==0)
		      SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
           if(xPos>rct[1].left && xPos<rct[1].right && 
			  yPos<rct[1].bottom && yPos>rct[1].top && gfstat[0]==0)
			  SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		}
        if(showtest6==TRUE){
           int orgs=gStaff[64-36]-295;
           int offset=testRect.right*0.5-50;
		   POINT p0,p1;
 
		   p0.x=offset;
		   if(currenthor==1280){ 
              p0.y=orgs-100;
			  p1.x=p0.x+318;
			  p1.y=p0.y+427;
		   }else{
              p0.y=orgs;
		      p1.x=p0.x+178;
		  	  p1.y=p0.y+240;
		   }
           if(xPos>=p0.x && xPos<=p1.x && yPos>=p0.y && yPos<=p1.y){
		      SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_FINGER)));
			  return 0; 
		   }
		}

		if (wParam == MK_LBUTTON && earnoteread==0)
		{
			if ((saveleft != -1) && (fromkeydown) && (ChordType == 0))
			{
				if ((yPos > top) && (yPos < (top+150)) && (xPos > ExtraSpace) && (xPos < (WhiteKeyWidth*35)))
				{
					if (yKey[ScanCode] == 0)//on a black key
					{
						if ((xPos < (saveleft-WhiteKeyWidth/3)) || (xPos > (saveleft+(WhiteKeyWidth/3))))
						{
							EndNote(2);
							SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else if (yKey[ScanCode] == 3)//on a white key with black keys on the left and right
					{
						if (yPos > (top+100))
						{
							if ((xPos < saveleft) || (xPos > (saveleft+WhiteKeyWidth)))
							{
								EndNote(2);
								SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
							}
						}
						else if ((xPos < (saveleft+WhiteKeyWidth/3)) || (xPos > (saveleft + (WhiteKeyWidth*2/3))))
						{
							EndNote(2);
							SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else if (yKey[ScanCode] == 1)//on a white key with a black key on the left
					{
						if (yPos > (top+100))
						{
							if ((xPos < saveleft) || (xPos > (saveleft+WhiteKeyWidth)))
							{
								EndNote(2);
								SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
							}
						}
						else if ((xPos < (saveleft+WhiteKeyWidth/3)) || (xPos > (saveleft + WhiteKeyWidth)))
						{
							EndNote(2);
							SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else if (yKey[ScanCode] == 2)//on a white key with a black key on the right
					{
						if (yPos > (top+100))
						{
							if ((xPos < saveleft) || (xPos > (saveleft+WhiteKeyWidth)))
							{
								EndNote(2);
								SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
							}
						}
						else if ((xPos < saveleft) || (xPos > (saveleft + (WhiteKeyWidth*2/3))))
						{
							EndNote(2);
							SendMessage(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
				}
				else
					EndNote(2);
			}
		}
		return 0;


	case WM_USER:
		if (wParam == 0x80)//Note Off
		{
			wParam = 0x90;//trick
			lParam &= 0xFF;//trick
		}
		if (wParam == 0x90)//Note On or Off
		{
			Velocity = (lParam >> 8) & 0xFF;
			SavedNote = lParam & 0xFF;
			ScanCode = Codes[SavedNote-36];//36 is the lowest MIDI note number in this program
			if (Velocity)
			{
				Velocity += 25;
				if (Velocity > 127)
					Velocity = 127;
				StartNote(3);
			}
			else//0 velocity means end note
			{
				EndNote(3);
			}
		}
		return 0;

	case WM_APP:          // por aqui se reciben las entradas del micro
		{
		   PITCH *pitch;
		   int index,r;
           static int noteon,nomic,comic;

           pitch=(PITCH *) wParam;
		   if(pitch->notekey!=0){
              for(r=0;r<49;r++)
                 if(noteindex[r]!=0) break;
		      index=pitch->notekey-36; 
              if(r==49){ ++noteindex[index]; return 0; }
              if(r==index){
                 if(++noteindex[index]>10) noteindex[index]=10;
              }else{ 
				 noteindex[r]-=2;
                 if(noteindex[r]<0) noteindex[r]=0;
              }
              if(noteindex[r]==10 && noteon==0){
		         nomic=SavedNote=pitch->notekey;
		         comic=ScanCode = Codes[SavedNote-36];
                 Velocity = concfg.DefaultVelocity;
                 noteon=1;               
				 StartNote(4);
              }
              if(noteindex[r]==0 && noteon==1){
                 noteon=0;
                 Velocity=0;
				 SavedNote=nomic;
				 ScanCode=comic;
				 EndNote(4);
			  }
		   }
		}
        return 0;

    case WM_APP+1:    // entrada micro reconocimiento de voz en modo run
		{
           int vv=(int) wParam;
           
		   SavedNote=notas00[14+vv];
		   ScanCode = Codes[SavedNote-36];
           Velocity = concfg.DefaultVelocity;
		   StartNote(4);
		   Sleep(500);
           Velocity=0;
		   EndNote(4);
		}
		return 0;

    case WM_APP+2:    // entrada micro reconocimiento de voz en modo learning
        {
		   static int r;     
		   static pianoinfo pianoinf;
		   char buff0[7][5]={"Do","Re","Mi","Fa","Sol","La","Si"};
		   int pronun[]={0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6};
		   UINT estado=(UINT) wParam;

		   if(estado==0){    // inicia captura
			  if(r!=0) r=0;
	          sprintf(pianoinf.buff,"Pronuncia la palabra %s                \n",buff0[r]);
	          pianoinf.Show(TRUE);
			  SendMessage(pianoinf.m_hDialog,WM_PAINT,0,0);
			  solfeohablado(0,1,NULL);  // activamos micro, learning modo
			  return 0;
		   }
		   if(estado==1){    // dato capturado
	          strcpy(pianoinf.buff,"Capturada correctamente                        \n");
			  SendMessage(pianoinf.m_hDialog,WM_PAINT,0,0);
              Sleep(1000);					 
			  ++r;
	          if(r<28) sprintf(pianoinf.buff,"Pronuncia la palabra %s                                 \n",buff0[pronun[r]]);
			  else{ r=0; return 28; }
	          SendMessage(pianoinf.m_hDialog,WM_PAINT,0,0);
              return pronun[r-1];							 
		   }
		   if(estado==2){
			  SendMessage(pianoinf.m_hDialog,WM_DESTROY,0,0);
			  solfeohablado(1,0,NULL);       // eliminamos stream, deshabilitamos micro
			  solfeohablado(-1,0,NULL);  // cargamos base de datos
			  solfeohablado(0,0,NULL); //base de datos ok, activamos reconocimiento de voz
		   }		
		}
		return 0;

	case WM_CTLCOLORSTATIC:	//here we color the controls
		HWND hwnds;

		hwnds=(HWND) lParam;
		if(hwnds==hwndmetronomotitle || hwnds==hwndaccnotestitle || hwnds==hwndacctempotitle || hwnds==hwndvolumetitle ||
		   hwnds==hwndaccnotes || hwnds==hwndacctempo || hwnds==hwndmetronomo || hwnds==hwndvolume ||
		   hwnds==hwndmetronomovalue || hwnds==hwndaccnotesvalue || hwnds==hwndacctempovalue || hwnds==hwndvolumevalue ||
		   hwnds==hwndmetronotereadtitle || hwnds==hwndmetronotereadvalue || hwnds==hwndmetronoteread ||
		   hwnds==hwndmetronoteintervaltitle | hwnds==hwndmetronoteintervalvalue || hwnds==hwndmetronoteinterval){
		   SetTextColor((HDC) wParam,BLACK);
		   SetBkMode((HDC) wParam,TRANSPARENT);	
		   return (long)hWhiteBrush; 
		}
		if(hwnds==hlpwnd){			
		   SetTextColor((HDC) wParam,BLACK);
		   SetBkMode((HDC) wParam,TRANSPARENT);			   
           return (long) hHelpBrush;
		}
		break;

	case WM_TIMER:
		{
	       UINT timerid=(UINT) wParam;
		   int wh;

		   if(timerid==1 || timerid==2){
		      KillTimer(hwnd,timerid);
              if(wParam==1) InvalidateRect(hwnd, NULL, TRUE);
		      saveconfiguracion();
		      break;
		   } 
		   if(timerid==3){
		      KillTimer(hwnd,timerid);
			  paintsplash(0,0,0);
			  MessageBox(hwnd, "Querido Usuario, \nes necesario para la mejora del programa \nque me envies tus comentarios y mejoras usando \
			  \nla opción de menu ayuda->preguntas y comentarios \no por correo a chkven7@gmail.com.  \
			  \n", "PIANOTRAIN", MB_OK|MB_ICONWARNING);
              InvalidateRect(hwnd, NULL, TRUE);
		      break;
		   } 
		   if(timerid==4){
			  int x,y,ax,ay;
			  HDC hdc;

			  hdc=GetDC(hwnd); 
              KillTimer(hwnd,timerid);
			  SelectObject(hdc,hfontsystem);
			  GetCharWidth32(hdc,'a','a',&wh); 
			  ReleaseDC(hwnd,hdc);
			  x=hlprect.left; y=hlprect.bottom; ax=(strlen(hlpvalue)+2)*wh; ay=20;
			  x=x+10-ax/2;
			  hlpwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"STATIC","",
			     WS_CHILD |WS_BORDER|WS_VISIBLE,
				 x,y,ax,ay,
				 hwnd, NULL, hInst, NULL);
			  SendMessage(hlpwnd,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));
			  SendMessage(hlpwnd,WM_SETTEXT,0,(LPARAM)(LPCTSTR) hlpvalue);
			  break;
		   }
		   if(timerid==5){
		      int interval,mi,ma,inf,sup;
              inf=concfg.mintono_earnoteread*2.0; 
			  sup=concfg.maxtono_earnoteread*2.0;
              interval=inf+(int)(uniform_deviate(rand()) * (sup-inf+1));
		      mi=concfg.noteinfgeneric[1]; ma=concfg.notesupgeneric[0];
		      if(gfstat[0]==1){ mi=concfg.noteinfgeneric[0]; ma=concfg.notesupgeneric[0]; }
		      if(gfstat[1]==1){ mi=concfg.noteinfgeneric[1]; ma=concfg.notesupgeneric[1]; }
		      RandomNote[1]=addinterval(&RandomNote[0],interval,mi,ma);
		      PlayNotaMidi((__int64) RandomNote[1].notakey,-1);
              KillTimer(hwnd,timerid);
			  break;
		   }
		   if(timerid==10){
			  static int hits;
			  int res;
              
			  if((int)notereadingscore.hits==0) 
		         hits=0;
			  res=notereadingscore.hits-hits;
			  hits=notereadingscore.hits;
              res=res*4;    // res es el numero de aciertos en 15seg*4=1minutos			  
			  notereadingscore.tempocompas=res;
		   }

		   if(timerid==12){			  
			  notereadtimersand=1;
			  if(concfg.metronoteread==0){
			     InvalidateRect(hwnd,NULL,TRUE);
				 KillTimer(hwnd,12);
				 notereadtimersand=0;
				 break;
			  }		
			  
			  hdc=GetDC(hwnd); 
		      pernoteread+=10; 
			  pernoteread%=100;
	          int numstaff=gfstat[0]+2*gfstat[1];
			  if(concfg.notereadingchord==1)
	             Paintnotereading(RandomNote,randomcannote,numstaff,concfg.armaduranotereading,1);        
			  else
				 Paintnotereading(RandomNote,notereadingdeep,numstaff,concfg.armaduranotereading,1);        
			  if(pernoteread==0){
                 notereading(0,0);
				 ++notereadingscore.fails;
			  }		
			  ReleaseDC(hwnd,hdc);
              break;			  
		   }
		   if(timerid==14){			  
			  noteintervaltimersand=1;
			  if(concfg.metronoteinterval==0){
			     InvalidateRect(hwnd,NULL,TRUE);
				 KillTimer(hwnd,14);
				 noteintervaltimersand=0;
				 break;
			  }		
			  
			  hdc=GetDC(hwnd); 
		      pernoteread+=10; 
			  pernoteread%=100;
	          int numstaff=gfstat[0]+2*gfstat[1];
			  PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas-1),6,0,concfg.armaduranoteinterval);		   
			  if(pernoteread==0){
		         melodia->penta[0].compas[melodia->puneje.compas].color[0]=PURPLE;                      // suponemos que la nota en el buffer es incorrecta por defecto		 
			     melodia->penta[1].compas[melodia->puneje.compas].color[0]=PURPLE;                      // suponemos que la nota en el buffer es incorrecta por defecto		 
				 if(melodia->puneje.eje!=NULL)
				    delete melodia->puneje.eje;
				 melodia->puneje.index=0;
				 ++melodia->puneje.compas;
				 melodia->puneje.eje=generaejecucion(melodia->penta,melodia->puneje.compas);
				 melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
				 IntervalTestUpdate(0,0,-1);
				 IntervalTestUpdate(0,0,-2);
                 IntervalTestUpdate2(melodia);
			  }		
			  ReleaseDC(hwnd,hdc);
			  
              break;			  
		   }

		   break;
		}
	case WM_HSCROLL:
	{
       // elimina el rectangulo del focus de los controles deslizantes
       HWND hwndScrollBar;

       hwndScrollBar = (HWND) lParam;
	   if((hwndScrollBar==hwndaccnotes || hwndScrollBar==hwndacctempo ||
          hwndScrollBar==hwndmetronoteread ||
		  hwndScrollBar==hwndmetronomo || hwndScrollBar==hwndvolume) && hwndScrollBar!=NULL){
   			SetFocus(hwnd);
		  updatesliders(hwndScrollBar);
		  if(hwndScrollBar==hwndvolume || hwndScrollBar==hwndmetronoteread) SetTimer(hwnd,2,1000,NULL);
	      else SetTimer(hwnd,1,1000,NULL);
	   }
       if(hwndScrollBar==hwndmetronoteread){
	     HDC tbhdc;
         RECT rct;
         char aa[128];
         int pos;
		 double fig[]={4,2,1,0.5,0.25};
    
         tbhdc=GetDC(hwndScrollBar);
         GetClientRect(hwndScrollBar,&rct);
		 FrameRect(tbhdc,&rct,hWhiteBrush);
		 pos=SendMessage(hwndScrollBar,TBM_GETPOS,0,0);	
         sprintf(aa,"%3d ",pos);
         mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
         mydrawtext(hwndmetronotereadvalue,RED,hfontsystem,aa,0,0);   
	     concfg.metronoteread=pos;
	     if(pos!=0){
			unsigned int dur;
            dur=(60*CLOCKS_PER_SEC)/pos; // duracion en negras por minuto
			if(concfg.notereadingfiguras)
		       dur=dur*fig[(int)RandomNote[0].dura];
			else dur=dur*fig[0]; // usamos redondas 
		    dur/=10;  // subdividimos el tempo en partes de 10
            SetTimer(hwnd,12,dur,NULL);
		 }
		 ReleaseDC(hwndScrollBar,tbhdc);
	  }   
      if(hwndScrollBar==hwndmetronoteinterval){
	     HDC tbhdc;
         RECT rct;
         char aa[128];
         int pos;
		 double fig[]={4,2,1,0.5,0.25};
    
         tbhdc=GetDC(hwndScrollBar);
         GetClientRect(hwndScrollBar,&rct);
		 FrameRect(tbhdc,&rct,hWhiteBrush);
		 pos=SendMessage(hwndScrollBar,TBM_GETPOS,0,0);	
         sprintf(aa,"%3d ",pos);
         mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
         mydrawtext(hwndmetronoteintervalvalue,RED,hfontsystem,aa,0,0);   
	     concfg.metronoteinterval=pos;
	     if(pos!=0){
			unsigned int dur;
            dur=(60*CLOCKS_PER_SEC)/pos; // duracion en negras por minuto
			dur=dur*fig[0]; // usamos redondas 
		    dur/=10;  // subdividimos el tempo en partes de 10
            SetTimer(hwnd,14,dur,NULL);
		 }
		 ReleaseDC(hwndScrollBar,tbhdc);
	  }   

        if(hwndScrollBar==NULL && helpexercise){
           int xDelta;     // xDelta = new_pos - current_pos  
           int xNewPos;    // new position 
           int yDelta = 0; 
           SCROLLINFO si;

		   switch (LOWORD(wParam)) 
            { 
                // User clicked the scroll bar shaft left of the scroll box. 
                case SB_PAGEUP: 
                    xNewPos = xCurrentScroll - 50; 
                    break; 
 
                // User clicked the scroll bar shaft right of the scroll box. 
                case SB_PAGEDOWN: 
                    xNewPos = xCurrentScroll + 50; 
                    break; 
 
                // User clicked the left arrow. 
                case SB_LINEUP: 
                    xNewPos = xCurrentScroll - 5; 
                    break; 
 
                // User clicked the right arrow. 
                case SB_LINEDOWN: 
                    xNewPos = xCurrentScroll + 5; 
                    break; 
 
                // User dragged the scroll box. 
                case SB_THUMBPOSITION: 
                    xNewPos = HIWORD(wParam); 
                    break; 
 
                default: 
                    xNewPos = xCurrentScroll; 
            } 
 
            // New position must be between 0 and the screen width. 
            xNewPos = max(0, xNewPos); 
            xNewPos = min(xMaxScroll, xNewPos); 
 
            // If the current position does not change, do not scroll.
            if (xNewPos == xCurrentScroll) 
                break; 
 
            // Set the scroll flag to TRUE. 
            fScroll = TRUE; 
 
            // Determine the amount scrolled (in pixels). 
            xDelta = xNewPos - xCurrentScroll; 
 
            // Reset the current scroll position. 
            xCurrentScroll = xNewPos; 
 
            // Scroll the window. (The system repaints most of the 
            // client area when ScrollWindowEx is called; however, it is 
            // necessary to call UpdateWindow in order to repaint the 
            // rectangle of pixels that were invalidated.) 
            ScrollWindowEx(hwnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
                (CONST RECT *) NULL, (HRGN) NULL, (PRECT) NULL, 
                SW_INVALIDATE); 
            UpdateWindow(hwnd); 
 
            // Reset the scroll bar. 
            si.cbSize = sizeof(si); 
            si.fMask  = SIF_POS; 
            si.nPos   = xCurrentScroll; 
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
        } 
	   break;
    }
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);		
        SelectObject(hdc, hMaestroFont);
		updatesliders(hwndmetronoteread);
		updatesliders(hwndmetronoteinterval);
        updatesliders(hwndmetronomo);
        updatesliders(hwndaccnotes);
        updatesliders(hwndacctempo);
		updatesliders(hwndvolume);
		paintsplash(0,0,1);
		if (first)
		{
			first = FALSE;
			SelectObject(hdc, hMaestroFont);
			GetCharWidth32(hdc, 61, 61, &StaffWidth[0]);//61 is '='
			GetCharWidth32(hdc, 119, 119, &NoteWidth[0]);//119 is 'w'
			GetCharWidth32(hdc, 38, 38, &GClefWidth[0]);//38 is '&'
			GetCharWidth32(hdc, 63, 63, &FClefWidth[0]);//63 is '?'
			NumOfStaffs = rect.right / StaffWidth[0];

			SelectObject(hdc, hMaestroFontSmall1);
			GetCharWidth32(hdc, 61, 61, &StaffWidth[1]);//61 is '='
			GetCharWidth32(hdc, 119, 119, &NoteWidth[1]);//119 is 'w'
			GetCharWidth32(hdc, 38, 38, &GClefWidth[1]);//38 is '&'
			GetCharWidth32(hdc, 63, 63, &FClefWidth[1]);//63 is '?'
			NumOfStaffs = rect.right / StaffWidth[1];

			//**************************************************************************
			// en 1280x1024 NumOfStaffs==40 lo que provoca que no se vea bien el ultimo compas
			// restamos 1 a 40 para evitar el problema
			// tamaño fuente
			//**************************************************************************
			if(NumOfStaffs==40)
				NumOfStaffs=39;

			//**************************************************************************
            // cargamos aqui para actualizar el numero de staffs en pantalla, indicador de volumen, etc
			cargarconfiguracion(NULL);		
			
			SelectObject(hdc, hMaestroFont);
			if (NumOfStaffs > 128)
				NumOfStaffs = 128;
			for (x = 0; x < NumOfStaffs; x++)
				Staff[x] = '=';
			Staff[x] = 0;
			SelectObject(hdc, hSmallFont);
			for (x = 0; x < 22;x++)
			{
				GetCharWidth32(hdc, ComputerKeys1[x], ComputerKeys1[x], &Widths1[x]);
				GetCharWidth32(hdc, ComputerKeys2[x], ComputerKeys2[x], &Widths2[x]);
			}
			SelectObject(hdc, hFont);
			for (x = 0; x < 7;x++)
				GetCharWidth32(hdc, CDEFGAB[x], CDEFGAB[x], &Widths[x]);
			hdcMem1=CreateCompatibleDC(hdc);						
			hMemBitmap1 = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(hdcMem1, hMemBitmap1);			
		}

        if(helpexercise){
            PRECT prect;  
            if(1) // (fSize) 
            { 
                BitBlt(ps.hdc,0, 0,bmphelp.bmWidth, bmphelp.bmHeight,hdccompathelp, 
                    xCurrentScroll, yCurrentScroll, 
                    SRCCOPY); 
                fSize = FALSE; 
            } 
 
            // If scrolling has occurred, use the following call to 
            // BitBlt to paint the invalid rectangle. 
            // 
            // The coordinates of this rectangle are specified in the 
            // RECT structure to which prect points. 
            // 
            // Note that it is necessary to increment the seventh 
            // argument (prect->left) by xCurrentScroll and the 
            // eighth argument (prect->top) by yCurrentScroll in 
            // order to map the correct pixels from the source bitmap. 
           if(fScroll){ 
                prect = &ps.rcPaint; 
                BitBlt(ps.hdc,prect->left, prect->top,(prect->right - prect->left), 
                    (prect->bottom - prect->top), 
                    hdccompathelp, 
                    prect->left + xCurrentScroll, 
                    prect->top + yCurrentScroll, 
                    SRCCOPY); 
                    fScroll = FALSE; 
           } 
           ModifyMenu(hMenu,IDM_HELPEXERCISE, MF_BYCOMMAND|MF_CHECKED,IDM_HELPEXERCISE,helpexer);								
		   DrawMenuBar(hwnd);	
		   SelectObject(hdc, hOldPen);
		   SelectObject(hdc, hOldBrush);
		   EndPaint(hwnd, &ps);
		   return 0;
		}
//**********************************************************************************
// PINTA PENTAGRAMA ENCIMA DEL TECLADO VIRTUAL
//**********************************************************************************
        if(showvirstaff==TRUE){
		   SelectObject(hdc, hMaestroFont);        
		   SetBkColor(hdc, BACKGROUND);
		   TextOut(hdc, ExtraSpace, top-212,Staff,NumOfStaffs);
		   TextOut(hdc, ExtraSpace, top-121,Staff,NumOfStaffs);		
		   SetBkMode(hdc, TRANSPARENT);  
		   TextOut(hdc, ExtraSpace, top-224, "&", 1);
		   TextOut(hdc, ExtraSpace-1, top-212, "\\", 1);
		   TextOut(hdc, ExtraSpace, top-158, "?", 1);
		   TextOut(hdc, ExtraSpace-1, top-121, "\\", 1);
		   TextOut(hdc, ExtraSpace-1, top-169, "\\", 1);
		   if ((showkeys) && (KeyName)){
			  switch (KeyName){
			    case 5:
				   TextOut(hdc, ExtraSpace+100, top-230, "#", 1);
				   TextOut(hdc, ExtraSpace+100, top-127, "#", 1);
			    case 4:
				   TextOut(hdc, ExtraSpace+85, top-248, "#", 1);
				   TextOut(hdc, ExtraSpace+85, top-145, "#", 1);
			    case 3:
				   TextOut(hdc, ExtraSpace+70, top-266, "#", 1);
				   TextOut(hdc, ExtraSpace+70, top-163, "#", 1);
			    case 2:
				   TextOut(hdc, ExtraSpace+55, top-242, "#", 1);
				   TextOut(hdc, ExtraSpace+55, top-139, "#", 1);
			    case 1:
				   TextOut(hdc, ExtraSpace+40, top-260, "#", 1);
				   TextOut(hdc, ExtraSpace+40, top-157, "#", 1);
				   break;
			    case 6:
				   if (accidental == 2)
				   {
					  TextOut(hdc, ExtraSpace+115, top-242, "b", 1);
					  TextOut(hdc, ExtraSpace+115, top-139, "b", 1);
				   }
				   else//if (accidental == 1)
				   {
					  TextOut(hdc, ExtraSpace+115, top-254, "#", 1);
					  TextOut(hdc, ExtraSpace+115, top-151, "#", 1);
					  TextOut(hdc, ExtraSpace+100, top-230, "#", 1);
					  TextOut(hdc, ExtraSpace+100, top-127, "#", 1);
					  TextOut(hdc, ExtraSpace+85, top-248, "#", 1);
					  TextOut(hdc, ExtraSpace+85, top-145, "#", 1);
					  TextOut(hdc, ExtraSpace+70, top-266, "#", 1);
					  TextOut(hdc, ExtraSpace+70, top-163, "#", 1);
					  TextOut(hdc, ExtraSpace+55, top-242, "#", 1);
					  TextOut(hdc, ExtraSpace+55, top-139, "#", 1);
					  TextOut(hdc, ExtraSpace+40, top-260, "#", 1);
					  TextOut(hdc, ExtraSpace+40, top-157, "#", 1);
					  break;
				   }
			    case 7:
				   TextOut(hdc, ExtraSpace+100, top-224, "b", 1);
				   TextOut(hdc, ExtraSpace+100, top-121, "b", 1);
			    case 8:
				   TextOut(hdc, ExtraSpace+85, top-248, "b", 1);
				   TextOut(hdc, ExtraSpace+85, top-145, "b", 1);
			    case 9:
				   TextOut(hdc, ExtraSpace+70, top-230, "b", 1);
				   TextOut(hdc, ExtraSpace+70, top-127, "b", 1);
			    case 10:
				   TextOut(hdc, ExtraSpace+55, top-254, "b", 1);
				   TextOut(hdc, ExtraSpace+55, top-151, "b", 1);
			    case 11:
				   TextOut(hdc, ExtraSpace+40, top-236, "b", 1);
				   TextOut(hdc, ExtraSpace+40, top-133, "b", 1);
			  }
		   }		
		   SelectObject(hdc, hSmallFont);
		   if (showkeys == FALSE){
		      SetTextColor(hdc, 0x6060D0);
		      TextOut(hdc, 50, top-200, "F", 3);
		      TextOut(hdc, 50, top-188, "D", 1);
		      TextOut(hdc, 50, top-176, "B", 1);
		      TextOut(hdc, 50, top-164, "G", 1);
		      TextOut(hdc, 50, top-152, "E", 1);
		      TextOut(hdc, 65, top-205, "G", 1);
		      TextOut(hdc, 65, top-193, "E", 1);
		      TextOut(hdc, 65, top-181, "C", 1);
		      TextOut(hdc, 65, top-169, "A", 1);
		      TextOut(hdc, 65, top-157, "F", 1);
		      TextOut(hdc, 65, top-145, "D", 1);
		      TextOut(hdc, 50, top-109, "A", 1);
		      TextOut(hdc, 50, top-97, "F", 1);
		      TextOut(hdc, 50, top-85, "D", 1);
		      TextOut(hdc, 50, top-73, "B", 1);
		      TextOut(hdc, 50, top-61, "G", 1);
		      TextOut(hdc, 65, top-114, "B", 1);
		      TextOut(hdc, 65, top-102, "G", 1);
		      TextOut(hdc, 65, top-90, "E", 1);
		      TextOut(hdc, 65, top-78, "C", 1);
		      TextOut(hdc, 65, top-66, "A", 1);
		      TextOut(hdc, 65, top-54, "F", 1);
		      TextOut(hdc, 50, top-130, "C", 1);
		   }
		}
		
//********************************************************************
//********************************************************************


// COMPUTERKEYS EN EL TECLADO
/*
		SetTextColor(hdc, 0x6060D0);
		for (x = ExtraSpace + (4*WhiteKeyWidth), z = 0; z < 10; x += WhiteKeyWidth, z++)			
			TextOut(hdc, x+((WhiteKeyWidth-Widths1[z])/2), top + 155, &ComputerKeys1[z], 1);
		SetTextColor(hdc, 0xFF0000);//blue
		for ( ; z < 22; x += WhiteKeyWidth, z++)
			TextOut(hdc, x+((WhiteKeyWidth-Widths1[z])/2), top + 155, &ComputerKeys1[z], 1);
		SetBkMode(hdc, OPAQUE);

		for (x = (4*WhiteKeyWidth) + ExtraSpace-(WhiteKeyWidth/2), z = 0; z < 10; x += WhiteKeyWidth, z++)
		{
			if ((z == 3) || (z == 6) || (z == 10) || (z == 13) || (z == 17) || (z == 20))
				SetTextColor(hdc, 0xD0D0D0);
			else
				SetTextColor(hdc, 0x6060D0);//reddish
			TextOut(hdc, x+((WhiteKeyWidth-Widths2[z])/2), top - 22, &ComputerKeys2[z], 1);
		}
		SetTextColor(hdc, 0x6060D0);
		for ( ; z < 22; x += WhiteKeyWidth, z++)
		{
			if ((z == 3) || (z == 6) || (z == 10) || (z == 13) || (z == 17) || (z == 20))
				SetTextColor(hdc, 0xD0D0D0);
			else
				SetTextColor(hdc, 0xFF0000);//blue
			TextOut(hdc, x+((WhiteKeyWidth-Widths2[z])/2), top - 22, &ComputerKeys2[z], 1);
		}
*/
//*********************************************************************************
// PINTA PIANO VIRTUAL
//*********************************************************************************

		SetBkMode(hdc, OPAQUE);
        paintwoodu(0,top-22);
		paintwoodd(0,top+150);
		hOldPen = SelectObject(hdc, hPen);
		hOldBrush = SelectObject(hdc,hWhiteBrush);

		for (x = ExtraSpace; x < (35*WhiteKeyWidth); x += WhiteKeyWidth)
			Rectangle(hdc, x, top+5, x+WhiteKeyWidth, top+150);
		SelectObject(hdc, hBlackBrush);
		for (x = ExtraSpace; x < (ExtraSpace+(35*WhiteKeyWidth)); x += (7*WhiteKeyWidth))
		{
			left = x + BlackKeyWidth;
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
			left = x + (WhiteKeyWidth*5/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
			left = x + (WhiteKeyWidth*11/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
			left = x + (WhiteKeyWidth*14/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
			left = x + (WhiteKeyWidth*17/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
			left = -1;
		}

		{
		   POINT p[2];

           HPEN pen=CreatePen(PS_SOLID,5,RED);
	       SelectObject(hdc,pen);
	       p[0].x=0; //ExtraSpace+1;
	       p[0].y=top+1;
	       p[1].x=rect.right; //35*WhiteKeyWidth+1;
	       p[1].y=top+1;
	       Polyline(hdc,p,2);
	       DeleteObject(pen);
		}
		if (shownamekeys == TRUE)
		{
		    SetBkMode(hdc, TRANSPARENT);  
			SetTextColor(hdc,0x8080ff);
			SelectObject(hdc,hfontsystemg);
			for (x = 0; x < 35; x++)
				TextOut(hdc, (x*WhiteKeyWidth) + (WhiteKeyWidth-Widths[x%7])/2, top+110, &CDEFGAB[x % 7], 1);
		}
        paintkey((14*WhiteKeyWidth) + ExtraSpace + (WhiteKeyWidth*0.35),top+125);		
		if(showtest1){  
		   ModifyMenu(hMenu, IDM_TEST1, MF_BYCOMMAND|MF_CHECKED, IDM_TEST1, PlayNote1);
		   DrawMenuBar(hwnd);
		   testRect.left = ExtraSpace;
		   testRect.right = testRect.left + (NumOfStaffs* StaffWidth[1]); //42 jordi
		   testRect.top = top-462-59;
		   if(showvirstaff==1) testRect.bottom = top260;				
		   else testRect.bottom = topvirstaff;				
		   hdc = GetDC(hwnd);
		   scrollrandommusic(melodia,offsetdispinterval);
		   paintrandommusic(melodia);
		   ReleaseDC(hwnd, hdc);
		}

		if(showtest2){  
		   //ModifyMenu(hMenu, IDM_TEST2, MF_BYCOMMAND|MF_STRING, IDM_TEST2, StopNotes2);
		   ModifyMenu(hMenu, IDM_TEST2, MF_BYCOMMAND|MF_CHECKED, IDM_TEST2,PlayNote2);
		   DrawMenuBar(hwnd);
		   testRect.left = top260min30+ 125;
		   testRect.right = testRect.left + (8* StaffWidth[0]);
		   testRect.top = top-462-59;
		   testRect.bottom = top260;
		   hdc = GetDC(hwnd);
           PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas-1),6,0,concfg.armaduranoteinterval);		   
		   ReleaseDC(hwnd, hdc);
		}

		if (showtest3)
		{
			//ModifyMenu(hMenu, IDM_TEST, MF_BYCOMMAND|MF_STRING, IDM_TEST, StopNotes);
			ModifyMenu(hMenu, IDM_TEST, MF_BYCOMMAND|MF_CHECKED, IDM_TEST, PlayNote);
			DrawMenuBar(hwnd);
			testRect.left = top260min30; // + 125;
			testRect.right = testRect.left + (8* StaffWidth[0]);
			testRect.top = top-462-59;
			testRect.bottom = top260;
			hdc = GetDC(hwnd);
			SelectObject(hdc, hMaestroFont);
		    notereading(1,0);
			ReleaseDC(hwnd, hdc);			
		}

	    if (showtest4)
		{
		   ModifyMenu(hMenu, IDM_TEST4, MF_BYCOMMAND|MF_CHECKED, IDM_TEST4,PlayNote4);
		   DrawMenuBar(hwnd);
		   testRect.left = top260min30 + 125;
		   testRect.right = testRect.left + (8* StaffWidth[0]);
		   testRect.top = top-462-59;
		   testRect.bottom = top260;
		   hdc = GetDC(hwnd);
           PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas-1),4,1,-100);
		   ReleaseDC(hwnd, hdc);
		}

		if (showtest5)
		{
		   ModifyMenu(hMenu, IDM_TEST5, MF_BYCOMMAND|MF_CHECKED, IDM_TEST5,PlayNote5);
		   DrawMenuBar(hwnd);
	       testRect.left =ExtraSpace;
	       testRect.right=testRect.top=testRect.bottom=0; 
		   NotereadInverse(NULL,1);
		}
		if (showtest6)
		{
		   ModifyMenu(hMenu, IDM_TEST6, MF_BYCOMMAND|MF_CHECKED, IDM_TEST6,PlayNote6);
		   DrawMenuBar(hwnd);	    
		   testRect.left = top260min30; //+125;
		   testRect.right = testRect.left + (8* StaffWidth[0]);
		   testRect.top = top-462-59;
		   testRect.bottom = top260;			   
		   hdc = GetDC(hwnd);
		   initmemoespacial(2);
		   ReleaseDC(hwnd,hdc);
		}

		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hOldBrush);
		/////////////////////////////////////////////////////////////////
		//BitBlt(hdcMem, 0, top260, rect.right, rect.bottom, hdc, 0, top260, SRCCOPY);
	    BitBlt(hdcMem1, 0,topvirstaff, rect.right, rect.bottom, hdc, 0, topvirstaff, SRCCOPY);
		/////////////////////////////////////////////////////////////////
		EndPaint(hwnd, &ps);
		return 0;

    case WM_VSCROLL: 
        { 
            int xDelta = 0; 
            int yDelta;     // yDelta = new_pos - current_pos 
            int yNewPos;    // new position 
            SCROLLINFO si;

            switch (LOWORD(wParam)) 
            { 
                // User clicked the scroll bar shaft above the scroll box. 
                case SB_PAGEUP: 
                    yNewPos = yCurrentScroll - 50; 
                    break; 
 
                // User clicked the scroll bar shaft below the scroll box. 
                case SB_PAGEDOWN: 
                    yNewPos = yCurrentScroll + 50; 
                    break; 
 
                // User clicked the top arrow. 
                case SB_LINEUP: 
                    yNewPos = yCurrentScroll - 5; 
                    break; 
 
                // User clicked the bottom arrow. 
                case SB_LINEDOWN: 
                    yNewPos = yCurrentScroll + 5; 
                    break; 
 
                // User dragged the scroll box. 
                case SB_THUMBPOSITION: 
                    yNewPos = HIWORD(wParam); 
                    break; 
 
                default: 
                    yNewPos = yCurrentScroll; 
            } 
 
            // New position must be between 0 and the screen height. 
            yNewPos = max(0, yNewPos); 
            yNewPos = min(yMaxScroll, yNewPos); 
 
            // If the current position does not change, do not scroll.
            if (yNewPos == yCurrentScroll) 
                break; 
 
            // Set the scroll flag to TRUE. 
            fScroll = TRUE; 
 
            // Determine the amount scrolled (in pixels). 
            yDelta = yNewPos - yCurrentScroll; 
 
            // Reset the current scroll position. 
            yCurrentScroll = yNewPos; 
 
            // Scroll the window. (The system repaints most of the 
            // client area when ScrollWindowEx is called; however, it is 
            // necessary to call UpdateWindow in order to repaint the 
            // rectangle of pixels that were invalidated.) 
            ScrollWindowEx(hwnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
                (CONST RECT *) NULL, (HRGN) NULL, (PRECT) NULL, 
                SW_INVALIDATE); 
            UpdateWindow(hwnd); 
 
            // Reset the scroll bar. 
            si.cbSize = sizeof(si); 
            si.fMask  = SIF_POS; 
            si.nPos   = yCurrentScroll; 
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
            break; 
        } 
    case WM_CLOSE:
       DestroyWindow(hwnd);
	   return 0;
	case WM_DESTROY:
        pitchdetection(1);
		if (midi_in)
		{
			midiInStop(hMidiIn);
			midiInReset(hMidiIn);
			midiInClose(hMidiIn);
		}
		midiOutShortMsg(hMidiOut, 0xB0 | (123 << 8));//set controller for channel 0 to all voices off
		midiOutReset(hMidiOut);
		midiOutClose(hMidiOut);
		RemoveFontResource("MAESTRO_.TTF");
		GenericControlsRemove();
		endthreads();
		liberabitmaps();
		liberathreads();
		if(helpexercise==TRUE){
	       DeleteDC(hdccompathelp);
		   DeleteObject(hbitmaphelp);
		}
		DeleteObject(hPen);
		DeleteObject(hWhiteBrush);
		DeleteObject(hBlackBrush);
		DeleteObject(hBlueBrush);
		DeleteObject(hlBlueBrush);
		DeleteObject(hPinkBrush);
		DeleteObject(hGreenBrush);
		DeleteObject(hRedBrush);
		DeleteObject(hFont);
		DeleteObject(hSmallFont);
		DeleteObject(hSmallFontItalic);
		DeleteObject(hMaestroFont);
		DeleteObject(hMaestroFontSmall);
		DeleteObject(hMaestroFontSmall1);
        DeleteObject(hMaestroFontSmall2);
		DeleteObject(hMaestroFontSmall3);
        DeleteObject(hfontsystem);
		DeleteObject(hfontsystemg);
		DeleteObject(hMemBitmap1);
		DeleteObject(hOldFont);
		DeleteObject(hOldPen);
		DeleteObject(hOldBrush);
		DeleteDC(hdcMem1);
		DeleteDC(hdc);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void removemousemsgqueue(void)
{
   MSG  msg;

   while(PeekMessage( &msg, hwnd,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE));   
   return;
}

void paintkeypiano1(int notekey,int estado,HBRUSH hbr,HDC hdcext)
{
   int scancode,left,bracketingkeys;
   POINT p[15];
   HDC hdc;

   if(hdcext!=NULL) hdc=hdcext;
   else hdc=GetDC(hwnd);
   scancode = Codes[notekey-36];
   bracketingkeys = yKey[scancode];
   left = (xKey[scancode] * WhiteKeyWidth) + ExtraSpace;
   SetBkMode(hdc, OPAQUE);
   hOldPen = SelectObject(hdc, hPen);
   if(calcnoteindex(notekey)==-1){
      if(estado==1) SelectObject(hdc,hbr); 
      else SelectObject(hdc, hBlackBrush);
	  left -= WhiteKeyWidth/3;
      Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);   
	  if(hdcext==NULL) ReleaseDC(hwnd,hdc);
	  return;
   }else{  
      if(estado==1) SelectObject(hdc, hbr); 
	  else SelectObject(hdc, hWhiteBrush);
	  if(bracketingkeys==LEFT){
	     p[0].x=left;                   p[0].y=top+100;
		 p[1].x=p[0].x;                 p[1].y=top+150;
		 p[2].x=left+WhiteKeyWidth;     p[2].y=p[1].y;
		 p[3].x=p[2].x;					p[3].y=top+5;
		 p[4].x=left+0.5*BlackKeyWidth; p[4].y=p[3].y;
		 p[5].x=p[4].x;                 p[5].y=top+100;
		 Polygon(hdc,p,6);
	  }else if(bracketingkeys == RIGHT){
         p[0].x=left;                   p[0].y=top+5;
		 p[1].x=p[0].x;                 p[1].y=top+150;
		 p[2].x=left+WhiteKeyWidth;     p[2].y=p[1].y;
		 p[3].x=p[2].x;                 p[3].y=top+100;
		 p[4].x=p[3].x-0.5*BlackKeyWidth; p[4].y=p[3].y;
		 p[5].x=p[4].x;                 p[5].y=top+5;
		 Polygon(hdc,p,6);
	  }else if(bracketingkeys == BOTH){
	     p[0].x=left;                   p[0].y=top+100;
		 p[1].x=p[0].x;                 p[1].y=top+150;
		 p[2].x=left+WhiteKeyWidth;     p[2].y=p[1].y;
		 p[3].x=p[2].x;                 p[3].y=top+100;
         p[4].x=p[3].x-0.5*BlackKeyWidth; p[4].y=p[3].y;		 
		 p[5].x=p[4].x;                 p[5].y=top+5;
		 p[6].x=left+0.5*BlackKeyWidth; p[6].y=p[5].y;
		 p[7].x=p[6].x;                 p[7].y=top+100;
		 Polygon(hdc,p,8);
	  }
   }
   SelectObject(hdc,hOldPen);
   SelectObject(hdc, hOldBrush);
   if(hdcext==NULL) ReleaseDC(hwnd,hdc);
   return;
}

void paintkeypiano(int notekey,int estado,int oneoctava,HBRUSH hbr)
{
   int r,l0,note;

// pinta la tecla que corresponde a notekey si oneoctava=0
   if(oneoctava==0){ 
      paintkeypiano1(notekey,estado,hbr,hdc);
	  return;
   } 
// pinta todas las teclas que se mapean a la misma nota 
   l0=concfg.noteinfpaintkey[1]+notekey-12*(notekey/12);  
   for(r=0;r<5;r++){
      note=l0+12*r;
	  if(note>concfg.notesuppaintkey[0]+1) continue;
	  if(note<concfg.noteinfpaintkey[1]) continue;
      paintkeypiano1(note,estado,hbr,hdc);
   }
   return;
}

void paintearnotereadsolution(int tipo,int tim,HBRUSH hbr)
{
	if(tipo==0){       // oido relativo
      paintkeypiano(RandomNote[0].notakey,1,0,hbr);
      paintkeypiano(RandomNote[1].notakey,1,0,hbr);
      Sleep(tim);
      paintkeypiano(RandomNote[0].notakey,0,0,hbr);
      paintkeypiano(RandomNote[1].notakey,0,0,hbr);
      removemousemsgqueue();
   }
	if(tipo==1){        // oido absoluto
      int numstaff=gfstat[0]+2*gfstat[1];
	  Paintnotereading(&RandomNote[0],1,numstaff,concfg.armaduranotereading,1);
	  paintkeypiano(RandomNote[0].notakey,1,concfg.earoctava,hbr);
	  Sleep(tim);
	  paintkeypiano(RandomNote[0].notakey,0,concfg.earoctava,hbr);
	  removemousemsgqueue();
   }
   return;
}

//SUBROUTINES************************************************************
// Parametro source
// 
// 0 no source o generacion interna
// 1 keyboard
// 2 mouse
// 3 midi
// 4 micro
//***********************************************************************
void DrawKey(int source)
{
	static clock_t downtime; 
	clock_t uptime;
	if(helpexercise==TRUE) return;
	hdc = GetDC(hwnd);
	hOldPen = SelectObject(hdc, hPen);
	hOldFont = SelectObject(hdc, hMaestroFont);	

    if(fromkeydown == FALSE){		
	   if(showvirstaff==1){
	      //BitBlt(hdc, 0, top260, rect.right, rect.bottom, hdcMem1, 0, top260, SRCCOPY);
	      //BitBlt(hdc, rect.right-215, top260-45, rect.right, rect.bottom, hdcMem1, rect.right-215, top260-45, SRCCOPY);
	      
		  BitBlt(hdc, 0, top260, rect.right,top-top260, hdcMem1, 0, top260, SRCCOPY);
	      BitBlt(hdc, rect.right-215, top260-45, rect.right,top-top260, hdcMem1, rect.right-215, top260-45, SRCCOPY);

	   }else{
		  //BitBlt(hdc, rect.right-215,topvirstaff, rect.right, rect.bottom, hdcMem1, rect.right-215,topvirstaff, SRCCOPY);	   
		  BitBlt(hdc, rect.right-215,topvirstaff, rect.right,20, hdcMem1, rect.right-215,topvirstaff, SRCCOPY);	   
	   }
	   if(Note>=concfg.noteinfgeneric[1] && Note<=concfg.notesupgeneric[0]){
	      if(showtest6){
	         int l0;

		     if(concfg.memoespacial==1  ) l0=Note-12*(Note/12);
			 else l0=Note;
		     memoespacialupdate(l0);		 
			 initmemoespacial(1);
		  }
		  if(showtest3){ 
			 pernoteread=0;
			 if(micentry==0){				
				if(earnoteread==1){
				   int l0,l1;
				   l0=Note-12*(Note/12);
				   l1=RandomNote[0].notakey-12*(RandomNote[0].notakey/12);
				   if(earinterval==1){
				      if(Note==RandomNote[1].notakey){
					     notereading(-3,1);
					     paintearnotereadsolution(0,2500,hGreenBrush);
					  }else{ 
					     notereading(-3,0);
					     paintearnotereadsolution(0,2500,hRedBrush);
					  }
                      notereading(0,0);
				   }else{
					  if((l0==l1)&&(concfg.earoctava==1)){
                         notereading(-3,1);
						 paintearnotereadsolution(1,2500,hGreenBrush);
						 notereading(0,0);
					  }else{
                         notereading(-3,0);
						 if(concfg.stoponerror==1){ 
					        notereadingfail(Note,0,0);
						 }else{
                            paintearnotereadsolution(1,2000,hRedBrush);
							notereading(0,0);
						 }
					  }
				   }
				}else{  // entrada sin micro y sin entreno de oido earnoteread=0				   
				   int r,oneok;
				   unsigned int t;
				   static int notereadlista[32],notereadallok,notereadtot;				   

				   for(r=t=0;r<randomcannote;r++)
				      t+=(unsigned int) RandomNote[r].color;
				   if(t==0)
					  for(r=notereadallok=notereadtot=0;r<32;r++) notereadlista[r]=0;				      
				   ++notereadtot;
				   oneok=-1;
				   for(r=0;r<randomcannote;r++){
					  if(concfg.notereadingchord==1){	
			             if(RandomNote[r].notakey==Note){ 
						    if(notereadlista[r]==0){oneok=r; ++notereadallok;}
						    notereadlista[r]=Note;
					        break;
						 }
					  }else{
			             if(RandomNote[r].notakey==Note && r==notereadtot-1){ 
						    oneok=r; 
							++notereadallok;
					        break;
						 }
					  }
				   }
				   if(oneok>=0)
				      RandomNote[oneok].color=GREEN;
				   //if(Note==RandomNote[0].notakey){
				   if(notereadallok==randomcannote){
					  double fig[]={4,2,1,0.5,0.25};
					  uptime=clock();
					  for(r=NOTEREADRILE-1;r>=1;r--){					     
					     notereadinglist[r].tdura=notereadinglist[r-1].tdura;
						 notereadinglist[r].etime=notereadinglist[r-1].etime;
						 notereadinglist[r].itime=notereadinglist[r-1].itime;
					  }
					  notereadinglist[0].tdura=0.25*fig[(int)RandomNote[0].dura];
					  notereadinglist[0].etime=uptime;
					  notereadinglist[0].itime=downtime;					  
					  notereadingupdate();
					  notereading(-3,1);
					  Sleep(300);
					  notereading(0,0);
					  for(r=notereadallok=notereadtot=0;r<32;r++) notereadlista[r]=0;
				   }else{
					  if(concfg.notereadingchord==0){
                         for(r=notereadtot;r<=notereadingmemo;r++)
						    RandomNote[r].color=-1;
					  }
					  if(concfg.notereadingchord==1 && concfg.notereadingmemo!=0){
						 for(r=0;r<randomcannote;r++){
                            if(notereadlista[r]==0) RandomNote[r].color=-1;
						 }
					  }
					  if(oneok<0){
						 if(concfg.notereadingchord==1){
							if(concfg.notereadingmemo==0){
						       for(r=0;r<randomcannote;r++){
                                  if(notereadlista[r]==0)
						             RandomNote[r].color=RED;
							   }
							}
						 }else{
							if(concfg.notereadingmemo==0) 
							   RandomNote[notereadtot-1].color=RED;
						 }
					  }
				      notereadingupdate();
					  if(concfg.stoponerror==1 && oneok==-1){ 
						   notereadingfail(Note,notereadtot-1,1);
						   --notereadtot;
					  }else{
				         if(notereadtot==randomcannote || oneok==-1){
					        notereading(-3,0);
					        for(r=notereadallok=notereadtot=0;r<32;r++) notereadlista[r]=0;
						    Sleep(300);
					        notereading(0,0);
						 }
					  }
				   }
				}
			 }
			 if(source==4 && micentry==1){
				int l0,l1;
				if(concfg.tiposolfeo==0){
				   if(concfg.singoctava==1){ 
				      l0=Note-12*(Note/12);
				      l1=RandomNote[0].notakey-12*(RandomNote[0].notakey/12);
		              if(l0==l1){
			             notereading(-3,1);
			             notereading(0,0);
					  }else{ 
					     notereading(-3,0);
					     if(concfg.stoponerror==1) notereading(1,0);
					     else notereading(0,0);
					  }
				   }else{  
		              if(Note == RandomNote[0].notakey){					  
			             notereading(-3,1);
			             notereading(0,0);
					  }else{ 
					     notereading(-3,0);
					     if(concfg.stoponerror==1) notereading(1,0);
					    else notereading(0,0);
					  }
				   }
				}else{  // si solfeo hablado
				   l0=Note-12*(Note/12);
				   l1=RandomNote[0].nota-12*(RandomNote[0].nota/12);
		           if(l0==l1){
			          notereading(-3,1);
			          notereading(0,0);
				   }else{ 
				      notereading(-3,0);
				      if(concfg.stoponerror==1) notereading(1,0);
				      else notereading(0,0);					  
				   }
				}
			 }
		  }
	   }
	}
	if(fromkeydown == TRUE)
	   downtime=clock();
	if(Note>=concfg.noteinfrandommusic[1] && Note<=concfg.notesuprandommusic[0]){	   
       if(showtest1){ // random music test
		  if(source==4){
		     if(micentry==1){  // si tenemos micro activo solo permitimos source de micro
	            IntervalTestUpdate(Note,source,0);     
			    //********************18-07*****************
			    ajustatiempoenties();
			    //******************************************
				paintrandommusic(melodia);    //ojo
	            //IntervalTestUpdate1(melodia);   // lo incluimos dentro de interval update
			 }
		  }
		  if(micentry==0){   // si no hay micro podemos entrar por teclado, raton o midi 			    
	         IntervalTestUpdate(Note,source,0); 
			 //********************18-07*****************
			 ajustatiempoenties();
			 //******************************************
			 paintrandommusic(melodia);    //ojo
	         //IntervalTestUpdate1(melodia);  // lo incluimos dentro de interval update
		  }
	   }
	}
    if(Note>=concfg.noteinfgeneric[1] && Note<=concfg.notesupgeneric[0]){	   
       if(showtest2){ 
		  if(micentry==1 && source==4){  // si tenemos micro activo solo permitimos micro
	         IntervalTestUpdate(Note,source,0);     
	         IntervalTestUpdate2(melodia);
		  }
		  if(micentry==0){   // si no hay micro podemos entrar con teclado, raton o midi
	         IntervalTestUpdate(Note,source,0);     
	         IntervalTestUpdate2(melodia);
		  }
	   }
	}
	if(ScanCode<0 || Note>96){        // si esta fuera de teclado no lo pintes
	   SelectObject(hdc, hOldPen);
	   SelectObject(hdc, hOldFont);
	   ReleaseDC(hwnd, hdc);
       return;
	}
	left = (xKey[ScanCode] * WhiteKeyWidth) + ExtraSpace;
	saveleft = left;
	BracketingKeys = yKey[ScanCode];
	if (BracketingKeys == NEITHER)
	{//black keys
		if (fromkeydown)
		{			
			//*********************************
			// pinta notas con accidentes en el teclado virtual
			//*********************************
            if(showvirstaff==TRUE){
			   SetTextColor(hdc,0xFF0000);
			   SelectObject(hdc, hFont);
			   SetBkMode(hdc, TRANSPARENT);
			   if (accidental == 2)
				  //TextOut(hdc, left + (WhiteKeyWidth-Widths[xKey[ScanCode]%7])/2, top-139, &CDEFGAB[xKey[ScanCode]%7], 1);
			      TextOut(hdc, left + WhiteKeyWidth*0.65, top-139, &CDEFGAB[xKey[ScanCode]%7], 1);			   
			   else//if (accidental == 1)
				  if (ScanCode != 3) // not middle C
				  //TextOut(hdc, left - (WhiteKeyWidth+Widths[(xKey[ScanCode]-1)%7])/2, top-139, &CDEFGAB[(xKey[ScanCode]-1)%7], 1);
					  TextOut(hdc, left - WhiteKeyWidth*0.65, top-139, &CDEFGAB[(xKey[ScanCode]-1)%7], 1);
			   SelectObject(hdc, hMaestroFont);
			   if (accidental == 1)
				  TextOut(hdc, left-(WhiteKeyWidth/3)-26, yStaff[Note-36], "#w", 2);
			   else
			      TextOut(hdc, left-(WhiteKeyWidth/3)+5, yStaff[Note-35], "bw", 2);
			}
		    SetBkMode(hdc, OPAQUE);
			SelectObject(hdc, hPinkBrush);
			//******************************************************************
		}
		else SelectObject(hdc, hBlackBrush);
		SetTextColor(hdc, 0);//black
		left -= WhiteKeyWidth/3;
		Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);   
		SelectObject(hdc, hOldBrush);
		if (fromkeydown){
			if(showvirstaff){
			   if (accidental == 1){
			      switch (xKey[ScanCode]){
				     case 15://middle C#
					    MoveToEx(hdc, left-BlackKeyWidth, top-122, NULL);//-1
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-122);//+29
					    break;
				     case 1://low C#
					    MoveToEx(hdc, left-BlackKeyWidth, top-28, NULL);
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-28);
				     case 2://low D#
				  	    MoveToEx(hdc, left-BlackKeyWidth, top-40, NULL);
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-40);
					    break;
				     case 33://very high G#
				     case 34://very high A#
					    MoveToEx(hdc, left-BlackKeyWidth, top-238, NULL);
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-238);
				     case 32://very high F#
					    MoveToEx(hdc, left-BlackKeyWidth, top-226, NULL);
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-226);
				     case 30://high C#
				     case 29://high D#
					    MoveToEx(hdc, left-BlackKeyWidth, top-214, NULL);
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-214);
				     case 27://high A#
					    MoveToEx(hdc, left-BlackKeyWidth, top-202, NULL);
					    LineTo(hdc, left+WhiteKeyWidth-BlackKeyWidth, top-202);
					    break;
				  }
			   }
			   else{
			      switch (Note){
					 case 37://low Db
					 case 39://low Eb
						MoveToEx(hdc, left, top-40, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-40);
						break;
						MoveToEx(hdc, left, top-202, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-202);
						break;
					 case 94://very high Bb
						MoveToEx(hdc, left, top-250, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-250);
					 case 92://very high Ab
					 case 90://high Gb
						MoveToEx(hdc, left, top-238, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-238);
					 case 87://high Db
						MoveToEx(hdc, left, top-226, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-226);
					 case 85://high Cb
						MoveToEx(hdc, left, top-214, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-214);
					 case 80://high Ab
					 case 82://high Bb
						MoveToEx(hdc, left, top-202, NULL);
						LineTo(hdc, left+WhiteKeyWidth, top-202);
						break;

				  }
			   }
			}
		}
	}	
	else if (BracketingKeys != -1)
	{//white keys
		if (fromkeydown)
		{
			//*******************************************************
            // pinta notas en el staff del teclado virtual
			//*******************************************************
            if(showvirstaff==TRUE){
			   SetTextColor(hdc,0xFF0000);
			   SelectObject(hdc,hfontsystemg);
			   SetBkMode(hdc, TRANSPARENT);
			   if (ScanCode != 16) // not middle C
				//TextOut(hdc, left + (WhiteKeyWidth-Widths[xKey[ScanCode]%7])/2, top-139, &CDEFGAB[xKey[ScanCode]%7], 1);
			      TextOut(hdc, left + WhiteKeyWidth*0.35, top-139, &CDEFGAB[xKey[ScanCode]%7], 1);
			   SetBkMode(hdc, OPAQUE);
			   SetTextColor(hdc, 0);
			   SelectObject(hdc, hMaestroFont);
			   SetBkMode(hdc, TRANSPARENT);
			   SetTextColor(hdc, 0xFF0000);
			   TextOut(hdc, left + WhiteKeyWidth*0.18, yStaff[Note-36], "w", 1);   
			}
			//*******************************************************************
			SelectObject(hdc,hMaestroFont);
			SetTextColor(hdc, 0);
			if(showvirstaff){
			   switch (xKey[ScanCode]){
			   case 14://middle C
				  MoveToEx(hdc, left, top-122, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-122);
				  break;
			   case 0://low C
				  MoveToEx(hdc, left, top-28, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-28);
				  MoveToEx(hdc, left, top-40, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-40);
				  break;
			   case 1://low D
			   case 2://low E
				  MoveToEx(hdc, left, top-40, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-40);
				  break;
			   case 34://very high B
				  MoveToEx(hdc, left, top-250, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-250);
			   case 32://very high G
			   case 33://very high A
				  MoveToEx(hdc, left, top-238, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-238);
			   case 30://high E
			   case 31://high F
				  MoveToEx(hdc, left, top-226, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-226);
			   case 28://high C
			   case 29://high D
				  MoveToEx(hdc, left, top-214, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-214);
			   case 26://high A
			   case 27://high B
				  MoveToEx(hdc, left, top-202, NULL);
				  LineTo(hdc, left+WhiteKeyWidth, top-202);
				  break;
			   }
			}
			SetBkMode(hdc, OPAQUE);
			SelectObject(hdc, hOldFont);
			SelectObject(hdc, hPinkBrush);  //Color pushing piano keys
		}
		Rectangle(hdc, left, top+5, left+WhiteKeyWidth, top+150);   
		if (!fromkeydown)   
		{
			SelectObject(hdc, hWhiteBrush);            
			Rectangle(hdc, left, top+5, left+WhiteKeyWidth, top+150);   
            SetBkColor(hdc,BACKGROUND);
			if(shownamekeys==TRUE){
			   SetTextColor(hdc,0x8080FF);
			   SelectObject(hdc,hfontsystemg);
			   x = xKey[ScanCode];
			   TextOut(hdc, (x*WhiteKeyWidth) + (WhiteKeyWidth-Widths[x%7])/2, top+110, &CDEFGAB[x % 7], 1);
			}
 		    paintkey((14*WhiteKeyWidth) + ExtraSpace + (WhiteKeyWidth*0.35),top+125);			
			SelectObject(hdc, hBlackBrush);
		}
		if (PreviousNote != (Note-1))
			SelectObject(hdc, hBlackBrush);
		//else
		//	SelectObject(hdc, hPinkBrush);
		if (BracketingKeys == LEFT)
		{
			left -= (WhiteKeyWidth/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);  
		}
		else if (BracketingKeys == RIGHT)
		{
			left += (WhiteKeyWidth*2/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
		}
		else if (BracketingKeys == BOTH)
		{
			left -= (WhiteKeyWidth/3);
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
			left += WhiteKeyWidth;
			Rectangle(hdc, left, top+5, left+BlackKeyWidth, top+100);
		}
		SelectObject(hdc, hOldBrush);
	}
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldFont);
	ReleaseDC(hwnd, hdc);
}

void StartShowAndPlay(int source)
{
	midiOutShortMsg(hMidiOut, 0x90 | (Velocity << 16) | (Note << 8));
	fromkeydown = TRUE;
	if ((Note >= 12) && (Note <= 100))	
	{
		DrawKey(source);
		for (x = 0; (x < 40) && (ScanCodes[x] != 0); x++)
			;
		if (x < 40)
			ScanCodes[x] = ScanCode;
	}
}

void StartNote(int source)
{//play note
	if (showscale)//All Notes in Scale
	{
		saveAccidental = accidental;
		if ((SavedNote == 41) || (SavedNote == 53) || (SavedNote == 65) || (SavedNote == 77) || (SavedNote == 89))
			accidental = 2;//Key of F
		else if ((SavedNote != 42) && (SavedNote != 54) && (SavedNote != 66) && (SavedNote != 78) && (SavedNote != 90))
		{
			accidental = 1;
			for (x = 0; x < 25; x++)
			{
				if (SavedNote == BlackKeyNotes[x])
				{
					accidental = 2;//if it's a flat key
					break;
				}
			}
		}
		for (x = 0; x < 7; x++)
		{
			Note = SavedNote + Interval[x];
			ScanCode = Codes[Note-36];
			fromkeydown = TRUE;
			if ((Note >= 36) && (Note <= 100))
				DrawKey(source);
			PreviousNote = Note;
		}
		PreviousNote = -1;
		accidental = saveAccidental;
		return;
	}

//	if ((showtest) && (SavedNote == 95))
//		SendMessage(hwnd, WM_COMMAND, IDM_TEST, 0);
	Note = SavedNote;
	StartShowAndPlay(source);
	if (ChordType == 1)//Major Triad
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 2)//Minor Triad
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 3)//Diminished Triad
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 4)//Augmented Triad
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 8;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 5)//Dominant 7th
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					StartShowAndPlay(source);
				}
			}
		}
	}
	else if (ChordType == 6)//Major 7th
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
				Note = SavedNote + 11;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					StartShowAndPlay(source);
				}
			}
		}
	}
	else if (ChordType == 7)//Minor 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					StartShowAndPlay(source);
				}
			}
		}
	}
	else if (ChordType == 8)//Diminished 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			StartShowAndPlay(source);
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				StartShowAndPlay(source);
				Note = SavedNote + 9;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					StartShowAndPlay(source);
				}
			}
		}
	}
}


void EndShowAndPlay(int source)
{
	midiOutShortMsg(hMidiOut, 0x90 | (Note << 8));//0 Velocity
	fromkeydown = FALSE;
	if ((Note >= 12) && (Note <= 100))
		DrawKey(source);
	for (x = 0; (x < 40) && (ScanCodes[x] != ScanCode); x++)
		;
	if (x < 40)
		ScanCodes[x] = 0;
	else
		for (x = 0; x < 40; x++)
			ScanCodes[x] = 0;
/*
		fromkeydown = TRUE;
	if (ChordType == 0)
	{
		for (x = 0; x < 40; x++)
		{
			if (ScanCodes[x] != 0)
			{
				ScanCode = ScanCodes[x];
				Note = Notes[ScanCode];
				DrawKey();//doesn't use x
			}
		}
	}
	*/
}

void EndNote(int source)
{
	if (showscale)//All Notes in Scale
	{
		saveAccidental = accidental;
		accidental = 1;
		for (x = 0; x < 25; x++)
			if (SavedNote == BlackKeyNotes[x])
			{
				accidental = 2;//if it's a flat key
				break;
			}
		for (x = 0; x < 7; x++)
		{
			Note = SavedNote + Interval[x];
			ScanCode = Codes[Note-36];
			fromkeydown = FALSE;
			if (Note <= 100)
				DrawKey(source);
			PreviousNote = Note;
		}
		PreviousNote = -1;
		accidental = saveAccidental;
		return;
	}

	Note = SavedNote;
	EndShowAndPlay(source);
	if (ChordType == 1)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 2)
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 3)
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 4)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 8;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
			}
		}
	}
	else if (ChordType == 5)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					EndShowAndPlay(source);
				}
			}
		}
	}
	else if (ChordType == 6)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
				Note = SavedNote + 11;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					EndShowAndPlay(source);
				}
			}
		}
	}
	else if (ChordType == 7)//Minor 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					EndShowAndPlay(source);
				}
			}
		}
	}
	else if (ChordType == 8)//Diminished 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 100)
		{
			ScanCode = Codes[Note-36];
			EndShowAndPlay(source);
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 100)
			{
				ScanCode = Codes[Note-36];
				EndShowAndPlay(source);
				Note = SavedNote + 9;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 100)
				{
					ScanCode = Codes[Note-36];
					EndShowAndPlay(source);
				}
			}
		}
	}
}

void CALLBACK MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{//from midiInOpen (if a MIDI keyboard is attached)
	if (wMsg == MIM_DATA)
		PostMessage(hwnd, WM_USER, (WPARAM)dwParam1 & 0xFF, (LPARAM)(dwParam1 >> 8) & 0xFFFF);//dwParam1 contains velocity, note, and status bytes
}

int Atoi(char *ptr)
{
	int x;

	for (x = 0; (*ptr >= '0') && (*ptr <= '9'); ptr++)
	{
		x *= 10;
		x += *ptr - '0';
	}
	return x;
}

double uniform_deviate(int seed)
{
	return seed * (1.0 / (RAND_MAX + 1.0));
}

void nripaintautomaticbutton(HDC hdcmem,int key,int nota,int acci,COLORREF color)
{
	int ntinterline;
	int r,k,offset;
    int RandomNoteLoc;
	RECT backp;

	offset=8;
	ntinterline=interline;
	backp.top=backp.left=0;
    backp.right=backp.bottom=100;
	FillRect(hdcmem, &backp,hWhiteBrush);
	SetBkMode(hdcmem, TRANSPARENT);
	SetTextColor(hdcmem,color); //BLACK);
	SelectObject(hdcmem, hMaestroFontSmall3);
	paintstaff(hdcmem,0,5+offset,3); // staff usando lineas
	//TextOut(hdcmem,0,5+offset, "===",3);	
	SelectObject(hdcmem, hMaestroFontSmall1);
	if(key==0){
	   TextOut(hdcmem,0,0-ntinterline+10+offset, "&", 1);
	}else{
	   TextOut(hdcmem,0,0-3*ntinterline+4+offset, "?", 1);
	}
    r=key;
	k=calcnoteindex(nota);
    if(r==1 && nota!=0){ 
	   RandomNoteLoc =5-(k-4)*ntinterline*0.5+offset;
	   SetTextColor(hdcmem,BLACK);
	   if(nota <= 36)
	      TextOut(hdcmem,StaffWidth[1],5+2*ntinterline+offset, "___", 2);
       if(nota <= 40) 
		  TextOut(hdcmem,StaffWidth[1],5+ntinterline+offset, "___", 2);
	   if(nota >= 60)
	      TextOut(hdcmem,StaffWidth[1],5-5*ntinterline+offset, "___", 2);
	   if(nota == 64)				
	      TextOut(hdcmem,StaffWidth[1],5-6*ntinterline+offset, "___", 2);
	}
	if(r==0 && nota!=0){
	   SetTextColor(hdcmem,BLACK);
	   RandomNoteLoc =5-(k-16)*ntinterline*0.5+offset; 
	   if(nota <= 57)
	      TextOut(hdcmem,StaffWidth[1],5+2*ntinterline+offset, "___", 2);
	   if(nota <= 60) 
	      TextOut(hdcmem,StaffWidth[1],5+ntinterline+offset, "___", 2);
	   if(nota >= 81)
	      TextOut(hdcmem,StaffWidth[1],5-5*ntinterline+offset, "___", 2);
	   if(nota >= 84)
	      TextOut(hdcmem,StaffWidth[1],5-6*ntinterline+offset, "___", 2);			 			 					 
	}	   // dibuja notas musicales			 
	SetTextColor(hdcmem,BLACK);
    if(acci==0){				
	   TextOut(hdcmem,(StaffWidth[1]+(NoteWidth[1]/2)),RandomNoteLoc,"w",1);
	}
    if(acci==1){				 
	   TextOut(hdcmem,(StaffWidth[1]+(NoteWidth[1]/2))-5,RandomNoteLoc,"#w",2); // sostenido
	}
	if(acci==2){
	   TextOut(hdcmem,(StaffWidth[1]+(NoteWidth[1]/2))-5,RandomNoteLoc,"bw",2);  // bemol
	}	
	return;
}


HBITMAP nriautomaticbitmaps(int key,int nota,int acci,COLORREF color)
{

    HDC hdcmem;   
    HBITMAP hbmmem;
	HGDIOBJ hold;
	int offset=0;
	double facx,facy;

	facx=currenthor*1.5/1024;
	facy=currentver*1.25/768;
    hdcmem = CreateCompatibleDC(hdc);
    hbmmem = CreateCompatibleBitmap(hdc,50*facx,60*facy);  //testRect.Bottom
    hold=SelectObject(hdcmem, hbmmem);
    nripaintautomaticbutton(hdcmem,key,nota,acci,color);
	SelectObject(hdcmem,hold);
    DeleteDC(hdcmem);
	return hbmmem;	
}

void NotereadInverseButtonshideshow(int hs) 
{
   int r;
   int cmdsh;

   if(hs==1) cmdsh=SW_HIDE;
   else cmdsh=SW_SHOW;
   for(r=0;r<16;r++){
	  ShowWindow(autobutton[r],cmdsh);
   }
   return;
}

void NotereadInverseCreateButtonsremove(void) 
{
   int r;

   for(r=0;r<16;r++){
      DeleteObject(bmpauto[r]);
	  DestroyWindow(autobutton[r]);
      autobutton[r]=NULL;
	  bmpauto[r]=NULL;
   }
   return;
}


int createkiz(NOTA *listkiz)
{
	int r,s,acci,an,j,y;
	double facx,facy;
	NOTA *h;
	
	facx=currenthor*1.5/1024;
	facy=currentver*1.25/768;
	s=(int)(uniform_deviate(rand()) * 16);
	for(r=0;r<16;r++){
	   do{
          while(1){
	         h=genrndnote();  // random uniforme
	         if(gfstat[0]==1){
				 if(h->penta!=0) continue;
	             if(h->nota<concfg.noteinfgeneric[0] || h->nota>concfg.notesupgeneric[0]) continue;
				 break;
			 }
			 if(gfstat[1]==1){
				 if(h->penta!=1) continue;
	             if(h->nota<concfg.noteinfgeneric[1] || h->nota>concfg.notesupgeneric[1]) continue;
				 break;
			 }
             break;
		  }
		  listkiz[r].nota=h->nota;
		  listkiz[r].penta=h->penta;
		  if(concfg.notereadingacci==1) acci=h->acci;
		  else acci=h->acci=0;
          listkiz[r].acci=h->acci;
	      acci=3-2*acci; if(acci==3) acci=0;
          listkiz[r].notakey=h->nota+acci;
		  for(j=0;j<r;j++)
		     if(listkiz[r].notakey==listkiz[j].notakey) break;
		  if(j<r){ --r; continue; }
	   }while (listkiz[r].notakey<concfg.noteinfgeneric[1] || 
			   listkiz[r].notakey>concfg.notesupgeneric[0]);
	}
	an=top260min30; y=40*facy;
	for(r=0;r<16;r++,an+=(55+ExtraSpace)*facx){
	   j=calcnoteindex(listkiz[r].nota);
	   bmpauto[r]=nriautomaticbitmaps(listkiz[r].penta,notas00[j],listkiz[r].acci,BLACK);
	   if(r>7) y=120*facy; 
	   if(r==8) an=top260min30;
	   autobutton[r]= CreateWindow("BUTTON",NULL,
					    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						an,y,55*facx,70*facy,
						hwnd, NULL, hInst, NULL);
       SendMessage(autobutton[r], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpauto[r]);

	}
	paintkeypiano(listkiz[s].notakey,1,0,hGreenBrush);
	PlayNotaMidi((__int64) listkiz[s].notakey,0);    // toca la nota pero no la pintes en el piano
	return s;
}
	

void NotereadInverse(HBITMAP respkiz,int paint)
{
	static NOTA listkiz[17];
	static int sol;

	if(paint==-1){ sol=0; return; }
	hdc=GetDC(hwnd);
    if(paint==1){
	   paintscores(&notereadinversescore);
	   if(sol!=0) paintkeypiano(listkiz[sol].notakey,1,0,hGreenBrush);
	   ReleaseDC(hwnd,hdc);
	   return;
	}	   
	if(respkiz==NULL){ 
	   notereadinversescore.hits=notereadinversescore.fails=0;
	   notereadinversescore.perhits=notereadinversescore.perfails=0;
       sol=createkiz(listkiz); 
	   paintscores(&notereadinversescore);
	   ReleaseDC(hwnd,hdc);
	   return;
	} 
	if(respkiz==bmpauto[sol]){ 
	   paintkeypiano(listkiz[sol].notakey,0,0,hGreenBrush);
       NotereadInverseCreateButtonsremove();
	   sol=createkiz(listkiz);
	   ++notereadinversescore.hits;
	}else ++notereadinversescore.fails;
    notereadinversescore.perhits=100.0*notereadinversescore.hits/(notereadinversescore.hits+notereadinversescore.fails);
	notereadinversescore.perfails=100.0*notereadinversescore.fails/(notereadinversescore.hits+notereadinversescore.fails);
	paintscores(&notereadinversescore);
	ReleaseDC(hwnd,hdc);
	return;
}


void FlashCardButtonshideshow(int hs) 
{
   int x;
   int cmdsh;

   if(hs==1) cmdsh=SW_HIDE;
   else cmdsh=SW_SHOW;
   for (x = 0; x < 12; x++){
      ShowWindow(hwndnumeric[x],cmdsh);
   }
   return;
}

void FlashCardCreateButtonsremove(void) 
{
   int x;

   for (x = 0; x < 12; x++){
      DestroyWindow(hwndnumeric[x]);
	  hwndnumeric[x] = NULL;
   }
   return;
}

void FlashCardCreateButtons(void)
{

	int x,posx,fil,col;
	int orgf,orgs;
	char buff[10];
    
	orgs=gStaff[64-36]-295;
	orgf=fStaff[43-36]-240;	
	posx=1.5*StaffWidth[0];
	for (x = 0; x < 12; x++){
	   sprintf(buff,"%d",x+1);
	   fil=x/4; col=x%4;
	   hwndnumeric[x] = CreateWindow("BUTTON",buff,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DLGFRAME,
						top260min30+350+posx+40+col*45,orgs+(fil+0.5)*40,45,30,
						hwnd, NULL, hInst, NULL);
	   
		//				top260min30+125+posx +40+col*45,orgf+(fil+0.5)*30, 45,30,
	//					hwnd, NULL, hInst, NULL);
	}
}

void NoteReadingButtonshideshow(int hs) 
{
   int x,cmdsh;
   
   if(hs==1) cmdsh=SW_HIDE;
   else cmdsh=SW_SHOW;

   for (x = 0; x < 21; x++){
      ShowWindow(hwndnumeric[x],cmdsh);
   }
   ShowWindow(buttonear,cmdsh);
   ShowWindow(buttonrepe,cmdsh);
   ShowWindow(hwndmetronoteread,cmdsh);
   ShowWindow(hwndmetronotereadtitle,cmdsh);
   ShowWindow(hwndmetronotereadvalue,cmdsh);

   return;
}

void NoteReadingCreateButtonsremove(void) 
{
   int x;

   for (x = 0; x < 21; x++){
      DestroyWindow(hwndnumeric[x]);
	  hwndnumeric[x] = NULL;
   }
   DestroyWindow(buttonear);
   DestroyWindow(buttonrepe);
   DestroyWindow(hwndmetronoteread);
   DestroyWindow(hwndmetronotereadtitle);
   DestroyWindow(hwndmetronotereadvalue);
   hwndmetronoteread=NULL;
   return;
}

void NoteIntervalCreateButtonsremove(void) 
{

   DestroyWindow(hwndmetronoteinterval);
   DestroyWindow(hwndmetronoteintervaltitle);
   DestroyWindow(hwndmetronoteintervalvalue);
   hwndmetronoteinterval=NULL;
   return;
}

void NoteReadingCreateButtons(void)
{

	int x,posx,fil,col;
	int orgs;
	char buff0[7][5]={"Do","Re","Mi","Fa","Sol","La","Si"};
    char buff1[7][5]={"Do#","Re#","Mi#","Fa#","Sol#","La#","Si#"};
	char buff2[7][5]={"Dob","Reb","Mib","Fab","Solb","Lab","Sib"};
	void *bb[3]={buff0,buff1,buff2};

	orgs=gStaff[64-36]-295;
	posx=1.5*StaffWidth[0];
	for (x = 0; x < 21; x++){
	   fil=x/7; col=x%7;
	   hwndnumeric[x] = CreateWindow("BUTTON",(char *) ((char *) bb[fil]+5*col),
					    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						top260min30+310+posx+40+col*45,orgs+(fil+0.5)*40,45,30,
						hwnd, NULL, hInst, NULL);
	}
	buttonear = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						820,6,20,20,
						hwnd, NULL, hInst, NULL);
    SendMessage(buttonear, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpearn);
	buttonrepe = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						840,6,20,20,
						hwnd, NULL, hInst, NULL);
    SendMessage(buttonrepe, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmprepe);

	hwndmetronoteread = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|TBS_NOTICKS|WS_VISIBLE,
								714,30,80,30,hwnd,NULL,hInst,NULL);	
	SendMessage(hwndmetronoteread,TBM_SETRANGE,true,MAKELONG(0,200));
	SendMessage(hwndmetronoteread,TBM_SETTICFREQ,2,0);
	SendMessage(hwndmetronoteread,TBM_SETPOS,true,concfg.metronoteread);
	hwndmetronotereadtitle = CreateWindow("static","Tempo:",WS_CHILD|WS_VISIBLE,674,30,40,20,hwnd,NULL,hInst,NULL);
	SendMessage(hwndmetronotereadtitle,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));
	hwndmetronotereadvalue = CreateWindow("static","",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW,794,30,20,20,hwnd,NULL,hInst,NULL);	
	SendMessage(hwnd,WM_HSCROLL,0,(LPARAM) hwndmetronoteread);//starting values
	return;
}

void NoteIntervalCreateSlidingControls(void)
{
	hwndmetronoteinterval = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|TBS_NOTICKS|WS_VISIBLE,
								714,30,80,30,hwnd,NULL,hInst,NULL);	
	SendMessage(hwndmetronoteinterval,TBM_SETRANGE,true,MAKELONG(0,200));
	SendMessage(hwndmetronoteinterval,TBM_SETTICFREQ,2,0);
	SendMessage(hwndmetronoteinterval,TBM_SETPOS,true,concfg.metronoteinterval);
	hwndmetronoteintervaltitle = CreateWindow("static","Tempo:",WS_CHILD|WS_VISIBLE,674,30,40,20,hwnd,NULL,hInst,NULL);
	SendMessage(hwndmetronoteintervaltitle,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));
	hwndmetronoteintervalvalue = CreateWindow("static","",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW,794,30,20,20,hwnd,NULL,hInst,NULL);	
	SendMessage(hwnd,WM_HSCROLL,0,(LPARAM) hwndmetronoteinterval);//starting values
	return;
}

void FlashCardNoteInterval(void)
{
	melodia=(MELODIA *) new MELODIA;
	melodia->penta[0].compas=melodia->penta[1].compas=NULL;
	melodia->randommodegenerationmusic=0;
    melodia->armadura=0;
	melodia->numsignature=melodia->densignature=1;
	melodia->fingering=0;
	melodia->metronomoreal=-1;
	melodia->noteinf[0]=57; melodia->notesup[0]=85; // de momento el SPAn es FIJO
	melodia->noteinf[1]=36; melodia->notesup[1]=64;
	melodia->numcompases=400;
	resetscores(melodia);
	melodia->penta[0].allsilencio=0;
	melodia->penta[1].allsilencio=0;
	melodia->penta[0].compas=(COMPAS *) new COMPAS[melodia->numcompases];
    melodia->penta[1].compas=(COMPAS *) new COMPAS[melodia->numcompases];
    melodia->penta[0].maxintervalo=concfg.maxintervalo_flashnoteinterval;
    melodia->penta[1].maxintervalo=concfg.maxintervalo_flashnoteinterval;
	melodia->penta[0].minintervalo=concfg.minintervalo_flashnoteinterval;
    melodia->penta[1].minintervalo=concfg.minintervalo_flashnoteinterval;    
	melodia->repeatg=melodia->repeatf=0;
	melodia->silencios=0;
	strcpy(melodia->ritmo.bloqueritmico[0].bloques[0],"1");
	melodia->ritmo.bloqueritmico[0].numbloques=1;
    strcpy(melodia->ritmo.bloqueritmico[1].bloques[0],"1");
    melodia->ritmo.bloqueritmico[1].numbloques=1;
	melodia->puneje.eje=NULL;
    generarmelodia(melodia);
	melodia->puneje.eje=generaejecucion(melodia->penta,0);
	melodia->puneje.compas=0;
	melodia->puneje.index=0;
	melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;      
	melodia->penta[0].compas[0].color[0]=GREENBOLD;
	melodia->penta[1].compas[0].color[0]=GREENBOLD;
	IntervalTestUpdate(0,0,-1);
    PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas),4,1,-100);
	FlashCardCreateButtons();
	++melodia->puneje.compas;
}

void NoteInterval(void)
{
	melodia=(MELODIA *) new MELODIA;
	melodia->penta[0].compas=melodia->penta[1].compas=NULL;
    melodia->armadura=concfg.armaduranoteinterval;
	melodia->numsignature=melodia->densignature=1;
	melodia->fingering=0;
	melodia->metronomoreal=-1;
	melodia->noteinf[0]=57; melodia->notesup[0]=85; // de momento el SPAn es FIJO
	melodia->noteinf[1]=36; melodia->notesup[1]=64;
	resetmelodia(melodia);
	melodia->randommodegenerationmusic=0;
	melodia->numcompases=400;
	melodia->penta[0].allsilencio=0;
	melodia->penta[1].allsilencio=0;
	melodia->penta[0].compas=(COMPAS *) new COMPAS[melodia->numcompases];
    melodia->penta[1].compas=(COMPAS *) new COMPAS[melodia->numcompases];
    melodia->penta[0].maxintervalo=concfg.maxintervalog_noteinterval;
    melodia->penta[1].maxintervalo=concfg.maxintervalof_noteinterval;
	melodia->penta[0].minintervalo=concfg.minintervalog_noteinterval;
    melodia->penta[1].minintervalo=concfg.minintervalof_noteinterval;    
	melodia->repeatg=melodia->repeatf=0;
	melodia->silencios=0;
	strcpy(melodia->ritmo.bloqueritmico[0].bloques[0],"1");
	melodia->ritmo.bloqueritmico[0].numbloques=1;
    strcpy(melodia->ritmo.bloqueritmico[1].bloques[0],"1");
    melodia->ritmo.bloqueritmico[1].numbloques=1;
	melodia->puneje.eje=NULL;
    generarmelodia(melodia);
	melodia->puneje.eje=generaejecucion(melodia->penta,1); // genera ejecucion del compas 1 que es el que deberemos tocar primero
	melodia->puneje.compas=1;
	melodia->puneje.index=0;
	melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;      
	melodia->penta[0].compas[0].color[0]=GREENBOLD;
	melodia->penta[1].compas[0].color[0]=GREENBOLD;
	resetscores(melodia);
	melodia->score.hits=2;
	IntervalTestUpdate(0,0,-1);   // buffers a 0
	IntervalTestUpdate(0,0,-2);   // elimina silencios iniciales 
    PaintNoteInterval(scrollnoteinterval(melodia,0),6,0,concfg.armaduranoteinterval);	
}

NOTA *scrollnoteinterval(MELODIA *melodia,int compas)
{
    int c,r,count;
	static NOTA nt[32];

	for(c=count=0;c<3;c++){      
	   for(r=0;r<2;r++){
          nt[count].nota=(int) melodia->penta[r].compas[compas+c].note[0];
	      nt[count].color=melodia->penta[r].compas[compas+c].color[0];
	      nt[count].acci=(int) melodia->penta[r].compas[compas+c].acci[0];
	      nt[count].notakey=(int) melodia->penta[r].compas[compas+c].notekey[0];
	      nt[count].penta=r;
		  nt[count].compas=c;
		  nt[count].posx=200;
	      nt[count++].dura=melodia->penta[r].compas[compas+c].dura[0];
	   }
	}
    return nt;
}

void notereadingupdatestatistics(HITSPROB *hitspr,int can,int hf)
{
	int r,acci;

	for(r=0;r<can;r++){
	   if(hitspr[r].note==RandomNote[0].nota && hitspr[r].penta==RandomNote[0].penta){
          acci=RandomNote[0].acci;
		  if(hf==1) ++hitspr[r].numhits[acci];
		  else ++hitspr[r].numfails[acci];
	   }
	}
	return;	  
}

void PlayNotaMidi(__int64 nota,int push)
{
	__int64 chord;
	int tnota;

    if(nota==0) return;
    chord=nota; 
	for(;chord!=0;chord>>=8){
       tnota=(int) chord&0xff;
	   if(tnota!=0){
	      midiOutShortMsg(hMidiOut, 0x90 | (concfg.DefaultVelocity << 16) | (tnota << 8));
	      if(push!=-1) paintkeypiano1(tnota,push,hBlueBrush,NULL);
	   }
	}
	return;
}

void PlayNotaEndMidi(__int64 nota,int pop=0)
{
    __int64 chord;
	int tnota;

    if(nota==0) return;
    chord=nota; 
	for(;chord!=0;chord>>=8){
       tnota=(int) chord&0xff;
	   if(tnota!=0){
	      midiOutShortMsg(hMidiOut, 0x90 | (0 << 16) | (tnota << 8));
	      if(pop!=-1) paintkeypiano1(tnota,pop,hBlueBrush,NULL);
	   }
	}
	return;
}

// seguir      
void PlayNotaTeclado(__int64 nota,__int64 anota,__int64 atie)
{
   int r,tnota;
   __int64 chord,achord;
   FILE *fh;

   if(nota==0) return;
   chord=nota; 

   int an[150],schord,stie;
   achord=anota;
   for(r=0;r<150;r++) an[r]=0;
   for(;achord!=0;achord>>=8,atie>>=8){
      stie=(int) atie&0xff;
	  schord=(int) achord&0xff;
	  an[schord]=stie;
   }

   for(;chord!=0;chord>>=8){
      tnota=(int) chord&0xff;

	  if(an[tnota]!=0) continue;
      
	  for(r=0;r<81;r++)
	     if(Notes[r]==tnota) break;
      if(r==81) r=-1;
      ScanCode=r; 
      if(0){
         fh=fopen("pepe2.txt","a");
	     fprintf(fh,"NOTA ON:%d \n",tnota);
	     fclose(fh);
	  }
      SavedNote = tnota;
      if(SavedNote != -1){
         Velocity = concfg.DefaultVelocity;
         StartNote(0);	
	  }
   }
   return;
}


void PlayNotaEndTeclado(__int64 nota,__int64 tie)
{
   int r,tnota;
   __int64 chord;
   FILE *fh;
   int stie;

   if(nota==0) return;
   chord=nota;
   for(;chord!=0;chord>>=8,tie>>=8){
	  stie=(int) tie&0xff;
	  if(stie!=0) continue;
      tnota=(int) chord&0xff;
      for(r=0;r<81;r++)
	     if(Notes[r]==tnota) break;
	  if(r==81) r=-1;
      ScanCode=r; 
	  if(0){
         fh=fopen("pepe2.txt","a");
	     fprintf(fh,"NOTA OFF:%d \n",tnota);
	     fclose(fh);
	  }
      SavedNote = tnota;
      if(SavedNote != -1){
         Velocity = 0;
         EndNote(0);	
	  }
   }
   return;
}

void initializefingering(NOTA *listanotas,int can)
{
   int r;
   NOTA *h;
   unsigned int seed;

   seed=(unsigned int) time(NULL);
   srand(seed);
   for(r=0;r<can;r++){
      h=genrndnote();
      listanotas[r]=*h;
   }
   viterbi(listanotas,can,0);
   viterbi(listanotas,can,1);
   srand(seed);
   return;
}

void notereading(int gen,int hitfail)
{
	NOTA *h;
    int r,j,count,acci,numstaff,intervalbool,dist;
    static int PreviousRandomNote=36,canhits;
	static HITSPROB *hitspr;
	static NOTA *listanotas;
    static NOTA *Noteant;
    static int iniciof[2];
	static int deep1;
	static double reaction;
	int q;

	if(concfg.notereadingchord==1){
	   notereadingchords(gen,hitfail);
	   return;
	}
	saveAccidental = accidental;
	intervalbool=!(concfg.maxintervalof_noteread==0 || concfg.maxintervalog_noteread==0 || 
			   concfg.minintervalof_noteread==0 || concfg.minintervalog_noteread==0);
	if(gen==-4) 
	   deep1=0;
	if(gen==-3){
		double movmean20;
		if(hitfail==1) ++notereadingscore.hits;
		else ++notereadingscore.fails;
		notereadingscore.reaction=clock()-reaction;
		notereadingscore.perhits=100.0*notereadingscore.hits/(notereadingscore.hits+notereadingscore.fails);
		notereadingscore.perfails=100.0*notereadingscore.fails/(notereadingscore.hits+notereadingscore.fails);
		if(concfg.notereadlearning==1)
           notereadingupdatestatistics(hitspr,canhits,hitfail);
        for(r=NOTEREADRILE-1;r>=1;r--)
		   notereadinglisthits[r]=notereadinglisthits[r-1];
		if(hitfail==1) notereadinglisthits[0]=1;
		else notereadinglisthits[0]=0;
		for(r=movmean20=0;r<NOTEREADRILE;r++){
		   if(notereadinglisthits[r]==-1) break;
		   if(notereadinglisthits[r]==1) ++movmean20;
		}
		movmean20/=r; movmean20*=10;
        notereadingscore.movmeanhits20=movmean20;
        return;
	}
	if(gen==-2){
       if(hitspr!=NULL) delete [] hitspr;
	   if(listanotas!=NULL) delete [] listanotas;        
	   listanotas=NULL;
	   hitspr=NULL; 
	   canhits=0;
	   return;
	}
	if(gen==-1){
	   pernoteread=0;
	   notereadingscore.fails=notereadingscore.hits=0;
	   notereadingscore.perfails=notereadingscore.perhits=0;
	   reaction=clock();
	   if(concfg.notereadlearning==1){
	      if(hitspr==NULL) 
		     hitspr=(HITSPROB *) new HITSPROB[256];
	      for(r=count=0;r<37;r++){
			 if(concfg.noteinfgeneric[1]<=notas00[r] && notas00[r]<=concfg.notesupgeneric[1]){
			    hitspr[count].note=notas00[r];
                hitspr[count].penta=1;   
                for(j=0;j<3;j++) hitspr[count].numhits[j]=hitspr[count].numfails[j]=0;
			    count++;
			 }
		  }
		  for(r=0;r<37;r++){	 
			 if(concfg.noteinfgeneric[0]<=notas00[r] && notas00[r]<=concfg.notesupgeneric[0]){
				hitspr[count].note=notas00[r];
                hitspr[count].penta=0;   
			    for(j=0;j<3;j++) hitspr[count].numhits[j]=hitspr[count].numfails[j]=0;
                count++;
			 }
		  }
          canhits=count;
	   }else{
		   if(concfg.notereadingfingering==1){  // || 1 activo para pruebas 31-05-2014 
			  if(listanotas==NULL){
                 listanotas=(NOTA *) new NOTA[1024];
		         initializefingering(listanotas,1024);
			  }
		   }
	   }
	   if(intervalbool && concfg.notereadlearning==0){
	      iniciof[0]=iniciof[1]=0; 
	      if(Noteant!=NULL) delete [] Noteant;
	      Noteant=(NOTA *) new NOTA[2];
	   }
	   for(r=0;r<NOTEREADRILE;r++){
	      notereadinglist[r].tdura=-1;
		  notereadinglist[r].etime=-1;
		  notereadinglist[r].itime=-1;
	   }
       for(r=0;r<NOTEREADRILE;r++)
		  notereadinglisthits[r]=-1;
	   NoteReadingCreateButtons();
	   deep1=0;
	   return;
	}       
	if(gen==0){
	   for(q=notereadingmemo+1;q<notereadingdeep;q++)
	      RandomNote[q-notereadingmemo-1]=RandomNote[q];
	   for(q=deep1;q<notereadingdeep;q++){
	      do{
             while(1){
				if(concfg.notereadlearning==0){ 
					 if(concfg.notereadingfingering==0) h=genrndnote();  // random uniforme
					 else h=genrndfingernote(listanotas,1024);
                     if(concfg.notereadingfiguras==0) h->dura=0; // solo redondas en noteinterval si no queremos figuras
				}else h=genrndnotestadistical(hitspr,canhits); // random basado en estadisticas de aciertos
			    if(intervalbool && concfg.notereadlearning==0){
			       if(iniciof[h->penta]==0){
			          iniciof[h->penta]=1;
   			          Noteant[h->penta]=*h;
				   }else{		
			          dist=interval(h,&Noteant[h->penta]);
				      if(h->penta==0)
			             if(!(dist>=concfg.minintervalog_noteread && dist<=concfg.maxintervalog_noteread)) 
				            continue;
				      if(h->penta==1)
			             if(!(dist>=concfg.minintervalof_noteread && dist<=concfg.maxintervalof_noteread)) 
				            continue;
				      Noteant[h->penta]=*h;
				   }
				}
			    if(gfstat[0]==1){
				   if(h->penta!=0) continue;
	               if(h->nota<concfg.noteinfgeneric[0] || h->nota>concfg.notesupgeneric[0]) continue;
				   break;
				}
			    if(gfstat[1]==1){
				   if(h->penta!=1) continue;
	               if(h->nota<concfg.noteinfgeneric[1] || h->nota>concfg.notesupgeneric[1]) continue;
				   break;
				}
                break;
			 }
		     RandomNote[q].nota=h->nota;
		     RandomNote[q].penta=h->penta;
		     RandomNote[q].dura=h->dura;
             RandomNote[q].finger=h->finger;
			 RandomNote[q].color=RGB(150,150,150);
			 RandomNote[q].compas=0;
			 acci=h->acci;
		     //if(concfg.notereadingacci==1) acci=h->acci;
		     //else acci=h->acci=0;		  
             accidental=RandomNote[q].acci=h->acci;
	         acci=3-2*acci; if(acci==3) acci=0;
             RandomNote[q].notakey=h->nota+armadura(h->nota,acci,concfg.armaduranotereading);
		  }while (RandomNote[q].notakey == PreviousRandomNote || 
		        RandomNote[q].notakey<concfg.noteinfgeneric[1] || 
				RandomNote[q].notakey>concfg.notesupgeneric[0]);
	      PreviousRandomNote = RandomNote[q].notakey;
	   }
	   //RandomNote[0].color=0;
	   for(q=0;q<=notereadingmemo;q++)
          RandomNote[q].color=0;
	   if(notereadinglead==1 && deep1!=0){
	      for(q=0;q<=notereadingmemo;q++){
             RandomNote[q].color=-1;
		  }
	   }
	   for(q=0;q<notereadingdeep;q++)
	      RandomNote[q].posx=60+30*q;
       //deep1=notereadingdeep-1;	   
	   deep1=notereadingdeep-notereadingmemo-1;
	   reaction=clock();
	   if(concfg.notereadingfiguras==1 && concfg.metronoteread!=0){
	      unsigned int dur;

		  double fig[]={4,2,1,0.5,0.25};
          dur=(60*CLOCKS_PER_SEC)/concfg.metronoteread; // duracion en negras por minuto
          dur=dur*fig[(int)RandomNote[0].dura];
		  dur/=10;  // subdividimos el tempo en partes de 10
          SetTimer(hwnd,12,dur,NULL);
	   }
       if(notereadingmemo==0)
	      PlayNotaMidi((__int64) RandomNote[0].notakey,-1);
	   if(earnoteread==1 && earinterval==1){
          SetTimer(hwnd,5,2000,NULL); // 2 segundos antes de que suene el intervalo
	   }
	}
	randomcannote=1+notereadingmemo; 
	numstaff=gfstat[0]+2*gfstat[1];
    // 0 pinta ambas armaduras
	// 1 pinta solo sol 
	// 2 pinta solo fa
	// 3 pinta sin notas
	if(earnoteread==1){
		if(earinterval==0){
	      Paintnotereading(&RandomNote[0],1,numstaff,concfg.armaduranotereading,0);
		  return;
		}else paintkeypiano(RandomNote[0].notakey,1,0,hGreenBrush);
	}
	Paintnotereading(&RandomNote[0],notereadingdeep,numstaff,concfg.armaduranotereading,1);
	return;
}	

int notereadingchordcompare( const void *arg1, const void *arg2 )
{
   int x,y;

   x=((NOTA *) arg1)->nota;
   y=((NOTA *) arg2)->nota;
   if(x<y) return -1;
   if(x==y) return 0;
   if(x>y) return 1;
   return 1;
}

void notereadingchords(int gen,int hitfail)
{
	CHORD *ch;
    int r,v,numstaff;
	__int64 chord,acci,ntk;
    static int canhits;
	static double reaction;
	static __int64 pvrn;

	saveAccidental = accidental;
	if(gen==-3){
		double movmean20;
		if(hitfail==1) ++notereadingscore.hits;
		else ++notereadingscore.fails;
		notereadingscore.reaction=clock()-reaction;
		notereadingscore.perhits=100.0*notereadingscore.hits/(notereadingscore.hits+notereadingscore.fails);
		notereadingscore.perfails=100.0*notereadingscore.fails/(notereadingscore.hits+notereadingscore.fails);
        for(r=NOTEREADRILE-1;r>=1;r--)
		   notereadinglisthits[r]=notereadinglisthits[r-1];
		if(hitfail==1) notereadinglisthits[0]=1;
		else notereadinglisthits[0]=0;
		for(r=movmean20=0;r<NOTEREADRILE;r++){
		   if(notereadinglisthits[r]==-1) break;
		   if(notereadinglisthits[r]==1) ++movmean20;
		}
		movmean20/=r; movmean20*=10;
        notereadingscore.movmeanhits20=movmean20;
        return;
	}
	if(gen==-2){
	   canhits=0;
	   return;
	}
	if(gen==-1){
	   pernoteread=0;
	   notereadingscore.fails=notereadingscore.hits=0;
	   notereadingscore.perfails=notereadingscore.perhits=0;
	   reaction=clock();
	   for(r=0;r<NOTEREADRILE;r++){
	      notereadinglist[r].tdura=-1;
		  notereadinglist[r].etime=-1;
		  notereadinglist[r].itime=-1;
	   }
       for(r=0;r<NOTEREADRILE;r++)
		  notereadinglisthits[r]=-1;
	   NoteReadingCreateButtons();
	   return;
	}       
	if(gen==0){
	   do{
          while(1){
	         ch=genrndchord();  // random uniforme
			 if(concfg.notereadingfiguras==0) ch->dura=0; // solo redondas en noteinterval si no queremos figuras
			 if(gfstat[0]==1){
				 if(ch->penta!=0) continue;
				 break;
			 }
			 if(gfstat[1]==1){
				 if(ch->penta!=1) continue;
				 break;
			 }
             break;
		  }
          chord=ch->nota;
		  acci=ch->acci;
		  ntk=ch->notakey;
		  for(r=0;chord!=0;chord>>=8,acci>>=8,ntk>>=8){		                  
		     RandomNote[r].nota=(int) (chord&0xff);
			 RandomNote[r].acci=(int) (acci&0xff);
			 RandomNote[r].notakey=(int) (ntk&0xff);
			 RandomNote[r].posx=150;
			 RandomNote[r].dura=ch->dura;
	         RandomNote[r].color=0;
	         RandomNote[r].compas=0;
		     RandomNote[r++].penta=ch->penta;
		  }	   	   
		  //ordena las notas de menor a mayor
          qsort((void *) RandomNote,(size_t) r,sizeof(NOTA),notereadingchordcompare);   
	      for(v=1;v<r;v++){
			 if(RandomNote[v].nota==RandomNote[v-1].nota){
			    ch->notakey=pvrn; // sirve para forzar un nuevo acorde, el que hemos elegido tiene notas repetidas 
				break;
			 }
		  }
	   }while (ch->notakey == pvrn);
	   pvrn=ch->notakey;
	   randomcannote=r;
	   reaction=clock();
	   if(concfg.notereadingfiguras==1 && concfg.metronoteread!=0){
	      unsigned int dur;

		  double fig[]={4,2,1,0.5,0.25};
          dur=(60*CLOCKS_PER_SEC)/concfg.metronoteread; // duracion en negras por minuto
          dur=dur*fig[(int)RandomNote[0].dura];
		  dur/=10;  // subdividimos el tempo en partes de 10
          SetTimer(hwnd,12,dur,NULL);
	   }
       PlayNotaMidi(ch->notakey,-1);
	}
	numstaff=gfstat[0]+2*gfstat[1];
    // 0 pinta ambas armaduras
	// 1 pinta solo sol 
	// 2 pinta solo fa
	// 3 pinta sin notas
	Paintnotereading(&RandomNote[0],randomcannote,numstaff,concfg.armaduranotereading,1);
	return;
}	

void paintnotereadingstickchord(HDC hdc,int posx,int posy0,int posy1)
{
	//t posx,posy0,posy1;
	int width,wd=0;
	TEXTMETRIC tm;
    POINT p[2];
	HPEN pen;
	ABC abc;

    GetTextMetrics (hdc, &tm) ;	
	GetCharWidth32(hdc,32,32,&width); 	
	GetCharABCWidths(hdc,207,207,&abc);
	//posx=melodia->penta[r].compas[c].posx[v]+2*width; 
	//posy0=melodia->penta[r].compas[c].posy[v][0]-dispy[0]+tm.tmAscent;
	//posy1=melodia->penta[r].compas[c].posy[v][chordp-1]-dispy[chordp-1]+tm.tmAscent;
    //color=melodia->penta[r].compas[c].color[v];
    pen=CreatePen(PS_SOLID,1,RGB(0,0,0));
	SelectObject(hdc,pen);
 	p[0].x=posx;     p[1].x=posx;
    p[0].y=posy0+tm.tmAscent; p[1].y=posy1+tm.tmAscent;
    Polyline(hdc,p,2);
	DeleteObject(pen);
}

void paintgenericmetro(int onestaff,int value)
{
	char *s,aa[64];
	SIZE p;
	int posy;
	int orgs=gStaff[64-36]-295;
	int orgf=fStaff[43-36]-240;	

	if(onestaff==0 || onestaff==1) posy=orgs; else posy=orgf+ntinterline;
	//s=getnumtonum(concfg.metronoteread);
	s=getnumtonum(value);
    strcpy(aa,"   "); strcat(aa,s);
    TextOut(hdc,top260min30,posy-7*ntinterline,aa,strlen(aa));
	GetTextExtentPoint32(hdc,aa,strlen(aa),&p);
	SelectObject(hdc, hMaestroFontSmall2);
	s=gettiponotasingle(4);
	strcpy(aa,"   ");
    strcat(aa,s);
	strcat(aa," ");
	SetTextColor(hdc,GRAY);
    TextOut(hdc,top260min30+p.cx,posy-4.3*ntinterline,aa,strlen(aa));
	SelectObject(hdc, hMaestroFont);
	SetTextColor(hdc,BLACK);
	return;
}

char notereadinggettiponota(int dur)
{
   char s;
   
   if(dur==0) s=119; //redonda
   if(dur==1) s=72; // blanca
   if(dur==2) s=81; // negra
   if(dur==3) s=69; // corchea
   if(dur==4) s=88; //semichorchea
   return s;
}

void paintnotereadingqosmeasure(void)
{
    double dat[64],dat1[64],ritmo,durnotas,dursilen,legato,qos;
	int r;

	durnotas=qos=dursilen=0;
	notereadingritmo=notereadinglegato=0;
	for(r=0;r<NOTEREADRILE;r++){
	   dat[r]=notereadinglist[r].etime-notereadinglist[r].itime;
	   dat1[r]=notereadinglist[r].tdura;
	   if(dat1[r]<0) break;
	   durnotas+=dat[r];
	   if(r!=0) 
	      dursilen+=(notereadinglist[r-1].itime-notereadinglist[r].etime);
	}
	if(r==0){ notereadingscore.qos=qos; return; }
	ritmo=notereadingritmo=similaridad(dat,dat1,r);
	legato=notereadinglegato=durnotas*10.0/(durnotas+dursilen);
	if(concfg.notereadqosritmo==1 && concfg.notereadqoslegato==0)
	   qos=(ritmo+4*notereadingscore.movmeanhits20)/5;
	if(concfg.notereadqosritmo==0 && concfg.notereadqoslegato==1){
	   qos=(legato+4*notereadingscore.movmeanhits20)/5;
	   if(legato<5) qos=legato;
	}
	if(concfg.notereadqosritmo==1 && concfg.notereadqoslegato==1){
       qos=(legato+ritmo+4*notereadingscore.movmeanhits20)/6;	
       if(legato<5) qos=legato;
	}
    if(concfg.notereadqosritmo==0 && concfg.notereadqoslegato==0)
	   qos=notereadingscore.movmeanhits20;	
    notereadingscore.qos=qos;
	return;
}
void Paintnotereadingfunction(NOTA *nt,int can,int onestaff,int armadura,int pintanota)
{
	int orgs,orgf,r,k,t,c,d,sb,v,offsetn;
    int RandomNoteLoc,offset,ofs;
	RECT backp,backp1;
	int **posarm,disp[32],alfa;
	char aa[64],s[64],cc[64];
	double adjust;
    TEXTMETRIC  tm ;
	SIZE sz;

	offset=40; 
	orgs=gStaff[64-36]-295;
	orgf=fStaff[43-36]-240;	
	SelectObject(hdc, hMaestroFont);
	backp=testRect;
	backp.top=backp.left=0;
	FillRect(hdc, &backp,hWhiteBrush);
	if(showtest3==TRUE && concfg.notereadingchord==0){
	   int sp,top,bottom;
	   if(onestaff==0){ 
		  top=orgs-ntinterline;
	      bottom=top+orgf-orgs+ntinterline*10;
	   }
	   if(onestaff==1){
		  top=orgs-ntinterline;
	      bottom=top+10*ntinterline;
	   }
	   if(onestaff==2){
          top=orgf-2*ntinterline;
          bottom=orgf+ntinterline*9;
	   }
	   if(notereadingdeep!=0) sp=(notereadingmemo+1)*32;
	   backp1.left=top260min30 + offset*1.5 + StaffWidth[0];
       backp1.right=backp1.left+sp;
	   backp1.top=top;
       backp1.bottom=bottom;
	   FillRect(hdc, &backp1,hlBlueBrush);
	}
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc,BLACK);
	GetTextMetrics (hdc, &tm) ;	
	if(armadura==-100) ofs=125; else ofs=0;
    if(onestaff==0){
	   paintstaff(hdc,top260min30+ofs,orgs,8); // staff usando lineas
	   SetTextColor(hdc,GRAY);
	   TextOut(hdc,top260min30+ofs , orgs-ntinterline, "&", 1);
	   paintstaff(hdc,top260min30+ofs,orgf,8); // staff usando lineas
	   TextOut(hdc,top260min30+ofs , orgf-3*ntinterline, "?", 1);	
	}
	if(onestaff==1){
       SetTextColor(hdc,GRAY);
	   paintstaff(hdc,top260min30+ofs,orgs,8); // staff usando lineas
	   TextOut(hdc,top260min30+ofs , orgs-ntinterline, "&", 1);
	}
	if(onestaff==2){
	   SetTextColor(hdc,GRAY);
	   paintstaff(hdc,top260min30+ofs,orgf,8); // staff usando lineas
	   TextOut(hdc,top260min30+ofs , orgf-3*ntinterline, "?", 1);		
	}

	// dibuja valor metronomo
	if(concfg.metronoteread!=0 && showtest3==TRUE) 
       paintgenericmetro(onestaff,concfg.metronoteread);
	if(concfg.metronoteinterval!=0 && showtest2==TRUE) 
       paintgenericmetro(onestaff,concfg.metronoteinterval);
    SetTextColor(hdc,BLACK);

	// dibuja armadura
	if(armadura!=-100){
	   //SetTextColor(hdc,BLACK);
       calcarmaduranotereading(posarm,armadura);
	   sb=abs(armadura);
       strcpy(aa,"   ");
	   for(v=0;v<sb;v++){	   
	      strcpy(cc,aa);
	      if(armadura>0) strcat(cc,"#");
	      else strcat(cc,"b");       
          if(onestaff==0){ 
			TextOut(hdc,top260min30+GClefWidth[0],orgs+posarm[0][v],cc,strlen(cc));
	        TextOut(hdc,top260min30+FClefWidth[0],orgf+posarm[1][v],cc,strlen(cc));						
		  }
		  if(onestaff==1)
			TextOut(hdc,top260min30+GClefWidth[0],orgs+posarm[0][v],cc,strlen(cc));
		  if(onestaff==2)
             TextOut(hdc,top260min30+FClefWidth[0],orgf+posarm[1][v],cc,strlen(cc));
	      strcat(aa,"     ");
	   }
	}
	if(pintanota==0){
	   SetBkMode(hdc, OPAQUE);
	   if(showtest3==TRUE){
          backp=testRect;
	      testRect.left =ExtraSpace;
	      testRect.right=testRect.top=testRect.bottom=0; 
	      paintscores(&notereadingscore);
	   }
	   testRect=backp;	
	   return;
	}
	disp[0]=0;
	for(t=0;t<can;t++){
	   if(t!=0){
	      if(nt[t].penta==nt[t-1].penta && nt[t].compas==nt[t-1].compas && nt[t].posx==nt[t-1].posx)
			 if(istogether(nt[t].nota,nt[t-1].nota)) disp[t++]=1; 
			 disp[t]=0;
	   }
	}
 	//GetTextExtentPoint32(hdc,"w",1,&sz);
    //alfatmp=sz.cx;
	for(t=0;t<can;t++){
       r=nt[t].penta;
	   c=nt[t].compas;
	   d=nt[t].dura;
	   if(nt[t].color==-1) 
	      continue; // color invisible, no imprimimos notas
	   offsetn=nt[t].posx;
	   k=calcnoteindex(&nt[t]);
	   if(nt[t].nota==0){   // es un silencio
		   if(r==0) k=18; else k=0;
	   }
	   if(showtest4==TRUE || showtest2==TRUE || concfg.notereadingfiguras==0) d=0; // solo redondas en noteinterval 
	   if(onestaff==1 && r==1) continue;
       if(r==1 && nt[t].nota!=0){ 
	      RandomNoteLoc = orgf-(k-4)*ntinterline*0.5;
	      SetTextColor(hdc,BLACK);
	      if(nt[t].nota <= 36)
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgf+2*ntinterline, "___", 2);
          if(nt[t].nota <= 40) 
		     TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgf+ntinterline, "___", 2);
	      if(nt[t].nota >= 60)
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgf-5*ntinterline, "___", 2);
	      if(nt[t].nota == 64)				
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgf-6*ntinterline, "___", 2);
	   }
	   if(r==0 && nt[t].nota!=0){
	      SetTextColor(hdc,BLACK);
	      RandomNoteLoc = orgs-(k-16)*ntinterline*0.5; 
	      if(nt[t].nota <= 57)
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgs+2*ntinterline, "___", 2);
	      if(nt[t].nota <= 60) 
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgs+ntinterline, "___", 2);
	      if(nt[t].nota >= 81)
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgs-5*ntinterline, "___", 2);
	      if(nt[t].nota >= 84)
	         TextOut(hdc,top260min30 + offsetn + StaffWidth[0]+c*offset, orgs-6*ntinterline, "___", 2);			 			 					 
	   }

	   // dibuja notas musicales			 

	   adjust=0; s[1]=0;
       if(showtest3==TRUE && t!=0 && d!=0 && concfg.notereadingchord==1){
		  if(d==1) s[0]=250;   // cabeza blanca
		  else s[0]=207;        // cabeza negra
       }else{
          s[0]=notereadinggettiponota((int) d);
		  if(d!=0) adjust=0.5*ntinterline;
	   }
       if(disp[t]==1){
 	      GetTextExtentPoint32(hdc,s,1,&sz);
          alfa=-sz.cx;
	   }else alfa=0;
	   SetTextColor(hdc,nt[t].color);
	   if(showtest2==TRUE && concfg.metronoteinterval!=0 && noteintervaltimersand==1 && t==0){
	      int x,y;
          x=top260min30 + 200 + (StaffWidth[0]+(NoteWidth[0]))+offset-9;
	      y=orgs-4*ntinterline;
	      paintclocksand(x,y,GREEN,GREEN,GREEN,pernoteread);			 
	   }
	   if(nt[t].nota==0){    // si es un silencio
		  aa[0]=183; aa[1]=0;
          RandomNoteLoc = orgs-(k-16)*ntinterline*0.5;
          TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset,RandomNoteLoc+adjust,aa,strlen(aa));
		  continue;
	   }
       if(nt[t].acci==0){
          strcpy(aa,s);
	      TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset+alfa,RandomNoteLoc+adjust,aa,2); //"w",1);
	      if(showtest3==TRUE && concfg.metronoteread!=0 && notereadtimersand==1 && t==0){			 
	         paintclocksand(top260min30 + nt[t].posx-40 + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset,RandomNoteLoc+tm.tmAscent-15,RED,ORANGE,GREEN,pernoteread);			 
		  }
	   }
       if(nt[t].acci==1){	
		  GetTextExtentPoint32(hdc,"I",1,&sz);
		  strcpy(aa,"I");
		  TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset-sz.cx+alfa,RandomNoteLoc,aa,1); //"Iw",2); // sostenido
		  strcpy(aa,s);
	      TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset+alfa,RandomNoteLoc+adjust,aa,1); //"Iw",2); // sostenido
	      if(showtest3==TRUE && concfg.metronoteread!=0 && notereadtimersand==1 && t==0){			 
	         paintclocksand(top260min30 + nt[t].posx-40+ (StaffWidth[0]+(NoteWidth[0]/2))+c*offset,RandomNoteLoc+tm.tmAscent-15,RED,ORANGE,GREEN,pernoteread);			 
		  }	  	   
	   }
	   if(nt[t].acci==2){
		  GetTextExtentPoint32(hdc,"i",1,&sz);
		  strcpy(aa,"i");
          TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset-sz.cx+alfa,RandomNoteLoc,aa,1); //"iw",2);  // bemol
		  strcpy(aa,s);
	      TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset+alfa,RandomNoteLoc+adjust,aa,1); //"iw",2);  // bemol
	      if(showtest3==TRUE && concfg.metronoteread!=0 && notereadtimersand==1 && t==0){
	         paintclocksand(top260min30 + nt[t].posx-40 + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset,RandomNoteLoc+tm.tmAscent-15,RED,ORANGE,GREEN,pernoteread);			 
		  }
	   }
	   // dibuja fingering
	   if(showtest3==1 && concfg.notereadingfingering==1 && concfg.notereadingchord==0){
	      char buf[64];
		  sprintf(buf,"  %d",nt[t].finger);
		  if(r==0)
	         mydrawtext2(RGB(122,14,86),hfontsystem,buf,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset+alfa,orgs-2*ntinterline);
	      if(r==1)
	         mydrawtext2(RGB(122,14,86),hfontsystem,buf,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2))+c*offset+alfa,orgf+9*ntinterline);	      
	   }
       if(t==0 && d!=0 && showtest3==TRUE && concfg.notereadingchord==1){
          int posx,posy0,posy1,k,r;
		  k=calcnoteindex(&nt[can-1]);
		  r=nt[can-1].penta;
          posx=top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2));
		  posy0=RandomNoteLoc+adjust;
          if(r==1) posy1=orgf-(k-4)*ntinterline*0.5-1;
          else posy1=orgs-(k-16)*ntinterline*0.5-1;
          paintnotereadingstickchord(hdc,posx,posy0,posy1);	   
	   }
	}	
	if(showtest3==TRUE)       // calcula qos
	   paintnotereadingqosmeasure();
	SetBkMode(hdc, OPAQUE);
	if(showtest3==TRUE){
       backp=testRect;
	   testRect.left =ExtraSpace;
	   testRect.right=testRect.top=testRect.bottom=0; 
	   paintscores(&notereadingscore);
	}
	testRect=backp;	
}

void Paintnotereading(NOTA *nt,int can,int onestaff,int armadura,int pintanota)
{    
    HDC hdcmem,hdcbackup;   
    HBITMAP hbmmem;
	HGDIOBJ hold;

    hdcbackup=hdc;	
    hdcmem = CreateCompatibleDC(hdc);
    hbmmem = CreateCompatibleBitmap(hdc,testRect.right,top260);  //testRect.Bottom
    hdcbackup=hdc;
	hdc=hdcmem;
    hold=SelectObject(hdcmem, hbmmem);
	Paintnotereadingfunction(nt,can,onestaff,armadura,pintanota);
	hdc=hdcbackup;
    BitBlt(hdc,0,0,testRect.right,top260,hdcmem, 0, 0, SRCCOPY);
    SelectObject(hdcmem, hold);
    DeleteObject(hbmmem);
    DeleteDC(hdcmem);
	return;
}

void PaintNoteIntervalfunction(NOTA *nt,int can,int onestaff,int armadura)
{
	POINT p[2];
	HGDIOBJ pen,holdpen;
	int orgs,orgf,offset;
	RECT backp;

	Paintnotereadingfunction(nt,can,onestaff,armadura,1);
	orgs=gStaff[64-36]-295;
	orgf=fStaff[43-36]-240;	
    offset=40;
    pen=CreatePen(PS_DASH,0,RED);
	holdpen= SelectObject(hdc,pen);
	p[0].x=top260min30 + 200 + (StaffWidth[0]+(NoteWidth[0]))+offset;
	p[0].y=orgs-2*ntinterline;
	p[1].x=top260min30 + 200 + (StaffWidth[0]+(NoteWidth[0]))+offset;
	p[1].y=orgf+10*ntinterline;
	if(onestaff==0) Polyline(hdc,p,2);
	DeleteObject(pen);
    SelectObject(hdc,holdpen);
	backp=testRect;
	testRect.left=ExtraSpace;
	testRect.right=testRect.top=testRect.bottom=0; 
	paintscores(&melodia->score);
	if(showtest2==TRUE) 
	   painttemposcoretext(melodia);		
	testRect=backp;
	return;
}

void PaintNoteInterval(NOTA *nt,int can,int onestaff,int armadura)
{    
    HDC hdcmem,hdcbackup;   
    HBITMAP hbmmem;
	HGDIOBJ hold;
	int offset=0;

    hdcbackup=hdc;
    hdcmem = CreateCompatibleDC(hdc);
	if(showtest4==TRUE || showtest2==TRUE) offset=92;
    hbmmem = CreateCompatibleBitmap(hdc,testRect.right-offset,top260);  //testRect.Bottom
    hdcbackup=hdc;
	hdc=hdcmem;
    hold=SelectObject(hdcmem, hbmmem);
	PaintNoteIntervalfunction(nt,can,onestaff,armadura);
	hdc=hdcbackup;
    BitBlt(hdc,0,0,testRect.right-offset,top260,hdcmem, 0, 0, SRCCOPY);
    SelectObject(hdcmem, hold);
    DeleteObject(hbmmem);
    DeleteDC(hdcmem);
	return;
}

//****************************  en experimentacion *******


int isnatural(int nota)
{
   int natu[7]={0,2,4,5,7,9,11};   // posicion notas naturales do=1 hasta si=12
   int r;

   for(r=0;r<7;r++)
       if(natu[r]==nota) break;
   if(r==7) return -1;
   return r;
}

int natural(int index)
{
	int natu[7]={0,2,4,5,7,9,11};   // notas naturales do=1 hasta si=12
	return natu[index];
}

char *getnamenote(int nota,int sb)
{
      static char *notass[12]={"SI#","DO#","RE","RE#","MI","MI#","FA#","SOL","SOL#","LA","LA#","SI"};
      static char *notasb[12]={"DO","REb","RE","MIb","FAb","FA","SOLb","SOL","LAb","LA","SIb","DOb"};
	  
//    static char *notass[12]={"DO","DO#","RE","RE#","MI","FA","FA#","SOL","SOL#","LA","LA#","SI"};
//    static char *notasb[12]={"DO","REb","RE","MIb","MI","FA","SOLb","SOL","LAb","LA","SIb","SI"};
      if(sb==0) return notass[nota];  // escala de sostenidos, si sb==0
      else return notasb[nota];       // escala de bemoles, si s==1
      return NULL;
}

char *tablatempo(int tempo)
{
   static char buf[32];

   if(tempo<=43) strcpy(buf,"Lento");
   if(tempo>=44 && tempo<=47) strcpy(buf,"Largo");
   if(tempo>=48 && tempo<=51) strcpy(buf,"Larghetto");
   if(tempo>=52 && tempo<=54) strcpy(buf,"Adagio");
   if(tempo>=55 && tempo<=65) strcpy(buf,"Andante");
   if(tempo>=66 && tempo<=69) strcpy(buf,"Andantino");
   if(tempo>=70 && tempo<=95) strcpy(buf,"Moderato");
   if(tempo>=96 && tempo<=112) strcpy(buf,"Allegretto");
   if(tempo>=113 && tempo<=120) strcpy(buf,"Allegro");
   if(tempo>=121 && tempo<=140) strcpy(buf,"Vivace");
   if(tempo>=141 && tempo<=175) strcpy(buf,"Presto");
   if(tempo>=176 && tempo<=208) strcpy(buf,"Prestissimo");
   return buf;
}

// hand=0 derecha hand=1 izquierda
void paintmemoespacialfunction(int notakiz,int sbkiz,int dedokiz,int notaref,int sbref,int dedoref,int hand,int displaypenta)
{
   int orgs=gStaff[64-36]-295,offy;
   int offset=testRect.right*0.5-50;
   RECT backp;
   UINT al;
   char *kizn,*refn;
   POINT p;

   backp=testRect;
   backp.top=backp.left=0;
   FillRect(hdc, &backp,hWhiteBrush);
   SetBkMode(hdc, TRANSPARENT);
   if(displaypenta==0){
      if(currenthor==1280){
         int hl[5][2]={{290,185},{221,25},{118,-5},{50,15},{-10,79}};
         int hr[5][2]={{-20,190},{59,32},{165,-5},{248,10},{309,76}};
         offy=100;

         painthand(offset,orgs-offy,hand);
         kizn=getnamenote(notakiz,sbkiz);
         refn=getnamenote(notaref,sbref);
         if(hand==0){ p.x=offset+hr[dedoref-1][0]; p.y=orgs-offy+hr[dedoref-1][1]; }
         else{ p.x=offset+hl[dedoref-1][0]; p.y=orgs-offy+hl[dedoref-1][1]; }
         SelectObject(hdc, hFont);
         al=SetTextAlign(hdc,TA_BOTTOM);
         SetTextColor(hdc,ref.color);
         if(kiz.dedo!=ref.dedo) TextOut(hdc,p.x,p.y,refn,strlen(refn));
         if(hand==0){ p.x=offset+hr[dedokiz-1][0]; p.y=orgs-offy+hr[dedokiz-1][1]; }
         else{ p.x=offset+hl[dedokiz-1][0]; p.y=orgs-offy+hl[dedokiz-1][1]; }
         SetTextColor(hdc,kiz.color);
         TextOut(hdc,p.x,p.y,kizn,strlen(kizn));
         SetTextAlign(hdc,al);
	  }else{
         int hl[5][2]={{180,105},{115,15},{65,-5},{25,8},{-20,49}};
         int hr[5][2]={{-30,120},{39,18},{92,-2},{135,10},{165,45}};
         offy=0;   

         painthand(offset,orgs-offy,hand);
         kizn=getnamenote(notakiz,sbkiz);
         refn=getnamenote(notaref,sbref);
         if(hand==0){ p.x=offset+hr[dedoref-1][0]; p.y=orgs-offy+hr[dedoref-1][1]; }
         else{ p.x=offset+hl[dedoref-1][0]; p.y=orgs-offy+hl[dedoref-1][1]; }
         SelectObject(hdc, hFont);
         al=SetTextAlign(hdc,TA_BOTTOM);
         SetTextColor(hdc,ref.color);
         if(kiz.dedo!=ref.dedo) TextOut(hdc,p.x,p.y,refn,strlen(refn));
         if(hand==0){ p.x=offset+hr[dedokiz-1][0]; p.y=orgs-offy+hr[dedokiz-1][1]; }
         else{ p.x=offset+hl[dedokiz-1][0]; p.y=orgs-offy+hl[dedokiz-1][1]; }
         SetTextColor(hdc,kiz.color);
         TextOut(hdc,p.x,p.y,kizn,strlen(kizn));
         SetTextAlign(hdc,al);
	  }
   }else{
      if(currenthor==1280){
         int hl[5][2]={{270,90},{221,-68},{118,-98},{0,-80},{-60,-15}};
         int hr[5][2]={{-30,90},{59,-70},{165,-98},{268,-40},{295,-20}};
		 offy=50;   

         painthand(offset,orgs-offy,hand);
         if(hand==0){ p.x=offset+hr[dedoref-1][0]; p.y=orgs-offy+hr[dedoref-1][1]; }
         else{ p.x=offset+hl[dedoref-1][0]; p.y=orgs-offy+hl[dedoref-1][1]; }
         SelectObject(hdc, hFont);
         if(kiz.dedo!=ref.dedo)
            paintpentamemoespacial(p.x,p.y,ref.hbmp[0]);
         if(hand==0){ p.x=offset+hr[dedokiz-1][0]; p.y=orgs-offy+hr[dedokiz-1][1]; }
         else{ p.x=offset+hl[dedokiz-1][0]; p.y=orgs-offy+hl[dedokiz-1][1]; }
		 if(kiz.color==BLACK) paintpentamemoespacial(p.x,p.y,kiz.hbmp[0]);
         if(kiz.color==GREEN) paintpentamemoespacial(p.x,p.y,kiz.hbmp[1]);
         if(kiz.color==RED) paintpentamemoespacial(p.x,p.y,kiz.hbmp[2]);
	  }else{
         int hl[5][2]={{185,80},{140,-10},{70,-74},{-20,-63},{-75,20}};
         int hr[5][2]={{-30,30},{0,-56},{92,-74},{150,-40},{185,25}};
         offy=-30;   

         painthand(offset,orgs-offy,hand);			
         if(hand==0){ p.x=offset+hr[dedoref-1][0]; p.y=orgs-offy+hr[dedoref-1][1]; }
         else{ p.x=offset+hl[dedoref-1][0]; p.y=orgs-offy+hl[dedoref-1][1]; }
         SelectObject(hdc, hFont);
         if(kiz.dedo!=ref.dedo)
            paintpentamemoespacial(p.x,p.y,ref.hbmp[0]);
         if(hand==0){ p.x=offset+hr[dedokiz-1][0]; p.y=orgs-offy+hr[dedokiz-1][1]; }
         else{ p.x=offset+hl[dedokiz-1][0]; p.y=orgs-offy+hl[dedokiz-1][1]; }
		 if(kiz.color==BLACK) paintpentamemoespacial(p.x,p.y,kiz.hbmp[0]);
         if(kiz.color==GREEN) paintpentamemoespacial(p.x,p.y,kiz.hbmp[1]);
         if(kiz.color==RED) paintpentamemoespacial(p.x,p.y,kiz.hbmp[2]);
	  }
   }
   backp=testRect;
   testRect.left =ExtraSpace;
   testRect.right=testRect.top=testRect.bottom=0; 
   paintscores(&memoespacialscore);
   testRect=backp;	
   SetBkMode(hdc, OPAQUE);
   return;
}
void paintmemoespacial(int notakiz,int sbkiz,int dedokiz,int notaref,int sbref,int dedoref,int hand,int displaypenta)
{    
    HDC hdcmem,hdcbackup;   
    HBITMAP hbmmem;
	HGDIOBJ hold;
	int offset=0;

    hdcbackup=hdc;
    hdcmem = CreateCompatibleDC(hdc);
    hbmmem = CreateCompatibleBitmap(hdc,testRect.right-offset,top260);  //testRect.Bottom
    hdcbackup=hdc;
	hdc=hdcmem;
    hold=SelectObject(hdcmem, hbmmem);
	paintmemoespacialfunction(notakiz,sbkiz,dedokiz,notaref,sbref,dedoref,hand,displaypenta);
	hdc=hdcbackup;
    BitBlt(hdc,0,0,testRect.right-offset,top260,hdcmem, 0, 0, SRCCOPY);
    SelectObject(hdcmem, hold);
    DeleteObject(hbmmem);
    DeleteDC(hdcmem);
	return;
}

void memoespacialupdate(int nota)
{
   HDC hdc;

   hdc=GetDC(hwnd);
   if((kiz.nota==nota && concfg.memoespacial==1) || 
	  (kiz.abnota.notakey==nota && concfg.memoespacial==2)){
      kiz.color=GREEN;
	  ++memoespacialscore.hits;
   }else{ 
	   kiz.color=RED;
	  ++memoespacialscore.fails;
   }
   memoespacialscore.perhits=100.0*memoespacialscore.hits/(memoespacialscore.hits+memoespacialscore.fails);
   memoespacialscore.perfails=100.0*memoespacialscore.fails/(memoespacialscore.hits+memoespacialscore.fails);
   initmemoespacial(2);
   Sleep(200);
   ReleaseDC(hwnd,hdc);
   return;   
}

// s[x][y][z]
// x dedo origen
// y dedo destino
// z direccion 1-izquierda 0-derecha

void memoespacial_getspan(int dedoref,int dedokiz,int &spanl,int &spanr,int hand)
{
	int s[6][6][2];

	if(dedoref==dedokiz){
		spanl=spanr=0;
		return;
	}
	s[1][2][0]=5; s[1][2][1]=2; 
	s[1][3][0]=5; s[1][3][1]=3;
	s[1][4][0]=5; s[1][4][1]=4;
	s[1][5][0]=4; s[1][5][1]=4;

	s[2][1][0]=s[1][2][1];       s[2][1][1]=s[1][2][0];
	s[2][3][0]=2; s[2][3][1]=0;  
	s[2][4][0]=2; s[2][4][1]=2;
	s[2][5][0]=2; s[2][5][1]=2;

	s[3][1][0]=   s[1][3][1];    s[3][1][1]=s[1][3][0];
	s[3][2][0]=   s[2][3][1];    s[3][2][1]=s[2][3][0];
	s[3][4][0]=2; s[3][4][1]=0;
	s[3][5][0]=2; s[3][5][1]=0;

	s[4][1][0]=   s[1][4][1];    s[4][1][1]=s[1][4][0];
	s[4][2][0]=   s[2][4][1];    s[4][2][1]=s[2][4][0];
	s[4][3][0]=   s[3][4][1];    s[4][3][1]=s[3][4][0];
	s[4][5][0]=2; s[4][5][1]=0;

	s[5][1][0]=   s[1][5][1];    s[5][1][1]=s[1][5][0];    
	s[5][2][0]=   s[2][5][1];    s[5][2][1]=s[2][5][0];
	s[5][3][0]=   s[3][5][1];    s[5][3][1]=s[3][5][1];
	s[5][4][0]=   s[4][5][1];    s[5][4][1]=s[4][5][1];

	if(hand==0){
	   spanl=s[dedoref][dedokiz][1];
	   spanr=s[dedoref][dedokiz][0];
	}else{
	   spanl=s[dedoref][dedokiz][0];
	   spanr=s[dedoref][dedokiz][1];
	}
	return;
}

void notakeytonotaacci(int notakey,int &nota,int &acci)
{
   int r,sb;

   sb=(int)(uniform_deviate(rand())*2);		 
   sb=1-2*sb;
   for(r=0;r<44;r++){
	  nota=notas00[r];
      if(notakey==notas00[r]+sb){
         acci=(3-sb)/2;
		 return;
	  }
   }
   nota=notakey;
   acci=0;
   return;
}

//***************** en construccion ***************
void liberamemoespacialbmp(MEMOESPACIAL *me)
{
	if(me->hbmp[0]!=NULL){ DeleteObject(me->hbmp[0]); me->hbmp[0]=NULL; }
	if(me->hbmp[1]!=NULL){ DeleteObject(me->hbmp[1]); me->hbmp[1]=NULL; }
	if(me->hbmp[2]!=NULL){ DeleteObject(me->hbmp[2]); me->hbmp[2]=NULL; }
   return;
}

void initmemoespacial2(int gen)
{
   NOTA *h;
   int acci,hand,index,tin;
   int span,spanl,spanr;
   static int prevnotabaseref,prevnotakiz;
   
   hand=gfstat[0];
   if(gen==0){
      memoespacialscore.hits=memoespacialscore.fails=0;
      memoespacialscore.perhits=memoespacialscore.perfails=0;
      prevnotakiz=0;
      while(1){
         while(1){
            h=genrndnote();  
            if(hand==0){
	           if(h->penta!=0) continue;
	           if(h->nota<concfg.noteinfgeneric[0] || h->nota>concfg.notesupgeneric[0]) continue;
		          break;
			}else{
	           if(h->penta!=1) continue;
	           if(h->nota<concfg.noteinfgeneric[1] || h->nota>concfg.notesupgeneric[1]) continue;
		          break;
			}
	        break;
		 }
         acci=h->acci;
		 ref.abnota.acci=acci;
		 acci=3-2*acci; if(acci==3) acci=0;
		 ref.abnota.nota=h->nota;
         ref.abnota.penta=h->penta;
         ref.abnota.notakey=h->nota+acci;		 
         ref.nota=h->nota;
		 ref.sb=0;
		 ref.dedo=1+(int)(uniform_deviate(rand())*5);	
         if(prevnotabaseref!=ref.abnota.notakey){
            prevnotabaseref=ref.abnota.notakey;
	        break;
		 }
	  }
	  liberamemoespacialbmp(&ref);
      ref.hbmp[0]=nriautomaticbitmaps(ref.abnota.penta,ref.abnota.nota,ref.abnota.acci,BLUE);
   }
   if(gen==1){
	   while(1){ // aca
          kiz.dedo=1+(int)(uniform_deviate(rand())*5);
          if(ref.dedo==kiz.dedo) continue;
	      memoespacial_getspan(ref.dedo,kiz.dedo,spanl,spanr,hand);  // span viene dado en numero de semitonos alcanzables con el dedo
	      span=(int)(uniform_deviate(rand())*(spanl+spanr+1));
	      span=span-spanl;
		  index=calcnoteindex(ref.abnota.notakey);
          if(index==-1) index=calcnoteindex(ref.abnota.notakey-1);
          //kiz.abnota.notakey=ref.abnota.notakey+span;
		  tin=index+kiz.dedo-ref.dedo; 
		  if(tin<0) tin=0;
		  kiz.abnota.notakey=notas00[tin]+span;
		  kiz.abnota.penta=ref.abnota.penta;
          if(hand==0){
             if(kiz.abnota.notakey<concfg.noteinfgeneric[0] || kiz.abnota.notakey>concfg.notesupgeneric[0]) continue;
		  }else{
             if(kiz.abnota.notakey<concfg.noteinfgeneric[1] || kiz.abnota.notakey>concfg.notesupgeneric[1]) continue;
		  }  
          notakeytonotaacci(kiz.abnota.notakey,kiz.abnota.nota,kiz.abnota.acci);
		  kiz.nota=kiz.abnota.nota;
		  kiz.sb=0;
          if(prevnotakiz!=kiz.abnota.notakey){
		    prevnotakiz=kiz.abnota.notakey;
			kiz.color=BLACK;
			ref.color=BLUE;
			break;
		  }
	   }
	   liberamemoespacialbmp(&kiz);
       kiz.hbmp[0]=nriautomaticbitmaps(kiz.abnota.penta,kiz.abnota.nota,kiz.abnota.acci,BLACK);
	   kiz.hbmp[1]=nriautomaticbitmaps(kiz.abnota.penta,kiz.abnota.nota,kiz.abnota.acci,GREEN);
       kiz.hbmp[2]=nriautomaticbitmaps(kiz.abnota.penta,kiz.abnota.nota,kiz.abnota.acci,RED);
	   paintmemoespacial(kiz.nota,kiz.sb,kiz.dedo,ref.nota,ref.sb,ref.dedo,hand,1);
	}
	if(gen==2) 
		paintmemoespacial(kiz.nota,kiz.sb,kiz.dedo,ref.nota,ref.sb,ref.dedo,hand,1);
}
//*********************************************************

// hand=0 para mano derecha y 1 para la izquierda
// si gen==0 se genera nota en el dedo de apoyo fijo

void initmemoespacial1(int gen)
{
	int hand;
	static int prevnotabaseref,prevnotakiz;
	int span,spanl,spanr,index,notabasekiz;

	hand=gfstat[0];
	if(gen==0){
	   memoespacialscore.hits=memoespacialscore.fails=0;
	   memoespacialscore.perhits=memoespacialscore.perfails=0;
	   prevnotakiz=0;
       while(1){
	     ref.nota=(int)(uniform_deviate(rand())*12);
         ref.dedo=1+(int)(uniform_deviate(rand())*5);	
         ref.sb=(int)(uniform_deviate(rand())*2);		 
		 if(prevnotabaseref!=ref.nota){
		    prevnotabaseref=ref.nota;
			break;
		 }
	   }
	   return;
	}
	if(gen==1){
	   while(1){
          kiz.dedo=1+(int)(uniform_deviate(rand())*5);
	      if(isnatural(ref.nota)==-1)
		     index=isnatural(ref.nota-1)+kiz.dedo-ref.dedo;
	      else index=isnatural(ref.nota)+kiz.dedo-ref.dedo;
		  index=(index%7+((index>=0)?0:7))%7;
	      notabasekiz=natural(index);
          if(ref.dedo==kiz.dedo) continue;
	      memoespacial_getspan(ref.dedo,kiz.dedo,spanl,spanr,hand);  // span viene dado en numero de semitonos alcanzables con el dedo
	      span=(int)(uniform_deviate(rand())*(spanl+spanr+1));
	      span=span-spanl;
          kiz.nota=((notabasekiz+span)%12+((notabasekiz+span>=0)?0:12))%12;
          kiz.sb=(int)(uniform_deviate(rand())*2);
          if(prevnotakiz!=kiz.nota){
		    prevnotakiz=kiz.nota;
			kiz.color=BLACK;
			ref.color=BLUE;
			break;
		  }
	   }
	   paintmemoespacial(kiz.nota,kiz.sb,kiz.dedo,ref.nota,ref.sb,ref.dedo,hand,0);
	}
	if(gen==2) 
		paintmemoespacial(kiz.nota,kiz.sb,kiz.dedo,ref.nota,ref.sb,ref.dedo,hand,0);
	return;
}

void initmemoespacial(int gen)
{
	if(concfg.memoespacial==1)
       initmemoespacial1(gen);
	else
       initmemoespacial2(gen);
    return;
}

void notereadingupdate(void)
{
	int numstaff;

	numstaff=gfstat[0]+2*gfstat[1];
    if(concfg.notereadingchord==1)
	   Paintnotereading(RandomNote,randomcannote,numstaff,concfg.armaduranotereading,1);        
    else
	   Paintnotereading(RandomNote,notereadingdeep,numstaff,concfg.armaduranotereading,1);        
	return;
}

void notereadingfail(int mnota,int index,int pintanota)
{
	int orgs,orgf,r,k,j,acci,numstaff;
    int RandomNoteLoc,offsetn;

	orgs=gStaff[64-36]-295;
	orgf=fStaff[43-36]-240;	
	if(gfstat[0]==1)
	   if(mnota<concfg.noteinfgeneric[0] || mnota>concfg.notesupgeneric[0]) return;
	if(gfstat[1]==1)
	   if(mnota<concfg.noteinfgeneric[1] || mnota>concfg.notesupgeneric[1]) return;
	numstaff=gfstat[0]+2*gfstat[1];
	//Paintnotereading(&RandomNote[0],1,numstaff,concfg.armaduranotereading,pintanota);
    if(concfg.notereadingchord==1)
	   Paintnotereading(RandomNote,randomcannote,numstaff,concfg.armaduranotereading,pintanota);        
    else
	   Paintnotereading(RandomNote,notereadingdeep,numstaff,concfg.armaduranotereading,pintanota);        
	SetTextColor(hdc,BLACK);
	SelectObject(hdc, hMaestroFont);
	SetBkMode(hdc, TRANSPARENT);
	r=RandomNote[index].penta;
	offsetn=RandomNote[index].posx;
	for(j=acci=0;j<37;j++) if(notas00[j]==mnota) break; 
    if(j==37)
       for(j=0;j<37;j++) if(notas00[j]==mnota-1){ mnota=notas00[j]; acci=1; break; }	
    if(j==37)
		exit(5);
    k=j; 
	if(mnota>concfg.notesupgeneric[1]) r=0;
	if(mnota<concfg.noteinfgeneric[0]) r=1;
    if(r==1){
	   RandomNoteLoc = orgf-(k-4)*ntinterline*0.5;
	   SetTextColor(hdc,BLACK);	 
	   if(mnota <= 36)
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgf+2*ntinterline, "___", 2);
       if(mnota <= 40) 
		  TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgf+ntinterline, "___", 2);
	   if(mnota >= 60)
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgf-5*ntinterline, "___", 2);
	   if(mnota >= 64)				
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgf-6*ntinterline, "___", 2);
	}
	if(r==0){
	   SetTextColor(hdc,BLACK);
	   RandomNoteLoc = orgs-(k-16)*ntinterline*0.5; 
	   if(mnota <= 57)
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgs+2*ntinterline, "___", 2);
	   if(mnota <= 60) 
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgs+ntinterline, "___", 2);
	   if(mnota >= 81)
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgs-5*ntinterline, "___", 2);
	   if(mnota >= 84)
	      TextOut(hdc,top260min30 + offsetn + StaffWidth[0], orgs-6*ntinterline, "___", 2);			 			 					 
	}
	// dibuja notas musicales			 
    if(acci==0) SetTextColor(hdc,RED);
    else SetTextColor(hdc,RGB(114,56,56));
    SelectObject(hdc, hMaestroFontSmall);    			
	TextOut(hdc,top260min30 + offsetn + (StaffWidth[0]+(NoteWidth[0]/2)),RandomNoteLoc+32,"w",1);
	SetTextColor(hdc, 0);
	SelectObject(hdc, hMaestroFont);
	SetBkMode(hdc, OPAQUE);
}

char *getnumtonum(int num)
{
	int count,r,j;
	static char s[8];
	char f[8];

	count=s[0]=0;
	if(num==0) count=1;
    while(num!=0){	   
	   s[count++]=num%10;
       num=num/10;
	}
    for(r=0;r<count;r++){
	   switch(s[r]){
	      case 0: s[r]=188; break;
		  case 1: s[r]=193; break;
		  case 2: s[r]=170; break;
		  case 3: s[r]=163; break;
		  case 4: s[r]=162; break;
		  case 5: s[r]=176; break;
		  case 6: s[r]=164; break;
		  case 7: s[r]=166; break;
		  case 8: s[r]=165; break;
		  case 9: s[r]=187; break;
	   }
	}
	s[r]=0;
	for(j=0;j<r;j++) f[r-1-j]=s[j];
	f[j]=0;
	strcpy(s,f);
	return s;
}

char *gettiponotasingle(int dura)
{
   static char s[4];

   s[0]=s[1]=0;
   if(dura==1) s[0]=119; // redonda
   if(dura==2) s[0]=72; // blanca
   if(dura==4) s[0]=81;  // negra
   if(dura==8) s[0]=69; // corchea
   if(dura==16) s[0]=88; // semicorchea
   return s;
}

int getindexchordbase(MELODIA *melodia,int r,int c,int v,int interln,int updo,int *&dsp,int *&dspy,int cl,int &mnmx)
{
    __int64 chords,chord;
	int mn,mx,n,imn,imx,chordp;
	int cho[10],totcho,s;
	static int dispx[10],dispy[10];

	mnmx=0;
    dsp=dispx; 
	dspy=dispy;
	dispx[0]=dispx[1]=dispx[2]=dispx[3]=dispx[4]=0;
	dispy[0]=dispy[1]=dispy[2]=dispy[3]=dispy[4]=0;
	if(cl==2) return -1;   // se trata de un acorde de redondas, no hacemos nada
	chords=chord=melodia->penta[r].compas[c].note[v];
	if(chord==0) return -1;
	mn=10000; mx=-10000;
	for(chordp=0;chord!=0;chord>>=8,++chordp){
	   n=(int) chord&0xff;
	   if(n>mx){ mx=n; imx=chordp; }
	   if(n<mn){ mn=n; imn=chordp; }
	   cho[chordp]=n;
	}
	totcho=chordp;
	chord=chords;
    for(chordp=0;chord!=0;chord>>=8,++chordp){
	   if(updo==1){
	      if(chordp==imx) continue;
		  //melodia->penta[r].compas[c].posy[v][chordp]-=0.5*interln;
		  dispy[chordp]=0.5*interln;
	   }
	   if(updo==-1){
		  if(chordp==imn) continue;
          //melodia->penta[r].compas[c].posy[v][chordp]-=0.5*interln;
		  dispy[chordp]=0.5*interln;
	   }
	}
	if(updo==-1){
	   for(s=totcho-1;s>0;s--){
		  if(abs(cho[s]-cho[s-1])<=2){
		     dispx[s-1]=-1; s--;
			 mnmx=-1;
		  }
	   }
	}else{
	   for(s=1;s<totcho;s++){
	      if(abs(cho[s]-cho[s-1])<=2){
		     dispx[s]=1; s++;
			 mnmx=1;
		  }
	   }
	}
    if(updo==1) return imx;
	if(updo==-1) return imn;
	return -1;
}

void paintclaveonpenta(HDC hdc,MELODIA *melodia,int r,int c,int v,int posxacci)
{
	int clef,clefa,posxclef,color,width,orgs,orgf;
	HGDIOBJ obj;
	COLORREF clobj;

	orgs=gStaff[64-36]-250;
	orgf=fStaff[43-36]-250;	
	if(melodia->penta[r].compas[c].note[v]==0) return;
	color=melodia->penta[r].compas[c].color[v];
	clef=melodia->penta[r].compas[c].clef[v];
	if(v>0) clefa=melodia->penta[r].compas[c].clef[v-1];
	else clefa=r;
	if(clef==clefa) return;
    if(r==0){
	   if(clef==0 && clefa==0) return;
       if(clef==1){
	      obj=SelectObject(hdc, hMaestroFontSmall);	
	      GetCharWidth32(hdc,'?','?',&width); 	
		  posxclef=posxacci-width;
		  clobj=SetTextColor(hdc,color);
		  TextOut(hdc,posxclef,orgs-interline, "?",1);
	   }else{
	      obj=SelectObject(hdc, hMaestroFontSmall);	
	      GetCharWidth32(hdc,'&','&',&width); 	
		  posxclef=posxacci-width;
		  clobj=SetTextColor(hdc,color);
          TextOut(hdc,posxclef,orgs+interline, "&",1);	      
	   }		  
	}else{
       if(clef==1 && clefa==1) return;
       if(clef==0){
	      obj=SelectObject(hdc, hMaestroFontSmall);	
	      GetCharWidth32(hdc,'&','&',&width); 	
		  posxclef=posxacci-width;
		  clobj=SetTextColor(hdc,color);
          TextOut(hdc,posxclef,orgf+interline, "&",1);	      
	   }else{
	      obj=SelectObject(hdc, hMaestroFontSmall);	
	      GetCharWidth32(hdc,'?','?',&width); 	
		  posxclef=posxacci-width;
		  clobj=SetTextColor(hdc,color);
		  TextOut(hdc,posxclef,orgf-interline, "?",1);
	   }
	}
    SelectObject(hdc,obj);
    SetTextColor(hdc,clobj);
    return;    
}

void paintstickchord(HDC hdc,MELODIA *melodia,int r,int c,int v,int *dispy,int chordp,int updo)
{
	int posx,posy0,posy1;
	int color,width,wd=0;
	TEXTMETRIC tm;
    POINT p[2];
	HPEN pen;
	ABC abc;

	if(melodia->penta[r].compas[c].dura[v]>=1) return;
    GetTextMetrics (hdc, &tm) ;	
	GetCharWidth32(hdc,32,32,&width); 	
	GetCharABCWidths(hdc,207,207,&abc);
	if(updo==1) wd=abc.abcA+abc.abcB-1;
	posx=melodia->penta[r].compas[c].posx[v]+2*width+wd; 
	posy0=melodia->penta[r].compas[c].posy[v][0]-dispy[0]+tm.tmAscent;
	posy1=melodia->penta[r].compas[c].posy[v][chordp-1]-dispy[chordp-1]+tm.tmAscent;
	if(updo==1) posy0-=5; 
    color=melodia->penta[r].compas[c].color[v];
    pen=CreatePen(PS_SOLID,1,color);
	SelectObject(hdc,pen);
 	p[0].x=posx;     p[1].x=posx;
    p[0].y=posy0; p[1].y=posy1;
    Polyline(hdc,p,2);
	DeleteObject(pen);
}

void paintties(HDC hdc,MELODIA *melodia,int r,int c,int v,int interln,int orgs,int orgf)
{
	HPEN pen;   
	TEXTMETRIC tm;
	__int64 tie,chord,chordn;
	int posxo,posyo,posxf,posyf,chordp,stie,schord,width,chordc,vchord;

	GetTextMetrics (hdc,&tm) ;	
	GetCharWidth32(hdc,207,207,&width); 	
    pen=CreatePen(PS_SOLID,1,RED); 
	SelectObject(hdc,pen);
  	tie=melodia->penta[r].compas[c].tie[v-1];
	chord=melodia->penta[r].compas[c].note[v-1];
    for(chordp=0;tie!=0;tie>>=8,chord>>=8,chordp++){				   	   
	   stie=(int) tie&0xff;
	   schord=(int) chord&0xff;
       if(stie!=0 && schord!=0){
	      posxo=melodia->penta[r].compas[c].posxfin[v-1][chordp]+0.5*width;
          posyo=melodia->penta[r].compas[c].posy[v-1][chordp]+5*interln; //en prueba
		  chordn=melodia->penta[r].compas[c].note[v];
		  for(chordc=0;chordn!=0;chordn>>=8,chordc++){				   	   
             vchord=(int) chordn&0xff;
             if(vchord==schord) break;
		  }          
		  posxf=melodia->penta[r].compas[c].posxfin[v][chordc]+0.5*width;		  
		  posyf=melodia->penta[r].compas[c].posy[v][chordc]+5*interln; //en prueba;
		  if(posyo==posyf){
		     Arc(hdc,posxo,posyo,posxf,posyf+interln,
			    posxo,posyo+(int) (0.5*interln),posxf,posyf+(int) (0.5*interln));
		  }else{
			  POINT p[2];
			  p[0].x=posxo; p[0].y=posyo;
			  p[1].x=posxf; p[1].y=posyf;
			  Polyline(hdc,p,2);
		  }	 
	   }
	}
	DeleteObject(pen);
}


// cl=0 figura negra, cl=1 figura negra
char *gettiponota(MELODIA *melodia,int penta,int compas,int pos,int &updo,int &cl)
{
	int h,tupla;
	double val;
	static char s[5];

	val=melodia->penta[penta].compas[compas].dura[pos];
	tupla=melodia->penta[penta].compas[compas].tupla[pos];
	if(tupla==3)     // se trata de un tresillo
       val=val*3/2;  // obtenemos la figura equivalente sin el tresillo 
	val=round(val,6);
	h=(int) melodia->penta[penta].compas[compas].note[pos]&0xff;
	s[0]=s[1]=s[2]=s[3]=s[4]=0; cl=0;
	if(h>0){
	   if((penta==0 && h<71) || (penta==1 && h<50)){
		  updo=1;
		  if(val==1.0){ s[0]=119; cl=2; }// redonda
		  if(val==0.5){ s[0]=104; cl=1; }// blanca
	      if(val==0.25) s[0]=113;  // negra
	      if(val==0.125) s[0]=101; // corchea
	      if(val==0.0625) s[0]=120; // semicorchea
		  if(val==0.03125){ s[0]=120; s[3]=145; s[4]=1; }  // fusa
		  if(val==0.015625){ s[0]=120; s[3]=144; s[4]=2; } // semifusa
	      if(val==1.5)   { s[0]=119; s[1]=107; cl=1; } // redonda con punto
	      if(val==0.75)  { s[0]=104; s[1]=107; cl=1; } // blanca con punto
	      if(val==0.375) { s[0]=113; s[1]=107; } // negra con punto
	      if(val==0.1875){ s[0]=101; s[1]=107; } // corchea con punto
		  if(val==0.09375){ s[0]=120; s[1]=107; } // semicorchea con punto
          if(val==0.046875){ s[0]=120; s[1]=107; s[3]=145; s[4]=1; } // fusa con punto
	   }else{
		  updo=-1;
		  if(val==1.0){ s[0]=119; cl=2;} // redonda
		  if(val==0.5){ s[0]=72; cl=1;} // blanca
	      if(val==0.25) s[0]=81;  // negra
	      if(val==0.125) s[0]=69; // corchea
	      if(val==0.0625) s[0]=88; // semicorchea
		  if(val==0.03125){ s[0]=88; s[3]=239; s[4]=1; }  // fusa
		  if(val==0.015625){ s[0]=88; s[3]=146; s[4]=2; }  // semifusa
	      if(val==1.5)   { s[0]=119; s[1]=107; cl=1; } // redonda con punto
	      if(val==0.75)  { s[0]=72; s[1]=107; cl=1; } // blanca con punto
	      if(val==0.375) { s[0]=81; s[1]=107; } // negra con punto
	      if(val==0.1875){ s[0]=69; s[1]=107; } // corchea con punto
		  if(val==0.09375){ s[0]=88; s[1]=107; } // semicorchea con punto
		  if(val==0.046875){ s[0]=88; s[1]=107; s[3]=239; s[4]=1;} // fusa con punto
	   }
	}else{ //silencio
	   updo=0;
	   if(val==1.0) s[0]=183; // redonda
	   if(val==0.5) s[0]=238; // blanca
	   if(val==0.25) s[0]=206;  // negra
	   if(val==0.125) s[0]=228; // corchea
	   if(val==0.0625) s[0]=197; // semicorchea
	   if(val==0.03125) s[0]=168; // fusa
	   if(val==0.015625) s[0]=244; // semifusa
	   if(val==1.5)   { s[0]=183; s[1]=107; } // redonda con punto
	   if(val==0.75)  { s[0]=238; s[1]=107; } // blanca con punto
	   if(val==0.375) { s[0]=206; s[1]=107; } // negra con punto
	   if(val==0.1875){ s[0]=228; s[1]=107; } // corchea con punto
	   if(val==0.09375){ s[0]=197; s[1]=107; } // semicorchea con punto
	   if(val==0.046875){ s[0]=168; s[1]=107; } // fusa con punto
	}
	return s;
}

int isnotewhole(MELODIA *melodia,int penta,int compas,int pos)
{
    double val;
    
	val=melodia->penta[penta].compas[compas].dura[pos];
	val=round(val,6);
	if(val==1.0) return 1;
	return 0;
}

int compare( const void *arg1, const void *arg2 )
{
   int x,y;

   x=*((int *) arg1);
   y=*((int *) arg2);
   if(x<y) return -1;
   if(x==y) return 0;
   if(x>y) return 1;
   return 1;
}

void scrollrandommusic(MELODIA *melodia,int scroll)
{
	int r,c,v,t,j,valme,valma,eq,m,chordp,mm;
	int soltofa[]={0,1,1,0,0,1,1};
	int fatosol[]={1,1,0,1,1,0,0};
	__int64 chord;
	int numnotas,nota,posme,posma,mean;
    PENTA *penta;
	EJECUCION *eje;
    int orgs,orgf,adjust;
	int maxcompascreen,miny,maxy;

	maxcompascreen=concfg.maxcompascreen;
	miny=10000; maxy=-10000;
	orgs=gStaff[64-36]-250;
	orgf=fStaff[43-36]-250;	
    penta=melodia->penta;	
	if(concfg.maxcompascreen==1){
	   int m0,m1,res;
	   c=offsetdispinterval;
	   m0=melodia->penta[0].compas[c].numnotas;
	   m1=melodia->penta[1].compas[c].numnotas;
	   if(m0>=m1) res=m0*50;
	   else res=m1*50;
	   BARTOBARSPACE=res*1.0/StaffWidth[1];
	   if(BARTOBARSPACE>NumOfStaffs-OFFSETPENTA+1)
	      BARTOBARSPACE=NumOfStaffs-OFFSETPENTA;
    }
	for(r=0;r<2;r++){
		for(c=offsetdispinterval-1;c<offsetdispinterval+maxcompascreen;c++){
		  if(c<0) c=0;
		  if(c>=melodia->numcompases) break;
		  numnotas=penta[r].compas[c].numnotas;
		  penta[r].compas[c].posxcompas=ExtraSpace+OFFSETPENTA*StaffWidth[1]+c*BARTOBARSPACE*StaffWidth[1];
		  penta[r].compas[c].posxcompas-=scroll*BARTOBARSPACE*StaffWidth[1];
		  penta[r].compas[c].posxfincompas=penta[r].compas[c].posxcompas+BARTOBARSPACE*StaffWidth[1];
	      for(v=0;v<numnotas;v++){
             penta[r].compas[c].posx[v]=ExtraSpace+OFFSETPENTA*StaffWidth[1]+c*BARTOBARSPACE*StaffWidth[1]+NoteWidth[1]+v*((BARTOBARSPACE*StaffWidth[1]-NoteWidth[1])/numnotas);
			 penta[r].compas[c].posx[v]-=scroll*BARTOBARSPACE*StaffWidth[1];
			 if(!isnotewhole(melodia,r,c,v)) adjust=-0.5*interline; 
			 else adjust=0;
			 chord=penta[r].compas[c].note[v];
			 for(chordp=mm=0;chord!=0 || (chord==0 && chordp==0) ;chord>>=8,++chordp){
		        nota=(int) chord&0xff;  
				if(r==0 && nota!=0 && nota<55) mm=1;  //minima nota en clave de sol
				if(r==1 && nota!=0 && nota>64) mm=1;
			 }
			 chord=penta[r].compas[c].note[v];
			 for(chordp=0;chord!=0 || (chord==0 && chordp==0) ;chord>>=8,++chordp){
			    nota=(int) chord&0xff;  
			    if(r==0){ 
				   if(nota==0) nota=71;
				   if(nota>=36)
				      penta[r].compas[c].posy[v][chordp]=gStaff[nota-36]-250-adjust;	
                   penta[r].compas[c].posycompas=orgs;
				   penta[r].compas[c].clef[v]=0;
				   if(mm==1){
					  int ss;
					  // notas ocupando el pentagrama de fa en clave de sol
					   // si nota es "mi" o  nota es si sumar 20
					   // en otro caso sumar 21
                      ss=calcnoteindex(nota);  
                      ss=ss%7;
					  ss=20+fatosol[ss];
                      //if(ss==2 || ss==6) ss=20; else ss=21; // revisar esta linea
					  penta[r].compas[c].posy[v][chordp]=gStaff[nota+ss-36]-250-adjust;	
					  penta[r].compas[c].clef[v]=1;
				   }
				}else{
				   if(nota==0) nota=50;
				   if(nota>=36)
				      penta[r].compas[c].posy[v][chordp]=fStaff[nota-36]-250-adjust;
				   else					  
					  penta[r].compas[c].posy[v][chordp]=outminfStaff[36-nota]-250-adjust;
				   penta[r].compas[c].posycompas=orgf;
				   penta[r].compas[c].clef[v]=1;
				   if(mm==1){
					  int ss;
					  // notas ocupando el pentagrama de sol en clave de fa
                      ss=calcnoteindex(nota);  
                      ss=ss%7;
					  ss=20+soltofa[ss];
                      //if(ss==2 || ss==6) ss=21; else ss=20;
                      if(nota-ss-36>=0) 
					     penta[r].compas[c].posy[v][chordp]=fStaff[nota-ss-36]-250-adjust;
					  else
						 penta[r].compas[c].posy[v][chordp]=outminfStaff[36-nota+ss]-250-adjust;
					  penta[r].compas[c].clef[v]=0;
				   }
				}
				if(c>=offsetdispinterval){
                   if(penta[r].compas[c].posy[v][chordp]<miny)
				      miny=penta[r].compas[c].posy[v][chordp];
                   if(penta[r].compas[c].posy[v][chordp]>maxy)
                      maxy=penta[r].compas[c].posy[v][chordp];
				}
			 }
		  }
	   }
	}
	for(c=offsetdispinterval;c<offsetdispinterval+maxcompascreen;c++){
	   if(c>=melodia->numcompases) break;
	   r=j=1;
       if(penta[0].compas[c].numnotas<=penta[1].compas[c].numnotas) r=0;
	   if(penta[0].compas[c].numnotas>penta[1].compas[c].numnotas) j=0;
	   if(penta[0].compas[c].numnotas==penta[1].compas[c].numnotas) eq=1; else eq=0;
	   eje=generaejecucion(melodia->penta,c);
	   index=eje->penta[0].hor;
	   numnotas=melodia->penta[r].compas[c].numnotas;
	   for(v=0,valme=valma=-1;v<index;v++){
		   posme=eje->penta[r].runnota[v].pos; 
		   posma=eje->penta[j].runnota[v].pos; 
		   if(valme==posme &&
			  melodia->penta[j].compas[c].posx[posme]>=melodia->penta[j].compas[c].posx[valme]){
			  continue;
		   }
		   if(valma==posma && 
			  melodia->penta[r].compas[c].posx[posme]>=melodia->penta[r].compas[c].posx[valme]){
		      continue;
		   }
		   melodia->penta[r].compas[c].posx[posme]=melodia->penta[j].compas[c].posx[posma];
	       valme=posme;
		   valma=posma;
	   } 
	   if(eq==1){
	      numnotas=melodia->penta[0].compas[c].numnotas;
		  for(t=0;t<numnotas;t++){
             for(v=t+1;v<numnotas;v++){
			    mean=(BARTOBARSPACE*StaffWidth[1]-NoteWidth[1])/(2*numnotas);
			    if(melodia->penta[0].compas[c].posx[v]==melodia->penta[0].compas[c].posx[t]){
				   for(m=0;m<index;m++)
				      if(eje->penta[0].runnota[m].pos==v) break;
				   if(eje->penta[0].runnota[m].pos>eje->penta[1].runnota[m].pos)					
                      melodia->penta[0].compas[c].posx[t]-=mean;
				   else melodia->penta[0].compas[c].posx[v]+=mean;
				}
			 }
		  }
	   }
       delete [] eje;
	}
 
	int rmargen,lmargen,disteo[2],disrea[2],nnmin,nnmax;
	double prop[2],fac,dist[2][128];

    for(c=offsetdispinterval;c<offsetdispinterval+maxcompascreen;c++){
	   if(c>=melodia->numcompases) break;
	   for(r=0;r<2;r++){
	      numnotas=melodia->penta[r].compas[c].numnotas;
	      lmargen=ExtraSpace+OFFSETPENTA*StaffWidth[1]+c*BARTOBARSPACE*StaffWidth[1]+NoteWidth[1];
          rmargen=ExtraSpace+OFFSETPENTA*StaffWidth[1]+c*BARTOBARSPACE*StaffWidth[1]+NoteWidth[1]+(numnotas-1)*((BARTOBARSPACE*StaffWidth[1]-NoteWidth[1])/numnotas);
	      disteo[r]=rmargen-lmargen;
		  disrea[r]=melodia->penta[r].compas[c].posx[numnotas-1]-melodia->penta[r].compas[c].posx[0];
	   }
       if(disrea[0]<=disteo[0] && disrea[1]<=disteo[1])
	      continue;
       prop[0]=disteo[0]*1.0/disrea[0];
	   prop[1]=disteo[1]*1.0/disrea[1];
	   if(disrea[0]>=disrea[1]) t=0; else t=1;
       fac=prop[t];
	   for(r=0;r<2;r++){
          numnotas=melodia->penta[r].compas[c].numnotas;
	      for(v=1;v<numnotas;v++){
             dist[r][v-1]=melodia->penta[r].compas[c].posx[v]-melodia->penta[r].compas[c].posx[v-1];
             dist[r][v-1]*=fac;
		  }
	   }
	   nnmin=nnmax=-1;
	   for(r=0;r<2;r++){
		  numnotas=melodia->penta[r].compas[c].numnotas;
          for(v=0;v<numnotas-1;v++){
	         if(dist[r][v]==0 && nnmin==-1) nnmin=v;
		     if(nnmin>=0 && dist[r][v]!=0){
		        nnmax=v;
                fac=dist[r][v]/(nnmax-nnmin+1);
			    for(j=nnmin;j<v;j++)
			       dist[r][j]=fac;   
                nnmin=nnmax-1;
				continue;
			 }
			 if(nnmin>=0 && dist[r][v]==0 && v==numnotas-2){
				nnmax=numnotas-1;
                fac=dist[r][nnmin-1]/(nnmax-nnmin+1);
				for(j=nnmin;j<numnotas;j++)
				   dist[r][j-1]=fac;
			 }
		  }
	   }
	   for(r=0;r<2;r++){
          numnotas=melodia->penta[r].compas[c].numnotas;
	      for(v=1;v<numnotas;v++)
             melodia->penta[r].compas[c].posx[v]=melodia->penta[r].compas[c].posx[v-1]+dist[r][v-1];
	   }
	}         
	for(c=offsetdispinterval;c<offsetdispinterval+maxcompascreen;c++){
	   if(c>=melodia->numcompases) break;
	   for(r=0;r<2;r++){
		  numnotas=melodia->penta[r].compas[c].numnotas;
	      qsort((void *) melodia->penta[r].compas[c].posx,(size_t) numnotas,sizeof(int),compare);
	   }
	}
	for(r=0;r<20;r++) if(orgs-r*interline<miny) break;
	posyminpenta=orgs-r*interline;
	for(r=0;r<20;r++) if(orgf+r*interline>maxy) break;
    posymaxpenta=orgf+r*interline;
	if(posyminpenta>=orgs) posyminpenta=orgs-2*interline;
	if(posymaxpenta<orgf) posymaxpenta=orgf;
	return;
}

void PaintGradientRect(HDC hDC, const RECT &rect,COLORREF col1,COLORREF col2,int iFillDir,int gradient)
{
   BYTE r1 = BYTE((col1 & 0XFF)),
   g1 = BYTE((col1 & 0XFF00) >> 8),
   b1 = BYTE((col1 & 0XFF0000) >> 16),
   r2 = BYTE((col2 & 0XFF)),
   g2 = BYTE((col2 & 0XFF00) >> 8),
   b2 = BYTE((col2 & 0XFF0000) >> 16);
   BYTE bRDiff = (r1 < r2 ) ? r2 - r1:r1 - r2;
   BYTE bGDiff = (g1 < g2 ) ? g2 - g1:g1 - g2;
   BYTE bBDiff = (b1 < b2 ) ? b2 - b1:b1 - b2;
   int max = bRDiff ;

   //const
	   int nNumColors = (max > bGDiff) ?
                        (max > bBDiff) ? max : bBDiff: (bGDiff > bBDiff) ? bGDiff :bBDiff;
   if(gradient==0) nNumColors=1;  // si queremos rectangulo solido gradiente 0 sino 1
   max = nNumColors;     
   int m_nPaintSteps = nNumColors;
   RECT rectVar = { rect.left, rect.top, rect.left, rect.top };
   int nTotalSize;
   if (iFillDir){
      rectVar.right = rect.right;
      nTotalSize = rect.bottom - rect.top;
   }
   else{
      rectVar.bottom = rect.bottom;
      nTotalSize = rect.right - rect.left;
   }
   float R = r1,G = g1,B = b1;
   float rInc = ((float)bRDiff / (float)max),
   gInc = ((float)bGDiff / (float)max),
   bInc = ((float)bBDiff / (float)max);
   for (int nIndex = 0; nIndex < m_nPaintSteps; nIndex++){
   if (iFillDir){
      rectVar.top = rectVar.bottom;
      rectVar.bottom = rect.top +
      ::MulDiv(nIndex + 1, nTotalSize, m_nPaintSteps);
   }else{
      rectVar.left = rectVar.right;
      rectVar.right = rect.left +
      ::MulDiv(nIndex + 1, nTotalSize, m_nPaintSteps);
   }
   if(r1 < r2){
      if(R < r2)
        R += rInc ;
   }else{
      if(R > r2)
      R -= rInc ;
   }
   if(g1 < g2){
      if(G < g2)
        G += gInc;
   }else{
      if(G > g2)
      G -= gInc;
   }
   if(b1 < b2){
   if(B < b2)
      B += bInc;
   }else{
      if(B > b2)
      B -= bInc;
   }
   const COLORREF clrBr = RGB(ceil(R),ceil(G),ceil(B));
   HBRUSH brush = CreateSolidBrush(clrBr) ;
   FillRect(hDC,&rectVar, brush);
   DeleteObject(brush);
   }
}

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;

    GetObject(hbmColour, sizeof(BITMAP), &bm);
    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);
    SelectObject(hdcMem, hbmColour);
    SelectObject(hdcMem2, hbmMask);
    SetBkColor(hdcMem, crTransparent);
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);
    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);
    return hbmMask;
}

void paintsplash(int posx,int posy,int upd)
{
    HDC MemDC;
	RECT rect;
	static int fin;
    
	if(upd==0 || fin==1){ fin=1; return;}
	GetClientRect(hwnd,&rect);
	MemDC = CreateCompatibleDC(hdc);
	SelectObject(MemDC, bmpsplash);
	posx=(rect.left+rect.right)/2.0-639/2;
	posy=rect.top;
	BitBlt(hdc, posx, posy,639,275, MemDC,0,0, SRCCOPY);
	DeleteDC(MemDC);
    return;
}

void paintpentamemoespacial(int posx,int posy,HBITMAP hbmp)
{
    HDC MemDC;
	double facx,facy;

	facx=currenthor*1.5/1024;
	facy=currentver*1.25/768;
	MemDC = CreateCompatibleDC(hdc);
	SelectObject(MemDC, hbmp);
	BitBlt(hdc, posx, posy,50*facx,60*facy, MemDC,0,0, SRCCOPY);
	DeleteDC(MemDC);
    return;
}

void paintok(int posx,int posy,int yn,int score)
{
    HDC MemDC;
	char aa[10];

	MemDC = CreateCompatibleDC(hdc);
	if(yn==1) SelectObject(MemDC, bmpokay);
	else SelectObject(MemDC, bmpokan);
	if(currenthor==1024) BitBlt(hdc, posx, posy,100,162, MemDC,0,0, SRCCOPY);
	else BitBlt(hdc, posx-40, posy,160,262, MemDC,0,0, SRCCOPY);
	_itoa(score,aa,10);
	if(yn==1) mydrawtext2(BLACK,hfontsystemg,aa,posx+25,posy+100);
	else mydrawtext2(BLACK,hfontsystemg,aa,posx+40,posy+45);
	DeleteDC(MemDC);
    return;
}

void painthand(int posx,int posy,int lr)
{
    HDC MemDC;

	MemDC = CreateCompatibleDC(hdc);
	if(lr==0) SelectObject(MemDC, bmphandr);
	else SelectObject(MemDC, bmphandl);
	//BitBlt(hdc, posx, posy,350,438, MemDC,0,0, SRCCOPY);
	if(currenthor==1024) StretchBlt(hdc,posx,posy,178,240,MemDC,0,0,178,240,SRCCOPY);
	else StretchBlt(hdc,posx,posy,318,427,MemDC,0,0,318,427,SRCCOPY);
	if(lr==0) mydrawtext2(BLACK,hfontsystemg,"R",posx+100,posy+140);
	else mydrawtext2(BLACK,hfontsystemg,"L",posx+70,posy+140);
	DeleteDC(MemDC);
    return;
}

void paintrect(RECT *rec)
{
    HDC hdc;
	HPEN pen;
	HBRUSH hbr;
    COLORREF col;

	
    col=GREEN;
	hdc=GetDC(hwnd);
	SetBkMode(hdc,OPAQUE);
	hbr=CreateSolidBrush(col);
	pen=CreatePen(PS_SOLID,1,col);
	SelectObject(hdc,pen);
    hOldBrush=SelectObject(hdc,hbr);
	Rectangle(hdc,rec->left,rec->top,rec->right,rec->bottom);
	SelectObject(hdc,hOldBrush);
	DeleteObject(hbr);
	DeleteObject(pen);
	ReleaseDC(hwnd,hdc);
    return;
}
//***************


void paintcheck(int posx,int posy)
{   
	HDC MemDC;
	MemDC = CreateCompatibleDC(hdc);
    SelectObject(MemDC, bmpcheck);
	//StretchBlt(hdc, posx, posy, 10, 20, MemDC, 0, 0,93,86, SRCCOPY);
	BitBlt(hdc, posx, posy, 10, 20, MemDC, 0, 0, SRCCOPY);
	DeleteDC(MemDC);
    return;
}

void paintbomba(int posx0,int posy0,int posx1,int posy1,COLORREF col)
{
    HDC hdc;
	HPEN pen;
	HBRUSH hbr;

	hdc=GetDC(hwnd);
	SetBkMode(hdc,OPAQUE);
	hbr=CreateSolidBrush(col);
	pen=CreatePen(PS_SOLID,1,col);
	SelectObject(hdc,pen);
    hOldBrush=SelectObject(hdc,hbr);
    if(currenthor==1024 && currentver==768){
	   if(concfg.maxcompascreen<=4){ 
	      posx0-=2; posy0-=2;
		  posx1+=2; posy1+=2;
	   }
	}

	Ellipse(hdc,posx0,posy0,posx1,posy1);
	SelectObject(hdc,hOldBrush);
	DeleteObject(hbr);
	DeleteObject(pen);
	ReleaseDC(hwnd,hdc);
    return;
}

void paintkey(int posx,int posy)
{
    HDC MemDC;

	MemDC = CreateCompatibleDC(hdc);
    SelectObject(MemDC, bmpkey);
	StretchBlt(hdc, posx, posy, 10, 15, MemDC, 0, 0,37,37, SRCCOPY);
	DeleteDC(MemDC);
    return;
}

void paintwoodu(int posx,int posy)
{
    HDC MemDC;

	MemDC = CreateCompatibleDC(hdc);
    SelectObject(MemDC,bmpwoodu);
	StretchBlt(hdc, posx, posy,477,26, MemDC, 0, 0,477,26, SRCCOPY);
	StretchBlt(hdc, posx+477, posy,477,26, MemDC, 0, 0,477,26, SRCCOPY);
	StretchBlt(hdc, posx+2*477, posy,477,26, MemDC, 0, 0,477,26, SRCCOPY);
	DeleteDC(MemDC);
    return;
}


void paintwoodd(int posx,int posy)
{
    HDC MemDC;

	MemDC = CreateCompatibleDC(hdc);
    SelectObject(MemDC,bmpwoodd);
	StretchBlt(hdc, posx, posy,477,31, MemDC, 0, 0,477,31, SRCCOPY);
	StretchBlt(hdc, posx+477, posy,477,31, MemDC, 0, 0,477,31, SRCCOPY);
	StretchBlt(hdc, posx+2*477, posy,477,31, MemDC, 0, 0,477,31, SRCCOPY);
	DeleteDC(MemDC);
    return;
}

unsigned char *MyGetDibBits(HDC hdcSrc, HBITMAP hBmpSrc, int nx, int ny)
    {
    BITMAPINFO bi;
    BOOL bRes;
	int error;
    unsigned char  *buf;
     
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = nx;
    bi.bmiHeader.biHeight = ny;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biSizeImage = nx * 4 * ny;
    bi.bmiHeader.biClrUsed = 0;
    bi.bmiHeader.biClrImportant = 0;
     
    buf = (unsigned char *) new unsigned char[nx * 4 * ny];
    bRes = GetDIBits(hdcSrc, hBmpSrc, 0, ny, buf, &bi,
    DIB_RGB_COLORS);
    if (!bRes) {
		error=GetLastError();
    delete [] buf;
    buf = 0;
    }
    return buf;
 }

void paintrectangle(int posx,int posy,int color,int per)
{
	int r,s;
    POINT p[2];
	HPEN pen;

	pen=CreatePen(PS_SOLID,1,color);
	SelectObject(hdc,pen);
 	p[0].x=posx+3; p[0].y=posy+9;
	p[1].x=p[0].x+10; p[1].y=p[0].y;
	s=per*8/100;
	for(r=0;r<s;r++){	   
       Polyline(hdc,p,2);
	   p[0].y-=1.5;
	   p[1].y-=1.5;
	}
	DeleteObject(pen);
    return;
}

// color: rellena el reloj simple con color
// color1: rellena la mitad del reloj complejo con color y la otra mitad con color1,
//         para el reloj simple solo se usa color como color de relleno

void paintclocksand(int posx,int posy,int color,int color1,int color2,int per)
{
	int row,col,stride;
	unsigned char *r,*g,*b;
    HDC MemDC;
    HBITMAP hBmpDst; 
	unsigned char *buf;
	BITMAP sbmp;
	BITMAPINFO bi;

	GetObject(bmpsand, sizeof(BITMAP), &sbmp);
    buf=MyGetDibBits(hdc,bmpsand,sbmp.bmWidth,sbmp.bmHeight);
	MemDC = CreateCompatibleDC(hdc);
	hBmpDst = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
	SelectObject(MemDC, hBmpDst);
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = sbmp.bmWidth;
	bi.bmiHeader.biHeight = sbmp.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = sbmp.bmWidth * 4 * sbmp.bmHeight;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	per=sbmp.bmHeight*per/100;
	stride = (sbmp.bmWidth * (32 / 8) + 3) & ~3;
    for (row =0; row < per; ++row){
	   if(row>=sbmp.bmHeight/2) color=color1;
	   if(row>=sbmp.bmHeight*3/4) color=color2;
       int rowBase = row*stride;
       for (col = 0; col < sbmp.bmWidth*4; col+=4){
          b =&buf[rowBase + col];
		  g =&buf[rowBase + col+1];
          r =&buf[rowBase + col+2];
		  if(RGB(*r,*g,*b)==WHITE){             
			 *r=GetRValue(color);
             *g=GetGValue(color);
			 *b=GetBValue(color);
		  }
	   }
	}
	SetDIBits(MemDC, hBmpDst,0,sbmp.bmHeight,buf,&bi,DIB_RGB_COLORS);
//    StretchBlt(hdc, posx, posy, 18,15,MemDC,0,0,sbmp.bmWidth,sbmp.bmHeight,SRCCOPY);
    BitBlt(hdc,posx,posy,19,15,MemDC, 0,0, SRCCOPY);
	DeleteObject(hBmpDst);
	DeleteDC(MemDC);
	delete [] buf;
    return;
}

// si per<-umbral color=RED
// si per>=-umbral < umbral color=GREEN
// si per>umbral color=BLUE
COLORREF paintclocksand1(int posx,int posy,double umbral,double per)
{
	HDC hdc;
	int row,col,stride,ssl,ssh,tt,color;
	unsigned char *r,*g,*b;
    HDC MemDC;
    HBITMAP hBmpDst; 
	unsigned char *buf;
	BITMAP sbmp;
	BITMAPINFO bi;

	semapaintclocksand1=1;
	hdc=GetDC(hwnd);
	GetObject(bmpsand, sizeof(BITMAP), &sbmp);
    buf=MyGetDibBits(hdc,bmpsand,sbmp.bmWidth,sbmp.bmHeight);
	MemDC = CreateCompatibleDC(hdc);
	hBmpDst = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
	SelectObject(MemDC, hBmpDst);
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = sbmp.bmWidth;
	bi.bmiHeader.biHeight = sbmp.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = sbmp.bmWidth * 4 * sbmp.bmHeight;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	//per=sbmp.bmHeight*per/100;
	ssl=(-umbral*0.01+1.0)*0.5*sbmp.bmHeight;
	ssh=(umbral*0.01+1.0)*0.5*sbmp.bmHeight;
	tt=(per*0.01+1.0)*0.5*sbmp.bmHeight;
	stride = (sbmp.bmWidth * (32 / 8) + 3) & ~3;
    for (row =0; row < sbmp.bmHeight; ++row){
	   if(row<=ssl) color=RED;
	   if(row>ssl && row<=ssh) color=GREEN;
       if(row>ssh) color=BLUE;
	   if(row>tt) break;
       int rowBase = row*stride;
       for (col = 0; col < sbmp.bmWidth*4; col+=4){
          b =&buf[rowBase + col];
		  g =&buf[rowBase + col+1];
          r =&buf[rowBase + col+2];
		  if(RGB(*r,*g,*b)==WHITE){             
			 *r=GetRValue(color);
             *g=GetGValue(color);
			 *b=GetBValue(color);
		  }
	   }
	}
	SetDIBits(MemDC, hBmpDst,0,sbmp.bmHeight,buf,&bi,DIB_RGB_COLORS);
//    StretchBlt(hdc, posx, posy, 18,15,MemDC,0,0,sbmp.bmWidth,sbmp.bmHeight,SRCCOPY);
    BitBlt(hdc,posx,posy,19,15,MemDC, 0,0, SRCCOPY);
	DeleteObject(hBmpDst);
	DeleteDC(MemDC);
	ReleaseDC(hwnd,hdc);
	delete [] buf;
	semapaintclocksand1=0;
    return color;
}

void calctempomedio(MELODIA *melodia,int penta,int compas)
{
	int c,numnotas,v;
	double tim,pul;

	pul=tim=0;
	if(compas-2<1){ melodia->metronomoreal=-1; return;}
	for(c=compas-2;c<=compas;c++){
	   numnotas=melodia->penta[penta].compas[c].numnotas;
	   for(v=0;v<numnotas;v++){
          tim+=melodia->penta[penta].compas[c].timenota[v].time/CLOCKS_PER_SEC;  
		  pul+=melodia->penta[penta].compas[c].densignature*melodia->penta[penta].compas[c].dura[v];
	   }
	}
	pul/=tim;
	pul*=60;
	melodia->metronomoreal=(int) pul;
	return;
}

void randommusicrithmscore(MELODIA *melodia)
{
   double num,den,tt,durredonda;
   int col,per,tempocompas,metronomo,valid,numsignature,densignature;   
   TIMENOTA *ap;
   int maxcompascreen,r,compas,numnotas,v;
 
   if(concfg.ritmo==0) return;
   maxcompascreen=concfg.maxcompascreen; //(NumOfStaffs-OFFSETPENTA)/(BARTOBARSPACE);  // 42 jordi
   for(r=0;r<2;r++){ 
      for(compas=0;compas<melodia->numcompases;compas++){	   
   	     numnotas=melodia->penta[r].compas[compas].numnotas;
         numsignature=melodia->penta[0].compas[compas].numsignature;
         densignature=melodia->penta[0].compas[compas].densignature;
		 if((compas%maxcompascreen)==0 && 
		    melodia->penta[r].compas[compas].posxcompas+melodia->penta[r].compas[compas].posx[numnotas-1]>40*StaffWidth[1])
		    break;
	     for(v=0;v<numnotas;v++){			 
            if(compas==0 && r==0 && v==0){	  
               melodia->score.fastduringnote=0;
               melodia->score.okduringnote=0;
               melodia->score.slowduringnote=0;
			} 
			if(v==0){
			   melodia->penta[r].compas[compas].std.notasslow=0;
               melodia->penta[r].compas[compas].std.notasfast=0;
               melodia->penta[r].compas[compas].std.notasjust=0;
			   melodia->penta[r].compas[compas].std.notasslowqos=0;
			   melodia->penta[r].compas[compas].std.notasfastqos=0;
			   melodia->penta[r].compas[compas].std.notasjustqos=0;
			   melodia->penta[r].compas[compas].std.durnotastotal=0;
			}
            if(melodia->metronomo<=0){
	           if(compas>0){
	              valid=melodia->penta[0].compas[compas-1].timecompas.valid;
                  if(valid==1){
	                 tempocompas=melodia->penta[0].compas[compas-1].timecompas.time;
	                 metronomo=60.0*CLOCKS_PER_SEC*numsignature/tempocompas;
					 if(metronomo==0) metronomo=1;
				  }else continue;
			   }else continue;
			}else metronomo=melodia->metronomo;
            ap=&melodia->penta[r].compas[compas].timenota[v];
            if(ap->valid==0 || ap->valid==2) continue;
            if(ap->valid==1 && ap->time==-1){
	           tt=100; col=BACKGROUND; ap->valid=0;
			}else{
	           per=concfg.accuracynotes;
               durredonda=(60.0*CLOCKS_PER_SEC)*densignature/metronomo; //duracion en ticks de la redonda
               num=ap->time;
	           den=durredonda*melodia->penta[r].compas[compas].dura[v]; // duracion de la nota en el pentagrama en ticks teorica     
               tt=(num/den-1.0)*100;
               if(tt>100) tt=100;
               col=REDBLACK;                // demasiado rapido respecto a la duracion de la nota teorica
               if(tt>per) col=BLUE;     // porcentage=10% demasiado lento respecto a la duracion de la nota teorica
               if(-per<tt && tt<=per) col=GREEN; //tt=100; }    // velocidad adecuada
               tt=fabs(tt);
			}
			melodia->penta[r].compas[compas].std.durnotastotal+=num;
            if(col==REDBLACK){ 
			   ++melodia->score.fastduringnote;
			   ++melodia->penta[r].compas[compas].std.notasfast;
               melodia->penta[r].compas[compas].std.notasfastqos+=5.0*(tt-100.0)/(per-100.0);
			}
            if(col==GREEN){
			   ++melodia->score.okduringnote;
               ++melodia->penta[r].compas[compas].std.notasjust;
			   melodia->penta[r].compas[compas].std.notasjustqos+=5.0*(2.0-tt/per);
			}
            if(col==BLUE){ 
			   ++melodia->score.slowduringnote;
               ++melodia->penta[r].compas[compas].std.notasslow;
			   melodia->penta[r].compas[compas].std.notasslowqos+=5.0*(tt-100.0)/(per-100.0);
			}
		 }
	  }
   }
   return;
}

void paintrandommusicqosmeasure(MELODIA *melodia)
{
   int maxcompascreen,r,c,numnotas,color,numsilen;
   double rit[2],legato[2],ritmo[2],durcompas,durnotas,durcompasqos,qos;
   char buf[10];
   STATISTICS *std;
   int metodo=0;
 
   if(concfg.ritmo==0) return;
   if(concfg.randommusicqosritmo==0 && concfg.randommusicqoscompas==0) return;
   maxcompascreen=concfg.maxcompascreen;
   for(c=offsetdispinterval;c<offsetdispinterval+maxcompascreen;c++){	   
	  if(c>=melodia->numcompases) break;
	  // durcompasqos puntua la duracion del compas que debe coincidir con el que viene marcado por el tempo de la melodia	  
	  // si las notas no se tocan con legato o las notas  duran demasiado la duracion del compas sera mayor que el de la melodia y la puntuacion bajara
      // asi pues durcompasqos puntua la desviacion del compas respecto a la teorica
	  // rit[x] es la puntuacion media de cada nota vale diez si se tocan con su duracion exacta, si no disminuira la puntuacion
	  // legato[r] puntua el legato real de las  notas sin tener en cuenta si han durado mas o menos
	  if(concfg.randommusicqosritmo==1 && concfg.randommusicqoscompas==1){
		 durcompas=melodia->penta[0].compas[c].std.durcompas;
		 durcompasqos=melodia->penta[0].compas[c].std.durcompasqos;
	     for(r=0;r<2;r++){ 
            durnotas=melodia->penta[r].compas[c].std.durnotastotal;
			if(durcompas!=0) legato[r]=10*durnotas/durcompas;
			else legato[r]=0;
			legato[r]=legato[r]*legato[r]*0.1;
		    numnotas=melodia->penta[r].compas[c].numnotas;
		    numsilen=melodia->penta[r].compas[c].numsilencios;
		    std=&melodia->penta[r].compas[c].std;
            rit[r]=std->notasfastqos+std->notasslowqos+std->notasjustqos;
		    if(numnotas-numsilen!=0) rit[r]/=(numnotas-numsilen);
		    else rit[r]=10;
		 }
		 qos=(4*rit[0]+4*rit[1]+8*durcompasqos)/16;
		 // si suspendemos en legato no puntuamos nada mas
		 if(concfg.randommusicqoslegato==1)
		    if(melodia->penta[0].compas[c].numsilencios==0 && melodia->penta[1].compas[c].numsilencios==0)
		       if(legato[0]<5 || legato[1]<5 || rit[0]<5 || rit[1]<5) 
			      qos=min(min(legato[0],legato[1]),min(rit[0],rit[1]));
         if(gfstat[0]==1 || melodia->penta[0].allsilencio==1){ 
		    qos=(4*rit[1]+8*durcompasqos)/12;
			if(concfg.randommusicqoslegato==1)
			   if(melodia->penta[1].compas[c].numsilencios==0)
                  if(legato[1]<5 || rit[1]<5) qos=min(legato[1],rit[1]);
		 }
         if(gfstat[1]==1 || melodia->penta[1].allsilencio==1){ 
		    qos=(4*rit[0]+8*durcompasqos)/12;         
			if(concfg.randommusicqoslegato==1)
			   if(melodia->penta[0].compas[c].numsilencios==0)
		          if(legato[0]<5 || rit[0]<5) qos=min(legato[0],rit[0]);
		 }
	  }
	  // se puntua el ritmo ritmo[x] sin tener en cuenta el tempo,las figuras deben mantener la relacion de tiempo entre ellas
      // pero la duracion de las mismas no tiene porque ser la que viene marcada por el tempo de la melodia.
	  // se puntua tambien que las notas esten tocadas en legato legato[x]
	  // si no hay legato durnotas sera mucho menor que durcompas
	  if(concfg.randommusicqosritmo==1 && concfg.randommusicqoscompas==0){
		 durcompas=melodia->penta[0].compas[c].std.durcompas;
		 for(r=0;r<2;r++){
            durnotas=melodia->penta[r].compas[c].std.durnotastotal;
			if(durcompas!=0) legato[r]=10*durnotas/durcompas;
			else legato[r]=0;
			legato[r]=legato[r]*legato[r]*0.1;
			ritmo[r]=melodia->penta[r].compas[c].std.ritmic*0.1;
		 }
		 qos=(4*ritmo[0]+4*ritmo[1])/8;
		 // si suspendemos en legato no puntuamos nada mas
		 if(concfg.randommusicqoslegato==1){
			qos=(8*legato[0]+8*legato[1]+4*ritmo[0]+4*ritmo[1])/24;
			if(melodia->penta[0].compas[c].numsilencios==0 && melodia->penta[1].compas[c].numsilencios==0){
		       if(legato[0]<5 || legato[1]<5 || ritmo[0]<5 || ritmo[1]<5) 
				   qos=min(min(legato[0],legato[1]),min(ritmo[0],ritmo[1]));			   
			}
		 }
         if(gfstat[0]==1 || melodia->penta[0].allsilencio==1){ 
		    qos=(4*ritmo[1])/4;
			if(concfg.randommusicqoslegato==1){
               qos=(8*legato[1]+4*ritmo[1])/12;
			   if(melodia->penta[1].compas[c].numsilencios==0)
			      if(legato[1]<5 || ritmo[1]<5) qos=min(legato[1],ritmo[1]);
			}
		 }
         if(gfstat[1]==1 || melodia->penta[1].allsilencio==1){ 
		    qos=(4*ritmo[0])/4;
			if(concfg.randommusicqoslegato==1){
			   qos=(8*legato[0]+4*ritmo[0])/12;
			   if(melodia->penta[0].compas[c].numsilencios==0)
		          if(legato[0]<5 || ritmo[0]<5) qos=min(legato[0],ritmo[0]);
			}
		 }
	  }
      if(concfg.randommusicqosritmo==0 && concfg.randommusicqoscompas==1){
		 qos=melodia->penta[0].compas[c].std.durcompasqos;
	  }
      sprintf(buf,"%1.1f",qos);
	  if(strcmp(buf,"10.0")==0) strcpy(buf,"10");
	  if(qos<5) color=RED;
	  if(qos>=5 && qos<8) color=ORANGE;
	  if(qos>=8 && qos<=10) color=GREEN;
	  mydrawtext2(color,hfontsystem,buf,melodia->penta[0].compas[c].posxfincompas-20,
		           melodia->penta[0].compas[c].posycompas-7*interline);
   }   
}
          
void paintrandommusicrithm(MELODIA *melodia,int penta,int compas,int v)
{
   double num,den,tt,durredonda;
   char *s;
   int orgf,orgs,col,posx0,posx1,aa,per,tempocompas,metronomo,valid,updo,cl;   
   int numsignature,densignature;
   RECT rect;
   TIMENOTA *ap;
    
   numsignature=melodia->penta[0].compas[compas].numsignature;
   densignature=melodia->penta[0].compas[compas].densignature;
   if(concfg.ritmo==0) return;
   if(melodia->metronomo<=0){
	   if(compas>0){
	      valid=melodia->penta[0].compas[compas-1].timecompas.valid;
          if(valid==1){
	         tempocompas=melodia->penta[0].compas[compas-1].timecompas.time;
	         metronomo=60.0*CLOCKS_PER_SEC*numsignature/tempocompas;
			 if(metronomo==0) metronomo=1;
		  }else return;
	   }else return;
   }else metronomo=melodia->metronomo;
   ap=&melodia->penta[penta].compas[compas].timenota[v];
   if(ap->valid==0 || ap->valid==2) 
	   return;
   if(ap->valid==1 && ap->time==-1){
	   tt=100; col=BACKGROUND; ap->valid=0;
   }else{
	  per=concfg.accuracynotes;
      durredonda=(60.0*CLOCKS_PER_SEC)*densignature/metronomo; //duracion en ticks de la redonda
      num=ap->time;
	  den=durredonda*melodia->penta[penta].compas[compas].dura[v]; // duracion de la nota en el pentagrama en ticks teorica     
      tt=(num/den-1.0)*100;
      if(tt>100) tt=100;
      col=REDBLACK;                // demasiado rapido respecto a la duracion de la nota teorica
      if(tt>per) 
		  col=BLUE;     // porcentage=10% demasiado lento respecto a la duracion de la nota teorica
      if(-per<tt && tt<=per){ col=GREEN; tt=100; }    // velocidad adecuada
      tt=fabs(tt);
      if(tt<15) tt=15;        // valor minimo para que se aprecie la medida   
   }
   orgs=gStaff[64-36]-250;
   orgf=fStaff[43-36]-250;	
   posx1=posx0=melodia->penta[penta].compas[compas].posx[v];
   s=gettiponota(melodia,penta,compas,v,updo,cl);
   GetCharWidth32(hdc,(UINT) 'w','w',&aa);
   posx1+=aa*0.5;
   rect.left=posx0+5;
   rect.right=posx1+5;
   if(penta==0){
      //rect.top=orgs-4.5*interline;
	  rect.top=posyminpenta-1.5*interline;
	  rect.bottom=rect.top-(4.5-1.5)*interline*0.01*tt;
	  if(col==GREEN) 
		  paintcheck(rect.left,rect.top-20);   // restamos 20 que es la altura del bitmap check.bmp
      else 
		  PaintGradientRect(hdc,rect,BACKGROUND,col,1,1-FAST);
   }else{
      //rect.top=orgf+12*interline;
	  rect.top=posymaxpenta+9*interline;
	  rect.bottom=rect.top+(12-9)*interline*0.01*tt;
	  if(col==GREEN) paintcheck(rect.left,rect.top);
	  else PaintGradientRect(hdc,rect,BACKGROUND,col,1,1-FAST);
   }
   return;
}

void tempobymeasurescore(MELODIA *melodia)
{
	int itempocompas,r,numsignature;
	int tempocompas,valid;
	double per;
	double limitinf,limitsup,desvi;
	int maxcompascreen,compas,numnotas;

   if(concfg.ritmo==0) return;
   maxcompascreen=concfg.maxcompascreen; //(NumOfStaffs-OFFSETPENTA)/(BARTOBARSPACE);  //42 jordi
   for(r=0;r<2;r++){ 
      for(compas=0;compas<melodia->numcompases;compas++){	   
   	     numnotas=melodia->penta[r].compas[compas].numnotas;
		 if((compas%maxcompascreen)==0 && 
		    melodia->penta[r].compas[compas].posxcompas+melodia->penta[r].compas[compas].posx[numnotas-1]>40*StaffWidth[1])
		    break;
	     if(r==1) continue;
	     if(compas<0) return;
	     if(compas==0){
            melodia->score.fasttempocompas=0;
	        melodia->score.slowtempocompas=0;
	        melodia->score.oktempocompas=0;
		 }
	     per=concfg.accuracytempo;
         tempocompas=melodia->penta[0].compas[compas].timecompas.time;
		 numsignature=melodia->penta[0].compas[compas].numsignature;
	     valid=melodia->penta[0].compas[compas].timecompas.valid;
	     if(valid==0) continue;
	     if(concfg.ritmo==0) continue;
	     if(concfg.metronomo<=0){
		    if(compas>0)
		       itempocompas=melodia->penta[0].compas[compas-1].timecompas.time;
		    else itempocompas=tempocompas;
		 }else itempocompas=60.0*CLOCKS_PER_SEC*numsignature/concfg.metronomo;
	     limitinf=itempocompas*(1.0-per*0.01);
	     limitsup=itempocompas*(1.0+per*0.01);
	     desvi=(tempocompas-itempocompas)*100.0/itempocompas;
	     melodia->score.itempocompas=itempocompas;
	     melodia->score.tempocompas=tempocompas;
	     desvi=fabs(desvi);
	     if(desvi<20) desvi=20;  // minima desviacion para pintar un reloj no vacio
	     if(desvi>100) desvi=100;
	     if(concfg.metronomo<=0) itempocompas=tempocompas;
         if(tempocompas<limitinf){
            ++melodia->score.fasttempocompas;
	        continue;
		 }
	     if(tempocompas>limitsup){
            ++melodia->score.slowtempocompas;
            continue;
		 }
         ++melodia->score.oktempocompas;
	  }
   }
   return;
}

void painttempobymeasure(MELODIA *melodia,int penta,int compas)
{
	int itempocompas,numsignature;
	int tempocompas,valid;
	double per;
	double limitinf,limitsup,desvi;

	if(penta==1) return;
	if(compas<0) return;
	per=concfg.accuracytempo;
    tempocompas=melodia->penta[0].compas[compas].timecompas.time;
	numsignature=melodia->penta[0].compas[compas].numsignature;
	valid=melodia->penta[0].compas[compas].timecompas.valid;
	if(valid==0) return;
	if(concfg.ritmo==0) return;
	if(concfg.metronomo<=0){
		if(compas>0)
		   itempocompas=melodia->penta[0].compas[compas-1].timecompas.time;
		else itempocompas=tempocompas;
	}else itempocompas=60.0*CLOCKS_PER_SEC*numsignature/concfg.metronomo;
	limitinf=itempocompas*(1.0-per*0.01);
	limitsup=itempocompas*(1.0+per*0.01);
	desvi=(tempocompas-itempocompas)*100.0/itempocompas;
	melodia->score.itempocompas=itempocompas;
	melodia->score.tempocompas=tempocompas;
	desvi=fabs(desvi);
	if(desvi>100) desvi=100;
	melodia->penta[penta].compas[compas].std.durcompas=tempocompas;
	melodia->penta[penta].compas[compas].std.durcompasqos=10.0-desvi*0.1;
	if(desvi<20) desvi=20;  // minima desviacion para pintar un reloj no vacio
	if(concfg.metronomo<=0) itempocompas=tempocompas;
    if(tempocompas<limitinf){
	   paintrectangle(melodia->penta[penta].compas[compas].posxfincompas-6,
		           melodia->penta[penta].compas[compas].posycompas-12*interline+10,RED,desvi);
	   return;
	}	
	if(tempocompas>limitsup){
       paintrectangle(melodia->penta[penta].compas[compas].posxfincompas-6,
		           melodia->penta[penta].compas[compas].posycompas-12*interline+10,BLUE,desvi);
       return;
	}
	paintcheck(melodia->penta[penta].compas[compas].posxfincompas,
		melodia->penta[penta].compas[compas].posycompas-12*interline);
		
	return;
}

double similaridad(double *x,double *y,int n)
{
	int r;
	double med,des,dest;

	med=des=0;
	for(r=0;r<n;r++)
       x[r]=x[r]/y[r];
    for(r=0;r<n;r++)
	   med+=x[r];
	med/=n;
    for(r=0;r<n;r++)
	   des+=(x[r]-med)*(x[r]-med);
	des/=n;
	des=dest=sqrt(des);
	des/=med;
	des=-6*log10(des);
	if(des>4 || 1){  // calculo desviaciones respecto a la redonda en cualquier puntuacion
	   double val,mn,durredonda;
	   int pos;
	   for(r=0;r<n;++r){
          val=fabs(x[r]-med);
		  if(r==0){ mn=val; pos=r; }         
		  if(val<mn) pos=r; 
	   }
       durredonda=x[pos];
	   for(r=0;r<n;r++){
	      x[r]=((x[r]/durredonda)-1)*100.0;        
		  if(x[r]>100) x[r]=100;
	   }
	}
	if(des>10) des=10;
	if(des<0) des=0;
	return des;
}

double crosscorrelation(double *x,double *y,int n)
{
   int i,j;
   double mx,my,sx,sy,sxy,r,co,tx,ty;
   
   co=sqrt((n-1.0)/n);
   mx = 0;
   my = 0;   
   for (i=0;i<n;i++) {
      mx += x[i];
      my += y[i];
   }
   mx /= n;
   my /= n;

   sx = 0;
   sy = 0;
   for (i=0;i<n;i++) {
      sx += (x[i] - mx) * (x[i] - mx);
      sy += (y[i] - my) * (y[i] - my);
   }

   sx/=(n-1); sx=sqrt(sx);
   sy/=(n-1); sy=sqrt(sy);
   sxy = 0;
   for (i=0;i<n;i++) {
      j = i;
	  if(round(sx,6)==0) tx=co;
	  else tx=(x[i] - mx)/sx;
	  if(round(sy,6)==0) ty=co;
	  else ty=(y[j] - my)/sy;
	  sxy += tx*ty;
   }

   r = sxy / (n-1);
   if(round(r,6)==0)
      if(sx<80 && sy<80) r=1.0;
   return r;      
}

COLORREF paintritmicexecution(MELODIA *melodia,int penta,int compas,int mode)
{
	int valid,v,numnotas,posx,posy,cn,can;
	double desvi,dat[64],dat1[64];
	COLORREF color;
	HPEN pen;
    TIMENOTA *ap;
	POINT p[2];

	if(compas<0) return 0;
	valid=melodia->penta[0].compas[compas].timecompas.valid;
	if(valid==0) return 0;
	if(concfg.ritmo==0) return 0;
	numnotas=melodia->penta[penta].compas[compas].numnotas;    
	cn=-1;
    for(v=can=0;v<numnotas;v++){			 
       ap=&melodia->penta[penta].compas[compas].timenota[v];
	   if(numnotas==1 && ap->time<0) return 0;
	   if(melodia->penta[penta].compas[compas].note[v]==0)
	      continue;
	   if(ap->valid==0 || ap->valid==2){ cn=1; break;}
	   cn=0;
	   dat[can]=ap->time;
	   dat1[can++]=melodia->penta[penta].compas[compas].dura[v];
	}
	// solo calculamos el ritmo si las notas son todas hit
	if(cn==-1){
		// todo el compas son silencios
		desvi=100;
	}
	if(cn==0){ 
	   //desvi=crosscorrelation(dat,dat1,can)*100.0;		  
	   desvi=similaridad(dat,dat1,can)*10;
	}
	if(cn==1) desvi=0;
    color=RED;
	if(desvi>70) color=GREEN;
	else if(desvi>50 && desvi<=70) 
		    color=BROWN;	
	melodia->penta[penta].compas[compas].std.ritmic=desvi;
    posx=melodia->penta[0].compas[compas].posxfincompas-6;
    posy=melodia->penta[0].compas[compas].posycompas-12*interline+10;
	if(penta==1) posy+=8.5;
    if(mode==1) return color;
	pen=CreatePen(PS_SOLID,3,color);
	SelectObject(hdc,pen);
 	p[0].x=posx+3; p[0].y=posy+9;
	p[1].x=p[0].x+10; p[1].y=p[0].y;
    p[0].y-=29.5;
	p[1].y-=29.5;
    Polyline(hdc,p,2);
	DeleteObject(pen);	
	return 0;
}

void paintrandommusicsolorithm(MELODIA *melodia,int penta,int compas)
{
	int valid,v,numnotas,cn,can,tt;
	double desvi,dat[64],dat1[64];
    TIMENOTA *ap;
	char *s;
    int orgf,orgs,col,posx0,posx1,aa,per,updo,cl;   
    RECT rect;


	if(compas<0) return;
	valid=melodia->penta[0].compas[compas].timecompas.valid;
	if(valid==0) return;
	if(concfg.ritmo==0) return;
	numnotas=melodia->penta[penta].compas[compas].numnotas;    
    for(v=can=cn=0;v<numnotas;v++){			 
       ap=&melodia->penta[penta].compas[compas].timenota[v];
	   if(numnotas==1 && ap->time<0) return;
	   if(melodia->penta[penta].compas[compas].note[v]==0)
	      continue;
	   if(ap->valid==0 || ap->valid==2){ cn=1; break;}
	   dat[can]=ap->time;
	   dat1[can++]=melodia->penta[penta].compas[compas].dura[v];
	}
	// solo calculamos el ritmo si las notas son todas hit
	if(cn==0){ 
	   desvi=similaridad(dat,dat1,can)*10;
	}else return;
    for(v=0;v<numnotas;v++){	
	   per=concfg.accuracynotes;
	   tt=dat[v];
       if(tt>100) tt=100;
       col=REDBLACK;                // demasiado rapido respecto a la duracion de la nota teorica
       if(tt>per) 
		  col=BLUE;     // porcentage=10% demasiado lento respecto a la duracion de la nota teorica
       if(-per<tt && tt<=per){ col=GREEN; tt=100; }    // velocidad adecuada
       tt=fabs(tt);
       if(tt<15) tt=15;        // valor minimo para que se aprecie la medida   
       orgs=gStaff[64-36]-250;
       orgf=fStaff[43-36]-250;	
       posx1=posx0=melodia->penta[penta].compas[compas].posx[v];
       s=gettiponota(melodia,penta,compas,v,updo,cl);
       GetCharWidth32(hdc,(UINT) 'w','w',&aa);
       posx1+=aa*0.5;
       rect.left=posx0+5;
       rect.right=posx1+5;
       if(penta==0){
	      rect.top=posyminpenta-1.5*interline;
	      rect.bottom=rect.top-(4.5-1.5)*interline*0.01*tt;
	      if(col==GREEN) 
		     paintcheck(rect.left,rect.top-20);   // restamos 20 que es la altura del bitmap check.bmp
          else 
		     PaintGradientRect(hdc,rect,BACKGROUND,col,1,1-FAST);
	   }else{
	      rect.top=posymaxpenta+9*interline;
	      rect.bottom=rect.top+(12-9)*interline*0.01*tt;
	      if(col==GREEN) paintcheck(rect.left,rect.top);
	      else PaintGradientRect(hdc,rect,BACKGROUND,col,1,1-FAST);
	   }
	}
    return;
}

void painttemposcoretext(MELODIA *melodia)
{
   char aa[128];

   mydrawtext1(BLACK,hfontsystem,"   Tempo: ",0,0,0);   
   if(melodia->metronomoreal>0){
      sprintf(aa,"%d ",(int) melodia->metronomoreal);
      mydrawtext1(RED,hfontsystem,aa,0,0,0);
   }else{
      sprintf(aa,"%s ","-----");
      mydrawtext1(RED,hfontsystem,aa,0,0,0);
   }
   return;
}

void painttemposcore(MELODIA *melodia)
{
   char aa[128];

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   mydrawtext(hwndnumeric[7],BLACK,hfontsystem,"Tempo: ",0,0);   
   if(melodia->metronomoreal>0){
      sprintf(aa,"%d ",(int) melodia->metronomoreal);
      mydrawtext(hwndnumeric[7],RED,hfontsystem,aa,0,0);
   }else{
      sprintf(aa,"%s ","-----");
      mydrawtext(hwndnumeric[7],RED,hfontsystem,aa,0,0);
   }
   return;
}


void RandomMusicButtonsScoreshideshow(int hs) 
{
   int x;
   int cmdsh;

   if(hs==1) cmdsh=SW_HIDE;
   else cmdsh=SW_SHOW;
   for (x = 0; x < 8; x++){
      ShowWindow(hwndnumeric[x],cmdsh);
   }
   ShowWindow(buttonritmo,cmdsh);
   ShowWindow(buttonmetro,cmdsh);
   return;
}

void RandomMusicButtonsScoresremove(void) 
{
   int x;

   for (x = 0; x < 8; x++){
      DestroyWindow(hwndnumeric[x]);
	  hwndnumeric[x] = NULL;
   }
   DestroyWindow(buttonritmo);
   DestroyWindow(buttonmetro);
   DestroyWindow(buttonplay);
   DestroyWindow(buttongoto);
   return;
}

static WNDPROC buttonplayorgproc;

LRESULT APIENTRY buttonplayproc(
    HWND hwndpy,
    UINT uMsg, 
    WPARAM wParam, 
    LPARAM lParam) 
{ 
    if(uMsg == WM_MOUSEMOVE){
	   SetCursor(LoadCursor(hInst, MAKEINTRESOURCE(IDC_FINGER)));
       return 0; 
	}	
	if(uMsg==WM_RBUTTONUP){
	   pianoplay pianopy;
       pianopy.Show(TRUE);	
	   saveconfiguracion();
	   return 0;
	}
    return CallWindowProc(buttonplayorgproc, hwndpy, uMsg,wParam, lParam); 
} 

void RandomMusicCreateButtonsScores(void)
{

	int x,posx,posy,fil,col;
//	int orgf;
	char buff[10];

    posx=testRect.left; //testRect.right*11/16;
	if(currenthor==1024){
	   if(concfg.maxcompascreen>4)
          posy=testRect.top+250;  //15
	   else posy=testRect.top+400;
	}else posy=testRect.top+250;
	for (x = 0; x < 8; x++){
	   sprintf(buff,"%d",x+1);
	   fil=x/8; col=x%8;
	   hwndnumeric[x] = CreateWindow("STATIC","",
						WS_CHILD | WS_VISIBLE | SS_OWNERDRAW ,
						posx+col*100,posy+(fil+0.5)*30,100,30,
						hwnd, NULL, hInst, NULL);
	}
	buttonritmo = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						555,6,20,20,
						hwnd, NULL, hInst, NULL);
	if(concfg.ritmo==0)
       SendMessage(buttonritmo, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpritmon);
	else
	   SendMessage(buttonritmo, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpritmoy);

	buttonplay = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						820,6,20,20,
						hwnd, NULL, hInst, NULL);
    SendMessage(buttonplay, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpplayn);
	buttongoto = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						840,6,20,20,
						hwnd, NULL, hInst, NULL);
    SendMessage(buttongoto, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgoto);

	buttonmetro = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						575,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttonmetro, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmetron);

// interceptamos los mensajes a buttonplay para aumentar la funcionalidad
	buttonplayorgproc = (WNDPROC) SetWindowLong(buttonplay, 
                               GWL_WNDPROC, (LONG) buttonplayproc); 
	return;
}

void RandomMusicSlidingControlshideshow(int hs) 
{
   int cmdsh;

   if(hs==1) cmdsh=SW_HIDE;
   else cmdsh=SW_SHOW;
   ShowWindow(hwndmetronomo,cmdsh);
   ShowWindow(hwndaccnotes,cmdsh);
   ShowWindow(hwndacctempo,cmdsh);
   ShowWindow(hwndmetronomotitle,cmdsh);
   ShowWindow(hwndaccnotestitle,cmdsh);
   ShowWindow(hwndacctempotitle,cmdsh);
   ShowWindow(hwndmetronomovalue,cmdsh);
   ShowWindow(hwndaccnotesvalue,cmdsh);
   ShowWindow(hwndacctempovalue,cmdsh);
   return;
}

void RandomMusicSlidingControlsremove(void) 
{
   DestroyWindow(hwndmetronomo);
   DestroyWindow(hwndaccnotes);
   DestroyWindow(hwndacctempo);
   DestroyWindow(hwndmetronomotitle);
   DestroyWindow(hwndaccnotestitle);
   DestroyWindow(hwndacctempotitle);
   DestroyWindow(hwndmetronomovalue);
   DestroyWindow(hwndaccnotesvalue);
   DestroyWindow(hwndacctempovalue);   
   hwndmetronomo=hwndaccnotes=hwndacctempo=NULL;
   return;
}

void RandomMusicCreateSlidingControls(void)
{

	hwndmetronomo = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|TBS_NOTICKS|WS_VISIBLE,
								120,2,80,30,hwnd,NULL,hInst,NULL);
	hwndaccnotes = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|TBS_NOTICKS|WS_VISIBLE,
							    285,2,80,30,hwnd,NULL,hInst,NULL);
	hwndacctempo = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|TBS_NOTICKS|WS_VISIBLE,
								445,2,80,30,hwnd,NULL,hInst,NULL);
	
	SendMessage(hwndmetronomo,TBM_SETRANGE,true,MAKELONG(0,200));
	SendMessage(hwndmetronomo,TBM_SETTICFREQ,2,0);	//now theres 50 ticks each valued at 2+			
	SendMessage(hwndaccnotes,TBM_SETRANGE,true,MAKELONG(1,100));
	SendMessage(hwndaccnotes,TBM_SETTICFREQ,2,0);	//now theres 50 ticks each valued at 2+			
	SendMessage(hwndacctempo,TBM_SETRANGE,true,MAKELONG(1,100));
	SendMessage(hwndacctempo,TBM_SETTICFREQ,2,0);	//now theres 50 ticks each valued at 2+			
	
	hwndmetronomotitle = CreateWindow("static","Tempo:",WS_CHILD|WS_VISIBLE,80,6,40,20,hwnd,NULL,hInst,NULL);
	hwndaccnotestitle = CreateWindow("static","TolNotas:",WS_CHILD|WS_VISIBLE,235,6,50,20,hwnd,NULL,hInst,NULL);
	hwndacctempotitle = CreateWindow("static","TolCompas:",WS_CHILD|WS_VISIBLE,386,6,60,20,hwnd,NULL,hInst,NULL);
	
	SendMessage(hwndmetronomotitle,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));
    SendMessage(hwndaccnotestitle,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));
    SendMessage(hwndacctempotitle,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));
	
	hwndmetronomovalue = CreateWindow("static","",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW,200,6,20,20,hwnd,NULL,hInst,NULL);
	hwndaccnotesvalue = CreateWindow("static","",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW,365,6,20,20,hwnd,NULL,hInst,NULL);
	hwndacctempovalue = CreateWindow("static","",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW,525,6,20,20,hwnd,NULL,hInst,NULL);
	
	SendMessage(hwndmetronomo,TBM_SETPOS,true,concfg.metronomo);//starting values
	SendMessage(hwndaccnotes,TBM_SETPOS,true,concfg.accuracynotes);//starting values
	SendMessage(hwndacctempo,TBM_SETPOS,true,concfg.accuracytempo);//starting values

	return;
}

void GenericControlsRemove(void)
{
   DestroyWindow(hwndvolume);
   DestroyWindow(hwndvolumetitle);
   DestroyWindow(hwndvolumevalue);
   DestroyWindow(buttonmic);
   DestroyWindow(buttoncfg);
   DestroyWindow(buttongame1);
   DestroyWindow(buttongame2);
   DestroyWindow(buttongame3);
   DestroyWindow(buttongame4);
   DestroyWindow(buttongame5);
   DestroyWindow(buttongame6);
   return;
}

void GenericCreateControls(HWND hwnd)
{
	hwndvolume = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|TBS_NOTICKS|WS_VISIBLE,
								714,2,80,30,hwnd,NULL,hInst,NULL);
	
	SendMessage(hwndvolume,TBM_SETRANGE,true,MAKELONG(0,128));
	SendMessage(hwndvolume,TBM_SETTICFREQ,2,0);	//now theres 50 ticks each valued at 2+				
	hwndvolumetitle = CreateWindow("static","Volume:",WS_CHILD|WS_VISIBLE,674,6,40,20,hwnd,NULL,hInst,NULL);	
	SendMessage(hwndvolumetitle,WM_SETFONT,(WPARAM) hfontsystem,MAKELPARAM(true,0));	
	hwndvolumevalue = CreateWindow("static","",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW,794,6,20,20,hwnd,NULL,hInst,NULL);
	SendMessage(hwndvolume,TBM_SETPOS,true,concfg.DefaultVelocity);//starting values

	buttonmic = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						860,6,20,20,
						hwnd, NULL, hInst, NULL);

    SendMessage(buttonmic, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpmicro);
	buttoncfg = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						880,6,20,20,
						hwnd, NULL, hInst, NULL);
    SendMessage(buttoncfg, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpcfg);

	buttongame1 = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						900,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttongame1, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgame1);

	buttongame2 = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						920,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttongame2, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgame2);

	buttongame3 = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						940,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttongame3, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgame3);

	buttongame4 = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						960,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttongame4, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgame4);
	buttongame5 = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						980,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttongame5, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgame5);
	buttongame6 = CreateWindow("BUTTON",NULL,
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|BS_BITMAP,
						1000,6,20,20,
						hwnd, NULL, hInst, NULL);
	SendMessage(buttongame6, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpgame6);

    return;
}

void disenabuttongeneric(BOOL ena)
{
    EnableWindow(buttonmic,ena);
    EnableWindow(buttoncfg,ena);
	EnableWindow(buttongame1,ena);
	EnableWindow(buttongame2,ena);
	EnableWindow(buttongame3,ena);
	EnableWindow(buttongame4,ena);
	EnableWindow(buttongame5,ena);
	EnableWindow(buttongame6,ena);
}

void genericbuttonhideshow(int hs)
{
   int cmdsh;

   if(hs==1) cmdsh=SW_HIDE;
   else cmdsh=SW_SHOW;
   ShowWindow(hwndvolume,cmdsh);
   ShowWindow(hwndvolumetitle,cmdsh);
   ShowWindow(hwndvolumevalue,cmdsh);
   ShowWindow(buttonmic,cmdsh);
   ShowWindow(buttoncfg,cmdsh);
   ShowWindow(buttongame1,cmdsh);
   ShowWindow(buttongame2,cmdsh);
   ShowWindow(buttongame3,cmdsh);
   ShowWindow(buttongame4,cmdsh);
   ShowWindow(buttongame5,cmdsh);
   ShowWindow(buttongame6,cmdsh);
   return;
}


void updatesliders(HWND hwndScrollBar)
{
   HDC tbhdc;
   RECT rct;
   char aa[128];
   int pos;
    
   if(hwndScrollBar==NULL) return;
   tbhdc=GetDC(hwndScrollBar);
   GetClientRect(hwndScrollBar,&rct);
   FrameRect(tbhdc,&rct,hWhiteBrush);
   pos=SendMessage(hwndScrollBar,TBM_GETPOS,0,0);	
   sprintf(aa,"%3d ",pos);
   if(hwndScrollBar==hwndmetronomo){
      mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
	  sprintf(aa,"%3d     ",pos);
	  if(pos==0) strcpy(aa,"Auto");
      mydrawtext(hwndmetronomovalue,RED,hfontsystem,aa,0,0);   
	  concfg.metronomo=melodia->metronomo=pos;
   }
   if(hwndScrollBar==hwndaccnotes){
      mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
      mydrawtext(hwndaccnotesvalue,RED,hfontsystem,aa,0,0);   
	  concfg.accuracynotes=pos;
   }
   if(hwndScrollBar==hwndacctempo){
      mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
      mydrawtext(hwndacctempovalue,RED,hfontsystem,aa,0,0);   
	  concfg.accuracytempo=pos;
   }
   if(hwndScrollBar==hwndvolume){
      mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
      mydrawtext(hwndvolumevalue,RED,hfontsystem,aa,0,0);   
	  concfg.DefaultVelocity=pos;
   }
   if(hwndScrollBar==hwndmetronoteread){
      mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
      mydrawtext(hwndmetronotereadvalue,RED,hfontsystem,aa,0,0);   
	  concfg.metronoteread=pos;
   }   
   if(hwndScrollBar==hwndmetronoteinterval){
      mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
      mydrawtext(hwndmetronoteintervalvalue,RED,hfontsystem,aa,0,0);   
	  concfg.metronoteinterval=pos;
   }   
   ReleaseDC(hwndScrollBar,tbhdc);
   return;
}

void mydrawtext2(COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposx,int sposy)
{
   int bkb;
   HGDIOBJ bkp1;
   SIZE p;

   bkb=SetBkMode(hdc, TRANSPARENT);
   bkp1=SelectObject(hdc,hgdiobj);
   SetTextColor(hdc,color);
   TextOut(hdc,sposx,sposy,buff,strlen(buff));
   GetTextExtentPoint32(hdc,buff,strlen(buff),&p);
   SelectObject(hdc,bkp1);
   SetBkMode(hdc,bkb);
   return;
}

void mydrawtext1(COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposx,int sposy,int init)
{
   static int posx,posy;
   SIZE p;

   if(init==1){ posx=sposx; posy=sposy; return; } 
   SelectObject(hdc,hgdiobj);
   SetTextColor(hdc,color);
   TextOut(hdc,posx,posy,buff,strlen(buff));
   GetTextExtentPoint32(hdc,buff,strlen(buff),&p);
   posx+=p.cx; 
   return;
}

//void mydrawtext(HWND hw,COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposy,int index,int init)
void mydrawtext(HWND hw,COLORREF color,HGDIOBJ hgdiobj,char *buff,int sposy,int init)
{
    static int posx,posy;
    SIZE p;
	//HWND hw;
	HDC hd;

	if(init==1){ posx=0; posy=sposy; return; } 
	p.cx=p.cy=0;
	hd=GetDC(hw);
    SelectObject(hd,hgdiobj);
    SetTextColor(hd,color);
	SetBkColor(hd, BACKGROUND);
    TextOut(hd,posx,posy,buff,strlen(buff));
	GetTextExtentPoint32(hd,buff,strlen(buff),&p);
	posx+=p.cx;
    DeleteDC(hd);
	return;
}

void paintrandommusicscores(SCORE *apscore)
{
   char aa[64];
   int score;

   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"Total Compases: ");
   mydrawtext(hwndnumeric[0],BLACK,hfontsystem,aa,0,0);
   score=melodia->numcompases;
   sprintf(aa,"%d ",(int) score);
   mydrawtext(hwndnumeric[0],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"Compas Actual: ");
   mydrawtext(hwndnumeric[0],BLACK,hfontsystem,aa,0,0);
   score=melodia->puneje.compas;
   sprintf(aa,"%d ",(int) score);
   mydrawtext(hwndnumeric[0],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"   C. Teorico: ");
   mydrawtext(hwndnumeric[1],BLACK,hfontsystem,aa,0,0);
   score=apscore->itempocompas;
   sprintf(aa,"%d      ",(int) score);
   mydrawtext(hwndnumeric[1],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"   C. Real: ");
   mydrawtext(hwndnumeric[1],BLACK,hfontsystem,aa,0,0);
   score=apscore->tempocompas;
   sprintf(aa,"%d      ",(int) score);
   mydrawtext(hwndnumeric[1],RED,hfontsystem,aa,0,0);
   
   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"   Compases OK: ");
   mydrawtext(hwndnumeric[2],BLACK,hfontsystem,aa,0,0);
   score=apscore->oktempocompas;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[2],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"   Compases Slow: ");
   mydrawtext(hwndnumeric[3],BLACK,hfontsystem,aa,0,0);
   score=apscore->slowtempocompas;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[3],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"   Compases Fast: ");
   mydrawtext(hwndnumeric[4],BLACK,hfontsystem,aa,0,0);
   score=apscore->fasttempocompas;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[4],RED,hfontsystem,aa,0,0);
   
   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"   Notas OK: ");
   mydrawtext(hwndnumeric[2],BLACK,hfontsystem,aa,0,0);
   score=apscore->okduringnote;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[2],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"   Notas Slow: ");
   mydrawtext(hwndnumeric[3],BLACK,hfontsystem,aa,0,0);
   score=apscore->slowduringnote;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[3],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"   Notas Fast: ");
   mydrawtext(hwndnumeric[4],BLACK,hfontsystem,aa,0,0);
   score=apscore->fastduringnote;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[4],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"   Nota Hits: ");
   mydrawtext(hwndnumeric[5],BLACK,hfontsystem,aa,0,0);
   score=apscore->hits;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[5],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,0,1);
   sprintf(aa,"   Nota Fails: ");
   mydrawtext(hwndnumeric[6],BLACK,hfontsystem,aa,0,0);
   score=apscore->fails;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[6],RED,hfontsystem,aa,0,0);

   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"   %%Nota Hits: ");
   mydrawtext(hwndnumeric[5],BLACK,hfontsystem,aa,0,0);
   score=apscore->perhits;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[5],RED,hfontsystem,aa,0,0);
   
   mydrawtext(NULL,BLACK,hfontsystem,NULL,15,1);
   sprintf(aa,"   %%Nota Fails: ");
   mydrawtext(hwndnumeric[6],BLACK,hfontsystem,aa,0,0);
   score=apscore->perfails;
   sprintf(aa,"%d    ",(int) score);
   mydrawtext(hwndnumeric[6],RED,hfontsystem,aa,0,0);
   return;
}

void paintscores(SCORE *apscore)
{
   char aa[64];
   int score,posx,posy;
   static int hits,fails,lastpaint,lastscore;
   int orgs=gStaff[64-36]-295;

   SetBkColor(hdc,BACKGROUND);
   posx=testRect.left+10; //testRect.right*11/16;
   posy=testRect.top+15;
   mydrawtext1(BLACK,hfontsystem,NULL,posx,posy,1);
   sprintf(aa,"%%Hits: ");
   mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
   score=apscore->perhits;
   sprintf(aa,"%d ",(int) score);
   mydrawtext1(RED,hfontsystem,aa,posx,posy,0);
   sprintf(aa,"   %%Fails: ");
   mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
   score=apscore->perfails;
   sprintf(aa,"%d ",(int) score);
   mydrawtext1(RED,hfontsystem,aa,posx,posy,0);

   sprintf(aa,"   Hits: ");
   mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
   score=apscore->hits;
   sprintf(aa,"%d ",(int) score);
   mydrawtext1(RED,hfontsystem,aa,posx,posy,0);
   sprintf(aa,"   Fails: ");
   mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
   score=apscore->fails;
   sprintf(aa,"%d  ",(int) score);
   mydrawtext1(RED,hfontsystem,aa,posx,posy,0);
   if(showtest3==TRUE || showtest4==TRUE){
      sprintf(aa,"  Hits/seg: ");
      mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
      score=apscore->tempocompas;
      sprintf(aa,"%d  ",(int) score);
      mydrawtext1(RED,hfontsystem,aa,posx,posy,0);
      sprintf(aa," (");
      strcat(aa,tablatempo(apscore->tempocompas));
	  strcat(aa,")");
      mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);     
      if(showtest3==TRUE){		 
		 COLORREF color;
		 posx=testRect.left+10; //testRect.right*11/16;
         posy=testRect.top+30;
         mydrawtext1(BLACK,hfontsystem,NULL,posx,posy,1);
		 sprintf(aa,"Ritmo: ");
	     mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
         sprintf(aa,"%1.1f",notereadingritmo);
		 if(notereadingritmo<5) color=RED;
		 if(notereadingritmo>=5 && notereadingritmo<8) color=ORANGE;
		 if(notereadingritmo>=8) color=GREEN;
         mydrawtext1(color,hfontsystem,aa,posx,posy,0);

		 sprintf(aa,"  Legato: ");
	     mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
         sprintf(aa,"%1.1f",notereadinglegato);
		 if(notereadinglegato<5) color=RED;
		 if(notereadinglegato>=5 && notereadinglegato<8) color=ORANGE;
		 if(notereadinglegato>=8) color=GREEN;
         mydrawtext1(color,hfontsystem,aa,posx,posy,0);
         
		 sprintf(aa,"  T.Reacción: ");
	     mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);
         sprintf(aa,"%2.2f",notereadingscore.reaction);
		 if(notereadingscore.reaction<200) color=GREEN;
		 if(notereadingscore.reaction>=200 && notereadingscore.reaction<400) color=ORANGE;
		 if(notereadingscore.reaction>=400 || notereadingscore.reaction<1) color=RED;
         mydrawtext1(color,hfontsystem,aa,posx,posy,0);

	     sprintf(aa,"     Puntuación: ");
	     mydrawtext1(BLACK,hfontsystem,aa,posx,posy,0);     
		 if(apscore->qos<5) color=RED;
		 if(apscore->qos>=5 && apscore->qos<8) color=ORANGE;
		 if(apscore->qos>=8) color=GREEN;
	     sprintf(aa,"%1.1f",apscore->qos);
         mydrawtext1(color,hfontsystem,aa,posx,posy,0);     
	  }
   }
   int offset;

   if(showtest4==1) offset=130;
   else offset=80;
   if(apscore->hits==0) hits=apscore->hits;
   if(apscore->fails==0) fails=apscore->fails;
   if(hits==0 && fails==0) lastpaint=-1;
   if(apscore->hits!=hits){
      hits=apscore->hits;
	  paintok(offset,orgs,1,hits);
	  lastpaint=1; lastscore=hits;
   }else
      if(apscore->fails!=fails){
         fails=apscore->fails;
	     paintok(offset,orgs,0,fails);
		 lastpaint=0; lastscore=fails;
	  }else if(lastpaint!=-1) paintok(offset,orgs,lastpaint,lastscore);
   return;
 }

void resetscores(MELODIA *melodia)
{
	melodia->score.fails=melodia->score.hits=0;
    melodia->score.perfails=melodia->score.perhits=0;
	melodia->score.itempocompas=melodia->score.tempocompas=0;
	melodia->score.fasttempocompas=melodia->score.oktempocompas=0;
	melodia->score.slowtempocompas=0;
	melodia->score.okduringnote=melodia->score.slowduringnote=0;
    melodia->score.fastduringnote=0;
	return;
}


void resetmelodia(MELODIA *melodia,int initcompas)
{
	int r,c,v,s;
	int numcompas,numnotas;
	EJECUCION *eje;

    numcompas=melodia->numcompases;
	for(r=0;r<2;r++){
	   for(c=initcompas;c<numcompas;c++){
          eje=generaejecucion(melodia->penta,c);
          numnotas=melodia->penta[r].compas[c].numnotas;
		  melodia->penta[r].compas[c].timecompas.time=-1;
		  melodia->penta[r].compas[c].timecompas.valid=0;
		  melodia->penta[r].compas[c].std.durcompas=0;
		  melodia->penta[r].compas[c].std.durcompasqos=0;
		  melodia->penta[r].compas[c].std.notasfast=0;
		  melodia->penta[r].compas[c].std.notasjust=0;
          melodia->penta[r].compas[c].std.notasslow=0;
		  melodia->penta[r].compas[c].std.notasfastqos=0;
		  melodia->penta[r].compas[c].std.notasjustqos=0;
          melodia->penta[r].compas[c].std.notasslowqos=0;
		  melodia->penta[r].compas[c].std.durnotastotal=0;
		  melodia->penta[r].compas[c].std.ritmic=0;
		  for(v=0;v<numnotas;v++){ 
			 melodia->penta[r].compas[c].color[v]=0;
             melodia->penta[r].compas[c].timenota[v].time=-1;
		  }
		  for(s=0;s<eje->penta[0].hor;s++)
			 melodia->penta[r].compas[c].errormarca[s]=-1;
		  delete [] eje;
	   }
	}
	return;
}


int calcnoteindex(NOTA *n1)
{
    int r;

	if(n1->nota<36){
	   for(r=0;r<15;r++) // solo en clave de fa
	      if(n1->nota==outnotasminfStaff[r]) return r;
	}
	for(r=0;r<44;r++)
	   if(n1->nota==notas00[r]) return r;
    return -1;
}

int calcnoteindex(int n1)
{
    int r;

	if(n1<36){
	   for(r=0;r<15;r++)
	      if(n1==outnotasminfStaff[r]) return r;
	}
	for(r=0;r<44;r++)
	   if(n1==notas00[r]) return r;
    return -1;
}

// devuelve 1 si esta en un espacio
int onspace(int n1)
{
	int v;

	v=calcnoteindex(n1);
    if((v%2)==0) return 0;
	return 1;
}

// devuelve 1 si las notas estan juntas en el pentagrama
int istogether(int n1,int n2)
{
	int v1,v2,d;

    v1=calcnoteindex(n1);
    v2=calcnoteindex(n2);
    d=v1-v2;
	d=abs(d);
	if(d!=1) return 0;
	return 1;    	
}

int armadura(int minota,int acci,int ar)
{
   int posar,s,b,r;
   NOTA nt;
   int sos[7]={3,0,4,1,5,2,6};  // fa,do,sol,re,la,mi,si
   int bem[7]={6,2,5,1,4,0,3};  // si,mi,la,re,sol,do,fa

   nt.nota=minota;
   if(minota==0) return 0;           // se trata de un silencio
   if(acci!=0){
      if(acci==1) return 1;     // sostenido
	  if(acci==-1) return -1;   // bemol
	  if(acci==-3) return 0;     // becuadrado
   }
   posar=calcnoteindex(&nt);
   if(posar==-1) 
	   exit(4);
   posar=posar%7;
   if(nt.nota<36)
      posar=7-posar;
   if(ar==0) return 0;
   s=b=-1;
   for(r=0;r<abs(ar);r++){
	  if(sos[r]==posar) s=r;
      if(bem[r]==posar) b=r;
   }
   if(ar>0 && s>=0) return 1;
   if(ar<0 && b>=0) return -1;	     
   return 0;
}

void calcarmadura(int **&posarm,int armadura)
{
   int v,k,m;
   static int *arma[2];
   static int sarmag[2][7]={{77,72,79,74,69,76,71},{71,76,69,74,67,72,65}};
   static int sarmaf[2][7]={{53,48,55,50,45,52,47},{47,52,45,50,43,48,41}};
   static int armag[7],armaf[7];

   arma[0]=armag; arma[1]=armaf;
   if(armadura>=0) m=0;
   else m=1;
   for(v=0;v<7;v++){
      k=armag[v]=gStaff[sarmag[m][v]-36]-250;
	  k=armaf[v]=fStaff[sarmaf[m][v]-36]-250;
   }
   posarm=arma;
   return;
}

void calcarmaduranotereading(int **&posarm,int armadura)
{
   int v,m,k,orgs;
   NOTA nt;
   static int *arma[2];
   static int sarmag[2][7]={{77,72,79,74,69,76,71},{71,76,69,74,67,72,65}};
   static int sarmaf[2][7]={{53,48,55,50,45,52,47},{47,52,45,50,43,48,41}};
   static int armag[7],armaf[7];

   arma[0]=armag; arma[1]=armaf;
   if(armadura>=0) m=0;
   else m=1;
   orgs=gStaff[64-36]-295;
   for(v=0;v<7;v++){
	  nt.nota=sarmag[m][v];
	  k=calcnoteindex(&nt);
      armag[v]=-(k-16)*ntinterline*0.5;
	  armaf[v]=-(k-16)*ntinterline*0.5;
   }
   posarm=arma;
   return;
}


void paintrandommusiccursor(MELODIA *melodia, COLORREF color)
{
    int index,compas,r;
	int pos[2],posx[2],orgs,orgf;
	char buff[2];
	
	orgs=gStaff[64-36]-250;
	orgf=fStaff[43-36]-250;	
	index=melodia->puneje.index;
	compas=melodia->puneje.compas;
	pos[0]=melodia->puneje.eje->penta[0].runnota[index].pos;
	pos[1]=melodia->puneje.eje->penta[1].runnota[index].pos;
	for(r=0;r<2;r++)
       posx[r]=melodia->penta[r].compas[compas].posx[pos[r]];
	SetTextColor(hdc,color);
	buff[0]=224; buff[1]=0;
//  TextOut(hdc,posx[0]+5,orgs-8*interline,buff,1);
	TextOut(hdc,posx[0]+5,posyminpenta-4*interline,buff,1);
    buff[0]=209; buff[1]=0;
//  TextOut(hdc,posx[1]+5,orgf+2.5*interline,buff,1);
	TextOut(hdc,posx[1]+5,posymaxpenta+2.5*interline,buff,1);
    return;
}

RECT *paintmetronomostaff(MELODIA *melodia,int gpos)
{
	char *s,aa[64];
	static RECT pos;
	int orgs,orgf,metronomo,tempocompas,compas,valid,b;
	int numsignature,densignature;
	TEXTMETRIC tm;
	SIZE p;

	if(gpos==1) return &pos;
	orgs=gStaff[64-36]-250;
	orgf=fStaff[43-36]-250;	
    SetTextColor(hdc,BLACK);
	metronomo=melodia->metronomo;
    
	compas=melodia->puneje.compas;
	numsignature=melodia->penta[0].compas[compas].numsignature;
    densignature=melodia->penta[0].compas[compas].densignature;
	
	if(metronomo<=0){
		//compas=melodia->puneje.compas;
	    if(compas>0){
	       valid=melodia->penta[0].compas[compas-1].timecompas.valid;
           if(valid==1){
	          tempocompas=melodia->penta[0].compas[compas-1].timecompas.time;
	          metronomo=60.0*CLOCKS_PER_SEC*numsignature/tempocompas;
		   }else return NULL;
		}else return NULL;
	}
	s=getnumtonum(metronomo);
    strcpy(aa,"            "); strcat(aa,s);
    TextOut(hdc,ExtraSpace+GClefWidth[1],orgs-7*interline,aa,strlen(aa));
	GetTextExtentPoint32(hdc,aa,strlen(aa),&p);
	SelectObject(hdc, hMaestroFontSmall2);
	s=gettiponotasingle(densignature*metrodivisor);
	strcpy(aa,"   ");
    for(b=0;b<metrodivisor;b++){
	   strcat(aa,s);
	   strcat(aa," ");
	}
	SetTextColor(hdc,GRAY);
    TextOut(hdc,ExtraSpace+GClefWidth[1]+p.cx,orgs-5.3*interline,aa,strlen(aa));
	SelectObject(hdc, hMaestroFontSmall1);
    GetTextMetrics(hdc,&tm);
	pos.left=ExtraSpace+GClefWidth[1]+p.cx;
	pos.top=orgs-2.5*interline;
	GetTextExtentPoint32(hdc,aa,strlen(aa),&p);
	pos.right=pos.left+p.cx;
	pos.bottom=pos.top+tm.tmDescent;
	SetTextColor(hdc,BLACK);
	return NULL;
}

void paintrandommusiccompas(MELODIA *melodia,int r,int c)
{
	char *s;
	PENTA *penta;

    penta=melodia->penta;
	SetTextColor(hdc,BLACK);
	TextOut(hdc,penta[r].compas[c].posxcompas,penta[r].compas[c].posycompas,"\\",1);
	s=getnumtonum(penta[r].compas[c].seqcompas+1);
	if(r==0){			 
	   SetTextColor(hdc,BLUE);
	   TextOut(hdc,penta[r].compas[c].posxfincompas,penta[r].compas[c].posycompas-12*interline,s,strlen(s));
	   SetTextColor(hdc,BLACK);
	   TextOut(hdc,penta[r].compas[c].posxfincompas,penta[r].compas[c].posycompas-6*interline,"ñ",1);
	}else{
	   SetTextColor(hdc,BLUE);
	   TextOut(hdc,penta[r].compas[c].posxfincompas,penta[r].compas[c].posycompas+9*interline,s,strlen(s));
	   SetTextColor(hdc,BLACK);
       TextOut(hdc,penta[r].compas[c].posxfincompas,penta[r].compas[c].posycompas+6*interline,"ñ",1);
	}
	return;
}

int paintstaff(HDC hdc,int x,int y,int can)
{
	LOGFONT lf;
	HFONT hf;
	HPEN pen;
    POINT p[2];
	int width,interln,r;

	hf=(HFONT) GetCurrentObject(hdc,OBJ_FONT);
	GetObject(hf,sizeof(LOGFONT),&lf);
	GetCharWidth32(hdc, 61, 61, &width);
	interln=-lf.lfHeight/4;
    pen=CreatePen(PS_SOLID,1,BLACK);
	SelectObject(hdc,pen);
 	p[0].x=x; p[1].x=x+width*can; 
	for(r=0;r<5;r++){
	   p[0].y=p[1].y=y+r*interln+1.8*interln;
       Polyline(hdc,p,2);
	}
	DeleteObject(pen);
    return interln;	
}

int ocultanotas(MELODIA *melodia,int r,int c,int v)
{
	int tmp,posme,posma,jj;
    PENTA *penta;

	penta=melodia->penta;
	if(concfg.ocultanota==1){
       if(melodia->puneje.index!=0 && melodia->puneje.compas==c && penta[r].compas[c].color[v]==BLACK)
	      return 1;
	}
    if(concfg.ocultanota==2){	      
	   if(melodia->puneje.index!=0 && melodia->puneje.compas==c){
		  jj=melodia->puneje.index; 
		  posme=melodia->puneje.eje->penta[r].runnota[jj].pos; 
		  posma=melodia->puneje.eje->penta[r].runnota[jj-1].pos; 
		  if(posme==posma) tmp=1;
		  else tmp=0;
	   }
	   if(melodia->puneje.index!=0 &&
		    melodia->puneje.compas==c && penta[r].compas[c].color[v]==BLACK &&
			tmp==0 && v==posme)
	      return 1;
	}
	if(concfg.ocultanota==3)
	   if(c<melodia->puneje.compas) return 1;
	return 0;
}

void paintrandommusicfunction(MELODIA *melodia)
{
    int v,r,c,h,maxcompascreen,fin,sb,oc,width,posx,posxfusa,posy,posxorg,posyorg,posyfusa,posxacci,posyacci,mnmx,canacci,nota;
	int chordp,updo,cl,*dispx,*dispy,interln,space,clef;
	int soltofa[]={0,1,1,0,0,1,1};
	int fatosol[]={1,1,0,1,1,0,0};
	__int64 chord,chordacci;
	int numnotas;
	int lenline,numsignature,densignature;
	int orgs,orgf,**posarm,fusaoffsetx,fusaoffsety;
	char aa[64],cc[64],*s,ss,sf;
	char bb[8]="////";
	ABC abc;
	PENTA *penta;
	RECT pp;

    SelectObject(hdc, hMaestroFontSmall3);
	orgs=gStaff[64-36]-250;
	orgf=fStaff[43-36]-250;
	pp=testRect; pp.left=0;pp.top=0; 
	FillRect(hdc,&pp,hWhiteBrush);
	SetBkMode(hdc, TRANSPARENT);
   
	paintstaff(hdc,ExtraSpace,orgs,NumOfStaffs); // staff usando lineas
	interln=paintstaff(hdc,ExtraSpace,orgf,NumOfStaffs); // staff usando lineas
	GetCharWidth32(hdc,250,250, &width);  // ancho de la cabeza de una negra (para acordes)
	GetCharWidth32(hdc,32,32, &space);
	GetCharABCWidths(hdc,207,207,&abc);
	width+=8;     // ajusta el desplazamiento de las cabezas en los acordes

	SetTextColor(hdc,GRAY);
	SelectObject(hdc, hMaestroFontSmall1);	
	TextOut(hdc,ExtraSpace, orgs-interline, "&", 1);
	TextOut(hdc, ExtraSpace-1,orgs, "\\", 1);
	TextOut(hdc,ExtraSpace, orgf-3*interline, "?", 1);		
    TextOut(hdc, ExtraSpace-1,orgf, "\\", 1);
    SetTextColor(hdc,BLACK);

	// dibuja manos a ejecutar
	if(melodia->oth!=0){
	   SelectObject(hdc, hSmallFont);
	   if(melodia->oth==1)
		  TextOut(hdc,ExtraSpace,orgs-3*interline,"(L.H.)",8);
	   if(melodia->oth==2)
		  TextOut(hdc,ExtraSpace,orgs-3*interline,"(R.H.)",8);
       SelectObject(hdc, hMaestroFontSmall1);	   
	}

	// actualiza armadura y signature en la cabecera de partitura segun el compas
	// que se ejecuta actualmente
	{
	   int compas=melodia->puneje.compas;
	   int index=melodia->puneje.index;
	   int canelem=melodia->puneje.canelem;
	   if(index>=canelem-2 && compas+1<melodia->numcompases) 
	      melodia->armadura=melodia->penta[0].compas[compas+1].armadura;
	   else 
          melodia->armadura=melodia->penta[0].compas[compas].armadura;
       numsignature=melodia->penta[0].compas[compas].numsignature;
       densignature=melodia->penta[0].compas[compas].densignature;
	}

	// dibuja armadura
	SetTextColor(hdc,BLACK);
    calcarmadura(posarm,melodia->armadura);	   
	sb=abs(melodia->armadura);	   
    strcpy(aa,"   ");
	for(v=0;v<sb;v++){	   
	   strcpy(cc,aa);
	   if(melodia->armadura>0) strcat(cc,"#");		  
	   else strcat(cc,"b");       
       TextOut(hdc,ExtraSpace+GClefWidth[1],posarm[0][v],cc,strlen(cc));
 	   TextOut(hdc,ExtraSpace+FClefWidth[1],posarm[1][v],cc,strlen(cc));
	   strcat(aa,"     ");
	}

    // dibuja valor de compas    
    strcpy(cc,aa);
	sprintf(cc,"%s   %d",aa,numsignature);
	TextOut(hdc, ExtraSpace+GClefWidth[1],orgs-3*interline,cc,strlen(cc));
    TextOut(hdc, ExtraSpace+FClefWidth[1],orgf-3*interline,cc,strlen(cc));		
	sprintf(cc,"%s   %d",aa,densignature);
	TextOut(hdc, ExtraSpace+GClefWidth[1],orgs-interline,cc,strlen(cc));
	TextOut(hdc, ExtraSpace+FClefWidth[1],orgf-interline,cc,strlen(cc));		
	lenline=2; 
	maxcompascreen=concfg.maxcompascreen; //(NumOfStaffs-OFFSETPENTA)/(BARTOBARSPACE);  //42 jordi
	penta=melodia->penta;
	
	for(r=0;r<2;r++){ 
	   for(c=offsetdispinterval;c<offsetdispinterval+maxcompascreen;c++){	   
		  if(c>=melodia->numcompases) break;
		  numnotas=penta[r].compas[c].numnotas;
		  if((c%maxcompascreen)==0 && 
			  penta[r].compas[c].posxcompas+penta[r].compas[c].posx[numnotas-1]>(NumOfStaffs-2)*StaffWidth[1] &&
			  maxcompascreen>1
			 )
			     break;
	      for(v=0;v<numnotas;v++){			 
			 chord=penta[r].compas[c].note[v];
			 posx=penta[r].compas[c].posx[v]-3;
			 clef=penta[r].compas[c].clef[v];
             for(;chord!=0;chord>>=8){
		        nota=(int) chord&0xff;
				if(r==0 && clef==1){
			       int ss;
                   ss=calcnoteindex(nota);  
                   ss=ss%7;
				   ss=20+fatosol[ss];
                   nota+=ss;
				}
				if(r==1 && clef==0){
			       int ss;
                   ss=calcnoteindex(nota);  
                   ss=ss%7;
				   ss=20+soltofa[ss];
                   nota-=ss;
				}
			    if(r==1 && nota>0){
				   SetTextColor(hdc,BLACK);
	               if(nota <= 26)
		              TextOut(hdc,posx, orgf+5*interline, "___", lenline);
	               if(nota <= 29)
		              TextOut(hdc,posx, orgf+4*interline, "___", lenline);
	               if(nota <= 33)
		              TextOut(hdc,posx, orgf+3*interline, "___", lenline);
	               if(nota <= 36)
		              TextOut(hdc,posx, orgf+2*interline, "___", lenline);
			       if(nota <= 40) 
			          TextOut(hdc,posx, orgf+interline, "___", lenline);
	               if(nota >= 60)
				      TextOut(hdc,posx, fStaff[60-36]-250, "___", lenline);
	               if(nota >= 64)				
				      TextOut(hdc,posx, fStaff[64-36]-250, "___", lenline);
	               if(nota >= 67)				
				      TextOut(hdc,posx, fStaff[67-36]-250, "___", lenline);
				}
		        if(r==0 && nota>0){
				   SetTextColor(hdc,BLACK);
	               if(nota <= 50)
		              TextOut(hdc,posx, orgs+4*interline, "___", lenline);
	               if(nota <= 53)
		              TextOut(hdc,posx, orgs+3*interline, "___", lenline);
	               if(nota <= 57)
		              TextOut(hdc,posx, orgs+2*interline, "___", lenline);
	               if(nota <= 60) 
			          TextOut(hdc,posx, orgs+interline, "___", lenline);
	               if(nota >= 81)
		              TextOut(hdc,posx, orgs-5*interline, "___", lenline);
	               if(nota >= 84)
		              TextOut(hdc,posx, orgs-6*interline, "___", lenline);			 			 					 
	               if(nota >= 88)
		              TextOut(hdc,posx, orgs-7*interline, "___", lenline);			 			 					 
	               if(nota >= 91)
		              TextOut(hdc,posx, orgs-8*interline, "___", lenline);			 			 					 
	               if(nota >= 95)
		              TextOut(hdc,posx, orgs-9*interline, "___", lenline);			 			 					 
	               if(nota >= 98)
		              TextOut(hdc,posx, orgs-10*interline, "___", lenline);			 			 					 
	               if(nota >= 102)
		              TextOut(hdc,posx, orgs-11*interline, "___", lenline);			 			 					 
	               if(nota >= 105)
		              TextOut(hdc,posx, orgs-12*interline, "___", lenline);			 			 					 
				}
			 }

			 // dibuja notas musicales			 
			 s=gettiponota(melodia,r,c,v,updo,cl);
			 oc=ocultanotas(melodia,r,c,v);
			 if(oc==0){	
				int fusa,bas,ischord;
                if(s[4]==1 || s[4]==2){ fusa=1;} else fusa=0;				
			    SetTextColor(hdc,penta[r].compas[c].color[v]);
				bas=getindexchordbase(melodia,r,c,v,interln,updo,dispx,dispy,cl,mnmx);
				ss=s[0]; sf=s[3];
				chord=penta[r].compas[c].note[v];
				if((unsigned __int64) chord>0xff) ischord=1; else ischord=0;
				chordacci=penta[r].compas[c].acci[v];
				for(canacci=0;chordacci>0;chordacci>>=8)
				   if((chordacci&0xff)>0) ++canacci;
				chordacci=penta[r].compas[c].acci[v];
                for(chordp=0;chord!=0 || (chord==0 && chordp==0) ;chord>>=8,chordacci>>=8,chordp++){				   
				   nota=(int) chord&0xff;
				   posxorg=penta[r].compas[c].posx[v];
				   posyorg=penta[r].compas[c].posy[v][chordp];
				   posx=posxorg+dispx[chordp]*width*0.5+2*space;
				   posy=posyorg-dispy[chordp];
				   //**************************************************
				   //12-07-14 memorizamos las posiciones de exactas de las notas
				   // para dibujar el tie a porteriori
				   penta[r].compas[c].posxfin[v][chordp]=posx;
				   posxfusa=penta[r].compas[c].posx[v]+2*space;
				   posyfusa=posyorg;
				   if(fusa==1){
				      SIZE sz;
				      if(updo==1){ 					     
					     fusaoffsetx=abc.abcA+abc.abcB-1;
					     GetTextExtentPoint32(hdc,"q",1,&sz);
					     if(s[4]==1) fusaoffsety=-0.25*sz.cy;
						 if(s[4]==2) fusaoffsety=-0.37*sz.cy;
					  }else{ 
						 GetTextExtentPoint32(hdc,"Q",1,&sz);
						 fusaoffsetx=0;
						 if(s[4]==1) fusaoffsety=0.25*sz.cy;
						 if(s[4]==2) fusaoffsety=0.37*sz.cy;
					  }
					  s[4]=0;
				   }
				   if(chord!=0){
					  int adjust;
				      //posyacci= r==0 ? gStaff[(chord&0xff)-36]-250 : fStaff[(chord&0xff)-36]-250;
			          if(!isnotewhole(melodia,r,c,v)) adjust=-0.5*interline; 
			          else adjust=0;
                      posyacci=posyorg+adjust;
				   }
				   //if(canacci>1 && ischord==1){ 
			       posxacci=posxorg; //-mnmx*width*0.5;
				   if(dispx[chordp]<0) posxacci-=width*0.4;
				   if(canacci>0){
				      if(ischord==1){
						 double alfa;
					   //posxacci=posx+mnmx*width*0.8; //*0.5;
						 if(onspace(nota)==1) alfa=0.5;
						 else alfa=1;    // esta sobre una ralla
					     posxacci=posxorg+2*space-width*0.8*alfa;
					     if(updo==-1 && dispx[chordp]==-1 && alfa==0.5)						 
					        posxacci-=4.5*space; //2.5

					     //if(mnmx==0) posxacci=posxorg;                       
					  }					  
				   }		
				   // dibuja clave pequeña en pentagrama si cabe
				   if(chordp==0)
					  paintclaveonpenta(hdc,melodia,r,c,v,posxacci);
				   if(chordp>=0){
				      if(chordp!=bas && bas!=-1){
						  s[3]=0; 
						  if(cl==0) s[0]=207;   // cabeza negra
						  if(cl==1) s[0]=250;        // cabeza blanca
                      }else{ s[0]=ss; s[3]=sf; }
				   }
				   // dibuja nota
 				   strcpy(aa,s);
				   TextOut(hdc,posx,posy,aa,strlen(aa));
				   if(fusa==1){
				      strcpy(aa,&s[3]);
					  TextOut(hdc,posxfusa+fusaoffsetx,posyfusa+fusaoffsety,aa,strlen(aa));
				   }
				   switch(chordacci&0xff){
				      case 3:    // BECUADRADO
				         aa[0]=233; aa[1]=0;
						 TextOut(hdc,posxacci,posyacci,aa,strlen(aa)); // becuadrado
						 break;
				      case 0:     // SIN ALTERACION
				         break;
					  case 1:     // SOSTENIDO
						 strcpy(aa,"I"); 
						 TextOut(hdc,posxacci,posyacci,aa,strlen(aa)); // sostenido
					     break;
					  case 2:     // BEMOL
					     strcpy(aa,"i");		
						 TextOut(hdc,posxacci,posyacci,aa,strlen(aa)); // bemol
					     break;
				   }
				}
				//***************************************************
				// 12-07-14 dibuja ties
				// de momento solo los del compas actual y no los de paso entre compases
				if(v>0) paintties(hdc,melodia,r,c,v,interln,orgs,orgf);
				//****************************************************************
                if(chordp>1)
				   paintstickchord(hdc,melodia,r,c,v,dispy,chordp,updo);   
			 }
	         // dibuja finguering	         
			 //SetTextColor(hdc,RGB(122,14,86));
			 fin=melodia->penta[r].compas[c].fingering[v];
			 if(fin>0){
				char buf[64];
				sprintf(buf,"  %d",melodia->penta[r].compas[c].fingering[v]);
			    if(r==0)
	               mydrawtext2(RGB(122,14,86),hfontsystem,buf,penta[r].compas[c].posx[v]+3,orgs-7*interline);
			    if(r==1)
	               mydrawtext2(RGB(122,14,86),hfontsystem,buf,penta[r].compas[c].posx[v]+3,orgf+12*interline);
			 }
		     // dibuja errores de ritmo
			 // siempre y cuando puntuemos que hay que tocar con el tempo indicado por metronomo
			 if(concfg.randommusicqoscompas==1)
                paintrandommusicrithm(melodia,r,c,v);
			 
		  }
		  
  		  // dibuja indicacion de teclas erroneas
          intervaltestfail(melodia,r,c);
	      
		  // dibuja figura de compas y su numero 
		  paintrandommusiccompas(melodia,r,c);
		  if(c==offsetdispinterval && c>0) paintrandommusiccompas(melodia,r,c-1);

          // dibuja barras de exactitud en el ritmo
		  paintritmicexecution(melodia,r,c-1,0);

		  // si puntuamos solo ritmo sin tener en cuenta el tempo del metronomo
		  // entonces dibujamos la exactitud de nuestro propio tempo (no el del metronomo)
		  if(concfg.randommusicqosritmo==1 && concfg.randommusicqoscompas==0)
             paintrandommusicsolorithm(melodia,r,c-1);

		  // dibuja la desviacion del tempo del compas actual respecto al anterior,
          painttempobymeasure(melodia,r,c-1);		  	   
       }
       h=penta[r].compas[c-1].posxcompas+BARTOBARSPACE*StaffWidth[1]; 
	   if(c==melodia->numcompases) 
		   strcpy(bb,"\\ \\\\"); // final de melodia
	   else strcpy(bb,"\\"); // final de compases en pantalla
       if(r==0) TextOut(hdc,h,orgs,bb, strlen(bb));
	   else TextOut(hdc,h,orgf,bb, strlen(bb));
	}

	randommusicrithmscore(melodia);
	tempobymeasurescore(melodia);
	//dibuja metronomo
	if(concfg.ritmo==1)
	   paintmetronomostaff(melodia,0);
	// dibuja cursor
	paintrandommusiccursor(melodia,BLUE);
	// dibuja scores
	paintrandommusicscores(&melodia->score);
	// dibuja score del tempo medio
    painttemposcore(melodia);
	if(siguemestatus==1 && htwoplay!=NULL){
	   paintrandommusiccursor1(NULL,0,hdc,0);
	}
    paintrandommusicqosmeasure(melodia);
	SetBkMode(hdc, OPAQUE);
	return;
}

void paintrandommusic(MELODIA *melodia)
{    
    HDC hdcmem,hdcbackup;   
    HBITMAP hbmmem;
	HGDIOBJ hold;

	while(semapaintclocksand1==1); // esperamos a que se pinte el reloj de arena antes de pintar
    hdcbackup=hdc;
    hdcmem = CreateCompatibleDC(hdc);
	if(currenthor==1024){
	   if(concfg.maxcompascreen>4)
          hbmmem = CreateCompatibleBitmap(hdc,testRect.right,top260-44); //testRect.Bottom
	   else 
		  hbmmem = CreateCompatibleBitmap(hdc,testRect.right,topvirstaff);  //testRect.Bottom
	}else hbmmem = CreateCompatibleBitmap(hdc,testRect.right,top260-44);
	hdcbackup=hdc;
	hdc=hdcmem;
    hold=SelectObject(hdcmem, hbmmem);
    paintrandommusicfunction(melodia);
	hdc=hdcbackup;
    // recortamos a 0.30 para que quepan los controles deslizantes
    // BitBlt(hdc,0,30,testRect.right,top260-44,hdcmem, 0, 30, SRCCOPY);
	if(currenthor==1024){
	   if(concfg.maxcompascreen<=4)
	      BitBlt(hdc,0,30,testRect.right,topvirstaff-120,hdcmem, 0, 30, SRCCOPY);
	   else
          BitBlt(hdc,0,30,testRect.right,topvirstaff,hdcmem, 0, 30, SRCCOPY);
	}else BitBlt(hdc,0,30,testRect.right,topvirstaff,hdcmem, 0, 30, SRCCOPY);
    SelectObject(hdcmem, hold);
    DeleteObject(hbmmem);
    DeleteDC(hdcmem);
	return;
}

void removenote(BUFFMIDI *buff,int *topbuff,int index)
{
	int r;

	for(r=index;r<*topbuff-1;r++){
	   if(buff[r].timenota!=NULL){
		  buff[r].timenota->time=clock()-buff[r].timestamp;
          buff[r].timenota->valid=2;      // =1 18-07    
	      //*********18-07***********************************
		  int penta,inx;
          inx=melodia->puneje.index;
		  penta=buff[r].timenota->penta;
		  buff[r].timenota->ncompas=melodia->puneje.compas;
		  buff[r].timenota->npos=melodia->puneje.eje->penta[penta].runnota[inx].pos;
//		  if(penta==0 && buff[r].timenota->npos==2)
//			  printf("");
  //*************************************************
	   }
	   buff[r]=buff[r+1];
	}
	if(*topbuff==1 || index==(*topbuff-1)){
	   if(buff[r].timenota!=NULL){
		  buff[r].timenota->time=clock()-buff[r].timestamp;
          buff[r].timenota->valid=2;       // =1 18-07
	      //*********18-07***********************************
		  int penta,inx;
          inx=melodia->puneje.index;
		  penta=buff[r].timenota->penta;
		  buff[r].timenota->ncompas=melodia->puneje.compas;
		  buff[r].timenota->npos=melodia->puneje.eje->penta[penta].runnota[inx].pos;
//		  if(penta==0 && buff[r].timenota->npos==2)
//			  printf("");
		  //*************************************************
       }
	}
    --(*topbuff);
   return;
}

int numfreables(BUFFMIDI *buff,int *topbuff)
{
   int r,can;
   for(r=can=0;r<*topbuff;r++)
      if(buff[r].freeable==1) ++can;
   return can;
}

int addnote(BUFFMIDI *buff,int source,int *topbuff,int n)
{
	int r;
	FILE *fh;

	if(0){
		// DEBUG ***************
       fh=fopen("pepe1.txt","a");
	   fprintf(fh,"Valor entrante %d Source:%d \n",n,source);
	   fprintf(fh,"datos en buff EN ENTRADA \n");
	   for(r=0;r<*topbuff;r++)
          fprintf(fh,"   %d\n",buff[r].notekey);
	   fclose(fh);
	   //**********************
	}
	for(r=0;r<*topbuff;r++){
	   if(buff[r].notekey==n){
          removenote(buff,topbuff,r);
          if(0){
			  //*****DEBUG ******************
             fh=fopen("pepe1.txt","a");
	         fprintf(fh,"Elimino del buffer %d source: %d\n",n,buff[r].source);
		     fclose(fh);
              //*********************************
		  }
		  return 1;   // 1 si elimina alguna nota del buffermidi
	   }
	}	
	buff[(*topbuff)].notekey=n;
    buff[(*topbuff)].freeable=0;
    buff[(*topbuff)].source=source;
	buff[(*topbuff)].timenota=NULL;    
    buff[(*topbuff)++].timestamp=clock();
    return 0;        // 0 si añade nota
}			   

typedef struct bufftorun
{
   int val;
   int ind;
   int penta;
} BUFFTORUN;

int bufftoruncompare1( const void *arg1, const void *arg2 )
{
   int x,y;

   x=((BUFFTORUN *) arg1)->val;
   y=((BUFFTORUN *) arg2)->val;
   if(x<y) return 1;                 // antes -1
   if(x==y) return 0;
   if(x>y) return -1;
   return 1;
}

int bufftoruncompare2( const void *arg1, const void *arg2 )
{
   int x,y;

   x=((BUFFTORUN *) arg1)->penta;
   y=((BUFFTORUN *) arg2)->penta;
   if(x<y) return -1;                 // antes 1
   if(x==y) return 0;
   if(x>y) return 1;
   return 1;
}



void bufftorunnota(BUFFMIDI *buff,int topbuff,EJECUCION *eje,int index)
{
   int r,j,dif,stie;
   __int64 chord,tie;
   BUFFTORUN temp;
   BUFFTORUN v[64];
   BUFFTORUN teo[64];
   
   for(r=0;r<topbuff;r++){ 
      v[r].val=buff[r].notekey; 
	  v[r].ind=r;
	  v[r].penta=-1;
   }
   // ordenamos de mayor a menor segun notakey hayado en buff
   qsort((void *) v,(size_t) r,sizeof(BUFFTORUN),bufftoruncompare1);   
   for(j=r=0;j<2;j++){
	  chord=eje->penta[j].runnota[index].notakey;
      for(;chord!=0;chord>>=8){
         teo[r].val=(int) chord&0xff; 
	     teo[r].ind=r; 
		 teo[r++].penta=j;
	  }
   }

   // ordenamos de mayor a menor segun notakey en pentagrama
   qsort((void *) teo,(size_t) r,sizeof(BUFFTORUN),bufftoruncompare1);

   // respetamos el orden marcado en el pentagrama
   for(r=0;r<topbuff;r++)
      v[r].penta=teo[r].penta;

   // ordenamos de pentagrama en fa al de sol
   // agrupamos penta==0 a la izquierda del array y penta==1 a la derecha
   int next;
   while(1){
	  next=0;
      for(r=1;r<topbuff;r++){
	     if(v[r].penta<v[r-1].penta){
            temp=v[r-1];
		    v[r-1]=v[r];
		    v[r]=temp;
		    next=1;
		 }
	  }
	  if(next==0) break;
   }
   // qsort((void *) v,(size_t) r,sizeof(BUFFTORUN),bufftoruncompare2);
   
   for(j=0,r=0;j<2;j++){
      chord=eje->penta[j].runnota[index].notakey;
	  dif=eje->penta[j].runnota[index].dif;
	  // si dif==1 habra un cambio de posicion en el cursor
	  // cuando se incremente index 
	  //**********************************************
      
	  tie=eje->penta[j].runnota[index].tie;
	  
	  //**********************************************
      for(;chord!=0;chord>>=8,tie>>=8){
		 stie=tie&0xff;
		 if(dif!=0 && stie==0) 
			 buff[v[r++].ind].freeable=1;
		 else buff[v[r++].ind].freeable=0;
	  }
   }
   return;
}


/*
void bufftorunnota(BUFFMIDI *buff,int topbuff,EJECUCION *eje,int index)
{
   int r,j,dif,stie;
   __int64 chord,tie;
   BUFFTORUN temp;
   BUFFTORUN v[64];
   BUFFTORUN teo[64];
   
   for(r=0;r<topbuff;r++){ 
      v[r].val=buff[r].notekey; 
	  v[r].ind=r;
	  v[r].penta=-1;
   }
   // ordenamos de menor a mayor segun notakey hayado en buff
   qsort((void *) v,(size_t) r,sizeof(BUFFTORUN),bufftoruncompare1);   
   for(j=r=0;j<2;j++){
	  chord=eje->penta[j].runnota[index].notakey;
      for(;chord!=0;chord>>=8){
         teo[r].val=(int) chord&0xff; 
	     teo[r].ind=r; 
		 teo[r++].penta=j;
	  }
   }

   // ordenamos de menor a mayor segun notakey en pentagrama
   qsort((void *) teo,(size_t) r,sizeof(BUFFTORUN),bufftoruncompare1);

   // respetamos el orden marcado en el pentagrama
   for(r=0;r<topbuff;r++)
      v[r].penta=teo[r].penta;

   // ordenamos de pentagrama en fa al de sol
   // agrupamos penta==1 a la izquierda del array y penta==0 a la derecha
   int next=0;
   while(1){
      for(r=1;r<topbuff;r++){
	     if(v[r].penta>v[r-1].penta){
            temp=v[r-1];
		    v[r-1]=v[r];
		    v[r]=temp;
		    next=1;
		 }
	  }
	  if(next==0) break;
   }
   // qsort((void *) v,(size_t) r,sizeof(BUFFTORUN),bufftoruncompare2);
   
   for(j=1,r=0;j>=0;j--){
      chord=eje->penta[j].runnota[index].notakey;
	  dif=eje->penta[j].runnota[index].dif;
	  // si dif==1 habra un cambio de posicion en el cursor
	  // cuando se incremente index 
	  //**********************************************
      
	  tie=eje->penta[j].runnota[index].tie;
	  
	  //**********************************************
      for(;chord!=0;chord>>=8,tie>>=8){
		 stie=tie&0xff;
		 if(dif!=0 && stie==0) 
			 buff[v[r++].ind].freeable=1;
		 else buff[v[r++].ind].freeable=0;
	  }
   }
   return;
}
*/
int numnotasonbufferprocessed(BUFFMIDI *buff,int topbuff)
{
   int r,can;

   for(r=can=0;r<topbuff;r++)
	   if(buff[r].timenota!=NULL) ++can;
   return can;
}

void paintrandommusicfail(MELODIA *melodia,int r,int c,int nota,int posx)
{
   int j,posy,acci;
   int orgs,orgf,lenline;
   char aa[10];   
   
   aa[0]=0x20; aa[1]='w'; aa[2]=0; //220
   lenline=2;
   orgs=gStaff[64-36]-250;
   orgf=fStaff[43-36]-250;	   
   if(nota<0) return;
   for(j=acci=0;j<44;j++) if(notas00[j]==nota) break; 
   if(j==44)
      for(j=0;j<44;j++) if(notas00[j]==nota-1){ nota=notas00[j]; acci=1; break; }	
   if(j==44) return;
   nota=notas00[j];   
   if(nota>=60) r=0;   // si mayor o igual que Do central representar en clave de SOL
   if(nota<60) r=1;    // si menor que Do central en clave de FA
   if(r==0) posy=gStaff[nota-36]-250;	
   else{ 
	  if(nota>=36)
         posy=fStaff[nota-36]-250;   
	  else
		 posy=outminfStaff[36-nota]-250;
   }
   SelectObject(hdc, hMaestroFontSmall1);
   if(r==1){
      SetTextColor(hdc,BLACK);
      if(nota <= 26)
         TextOut(hdc,posx, orgf+5*interline, "___", lenline);
      if(nota <= 29)
         TextOut(hdc,posx, orgf+4*interline, "___", lenline);
      if(nota <= 33)
         TextOut(hdc,posx, orgf+3*interline, "___", lenline);
	  if(nota <= 36)
	     TextOut(hdc,posx, orgf+2*interline, "___", lenline);
      if(nota <= 40) 
	     TextOut(hdc,posx, orgf+interline, "___", lenline);
	  if(nota >= 60)
	     TextOut(hdc,posx, fStaff[60-36]-250, "___", lenline);
	  if(nota == 64)				
	     TextOut(hdc,posx, fStaff[64-36]-250, "___", lenline);
	  }
   if(r==0){
      SetTextColor(hdc,BLACK);
	  if(nota <= 57)
		 TextOut(hdc,posx, orgs+2*interline, "___", lenline);
	  if(nota <= 60) 
	     TextOut(hdc,posx, orgs+interline, "___", lenline);
	  if(nota >= 81)
	     TextOut(hdc,posx, orgs-5*interline, "___", lenline);
	  if(nota >= 84)
	     TextOut(hdc,posx, orgs-6*interline, "___", lenline);			 			 					 
	  if(nota >= 88)
	     TextOut(hdc,posx, orgs-7*interline, "___", lenline);			 			 					 
      if(nota >= 91)
         TextOut(hdc,posx, orgs-8*interline, "___", lenline);			 			 					 
      if(nota >= 95)
         TextOut(hdc,posx, orgs-9*interline, "___", lenline);			 			 					 
      if(nota >= 98)
         TextOut(hdc,posx, orgs-10*interline, "___", lenline);			 			 					 
      if(nota >= 102)
         TextOut(hdc,posx, orgs-11*interline, "___", lenline);			 			 					 
      if(nota >= 105)
         TextOut(hdc,posx, orgs-12*interline, "___", lenline);			 			 					 
   }

   if(acci==0) SetTextColor(hdc,RGB(167,84,84));
   else SetTextColor(hdc,RGB(114,56,56));
   SelectObject(hdc, hMaestroFontSmall2);
   //TextOut(hdc,posx+5,posy+2*interline+1,aa,strlen(aa));
   // 1024x768 ---> 1.9 fuente 16
   // 1280x1024 ---> 2.3  fuente 24
   if(STAFFSIZEINTERVAL==24){
	  if(concfg.maxcompascreen<=4) TextOut(hdc,posx+5,posy+2.3*interline,aa,strlen(aa));
	  else TextOut(hdc,posx+5,posy+1.9*interline,aa,strlen(aa));
   }
   if(STAFFSIZEINTERVAL==40)
      TextOut(hdc,posx+5,posy+2.3*interline,aa,strlen(aa));
   SelectObject(hdc, hMaestroFontSmall1);
   return;
}

void intervaltestfail(MELODIA *melodia,int r,int c)
{
   int s,nota,index,t0,t1;
   EJECUCION *eje;
   
   eje=generaejecucion(melodia->penta,c);
   for(s=0;s<eje->penta[0].hor;s++){
      nota=melodia->penta[r].compas[c].errormarca[s];
	  index=eje->penta[0].runnota[s].pos;
	  t0=melodia->penta[0].compas[c].posx[index];
	  index=eje->penta[1].runnota[s].pos;
	  t1=melodia->penta[1].compas[c].posx[index];
	  if(t1>t0) t0=t1;
      paintrandommusicfail(melodia,r,c,nota,t0);
   }
   delete [] eje;
   return;
}

void intervalscores(MELODIA *melodia)
{
   int r,numnotas,c,v;
   int tot;

   melodia->score.fails=melodia->score.hits=0;
   melodia->score.perfails=melodia->score.perhits=0;
   for(r=0;r<2;r++){
	  if(gfstat[r]==1) continue;      
	    for(c=0;c<=melodia->puneje.compas;c++){
		 numnotas=melodia->penta[r].compas[c].numnotas;
         for(v=0;v<numnotas;v++){
	        if(melodia->penta[r].compas[c].color[v]==RED || 
				melodia->penta[r].compas[c].color[v]==PURPLE ){
			    ++melodia->score.fails;
			}
			if(melodia->penta[r].compas[c].color[v]==GREENBOLD){
			   ++melodia->score.hits;
			}
		 }
	  }
   }
   tot=melodia->score.fails+melodia->score.hits;
   melodia->score.perfails=melodia->score.fails*100.0/tot;
   melodia->score.perhits=melodia->score.hits*100.0/tot;
   return;
}

void IntervalTestUpdate3(MELODIA *melodia)
{
   intervalscores(melodia);
   PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas-1),4,1,-100);
   return;
}

void IntervalTestUpdate2(MELODIA *melodia)
{
   intervalscores(melodia);
   calctempomedio(melodia,0,melodia->puneje.compas-2);
   PaintNoteInterval(scrollnoteinterval(melodia,melodia->puneje.compas-1),6,0,concfg.armaduranoteinterval);   
   return;
}

void IntervalTestUpdate1(MELODIA *melodia)
{
   int numc;

   if(showtest2==TRUE) return; // si localizacion de notas TRUE no ejecutamos esta rutina 
   intervalscores(melodia);
   numc=concfg.maxcompascreen; //(NumOfStaffs-OFFSETPENTA)/(BARTOBARSPACE);   //42 jordi
   offsetdispinterval=numc*(melodia->puneje.compas/numc);
   // jordi linea siguiente añadida el 18-12-2013
   // puede ocurrir que tengamos silencios al final del compas anterior, intervaltestupdate
   // se los saltara y avanzara al compas siguiente, y si este empieza por silencio, tambien se lo saltara
   // con lo cual index sera distinto de cero y la condicion de mas abajo no se cumplira 
   // y no se hara el scroll correspondiente si se requiere.
   // Se resuelve el problema incluyendo Intervaltestupdate1 en intervaltestupdate
   if(melodia->puneje.index==0 && melodia->puneje.compas==offsetdispinterval)
      scrollrandommusic(melodia,offsetdispinterval);
   calctempomedio(melodia,0,melodia->puneje.compas-2);
   paintrandommusic(melodia);    //ojo
   return;
}
 
int isinchord(int notekey,__int64 chord,__int64 tie,int &istie)
{
   int stie;
   for(;chord!=0;chord>>=8,tie>>=8){
      stie=(int) (tie&0xff);
      if((chord&0xff)==notekey){ 
		  istie=stie;
		  return 1;
	  }
   }
   return 0;
}

int numnotasverticaleje(PUNTERO *ap,int *numnotasindex)
{
   EJECUCION *eje;
   int index,r,can;
   __int64 temp;

   eje=ap->eje;
   index=ap->index;   
   for(r=can=0;r<2;r++){
	  numnotasindex[r]=0;
	  temp=eje->penta[r].runnota[index].nota;
	  for(;temp!=0;temp>>=8){ ++numnotasindex[r]; ++can;}
   }
   return can;
}

int cardinal(__int64 aa)
{
	int r;

	for(r=0;aa!=0;aa>>=8)
	   if((aa&0xff)!=0) ++r;
	return r;
}
	   
__int64 extract(__int64 aa,__int64 ar,__int64 taa,__int64 tar)
{
	int v,t,r[16],saa,sar,found;
	__int64 res,temp;

	for(v=0;aa!=0;aa>>=8,taa>>=8){
	   saa=(int) aa&0xff;
	   if((taa&0xff)!=0)          
	      r[v++]=saa;
       else{
		  res=ar;
		  temp=tar;
          found=0;
		  for(;res!=0;res>>=8,temp>>=8){
	         sar=(int) res&0xff;
             if(sar==saa){
				found=1;
			    if((temp&0xff)==0)
				   r[v++]=saa;
				break;
			 }
		  }
		  if(found==0) r[v++]=saa;
	   }
	}
	res=0;
	for(t=v-1;t>=0;t--){
	   res=res<<8;
	   res|=(r[t]&0xff);
	}
	return res;
}

void avanceindex(MELODIA *melodia)
{
	int index,canelem,r,compas,pos[2],npos[2];
	__int64 tie[2],nota[2],nnota[2];
	int cnota[2],cnnota[2],ctie[2];

	canelem=melodia->puneje.canelem;
	index=melodia->puneje.index;
	compas=melodia->puneje.compas;
	while(1){
	   if(index==canelem-1){ ++index; break; }
	   for(r=0;r<2;r++){
	      tie[r]=melodia->puneje.eje->penta[r].runnota[index].tie;
          nota[r]=melodia->puneje.eje->penta[r].runnota[index].notakey;
          nnota[r]=melodia->puneje.eje->penta[r].runnota[index+1].notakey;
		  pos[r]=melodia->puneje.eje->penta[r].runnota[index].pos;
		  npos[r]=melodia->puneje.eje->penta[r].runnota[index+1].pos;
		  cnota[r]=cardinal(nota[r]);
		  cnnota[r]=cardinal(nnota[r]);
		  ctie[r]=cardinal(tie[r]);
	   }
	   if(nota[0]==0 && ctie[0]!=0) cnota[0]=ctie[0];
	   if(nota[1]==0 && ctie[1]!=0) cnota[1]=ctie[1];
	   if(index==2 && compas==13)
		   printf("");
	   // if(nota[0]==nnota[0] && tie[0]==1 && nota[1]==nnota[1] && tie[1]==1){
       if(nota[0]==nnota[0] && ctie[0]==cnota[0] && nota[1]==nnota[1] && ctie[1]==cnota[1]){
	      melodia->penta[0].compas[compas].color[npos[0]]=melodia->penta[0].compas[compas].color[pos[0]];	  
		  melodia->penta[1].compas[compas].color[npos[1]]=melodia->penta[1].compas[compas].color[pos[1]];	  
          melodia->penta[0].compas[compas].errormarca[index+1]=melodia->penta[0].compas[compas].errormarca[index];
		  melodia->penta[1].compas[compas].errormarca[index+1]=melodia->penta[1].compas[compas].errormarca[index];
          ++index;
		  continue;
	   }
	   if(nota[0]==nnota[0] && ctie[0]==cnota[0] && pos[1]==npos[1] && ctie[1]==0){
	      melodia->penta[0].compas[compas].color[npos[0]]=melodia->penta[0].compas[compas].color[pos[0]];	  
		  melodia->penta[0].compas[compas].errormarca[index+1]=melodia->penta[0].compas[compas].errormarca[index];
		  ++index;
		  continue;
	   }
	   //if(nota[1]==nnota[1] && tie[1]==1 && pos[0]==npos[0] && tie[0]==0){
	   if(nota[1]==nnota[1] && ctie[1]==cnota[1] && pos[0]==npos[0] && ctie[0]==0){
		  melodia->penta[1].compas[compas].color[npos[1]]=melodia->penta[1].compas[compas].color[pos[1]];	  
		  melodia->penta[1].compas[compas].errormarca[index+1]=melodia->penta[1].compas[compas].errormarca[index];
		  ++index;
		  continue;
	   }
	   ++index;
	   break;
	}
    melodia->puneje.index=index;
	return;
}

void ajustatiempoenties(void)
{
	double duratotal,dura,timetotal;
	int r,c,v,numnotas,pos,npos,dif,t,s;
	TIMENOTA *ap,*apt;

	for(r=0;r<2;r++){
	   s=melodia->puneje.compas;	   
	   for(c=s-1;c<=s;c++){	
		  if(c<0) c=0;
	      numnotas=melodia->penta[r].compas[c].numnotas;    
		  for(v=0;v<numnotas;v++){			 
             ap=&melodia->penta[r].compas[c].timenota[v];
		 	 if(numnotas==1 && ap->time<0) return;
	         if(melodia->penta[r].compas[c].note[v]==0) continue;
	         if(ap->valid==0) break;
			 if(ap->valid==1) continue;
             pos=ap->pos;
			 npos=ap->npos;
			 // suponemos que no hay tie entre compases, de otro modo hay que modificar la linea que sigue			 
             dif=npos-pos;
			 if(dif<=0 && npos==0){ 
				 npos=numnotas; dif=npos-pos; 
			 }//prueba
			 if(dif==1){ // || dif<=0){ 
				 ap->valid=1;
				 continue; // no hay ties --> continue
			 }
			 timetotal=ap->time;
             //for(t=v,duratotal=0;t<npos;t++)
			 for(t=pos,duratotal=0;t<npos;t++)
			    duratotal+=melodia->penta[r].compas[c].dura[t];
             //for(t=v;t<npos;t++){
			 for(t=pos;t<npos;t++){
			    apt=&melodia->penta[r].compas[c].timenota[t];
				dura=melodia->penta[r].compas[c].dura[t];
				apt->pos=apt->pos;
				apt->npos=apt->pos+1;
				apt->penta=r;
				apt->ncompas=c;
                apt->time=timetotal*dura/duratotal;
				apt->valid=1;
				if(t+1<npos) 
				   melodia->penta[r].compas[c].timenota[t+1].timestamp=
					                          apt->timestamp+apt->time;
			 }		
			 v=t;
		  }
	   }
	}
	return;
}


	   
// para ejecutar esta rutina debemos tener en melodia cargada la variable puneje con una ejecucion
// solo tenemos medida del tiempo en analisis ritmico 
// y lo representamos en pantalla cuando se han pulsado correctamente

void IntervalTestUpdate(int nta,int source, BOOL init)
{
   static BUFFMIDI buff[256];
   static int topbuff;
   static int timefincompas;
   int index,compas,pos[2],numnotbuff,totalok;
   __int64 nota[2],notanat[2];
   int *temp;
   int chord[2];
   int numnotasindex[2];
   EJECUCION *eje; 
   int numfreable;

   BUFFMIDI *app;
   app=buff;
// topbuff estaba a -1 cuando terminan todos los compases
// esto provoca problemas ya que se pierden las primeras entradas de midi o teclado
//   if(topbuff<0){ 
//	   topbuff=0; 
//	   return; }

   temp=&topbuff; 
   if(init==-1){ 
      topbuff=timefincompas=0; 
	  return;
   }
   if(init==-2){ // elimina silencios iniciales
	  int index,bo;
	  index=melodia->puneje.index;
	  bo=(melodia->puneje.eje->penta[0].runnota[index].nota==0 &&
		 melodia->puneje.eje->penta[1].runnota[index].nota==0);
	  if(bo==0) return; 
	  else nta=0; // simulamos un silencio como si viniera del teclado
   }

   do{
      if(nta!=0) addnote(buff,source,&topbuff,nta);   
      numnotbuff=numnotasverticaleje(&melodia->puneje,numnotasindex);
	  numfreable=numfreables(buff,&topbuff);  
      //************************************************
      // Empezamos el compas con el buffer midi vacio, cuando lo esté medimos el tiempo 
      // que ha durado el compas anterior (para ligaduras de union la condion deberia de ser 
      // falsa
      if(timefincompas==1){               
	    if(topbuff!=0) 
		   return;          
	    melodia->penta[0].compas[melodia->puneje.compas-1].timecompas.time=clock()-
	    melodia->penta[0].compas[melodia->puneje.compas-1].timecompas.time;
        melodia->penta[0].compas[melodia->puneje.compas-1].timecompas.valid=1;
	    timefincompas=0;                
        if(concfg.repsierrritmo==1){
		   COLORREF color0,color1;
           color0=paintritmicexecution(melodia,0,melodia->puneje.compas-1,1);
		   color1=paintritmicexecution(melodia,1,melodia->puneje.compas-1,1);
		   if(color0==RED || color1==RED){
	          --melodia->puneje.compas;
		      melodia->puneje.index=0;
		   }
		}
	  }

	  {
	     int nproc,mindex,a[2],p[2],r;
       
	     nproc=numnotasonbufferprocessed(buff,topbuff);
         mindex=melodia->puneje.index;
         if(mindex>0){
		    for(r=0;r<2;r++){
	           a[r]=melodia->puneje.eje->penta[r].runnota[mindex].pos;
               p[r]=melodia->puneje.eje->penta[r].runnota[mindex-1].pos;
			}
		 }
		 //*************************************************************************
		 // 12-07-14 comprueba si hay algun tie entre el acorde actual y el anterior
         __int64 tp[2];
         if(mindex>0){
		    for(r=0;r<2;r++){
	           tp[r]=melodia->puneje.eje->penta[r].runnota[mindex-1].tie;
			}
		 }

		 //*************************************************************************

	     if(a[0]!=p[0] && tp[0]==0 && a[1]!=p[1] && tp[1]==0 && nproc>0 && topbuff!=0)
		    return;

	     /*if(a[0]!=p[0] && a[1]!=p[1] && nproc>0 && topbuff!=0)
		    return;*/
	  }
      if(topbuff!=numnotbuff || numfreable!=0 ){  
	     return;         
	  }
      eje=melodia->puneje.eje;
      index=melodia->puneje.index;
      compas=melodia->puneje.compas;	  

	  //bufftorunnota(buff,topbuff,eje,index); // asociamos cada nota en el buffer midi con la ejecucion      
      int loc[2],n,b,l0,l1,tt,silendo,silenup;
      int n0,n1;
	  __int64 n2,n3;
      tt=index;
	  silendo=silenup=1;
      while(1){
         for(n=0;n<2;n++){ 
            loc[n]=0; 
	        pos[n]=eje->penta[n].runnota[tt].pos;
            if(eje->penta[n].runnota[tt].nota==0){            // si es una nota de silencio la marcamos correcta
	           melodia->penta[n].compas[compas].color[pos[n]]=GREENBOLD;	  
	           melodia->penta[n].compas[compas].errormarca[tt]=-1;
		       loc[n]=1;
			}
		 }
	     if(tt==melodia->puneje.canelem-1){
		    index=melodia->puneje.index=tt; break;
		 }
         n0=eje->penta[0].runnota[tt].pos;
		 n1=eje->penta[0].runnota[tt+1].pos;
         n2=eje->penta[1].runnota[tt].notakey;
		 n3=eje->penta[1].runnota[tt+1].notakey;
		 if(n0==n1 && n2==n3 && n2==0 && silendo==1){ ++tt; silenup=0; continue; }
         n0=eje->penta[1].runnota[tt].pos;
		 n1=eje->penta[1].runnota[tt+1].pos;
         n2=eje->penta[0].runnota[tt].notakey;
		 n3=eje->penta[0].runnota[tt+1].notakey;
         if(n0==n1 && n2==n3 && n2==0 && silenup==1){ ++tt; silendo=0; continue; }
	     index=melodia->puneje.index=tt; break;
	  }
      bufftorunnota(buff,topbuff,eje,index); // asociamos cada nota en el buffer midi con la ejecucion      
      totalok=0;
	  chord[0]=chord[1]=0;
      for(b=0;b<numnotbuff;b++){  
		 //**18-07******************************
		 if(buff[b].timenota!=NULL)
		    if(buff[b].timenota->valid==1)
		       buff[b].timenota=NULL;
		  //**************************
         //buff[b].timenota=NULL;                                                      // para cada nota en el buffer
         for(n=0;n<2;n++){                                                           // para cada pentagrama
            if(loc[n]==1) continue;
            nota[n]=eje->penta[n].runnota[index].notakey;                         // nota a tocar en pentagrama n
            notanat[n]=eje->penta[n].runnota[index].nota;                         // nota a tocar en pentagrama n
			pos[n]=eje->penta[n].runnota[index].pos;                              // posicion en el petagrama n      
			if(micentry==1){
				if(concfg.tiposolfeo==0){  // solfeo cantado
		          if(concfg.singoctava==1){
					 int istie;
		             l0=buff[b].notekey-12*(buff[b].notekey/12);
		             l1=(int) ((nota[n]&0xff)-12*((nota[n]&0xff)/12));
			         if(l0==l1){
						++chord[n];
						if(chord[n]==numnotasindex[n]){
                           loc[n]=1;
		                   melodia->penta[n].compas[compas].color[pos[n]]=GREENBOLD;	  
	                       melodia->penta[n].compas[compas].errormarca[index]=-1;
						   if(buff[b].timenota==NULL){
	                          buff[b].timenota=&melodia->penta[n].compas[compas].timenota[pos[n]];
                              buff[b].timenota->timestamp=buff[b].timestamp;
                              buff[b].timenota->penta=n;						   
                              int v,c[64],idx;
						      __int64 nt,t;
						      for(v=idx=0;v<pos[n];v++){
						         nt=melodia->penta[n].compas[compas].notekey[v];
                                 t=melodia->penta[n].compas[compas].tie[v];
						         if(isinchord(buff[b].notekey,nt,t,istie)){
						            if(istie==1) c[idx++]=v;
							        else idx=0;
								 }else idx=0;
							  }
						      if(idx!=0) buff[b].timenota->pos=c[0];
						      else buff[b].timenota->pos=pos[n]; //posicion donde la nota fue pulsada por primera vez dentro de tie                             
					          //buff[b].timenota->pos=pos[n];    //posicion donde la nota fue pulsada por primera vez dentro de tie
						   }
			               totalok+=chord[n];
                           break;
						}
					 }
				  }else{
					 int istie;
			         if(buff[b].notekey==(nota[n]&0xff)){
						++chord[n];
						if(chord[n]==numnotasindex[n]){
                           loc[n]=1;	     
		                   melodia->penta[n].compas[compas].color[pos[n]]=GREENBOLD;	  
	                       melodia->penta[n].compas[compas].errormarca[index]=-1;
						   if(buff[b].timenota==NULL){
	                          buff[b].timenota=&melodia->penta[n].compas[compas].timenota[pos[n]];
						      buff[b].timenota->timestamp=buff[b].timestamp;
                              buff[b].timenota->penta=n;	
                              int v,c[64],idx;
						      __int64 nt,t;
						      for(v=idx=0;v<pos[n];v++){
						         nt=melodia->penta[n].compas[compas].notekey[v];
                                 t=melodia->penta[n].compas[compas].tie[v];
						         if(isinchord(buff[b].notekey,nt,t,istie)){
						            if(istie==1) c[idx++]=v;
							        else idx=0;
								 }else idx=0;
							  }
						      if(idx!=0) buff[b].timenota->pos=c[0];
						      else buff[b].timenota->pos=pos[n]; //posicion donde la nota fue pulsada por primera vez dentro de tie                             
					          //buff[b].timenota->pos=pos[n];    //posicion donde la nota fue pulsada por primera vez dentro de tie
						   }
			               totalok+=chord[n];
                           break;
						}
					 }  
				  } 
			   }else{   //solfeo hablado		          
				  int istie;
				  l0=buff[b].notekey-12*(buff[b].notekey/12);
		          l1=(int) ((notanat[n]&0xff)-12*((notanat[n]&0xff)/12));
			      if(l0==l1){
					 ++chord[n];
					 if(chord[n]==numnotasindex[n]){
                        loc[n]=1;
		                melodia->penta[n].compas[compas].color[pos[n]]=GREENBOLD;	  
	                    melodia->penta[n].compas[compas].errormarca[index]=-1;
                        if(buff[b].timenota==NULL){
	                       buff[b].timenota=&melodia->penta[n].compas[compas].timenota[pos[n]];
                           buff[b].timenota->timestamp=buff[b].timestamp;
                           buff[b].timenota->penta=n;		
                           int v,c[64],idx;
						   __int64 nt,t;
						   for(v=idx=0;v<pos[n];v++){
						      nt=melodia->penta[n].compas[compas].notekey[v];
                              t=melodia->penta[n].compas[compas].tie[v];
						      if(isinchord(buff[b].notekey,nt,t,istie)){
						         if(istie==1) c[idx++]=v;
							     else idx=0;
							  }else idx=0;
						   }
						   if(idx!=0) buff[b].timenota->pos=c[0];
						   else buff[b].timenota->pos=pos[n]; //posicion donde la nota fue pulsada por primera vez dentro de tie                             
					       //buff[b].timenota->pos=pos[n];    //posicion donde la nota fue pulsada por primera vez dentro de tie
						}
			            totalok+=chord[n]; 
                        break;
					 }
				  }
			   }
			}else{			 
			  int istie;
			  if(isinchord(buff[b].notekey,nota[n],0,istie)){
				  ++chord[n];                  
			      if(chord[n]==numnotasindex[n]){
                     loc[n]=1;	     
		             melodia->penta[n].compas[compas].color[pos[n]]=GREENBOLD;	  
	                 melodia->penta[n].compas[compas].errormarca[index]=-1;
                     if(buff[b].timenota==NULL){						
					    buff[b].timenota=&melodia->penta[n].compas[compas].timenota[pos[n]];
                        buff[b].timenota->timestamp=buff[b].timestamp;					 
                        buff[b].timenota->penta=n;	
						// programar bucle que mire donde esta el origen del tie
						// empezando por la primera nota v=0 hasta pos[n]-1 y una vez localizada
						// y comprobado que hay tie hasta pos[n] entonces asignar la posicion del origen del tie
						// a buff[b].timenota->pos
                        int v,c[64],idx;
						__int64 nt,t;
						for(v=idx=0;v<pos[n];v++){
						   nt=melodia->penta[n].compas[compas].notekey[v];
                           t=melodia->penta[n].compas[compas].tie[v];
						   if(isinchord(buff[b].notekey,nt,t,istie)){
						      if(istie==1) c[idx++]=v;
							  else idx=0;
						   }else idx=0;
						}
						if(idx!=0) buff[b].timenota->pos=c[0];
						else buff[b].timenota->pos=pos[n]; //posicion donde la nota fue pulsada por primera vez dentro de tie                             
					    //buff[b].timenota->pos=pos[n];    //posicion donde la nota fue pulsada por primera vez dentro de tie
					 }
			         totalok+=chord[n];
					 break;
				  }
			   }  
			}
		 }
	  }
      for(b=0;b<numnotbuff;b++){
         if(buff[b].timenota!=NULL) continue;
         for(n=0;n<2;n++){
			//buff[b].freeable=1; // revisar como asignar los freeables teniendo en cuenta la estructura ejecucion
            if(loc[n]==1) continue;
            melodia->penta[n].compas[compas].color[pos[n]]=RED;                      // suponemos que la nota en el buffer es incorrecta por defecto
            melodia->penta[n].compas[compas].errormarca[index]=buff[b].notekey;
			// anulamos las dos lineas siguientes y añadimos la tercera para 
			// evitar que calcule el timestamp cuando las notas pulsadas son erroneas
			// Ya que si se da el caso no sabra en que clave estan las notas.
			//buff[b].timenota=&melodia->penta[n].compas[compas].timenota[pos[n]];     // iniciamos temporizador
            //buff[b].timenota->timestamp=buff[b].timestamp;
			buff[b].timenota=NULL;
            loc[n]=1;
            break;
		 }
	  }
      if(melodia->puneje.index==0){
         melodia->penta[0].compas[melodia->puneje.compas].timecompas.time=clock();	  
	     melodia->penta[0].compas[melodia->puneje.compas].timecompas.valid=0;
	  }
	  if(concfg.stoponerror==1){
		 if(numnotbuff==totalok){
			avanceindex(melodia);
		    //++melodia->puneje.index;
		 }
	  }else{
		    avanceindex(melodia);
	  	  //++melodia->puneje.index;
      }
	  if(melodia->puneje.index==melodia->puneje.canelem){
	     timefincompas=1;
         ++melodia->puneje.compas;
	     if(melodia->puneje.compas==melodia->numcompases){
	        melodia->puneje.compas=0;
		    topbuff=0; //topbuff=-1; 
			timefincompas=0;
		    resetmelodia(melodia);
		    resetscores(melodia);
		    MessageBox(hwnd, "Fin de la partitura.\n", "PIANOTRAIN", MB_OK|MB_ICONWARNING);
			if(playmelodiastatus==1)
			   PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(0,BN_CLICKED),(LPARAM) buttonplay);
			if(followmestatus==1 || siguemestatus==1)
			   PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(1,BN_CLICKED),(LPARAM) buttongoto);
			InvalidateRect(hwnd, NULL, TRUE);
		 }
         melodia->puneje.index=0;
	     delete eje;
	     melodia->puneje.eje=generaejecucion(melodia->penta,melodia->puneje.compas);
	     melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
	  }
      if(numnotbuff==0) nta=0;  // simulamos una nota de silencio como si viniera de drawkey()
      IntervalTestUpdate1(melodia);   
   }while(numnotbuff==0);   
   return;
}

void liberathreads(void)
{
   if(htwoplay!=NULL){
      CloseHandle(htwoplay); 
      htwoplay=NULL;
   }
   if(hmetronomo!=NULL){ 
      CloseHandle(hmetronomo); 
      hmetronomo=NULL;
   }
   if(hfollowme!=NULL){ 
      CloseHandle(hfollowme); 
      hfollowme=NULL;
   }
   if(hplaymelodia!=NULL){
      CloseHandle(hplaymelodia);
      hplaymelodia=NULL;
   }
   return;
}

void endthreads(void)
{
   if(followmestatus==1){ 
      followmestatus=0;
	  followmestop=1;
	  while(followmestop==1 && hfollowme!=NULL);
   }
   if(siguemestatus==1){ 
      twoplaystop=1;
	  while(twoplaystop==1 && htwoplay!=NULL);
	  siguemestatus=0;
   }
   if(metronomostatus==1){
      metronomostatus=0;
	  metronomostop=1;
	  while(metronomostop==1 && hmetronomo!=NULL);
   }
   if(playmelodiastatus==1){
      playmelodiastatus=0;
	  playmelodiastopv=1;
	  while(playmelodiastopv==1 && hplaymelodia!=NULL);
   }				
   return;
}
void LiberaMelodia(MELODIA *&melodia)
{
	if(melodia==NULL) return;
	if(melodia->puneje.eje!=NULL){ 
		delete melodia->puneje.eje;
		melodia->puneje.eje=NULL;
	}
	if(melodia->penta[0].compas!=NULL){ 
	   delete [] melodia->penta[0].compas;
	   melodia->penta[0].compas=NULL;
	}
    if(melodia->penta[1].compas!=NULL){ 
	   delete [] melodia->penta[1].compas;
	   melodia->penta[1].compas=NULL;
	}
	delete [] melodia;
	melodia=NULL;
	return;
}

MELODIA *copiamelodia(MELODIA *melodia)
{
    COMPAS *ap;
    MELODIA *melod;
	int r,c,numc;

	melod=(MELODIA *) new MELODIA;
	numc=melodia->numcompases;
	*melod=*melodia;
	melod->penta[0].allsilencio=melodia->penta[0].allsilencio;
	melod->penta[1].allsilencio=melodia->penta[1].allsilencio;
	melod->penta[0].compas=(COMPAS *) new COMPAS[numc];
	melod->penta[1].compas=(COMPAS *) new COMPAS[numc];
    for(r=0;r<2;r++){
	   for(c=0;c<numc;c++){
		  ap=&melodia->penta[r].compas[c];
          melod->penta[r].compas[c]=*ap;
	   }
	} 
	return melod;
}

void addcompassilencio(MELODIA *melodia,int penta,int compas,int armadura,int num,int den,double dura)
{
	int j,c;
    
	j=penta;
	c=compas;
	melodia->penta[j].compas[c].note[0]=0;
	melodia->penta[j].compas[c].clef[0]=j;
	melodia->penta[j].compas[c].tupla[0]=1;
	melodia->penta[j].compas[c].tie[0]=0;
	melodia->penta[j].compas[c].notekey[0]=0;
	melodia->penta[j].compas[c].acci[0]=0;
	melodia->penta[j].compas[c].dura[0]=dura;
	melodia->penta[j].compas[c].timenota[0].time=-1;
	melodia->penta[j].compas[c].timenota[0].valid=0;
	melodia->penta[j].compas[c].errormarca[0]=-1;
    melodia->penta[j].compas[c].fingering[0]=-1;
	melodia->penta[j].compas[c].color[0]=0;

    melodia->penta[penta].compas[c].numsignature=num;
    melodia->penta[penta].compas[c].densignature=den;
	melodia->penta[j].compas[c].numsilencios=1;
	melodia->penta[j].compas[c].numnotas=1;
	melodia->penta[j].compas[c].armadura=armadura;
    melodia->penta[j].compas[c].seqcompas=compas;
    melodia->penta[j].compas[c].timecompas.time=-1;
    melodia->penta[j].compas[c].timecompas.valid=0;
	return;
}

void addnotasilencio(MELODIA *melodia,int j,int c,double dura)
{
	int numnotas;
    
	numnotas=melodia->penta[j].compas[c].numnotas;
	melodia->penta[j].compas[c].note[numnotas]=0;
	melodia->penta[j].compas[c].clef[numnotas]=j;
	melodia->penta[j].compas[c].tupla[numnotas]=1;
	melodia->penta[j].compas[c].tie[numnotas]=0;
	melodia->penta[j].compas[c].notekey[numnotas]=0;
	melodia->penta[j].compas[c].acci[numnotas]=0;
	melodia->penta[j].compas[c].dura[numnotas]=dura;
	melodia->penta[j].compas[c].timenota[numnotas].time=-1;
	melodia->penta[j].compas[c].timenota[numnotas].valid=0;
	melodia->penta[j].compas[c].errormarca[numnotas]=-1;
    melodia->penta[j].compas[c].fingering[numnotas]=-1;
	melodia->penta[j].compas[c].color[numnotas]=0;
	melodia->penta[j].compas[c].numsilencios+=1;
	melodia->penta[j].compas[c].numnotas+=1;
	return;
}

void addnotasilenciohead(MELODIA *melodia,int j,int c,double dura)
{
	int numnotas,r;
    
	numnotas=melodia->penta[j].compas[c].numnotas;
    for(r=numnotas;r>=1;r--){
	   melodia->penta[j].compas[c].note[r]=melodia->penta[j].compas[c].note[r-1];
	   melodia->penta[j].compas[c].clef[r]=melodia->penta[j].compas[c].clef[r-1];
	   melodia->penta[j].compas[c].tupla[r]=melodia->penta[j].compas[c].tupla[r-1];
	   melodia->penta[j].compas[c].tie[r]=melodia->penta[j].compas[c].tie[r-1];
	   melodia->penta[j].compas[c].notekey[r]=melodia->penta[j].compas[c].notekey[r-1];
  	   melodia->penta[j].compas[c].acci[r]=melodia->penta[j].compas[c].acci[r-1];
	   melodia->penta[j].compas[c].dura[r]=melodia->penta[j].compas[c].dura[r-1];
	   melodia->penta[j].compas[c].timenota[r].time=melodia->penta[j].compas[c].timenota[r-1].time;
	   melodia->penta[j].compas[c].timenota[r].valid=melodia->penta[j].compas[c].timenota[r-1].valid;
	   melodia->penta[j].compas[c].errormarca[r]=melodia->penta[j].compas[c].errormarca[r-1];
       melodia->penta[j].compas[c].fingering[r]=melodia->penta[j].compas[c].fingering[r-1];
	   melodia->penta[j].compas[c].color[r]=melodia->penta[j].compas[c].color[r-1];
	}
	melodia->penta[j].compas[c].note[0]=0;
	melodia->penta[j].compas[c].clef[0]=j;
	melodia->penta[j].compas[c].tupla[0]=1;
	melodia->penta[j].compas[c].tie[0]=0;
	melodia->penta[j].compas[c].notekey[0]=0;
	melodia->penta[j].compas[c].acci[0]=0;
	melodia->penta[j].compas[c].dura[0]=dura;
	melodia->penta[j].compas[c].timenota[0].time=-1;
	melodia->penta[j].compas[c].timenota[0].valid=0;
	melodia->penta[j].compas[c].errormarca[0]=-1;
    melodia->penta[j].compas[c].fingering[0]=-1;
	melodia->penta[j].compas[c].color[0]=0;
	melodia->penta[j].compas[c].numsilencios+=1;
	melodia->penta[j].compas[c].numnotas+=1;
	return;
}

void allsilencio(MELODIA *melodia,int penta)
{
	int c,numnotas,v,numsignature,densignature;

	for(c=0;c<melodia->numcompases;c++){
	   numnotas=1; //todo el compas sera silencio
       numsignature=melodia->penta[0].compas[c].numsignature;
       densignature=melodia->penta[0].compas[c].densignature;
	   for(v=0;v<numnotas;v++){
		  melodia->penta[penta].compas[c].note[v]=0;
		  melodia->penta[penta].compas[c].clef[v]=penta;
		  melodia->penta[penta].compas[c].tupla[v]=1;
          melodia->penta[penta].compas[c].tie[v]=0;
		  melodia->penta[penta].compas[c].notekey[v]=0;
		  melodia->penta[penta].compas[c].acci[v]=0;
	      melodia->penta[penta].compas[c].dura[v]=numsignature*1.0/densignature;
		  melodia->penta[penta].compas[c].timenota[v].time=-1;
		  melodia->penta[penta].compas[c].timenota[v].valid=0;
		  melodia->penta[penta].compas[c].errormarca[v]=-1;
          melodia->penta[penta].compas[c].fingering[v]=-1;
		  melodia->penta[penta].compas[c].color[v]=0;		          
	   }
       melodia->penta[penta].compas[c].numsignature=numsignature;
       melodia->penta[penta].compas[c].densignature=densignature;
       melodia->penta[penta].compas[c].numnotas=numnotas;
       melodia->penta[penta].compas[c].numsilencios=numnotas;
       melodia->penta[penta].compas[c].seqcompas=c;
       melodia->penta[penta].compas[c].timecompas.time=-1;
       melodia->penta[penta].compas[c].timecompas.valid=0;

	   melodia->penta[penta].compas[c].std.durcompas=0;
       melodia->penta[penta].compas[c].std.durcompasqos=0;
       melodia->penta[penta].compas[c].std.notasfast=0;
       melodia->penta[penta].compas[c].std.notasjust=0;
       melodia->penta[penta].compas[c].std.notasslow=0;
       melodia->penta[penta].compas[c].std.notasfastqos=0;
       melodia->penta[penta].compas[c].std.notasjustqos=0;
       melodia->penta[penta].compas[c].std.notasslowqos=0;
	   melodia->penta[penta].compas[c].std.durnotastotal=0;
       melodia->penta[penta].compas[c].std.ritmic=0;
	}
	return;
}

void aplicarepeticiones(MELODIA *&melodia)
{
	int r,c,s,numc,count;
    int rep[2];
	COMPAS *ap;
    MELODIA *melod;

	rep[0]=melodia->repeatg; ++rep[0];
	rep[1]=melodia->repeatf; ++rep[1];
	numc=melodia->numcompases;
	melod=(MELODIA *) new MELODIA;
	*melod=*melodia;
	melod->penta[0].allsilencio=melodia->penta[0].allsilencio;
    melod->penta[1].allsilencio=melodia->penta[1].allsilencio;
	melod->penta[0].compas=(COMPAS *) new COMPAS[numc];
	melod->penta[1].compas=(COMPAS *) new COMPAS[numc];
	melod->puneje.eje=NULL;
	count=0;
    for(r=0;r<2;r++){
	   for(c=count=0;c<numc;c++){
		  ap=&melodia->penta[r].compas[c];
		  for(s=0;s<rep[r];s++){
			 if(count>=numc) break;
             melod->penta[r].compas[count]=*ap;
			 melod->penta[r].compas[count].seqcompas=count; ++count;
		  }
	   }
	} 
    LiberaMelodia(melodia);
	melodia=melod;
	return;
}

void loadbitmaps(void)
{
   bmpsand = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CLOCKSAND));
   bmpcheck = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CHECK));
   bmpkey=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_KEY));
   bmpwoodu=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_WOODU));
   bmpwoodd=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_WOODD));
   bmpmicro=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MICRO));
   bmpmicroon=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MICROON));
   bmpritmon=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RITMON));
   bmpritmoy=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RITMOY));
   bmpmetron=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_METRON));
   bmpmetroy=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_METROY));
   //bmpokan=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OKAN));
   //bmpokay=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OKAY));
   bmpcfg=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CFG));
   bmpcfgy=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CFGY));
   bmpgame1=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAME1));
   bmpgame2=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAME2));
   bmpgame3=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAME3));
   bmpgame4=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAME4));
   bmpgame5=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAME5));
   bmpgame6=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAME6));
   bmpsplash=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SPLASH));
   bmpearn=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EARN));
   bmpeary=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EARY));
   bmprepe=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_REPE));   
   bmpplayn=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYN));
   bmpplayy=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYY));
   bmpgoto=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GOTO));
   if(currenthor==1280){
      bmphandl=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HANDL));
      bmphandr=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HANDR));
      bmpokan=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OKANH));
      bmpokay=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OKAYH));
   }else{
      bmphandl=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HANDLL));
      bmphandr=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HANDRL));
      bmpokan=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OKAN));
      bmpokay=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OKAY));
   }
}

void liberabitmaps(void)
{
	DeleteObject(bmpcheck);
	DeleteObject(bmpsand);
	DeleteObject(bmpkey);	
	DeleteObject(bmpwoodu);
	DeleteObject(bmpwoodd);
	DeleteObject(bmpmicro);
    DeleteObject(bmpmicroon);
	DeleteObject(bmpritmon);
    DeleteObject(bmpritmoy);
	DeleteObject(bmpmetron);
    DeleteObject(bmpmetroy);
	DeleteObject(bmpokan);
    DeleteObject(bmpokay);
	DeleteObject(bmpcfg);
	DeleteObject(bmpcfgy);
    DeleteObject(bmpgame1);
    DeleteObject(bmpgame2);
    DeleteObject(bmpgame3);
    DeleteObject(bmpgame4);
	DeleteObject(bmpgame5);
	DeleteObject(bmpgame6);
	DeleteObject(bmpsplash);
	DeleteObject(bmpearn);
	DeleteObject(bmpeary);
	DeleteObject(bmprepe);
    DeleteObject(bmphandl);
    DeleteObject(bmphandr);
    DeleteObject(bmpplayn);
	DeleteObject(bmpplayy);
    DeleteObject(bmpgoto);
	return;
}


void ajustafuentesrandommusic(int togreat)
{
   if(currenthor==1024){
	   if(lf.lfHeight!=-29 && togreat==1){
		  showvirstaff=TRUE;
	      DeleteObject(hFont);
          DeleteObject(hMaestroFontSmall1);
		  DeleteObject(hMaestroFontSmall2);
		  DeleteObject(hMaestroFontSmall3);

          lf.lfHeight =-29;
		  hFont = CreateFontIndirect(&lf);

          lf4.lfHeight =-40;
		  hMaestroFontSmall1 = CreateFontIndirect(&lf4);
          interline=-lf4.lfHeight/4;

          lf5.lfHeight = -24;
          hMaestroFontSmall2 = CreateFontIndirect(&lf5);

          lf7.lfHeight = -40;
		  hMaestroFontSmall3 = CreateFontIndirect(&lf7);
		  noteloc = top-98+100; 
          calccoordfgmystaff(noteloc);
		  return;
	   }
	   if(lf.lfHeight==-29 && togreat==0){
		  showvirstaff=FALSE;
		  DeleteObject(hFont);
          DeleteObject(hMaestroFontSmall1);
		  DeleteObject(hMaestroFontSmall2);
		  DeleteObject(hMaestroFontSmall3);

          lf.lfHeight =-15;
		  hFont = CreateFontIndirect(&lf);

          lf4.lfHeight =-24;
		  hMaestroFontSmall1 = CreateFontIndirect(&lf4);
          interline=-lf4.lfHeight/4;

          lf5.lfHeight = -16;
          hMaestroFontSmall2 = CreateFontIndirect(&lf5);

          lf7.lfHeight = -24;
		  hMaestroFontSmall3 = CreateFontIndirect(&lf7);
		  noteloc = top-98; 
          calccoordfgmystaff(noteloc);
		  return;
	   }
   }
   return;
}	      

/*
// esta rutina de retardo introduce errores en la temporizacion
static void usleep(double waitTime) 
{
    MSG msg;

    if(waitTime == 0)
    	return;

    Sleep(waitTime);
	if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)!=0){
       GetMessage(&msg,NULL,0,0);
	   TranslateMessage(&msg);
	   DispatchMessage(&msg);
	}
	return;
}
*/

void paintbeam(int posx0,int posy0,int posx1,int posy1,HBRUSH hbr)
{
    HDC hdc;
	HPEN pen;

	hdc=GetDC(hwnd);
	SetBkMode(hdc,OPAQUE);
	pen=CreatePen(PS_SOLID,1,0);
	SelectObject(hdc,pen);
    //hOldBrush=SelectObject(hdc,hBlackBrush);
	hOldBrush=SelectObject(hdc,hbr);
	Ellipse(hdc,posx0,posy0,posx1,posy1);
	SelectObject(hdc,hOldBrush);
	DeleteObject(pen);
	ReleaseDC(hwnd,hdc);
    return;
}

void paintremovebeam(int posx0,int posy0)
{
    HDC hdc;
    RECT pp;

	hdc=GetDC(hwnd);
	pp.left=posx0;
	pp.top=posy0;
	pp.right=posx0+4*15;
	pp.bottom=posy0+10;
	FillRect(hdc,&pp,hWhiteBrush);
	ReleaseDC(hwnd,hdc);
    return;
}
   

DWORD WINAPI paintmetronomo(void)  
{
   int metro,num,den,r,orgs,posx0,posy0,index,compas,p,*beat,j;
   double durredonda,factor;

   
   orgs=gStaff[64-36]-250;
   while(1){
	  if(metronomostop==1) break;
      metro=melodia->metronomo;
      if(metro==0) continue;   
      posx0=610;
      posy0=8; 
	  compas=melodia->puneje.compas;
      num=melodia->penta[0].compas[compas].numsignature;
      den=melodia->penta[0].compas[compas].densignature;
	  durredonda=(60.0*CLOCKS_PER_SEC)*den/metro; //duracion en ticks de la redonda	  
	  factor=1.0/den; 
	  factor/=metrodivisor;
	  metronomobeat=-1;
	  for(r=0;r<num;r++){	
		 //metronomobeat=r;
         if(metronomostop==1) break;		 
         index=melodia->puneje.index;
	     beat=melodia->puneje.eje->penta[0].runnota[index].beat;			   
         for(j=0;;j++){
		    if(beat[j]==-1) break;
			if(beat[j]==r){
	           paintbeam(posx0,posy0,posx0+10,posy0+10,hGreenBrush);
			   break;
			}
		 }
		 if(beat[j]==-1)
		    paintbeam(posx0,posy0,posx0+10,posy0+10,hBlackBrush);
		 posx0+=15;
	     for(p=0;p<metrodivisor;p++){
		    PlaySound((LPCTSTR)IDR_WAVE1,NULL,SND_RESOURCE|SND_ASYNC);
			metronomobeat=r;
	        Sleep((DWORD) (factor*durredonda));		
		 }			
	  }
	  if(metronomostop==1) break;	
      posx0=610;
      posy0=8;
	  paintremovebeam(posx0,posy0);
   }
   metronomostop=0;
   InvalidateRect(hwnd, &rect, FALSE);
   UpdateWindow(hwnd);
   return 1;
}

void runfollowme(void)
{   
   followmestatus=1;
   followmestop=0;
   if(hfollowme!=NULL) return;
   hfollowme=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) paintfollowme,NULL, 0, NULL);  
   return;
}

void endfollowme(void)
{
   followmestatus=0;
   followmestop=1;
   return;
}

void runsigueme(void)
{
   siguemestatus=1;
   twoplaystop=0;
   if(htwoplay!=NULL) return;
   htwoplay=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) twoplaymelodia,NULL, 0, NULL);  
   return;
}

void endsigueme(void)
{  
   if(htwoplay==NULL) return;
   twoplaystop=1;
   while(twoplaystop==1);
   if(htwoplay!=NULL){ CloseHandle(htwoplay); htwoplay=NULL;}    			       
}

void paintcursorfollowme(MELODIA *melodia,int penta,int index,int indexs,int compas)
{
   int p,px,cmp,r;
   char buff[2];
   HDC hdc;

   r=penta;
   hdc=GetDC(hwnd);
   cmp=compas;
   if(index>=0){
      if(melodia->puneje.eje->penta[r].runnota[index].dif==0) return;
   }else ++cmp;
   p=melodia->puneje.eje->penta[r].runnota[indexs].pos;
   px=melodia->penta[r].compas[cmp].posx[p];
   SelectObject(hdc, hMaestroFontSmall1);
   SetBkMode(hdc, TRANSPARENT);
   SetTextColor(hdc,GREEN);
   buff[0]=224; buff[1]=0;       
   if(r==0) TextOut(hdc,px+5,posyminpenta-4*interline,buff,1);
   buff[0]=209; buff[1]=0;
   if(r==1) TextOut(hdc,px+5,posymaxpenta+2.5*interline,buff,1);			 
   ReleaseDC(hwnd,hdc);
   return;
}

DWORD WINAPI paintfollowme(void)
{
   int index,compas,r,pos[2],posx[2],posy[2],orgs,orgf,metro,valid,num;
   int indexs,posa[2];
   COLORREF col;
   double per,durredonda,den,tt;   
   COMPAS*ap;

   orgs=gStaff[64-36]-250;
   orgf=fStaff[43-36]-250;				     
   posy[0]=orgs-4*interline;
   posy[1]=orgf+8.5*interline;
   posa[0]=melodia->puneje.eje->penta[0].runnota[0].pos;
   posa[1]=melodia->puneje.eje->penta[1].runnota[0].pos;
   while(1){
	  if(playmelodiastopv==1) continue;
	  if(followmestop==1) break;
	  per=concfg.accuracynotes;
      metro=melodia->metronomo;
      if(metro==0) continue;   
	  if(followmestop==1) break;
	  // mientras esta cambiando de compas index tomara un valor mayor que canelem
	  // con el while esperamos a que intervaltestupdate actualice index
	  while(melodia->puneje.index==melodia->puneje.canelem);
      index=indexs=melodia->puneje.index;
	  if(index!=0){
	     posa[0]=melodia->puneje.eje->penta[0].runnota[index].pos;
	     posa[1]=melodia->puneje.eje->penta[1].runnota[index].pos;
	  }
	  --index;
      compas=melodia->puneje.compas;
      if(index<0) --compas; 
      if(compas<0) continue;
	  if(followmestop==1) break;	  
	  if(index<0 && compas>=0){
	     pos[0]=posa[0]; //eje->penta[0].runnota[index].pos;
		 pos[1]=posa[1]; //eje->penta[1].runnota[index].pos;
	  }else{
		 //indexa=melodia->puneje.eje->penta[0].hor-1;
         pos[0]=melodia->puneje.eje->penta[0].runnota[index].pos;
         pos[1]=melodia->puneje.eje->penta[1].runnota[index].pos;
	  }
      durredonda=(60.0*CLOCKS_PER_SEC)*melodia->penta[0].compas[compas].densignature/metro; //duracion en ticks de la redonda      
      for(r=0;r<2;r++){
		 if(followmestop==1) break;
         ap=&melodia->penta[r].compas[compas];	  
	     if(ap->note[pos[r]]==0) continue;
	     valid=ap->timenota[pos[r]].valid;
         if(valid==1) continue;
		 if(ap->color[pos[r]]!=GREENBOLD) continue;
		 posx[r]=ap->posx[pos[r]]+5;
		 den=durredonda*ap->dura[pos[r]]; // duracion de la nota en el pentagrama en ticks teorica     
	     num=clock()-ap->timenota[pos[r]].timestamp;
         tt=(num/den-1.0)*100;
         if(tt>100) tt=100;
		 col=paintclocksand1(posx[r],posy[r],per,tt);			 		 
		 if(col==GREEN)
            paintcursorfollowme(melodia,r,index,indexs,compas);
/*		 if(col==GREEN){
			int p,px,cmp;
			char buff[2];
			HDC hdc;

            hdc=GetDC(hwnd);
			cmp=compas;
			if(index>=0){
		       if(melodia->puneje.eje->penta[r].runnota[index].dif==0) continue;
			}else ++cmp;
            p=melodia->puneje.eje->penta[r].runnota[indexs].pos;
            px=melodia->penta[r].compas[cmp].posx[p];
	        SelectObject(hdc, hMaestroFontSmall1);
	        SetBkMode(hdc, TRANSPARENT);
	        SetTextColor(hdc,GREEN);
	        buff[0]=224; buff[1]=0;       
	        TextOut(hdc,px+5,posyminpenta-4*interline,buff,1);
            buff[0]=209; buff[1]=0;
	        TextOut(hdc,px+5,posymaxpenta+2.5*interline,buff,1);			 
	        ReleaseDC(hwnd,hdc);
		 }
*/
	  }
   }
   followmestop=0;
   hfollowme=NULL;
   return 1;
}  
//paintbomba(posx[r],posy[r],posx[r]+10,posy[r]+10,col);

void paintrandommusiccursor1(MELODIA *melodia, COLORREF color,HDC exhdc,int init)
{
    int index,compas,r;
	int pos[2],orgs,orgf;
	char buff[2];
	static int posx[2]={-10000,-10000};
	static COLORREF col;
	HDC hdc;

    if(init==1){ posx[0]=posx[1]=-10000; return; }
	orgs=gStaff[64-36]-250;
	orgf=fStaff[43-36]-250;	
	if(exhdc==NULL){ 
	   hdc=GetDC(hwnd);
	   index=melodia->puneje.index;
	   compas=melodia->puneje.compas;
	   pos[0]=melodia->puneje.eje->penta[0].runnota[index].pos;
	   pos[1]=melodia->puneje.eje->penta[1].runnota[index].pos;
	   for(r=0;r<2;r++)
          posx[r]=melodia->penta[r].compas[compas].posx[pos[r]];
	   col=color;
	}else hdc=exhdc;
	SelectObject(hdc, hMaestroFontSmall1);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc,col);
	buff[0]=224; buff[1]=0;
 // if(posx[0]>=0) TextOut(hdc,posx[0]+5,orgs-8*interline-interline,buff,1);
	if(posx[0]>=0) 
	   TextOut(hdc,posx[0]+5,posyminpenta-4*interline-interline,buff,1);
    buff[0]=209; buff[1]=0;
 // if(posx[0]>=0) TextOut(hdc,posx[1]+5,orgf+2.5*interline+interline,buff,1);
    if(posx[0]>=0) 
	   TextOut(hdc,posx[1]+5,posymaxpenta+2.5*interline+interline,buff,1);
	if(exhdc==NULL) ReleaseDC(hwnd,hdc);
    return;
}

void soloritmo(void)
{
	int r,c,v,numnotas;

    for(r=0;r<2;r++){
	   for(c=0;c<melodia->numcompases;c++){
	      numnotas=melodia->penta[r].compas[c].numnotas;
		  melodia->penta[r].compas[c].armadura=0;
	      for(v=0;v<numnotas;v++){
			 if(melodia->penta[r].compas[c].note[v]==0) continue;
             melodia->penta[r].compas[c].acci[v]=0;
			 if(r==0){
		        melodia->penta[r].compas[c].note[v]=71;
				melodia->penta[r].compas[c].notekey[v]=71;
			 }else{ 
			    melodia->penta[r].compas[c].note[v]=50;
				melodia->penta[r].compas[c].notekey[v]=50;
			 }

		  }
	   }
	}
	melodia->armadura=0;
	return;
}

DWORD WINAPI twoplaymelodia(void)
{
   int c,i,canelem,metro,posc,posm,finscr,toca0,toca1;
   int maxcompasscreen,compas,index;
   double mindur,con[2],redon;
   clock_t init,fin;
   COLORREF color;
   RUNPENTA *rp[2];
   MELODIA *cmelodia;   
   EJECUCION *eje;

   maxcompasscreen=concfg.maxcompascreen;
   c=offsetdispinterval;
   cmelodia=NULL;
   while(1){

      compas=melodia->puneje.compas;
      if(compas<c){
		  if(twoplaystop==1) break;
		  continue;
	  }	  
      //**************************************
      // para sincronizar el thread con el principal usamos un sendmessage
      SendMessage(hwnd, WM_USER, (WPARAM) 0x0000, (LPARAM) 0x0000);
	  cmelodia=copiamelodia(melodia);
      metro=cmelodia->metronomo;
      cmelodia->oth=0;
      cmelodia->puneje.eje=NULL;
      cmelodia->metronomoreal=-1;   
      cmelodia->puneje.index=0;
      cmelodia->puneje.compas=0;
	  finscr=offsetdispinterval+maxcompasscreen;
      for(c=compas;c<offsetdispinterval+maxcompasscreen;c++){	
		 // añadimos el while y comentamos el e mas abajo
		 // para conseguir que se pare el modo automatico al inicio del siguiente 
		 // compas
		 while(1){
		    if(twoplaystop==1) break;
		    compas=melodia->puneje.compas;
			if(c>compas) continue;  // c es el siguiente compas que tocara la maquina
			break;                  // compas el siguiente compas que tocara el usuario
		 }
		 if(twoplaystop==1) break;
	     if(c==cmelodia->numcompases) break;
		 redon=60.0*melodia->penta[0].compas[c].densignature*1000/metro;    //duracion redonda en milisegundos 
		 compas=melodia->puneje.compas;
		 if(compas>=finscr){
			 c=compas;
			 break;
		 }
         //**************************************
         // para sincronizar el thread con el principal usamos un sendmessage
	     SendMessage(hwnd, WM_USER, (WPARAM) 0x0000, (LPARAM) 0x0000);
		 cmelodia->puneje.eje=eje=generaejecucion1(cmelodia->penta,c);
         rp[0]=&eje->penta[0];
         rp[1]=&eje->penta[1];
         cmelodia->puneje.canelem=canelem=rp[0]->hor;
	     cmelodia->puneje.compas=c;
	     con[0]=con[1]=0;
	     cmelodia->puneje.index=0;
	     cmelodia->puneje.compas=c;
	     index=melodia->puneje.index;  
	     posc=c*100;
	     posm=compas*100+index;
         if(posc==posm) color=GREEN;
	     if(posm<posc) color=BLUEBOLD;
	     if(posm>posc) color=RED;
         paintrandommusiccursor1(cmelodia,color,NULL,0);
         for(i=0;i<canelem;i++){
            compas=melodia->puneje.compas;
		    if(compas>=finscr){
			   c=compas;
			   if(i-1>=0){
			      PlayNotaEndMidi(rp[0]->runnota[i-1].notakey);
	              PlayNotaEndMidi(rp[1]->runnota[i-1].notakey);
			   }
			   break;
			}
		    if(i!=0){
			   int posc,posa;
		       posc=eje->penta[0].runnota[i].pos;         
               posa=eje->penta[0].runnota[i-1].pos;         
               if(posc!=posa) con[0]=0;
		       posc=eje->penta[1].runnota[i].pos;         
               posa=eje->penta[1].runnota[i-1].pos;         
			   if(posc!=posa) con[1]=0;
			}
			init=clock();
		    if(round(con[0],6)==0){
			   if(i!=0) PlayNotaEndMidi(rp[0]->runnota[i-1].notakey);
		       con[0]=rp[0]->runnota[i].dura;
			   toca0=1;
			}
            // si con[0] es menor que una semifusa consideraremos que la nota ya se ha extinguido		 
		    if(round(con[1],6)==0){
			   if(i!=0) PlayNotaEndMidi(rp[1]->runnota[i-1].notakey);
		       con[1]=rp[1]->runnota[i].dura;
			   toca1=1;
			}
            if(toca0==1)
	           PlayNotaMidi(rp[0]->runnota[i].notakey);
            if(toca1==1)
               PlayNotaMidi(rp[1]->runnota[i].notakey);									
		    fin=clock();
		    fin=fin-init;
			mindur=min(con[0],con[1]);
			if(redon*mindur-fin>0)
		       Sleep((DWORD) (redon*mindur-fin));
            con[0]-=mindur;
		    con[1]-=mindur;
		    if(twoplaystop==1){ 
	           PlayNotaEndMidi(rp[0]->runnota[i].notakey);
	           PlayNotaEndMidi(rp[1]->runnota[i].notakey);
			   break;
			}
		    cmelodia->puneje.index=i;
		    cmelodia->puneje.compas=c;
            paintrandommusiccursor1(cmelodia,BACKGROUND,NULL,0);			   
			if(i+1==canelem) break;			
            cmelodia->puneje.index=i+1;
		    cmelodia->puneje.compas=c;
			compas=melodia->puneje.compas;
		    index=melodia->puneje.index;
		    posc=c*100+i+1;
		    posm=compas*100+index;
            if(posc==posm) color=GREEN;
	        if(posm<posc) color=BLUEBOLD;
		    if(posm>posc) color=RED;
		    paintrandommusiccursor1(cmelodia,color,NULL,0); //BLACK);             
		 }
	     PlayNotaEndMidi(rp[0]->runnota[i].notakey);
	     PlayNotaEndMidi(rp[1]->runnota[i].notakey);
	     delete [] cmelodia->puneje.eje;
	     cmelodia->puneje.eje=NULL;
	     if(twoplaystop==1) break;
	  }
	  if(twoplaystop==1) break;
	  LiberaMelodia(cmelodia);
   }  
   paintrandommusiccursor1(NULL,0,NULL,1);
   twoplaystop=0;
   LiberaMelodia(cmelodia);
//   InvalidateRect(hwnd, &rect, FALSE);
//   UpdateWindow(hwnd);
   htwoplay=NULL;
   return 1;
}

DWORD WINAPI playmelodia(void)
{
   int c,i,canelem,metro,compas,interval,toca0,toca1;
   __int64 tie[2]; // 15-07
   double mindur,con[2],redon;
   EJECUCION *eje;
   RUNPENTA *rp[2];
   static MELODIA *bufmelodia;
   clock_t init,fin;

   compas=melodia->puneje.compas;
   if(concfg.playmode==0) interval=compas+1;
   else interval=melodia->numcompases;
   playmelodiastatus=1;
   bufmelodia=melodia;
   melodia=copiamelodia(melodia);
   melodia->oth=0;
   if(melodia->puneje.eje!=NULL)
      delete [] melodia->puneje.eje;
   melodia->puneje.eje=NULL;   // eliminamos ejecucion actual
   IntervalTestUpdate(0,0,-1); // buffers midi a cero   
   melodia->metronomoreal=-1;
   metro=melodia->metronomo;
   melodia->puneje.index=0;
   melodia->puneje.compas=compas;
   for(c=compas;c<interval;c++){
	  if(c==melodia->numcompases) break;
	  redon=60.0*melodia->penta[0].compas[c].densignature*1000/metro;    //duracion redonda en milisegundos 
	  eje=generaejecucion(melodia->penta,c);
      rp[0]=&eje->penta[0];
      rp[1]=&eje->penta[1];
	  melodia->puneje.eje=generaejecucion(melodia->penta,c); // este eje es para intervaltestupdate ya que lo necesita internamente
	  IntervalTestUpdate(0,0,-2); // elimina silencios iniciales
      melodia->puneje.canelem=canelem=rp[0]->hor;
	  i=melodia->puneje.index;
//	  melodia->puneje.index=0;
	  melodia->puneje.compas=c;
	  con[0]=con[1]=0;
	  if(metronomostatus==1 && c==compas){
	     while(metronomobeat!=0);  // sincronizamos solo una vez para evitar pausas en la ejecucion de la melodia empezando en el compas desde el que pulsamos play
	  }
      for(;i<canelem;i++){
		 toca0=toca1=0;
		 if(i!=0){
			int posc,posa;
		    posc=eje->penta[0].runnota[i].pos;         
            posa=eje->penta[0].runnota[i-1].pos;         
            if(posc!=posa) con[0]=0;
		    posc=eje->penta[1].runnota[i].pos;         
            posa=eje->penta[1].runnota[i-1].pos;         
			if(posc!=posa) con[1]=0;
		 }
		 if(i==0) tie[0]=tie[1]=0; //15-07
		 else{ tie[0]=rp[0]->runnota[i-1].tie; tie[1]=rp[1]->runnota[i-1].tie; } //15-07
		 // si con[0] es menor que una semifusa consideraremos que la nota ya se ha extinguido
		 init=clock();
		 if(round(con[0],6)==0){
			if(i!=0) PlayNotaEndTeclado(rp[0]->runnota[i-1].notakey,tie[0]);   //15-07
		    con[0]=rp[0]->runnota[i].dura;
			toca0=1;
		 }
         // si con[0] es menor que una semifusa consideraremos que la nota ya se ha extinguido		 
		 if(round(con[1],6)==0){
			if(i!=0) PlayNotaEndTeclado(rp[1]->runnota[i-1].notakey,tie[1]);   //15-07
		    con[1]=rp[1]->runnota[i].dura;
			toca1=1;
		 }
         if(toca0==1){
	        if(i!=0) PlayNotaTeclado(rp[0]->runnota[i].notakey,rp[0]->runnota[i-1].notakey,tie[0]);                 //15-07
			else PlayNotaTeclado(rp[0]->runnota[i].notakey,0,0);
		 }
         if(toca1==1){
            if(i!=0) PlayNotaTeclado(rp[1]->runnota[i].notakey,rp[1]->runnota[i-1].notakey,tie[1]);                 //15-07
			else PlayNotaTeclado(rp[1]->runnota[i].notakey,0,0);                 //15-07
		 }
		 fin=clock();
		 fin=fin-init;
		 mindur=min(con[0],con[1]);
         if(redon*mindur-fin>0)
		    Sleep((DWORD) (redon*mindur-fin)); // eliminamos el retardo en printar y tocar la nota
         con[0]-=mindur;
		 con[1]-=mindur;
		 if(playmelodiastopv==1){ 
            tie[0]=rp[0]->runnota[i].tie; //15-07
			tie[1]=rp[1]->runnota[i].tie; //15-07
	        PlayNotaEndTeclado(rp[0]->runnota[i].notakey,tie[0]);
	        PlayNotaEndTeclado(rp[1]->runnota[i].notakey,tie[1]);
			break;
		 }	
	  }
	  if(i>0){        
		 tie[0]=rp[0]->runnota[i-1].tie; //15-07
		 tie[1]=rp[1]->runnota[i-1].tie; //15-07
	     PlayNotaEndTeclado(rp[0]->runnota[i-1].notakey,tie[0]);
	     PlayNotaEndTeclado(rp[1]->runnota[i-1].notakey,tie[1]);
	  }
	  delete [] eje;
	  delete [] melodia->puneje.eje;
	  melodia->puneje.eje=NULL;
	  if(playmelodiastopv==1) break;
   }	  
   LiberaMelodia(melodia);
   melodia=bufmelodia;
   melodia->puneje.eje=generaejecucion(melodia->penta,compas);
   IntervalTestUpdate(0,0,-1);
   IntervalTestUpdate(0,0,-2);
   offsetdispinterval=concfg.maxcompascreen*(melodia->puneje.compas/concfg.maxcompascreen);
   playmelodiastopv=0;
   playmelodiastatus=0;
   if(concfg.playmode==0){
      SendMessage(buttonplay, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpplayn);
	  InvalidateRect(hwnd, &rect, FALSE);
	  UpdateWindow(hwnd);
   }
//   PostMessage(hwnd,WM_APP+3,(WPARAM) 0, (LPARAM) 0);    
//   InvalidateRect(hwnd, &rect, FALSE);
//	   UpdateWindow(hwnd);
   return 1;
}

void eliminatiesctoc(MELODIA *melodia)
{
   int c,r,numnotas;
   for(r=0;r<2;r++){
      for(c=0;c<melodia->numcompases;c++){
         numnotas=melodia->penta[r].compas[c].numnotas;
         melodia->penta[r].compas[c].tie[numnotas-1]=0;
	  }
   }
   return;
}

int InitRandomMusicTest(void)
{
	int err,costo;

	melodia=(MELODIA *) new MELODIA;
	melodia->penta[0].compas=melodia->penta[1].compas=NULL;
	offsetdispinterval=0;
	strcpy(titlebar,"MUSICA ALEATORIA ATONAL");
	cargarconfiguracion(melodia);
	melodia->oth=0;
	if((err=generarmelodia(melodia))<0){  // genera melodia y repeticiones
	   if(err==-1)
          MessageBox(hwnd, "No puedo generar musica con estos bloques ritmicos. \nVe a opciones y añade nuevos y\\o cambia el tipo de compas. ",ERROR, MB_OK);
	   if(err==-2)
          MessageBox(hwnd, "No existe fichero intermedio g.abc \nElige en opciones->Tipo de generacion->midi y carga un fichero midi. ", ERROR, MB_OK);
       return -1;
	}
	if(melodia->fingering==1){
	   //pianoinfo pianoinf("Generando Finguering Clave De SOL \n");
	   //pianoinf.Show(TRUE);
	   //genetic(melodia,0);
	   costo=viterbi(melodia,0);
	   //strcpy(pianoinf.buff,"Generando Finguering Clave De FA   \n");
	   //SendMessage(pianoinf.m_hDialog,WM_PAINT,0,0);
	   //genetic(melodia,1);
	   costo=viterbi(melodia,1);
       //SendMessage(pianoinf.m_hDialog,WM_DESTROY,0,0);
	}
	if(concfg.soloritmo==1) 
	   soloritmo();
	eliminatiesctoc(melodia);    // eliminamos ties entre compases mientras no sepamos como tratarlos
	melodia->puneje.eje=generaejecucion(melodia->penta,0);
	melodia->puneje.compas=melodia->puneje.index=0;
	melodia->puneje.canelem=melodia->puneje.eje->penta[0].hor;
	melodia->metronomoreal=-1;	
	SetWindowText(hwnd,titlebar);
	resetscores(melodia);
	IntervalTestUpdate(0,0,-1);   // buffers a 0
	IntervalTestUpdate(0,0,-2);   // elimina silencios iniciales 
    scrollrandommusic(melodia,0);
	return 1;
}


/*
VARIACION PARA TIES ENTRE COMPASES
  
DWORD WINAPI playmelodia(void)
{
   int c,i,canelem,metro,compas,interval,toca0,toca1;
   __int64 tie[2],notakey[2]; // 15-07
   double mindur,con[2],redon;
   EJECUCION *eje;
   RUNPENTA *rp[2];
   static MELODIA *bufmelodia;
   clock_t init,fin;

   compas=melodia->puneje.compas;
   if(concfg.playmode==0) interval=compas+1;
   else interval=melodia->numcompases;
   playmelodiastatus=1;
   bufmelodia=melodia;
   melodia=copiamelodia(melodia);
   melodia->oth=0;
   if(melodia->puneje.eje!=NULL)
      delete [] melodia->puneje.eje;
   melodia->puneje.eje=NULL;   // eliminamos ejecucion actual
   IntervalTestUpdate(0,0,-1); // buffers midi a cero   
   melodia->metronomoreal=-1;
   metro=melodia->metronomo;
   melodia->puneje.index=0;
   melodia->puneje.compas=compas;
   for(c=compas;c<interval;c++){
	  if(c==melodia->numcompases) break;
	  redon=60.0*melodia->penta[0].compas[c].densignature*1000/metro;    //duracion redonda en milisegundos 
	  eje=generaejecucion(melodia->penta,c);
      rp[0]=&eje->penta[0];
      rp[1]=&eje->penta[1];
	  melodia->puneje.eje=generaejecucion(melodia->penta,c); // este eje es para intervaltestupdate ya que lo necesita internamente
	  IntervalTestUpdate(0,0,-2); // elimina silencios iniciales
      melodia->puneje.canelem=canelem=rp[0]->hor;
	  i=melodia->puneje.index;
//	  melodia->puneje.index=0;
	  melodia->puneje.compas=c;
	  con[0]=con[1]=0;
	  if(metronomostatus==1 && c==compas){
	     while(metronomobeat!=0);  // sincronizamos solo una vez para evitar pausas en la ejecucion de la melodia empezando en el compas desde el que pulsamos play
	  }
      for(;i<canelem;i++){
		 toca0=toca1=0;
		 if(i!=0){
			int posc,posa;
		    posc=eje->penta[0].runnota[i].pos;         
            posa=eje->penta[0].runnota[i-1].pos;         
            if(posc!=posa) con[0]=0;
		    posc=eje->penta[1].runnota[i].pos;         
            posa=eje->penta[1].runnota[i-1].pos;         
			if(posc!=posa) con[1]=0;
		 }
		 if(i==0 && c==compas) tie[0]=tie[1]=notakey[0]=notakey[1]=0; //15-07
		 if(i!=0){ 
	        tie[0]=rp[0]->runnota[i-1].tie; tie[1]=rp[1]->runnota[i-1].tie; 
			notakey[0]=rp[0]->runnota[i-1].notakey;
			notakey[1]=rp[1]->runnota[i-1].notakey;
		 } 

		 // si con[0] es menor que una semifusa consideraremos que la nota ya se ha extinguido
		 init=clock();
		 if(round(con[0],6)==0){
			if(i!=0) PlayNotaEndTeclado(notakey[0],tie[0]);   //15-07								
			if(i==0){
			   if(tie[0]==1 && rp[0]->runnota[i].tie==0)
                  PlayNotaEndTeclado(rp[0]->runnota[i].notakey,0);
			}
		    con[0]=rp[0]->runnota[i].dura;
			toca0=1;
		 }
         // si con[0] es menor que una semifusa consideraremos que la nota ya se ha extinguido		 
		 if(round(con[1],6)==0){
			if(i!=0) PlayNotaEndTeclado(notakey[1],tie[1]);   //15-07
			if(i==0){
			   if(tie[1]==1 && rp[1]->runnota[i].tie==0)
                  PlayNotaEndTeclado(rp[1]->runnota[i].notakey,0);
			}
		    con[1]=rp[1]->runnota[i].dura;
			toca1=1;
		 }
         if(toca0==1){
	        PlayNotaTeclado(rp[0]->runnota[i].notakey,notakey[0],tie[0]);                 //15-07
		 }
         if(toca1==1){
            PlayNotaTeclado(rp[1]->runnota[i].notakey,notakey[1],tie[1]);                 //15-07			
		 }
		 fin=clock();
		 fin=fin-init;
		 mindur=min(con[0],con[1]);
         if(redon*mindur-fin>0)
		    Sleep((DWORD) (redon*mindur-fin)); // eliminamos el retardo en printar y tocar la nota
         con[0]-=mindur;
		 con[1]-=mindur;
		 if(playmelodiastopv==1){ 
            tie[0]=rp[0]->runnota[i].tie; //15-07
			tie[1]=rp[1]->runnota[i].tie; //15-07
	        PlayNotaEndTeclado(rp[0]->runnota[i].notakey,tie[0]);
	        PlayNotaEndTeclado(rp[1]->runnota[i].notakey,tie[1]);
			break;
		 }	
	  }
	  if(i>0){        
		 tie[0]=rp[0]->runnota[i-1].tie; //15-07
		 tie[1]=rp[1]->runnota[i-1].tie; //15-07
		 notakey[0]=rp[0]->runnota[i-1].notakey;
		 notakey[1]=rp[1]->runnota[i-1].notakey;
	     PlayNotaEndTeclado(notakey[0],tie[0]);
	     PlayNotaEndTeclado(notakey[1],tie[1]);
	  }
	  delete [] eje;
	  delete [] melodia->puneje.eje;
	  melodia->puneje.eje=NULL;
	  if(playmelodiastopv==1) break;
   }	  
   LiberaMelodia(melodia);
   melodia=bufmelodia;
   melodia->puneje.eje=generaejecucion(melodia->penta,compas);
   IntervalTestUpdate(0,0,-1);
   IntervalTestUpdate(0,0,-2);
   offsetdispinterval=concfg.maxcompascreen*(melodia->puneje.compas/concfg.maxcompascreen);
   playmelodiastopv=0;
   playmelodiastatus=0;
   if(concfg.playmode==0){
      SendMessage(buttonplay, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) bmpplayn);
	  InvalidateRect(hwnd, &rect, FALSE);
	  UpdateWindow(hwnd);
   }
//   PostMessage(hwnd,WM_APP+3,(WPARAM) 0, (LPARAM) 0);    
//   InvalidateRect(hwnd, &rect, FALSE);
//	   UpdateWindow(hwnd);
   return 1;
}
*/  
