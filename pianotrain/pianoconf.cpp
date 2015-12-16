#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "piano.h"
#include "pianoconf.h"

// Constructor/destructor

extern HINSTANCE hInst;
extern CONFIG concfg;
extern TCHAR currentdir[256],titlebar[256];
double round(double doValue, int nPrecision,int ro=1);
void cargarconfiguracion(MELODIA *melodia);
void saveconfiguracion(void);
char *openfile(void);
int miditoabc(char *file);

pianoconf::pianoconf(unsigned int dial)
{
	m_dlgvisible = FALSE;
	dialogo=dial;
}

pianoconf::~pianoconf()
{
}

// Initialisation
BOOL
pianoconf::Init()
{
	return TRUE;
}

// Dialog box handling functions
BOOL
pianoconf::Show(BOOL show)
{
	int error;

	if (show)
	{
		if (!m_dlgvisible)
		{ 
			error=DialogBoxParam(hInst,
				//MAKEINTRESOURCE(DIAL_APPR), 
				MAKEINTRESOURCE(dialogo), // prueba
				NULL,
				(DLGPROC) DialogProc,
				(LONG) this);
		}
		else
		{
			// The dialog is already displayed, just raise it to foreground.
			SetForegroundWindow(m_hDialog);
		}
	}
	return error;
}

BOOL CALLBACK
pianoconf::DialogProc(HWND hwnd,
					 UINT uMsg,
					 WPARAM wParam,
					 LPARAM lParam )
{
    int idButton,idevent;
	char buff[256];
	
	pianoconf *_this = (pianoconf *) GetWindowLong(hwnd, GWL_USERDATA);    
	switch (uMsg){
	   case WM_INITDIALOG:
		{
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (pianoconf *) lParam;
			cargarconfiguracion(NULL);
            SetDlgItemText(hwnd,IDC_COMPAS,concfg.compas);
			SetDlgItemText(hwnd,IDC_MAXINTERSOL,_itoa(concfg.maxintervalog,buff,10));
            SetDlgItemText(hwnd,IDC_MININTERSOL,_itoa(concfg.minintervalog,buff,10));
			SetDlgItemText(hwnd,IDC_MAXINTERFA,_itoa(concfg.maxintervalof,buff,10));
			SetDlgItemText(hwnd,IDC_MININTERFA,_itoa(concfg.minintervalof,buff,10));
			SetDlgItemText(hwnd,IDC_MAXINTERSOL_NOTEINT,_itoa(concfg.maxintervalog_noteinterval,buff,10));
            SetDlgItemText(hwnd,IDC_MININTERSOL_NOTEINT,_itoa(concfg.minintervalog_noteinterval,buff,10));
			SetDlgItemText(hwnd,IDC_MAXINTERFA_NOTEINT,_itoa(concfg.maxintervalof_noteinterval,buff,10));
			SetDlgItemText(hwnd,IDC_MININTERFA_NOTEINT,_itoa(concfg.minintervalof_noteinterval,buff,10));									
			SetDlgItemText(hwnd,IDC_MAXINTERVAL_FLASHNOTEINT,_itoa(concfg.maxintervalo_flashnoteinterval,buff,10));
            SetDlgItemText(hwnd,IDC_MININTERVAL_FLASHNOTEINT,_itoa(concfg.minintervalo_flashnoteinterval,buff,10));
			SetDlgItemText(hwnd,IDC_ARMA,concfg.armadura);
			SetDlgItemText(hwnd,IDC_NUMCOMPAS,concfg.numcompases);
			SetDlgItemText(hwnd,IDC_BLOQSOL,concfg.bloquesritmicosg);
            SetDlgItemText(hwnd,IDC_BLOQFA,concfg.bloquesritmicosf);
			SetDlgItemText(hwnd,IDC_REPEATG,_itoa(concfg.repeatg,buff,10));
			SetDlgItemText(hwnd,IDC_REPEATF,_itoa(concfg.repeatf,buff,10));			
			SetDlgItemText(hwnd,IDC_METRONOMO,_itoa(concfg.metronomo,buff,10));
			SetDlgItemText(hwnd,IDC_MAXCOMPASCREEN,_itoa(concfg.maxcompascreen,buff,10));
			SetDlgItemText(hwnd,IDC_MAXVARIACION,_itoa(concfg.maxvariacion,buff,10));
			SetDlgItemText(hwnd,IDC_ACCURACYNOTES,_itoa(concfg.accuracynotes,buff,10));
			SetDlgItemText(hwnd,IDC_ACCURACYTEMPO,_itoa(concfg.accuracytempo,buff,10));
			SetDlgItemText(hwnd,IDC_ARMANOTEREADING,_itoa(concfg.armaduranotereading,buff,10));
			SetDlgItemText(hwnd,IDC_ARMANOTEINTERVAL,_itoa(concfg.armaduranoteinterval,buff,10));
            SetDlgItemText(hwnd,IDC_NOTEREADINGMEMO,_itoa(concfg.notereadingmemo,buff,10));
			SetDlgItemText(hwnd,IDC_MAXINTERSOL_NOTEREAD,_itoa(concfg.maxintervalog_noteread,buff,10));
            SetDlgItemText(hwnd,IDC_MININTERSOL_NOTEREAD,_itoa(concfg.minintervalog_noteread,buff,10));
			SetDlgItemText(hwnd,IDC_MAXINTERFA_NOTEREAD,_itoa(concfg.maxintervalof_noteread,buff,10));
			SetDlgItemText(hwnd,IDC_MININTERFA_NOTEREAD,_itoa(concfg.minintervalof_noteread,buff,10));									

			char *ComboBoxItems[2] = { "Rand", "SmartRand"}; 
			HWND hCtl = GetDlgItem(hwnd, IDC_NOTERESE);		
			if(hCtl!=NULL){
               SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems[0]);
               SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems[1]);
			   SendMessage(hCtl,CB_SETCURSEL,concfg.notereadlearning,0);
			   SetForegroundWindow(hwnd);
			}

			char *ComboBoxItems1[6] = { "FullRnd", "Progressive","Markov-Beyer","Markov-Bach","midi","musicxml"}; 
			hCtl = GetDlgItem(hwnd, IDC_RANDMODE);		
			if(hCtl!=NULL){
               SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems1[0]);
               SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems1[1]);
			   SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems1[2]);
			   SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems1[3]);
			   SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems1[4]);
			   SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems1[5]);
			   SendMessage(hCtl,CB_SETCURSEL,concfg.randommodegenerationmusic,0);
			   SetForegroundWindow(hwnd);
			}

			char *ComboBoxItems2[4] = { "Ninguno", "Compas","Nota","Tocado"}; 
			hCtl = GetDlgItem(hwnd, IDC_OCULTANOTA);		
			if(hCtl!=NULL){
               SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems2[0]);
               SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems2[1]);
			   SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems2[2]);
			   SendMessage(hCtl,CB_ADDSTRING,0,(LPARAM) (LPCTSTR)ComboBoxItems2[3]);
			   SendMessage(hCtl,CB_SETCURSEL,concfg.ocultanota,0);
			   SetForegroundWindow(hwnd);
			}
			HWND hCtly = GetDlgItem(hwnd, IDC_FINGUERY);		
			HWND hCtln = GetDlgItem(hwnd, IDC_FINGUERN);		
			if(hCtly!=NULL && hCtln!=NULL){ 
			   if(concfg.fingering==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_SINGOCTAVAY);		
			hCtln = GetDlgItem(hwnd, IDC_SINGOCTAVAN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.singoctava==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_EAROCTAVAY);		
			hCtln = GetDlgItem(hwnd, IDC_EAROCTAVAN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.earoctava==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_STOPONERRORY);		
			hCtln = GetDlgItem(hwnd, IDC_STOPONERRORN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.stoponerror==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGACCIY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGACCIN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadingacci==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGCHORDY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGCHORDN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadingchord==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGDEEPY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGDEEPN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadingdeep==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   } 
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGLEADY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGLEADN);		
			if(hCtly!=NULL && hCtln!=NULL){
			  if(concfg.notereadingmemo==0){
			     SendMessage(hCtly,BM_SETCHECK ,BST_INDETERMINATE,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_INDETERMINATE,0);
			  }else{
			     if(concfg.notereadinglead==1){ 
				    SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                    SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
				 }else{
				    SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                    SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
				 }
			  }
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGFIGURY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGFIGURN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadingfiguras==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGFINGERY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGFINGERN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadingfingering==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICACCIY);		
			hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICACCIN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.randommusicacci==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_SOLORITMOY);		
			hCtln = GetDlgItem(hwnd, IDC_SOLORITMON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.soloritmo==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}

			hCtly = GetDlgItem(hwnd, IDC_RITMOY);		
			hCtln = GetDlgItem(hwnd, IDC_RITMON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.ritmo==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_SILENCIOY);		
			hCtln = GetDlgItem(hwnd, IDC_SILENCION);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.silencios==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   } 
			}
			hCtly = GetDlgItem(hwnd, IDC_RITMOASISY);		
			hCtln = GetDlgItem(hwnd, IDC_RITMOASISN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.followme==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_REPMALRITMOY);		
			hCtln = GetDlgItem(hwnd, IDC_REPMALRITMON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.repsierrritmo==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }			
			}
			hCtly = GetDlgItem(hwnd, IDC_SIGUEMEY);		
			hCtln = GetDlgItem(hwnd, IDC_SIGUEMEN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.sigueme==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			if(concfg.randommusicqosritmo==0 && concfg.randommusicqoslegato==0 &&
               concfg.randommusicqoscompas==0) 
			   concfg.randommusicqoscompas=1;
			hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSRITMOY);		
			hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSRITMON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.randommusicqosritmo==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   } 
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADQOSRITMOY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADQOSRITMON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadqosritmo==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   } 
			}
			hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSLEGATOY);		
			hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSLEGATON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.randommusicqoslegato==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_NOTEREADQOSLEGATOY);		
			hCtln = GetDlgItem(hwnd, IDC_NOTEREADQOSLEGATON);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.notereadqoslegato==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSCOMPASY);		
			hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSCOMPASN);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.randommusicqoscompas==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_OIDOABSOLUTO);		
			hCtln = GetDlgItem(hwnd, IDC_OIDORELATIVO);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.oidoabrel==1){ 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{
                  if(round(concfg.maxtono_earnoteread,6)==0.0) concfg.maxtono_earnoteread=1;
				  if(round(concfg.mintono_earnoteread,6)==0.0) concfg.mintono_earnoteread=1;
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   }
			}
			hCtly = GetDlgItem(hwnd, IDC_SOLFEOHABLADO);		
			hCtln = GetDlgItem(hwnd, IDC_SOLFEOCANTADO);		
			if(hCtly!=NULL && hCtln!=NULL){
			   if(concfg.tiposolfeo==1){    // solfeo hablado 
				  SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			   }else{  // solfep cantado
				  SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                  SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			   } 
			}
            {
			// supervisamos estado del boton para el caso de seleccion midi
			   HWND hCtl = GetDlgItem(hwnd, IDC_RANDMODE);
			   if(hCtl!=NULL){
                  int val=SendMessage(hCtl,CB_GETCURSEL, 0, 0);
			      hCtl = GetDlgItem(hwnd,IDC_OPENFILE);
			      if(val==4 || val==5){ 
			         ShowWindow(hCtl,SW_SHOW);
				     EnableWindow(hCtl,1);
				  }else{ 
			 	     EnableWindow(hCtl,0);
			         ShowWindow(hCtl,SW_HIDE);
				  }
			   }
			}
			sprintf(buff,"%.1f",concfg.maxtono_earnoteread);
			SetDlgItemText(hwnd,IDC_MAXTONO_EARNOTEREAD,buff);
			sprintf(buff,"%.1f",concfg.mintono_earnoteread);
            SetDlgItemText(hwnd,IDC_MINTONO_EARNOTEREAD,buff);
			_this->m_hDialog = hwnd;
			_this->m_dlgvisible = TRUE;
			return TRUE;
		}

	   case WM_COMMAND:
		   idButton = (int) LOWORD(wParam);    // identifier of button 
		   idevent=(int) HIWORD(wParam);
		   if(idButton==IDCANCEL){
			  EndDialog(hwnd, FALSE);
			  _this->m_dlgvisible = FALSE;
			  return FALSE;
		   }
		   if(idButton==IDOK){
              GetDlgItemText(hwnd,IDC_COMPAS,buff,64); if(buff[0]!=0) strcpy(concfg.compas,buff);
			  GetDlgItemText(hwnd,IDC_MAXINTERSOL,buff,64); if(buff[0]!=0) concfg.maxintervalog=atoi(buff);
              GetDlgItemText(hwnd,IDC_MININTERSOL,buff,64); if(buff[0]!=0) concfg.minintervalog=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MAXINTERFA,buff,64);  if(buff[0]!=0) concfg.maxintervalof=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MININTERFA,buff,64);  if(buff[0]!=0) concfg.minintervalof=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MAXINTERSOL_NOTEINT,buff,64); if(buff[0]!=0) concfg.maxintervalog_noteinterval=atoi(buff);
              GetDlgItemText(hwnd,IDC_MININTERSOL_NOTEINT,buff,64); if(buff[0]!=0) concfg.minintervalog_noteinterval=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MAXINTERFA_NOTEINT,buff,64);  if(buff[0]!=0) concfg.maxintervalof_noteinterval=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MININTERFA_NOTEINT,buff,64);  if(buff[0]!=0) concfg.minintervalof_noteinterval=atoi(buff);			
			  GetDlgItemText(hwnd,IDC_MAXINTERVAL_FLASHNOTEINT,buff,64);  if(buff[0]!=0) concfg.maxintervalo_flashnoteinterval=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MININTERVAL_FLASHNOTEINT,buff,64);  if(buff[0]!=0) concfg.minintervalo_flashnoteinterval=atoi(buff);									
			  GetDlgItemText(hwnd,IDC_MAXTONO_EARNOTEREAD,buff,64);  if(buff[0]!=0) concfg.maxtono_earnoteread=atof(buff);
			  GetDlgItemText(hwnd,IDC_MINTONO_EARNOTEREAD,buff,64);  if(buff[0]!=0) concfg.mintono_earnoteread=atof(buff);									
			  GetDlgItemText(hwnd,IDC_ARMA,buff,64); if(buff[0]!=0) strcpy(concfg.armadura,buff);
			  GetDlgItemText(hwnd,IDC_NUMCOMPAS,buff,64); if(buff[0]!=0) strcpy(concfg.numcompases,buff);
			  GetDlgItemText(hwnd,IDC_BLOQSOL,buff,256); if(buff[0]!=0) strcpy(concfg.bloquesritmicosg,buff);
              GetDlgItemText(hwnd,IDC_BLOQFA,buff,256);  if(buff[0]!=0) strcpy(concfg.bloquesritmicosf,buff);
              GetDlgItemText(hwnd,IDC_REPEATG,buff,64); if(buff[0]!=0) concfg.repeatg=atoi(buff);
			  GetDlgItemText(hwnd,IDC_REPEATF,buff,64); if(buff[0]!=0) concfg.repeatf=atoi(buff);
			  GetDlgItemText(hwnd,IDC_METRONOMO,buff,64); if(buff[0]!=0) concfg.metronomo=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MAXCOMPASCREEN,buff,64); if(buff[0]!=0) concfg.maxcompascreen=atoi(buff);
              GetDlgItemText(hwnd,IDC_MAXVARIACION,buff,64); if(buff[0]!=0) concfg.maxvariacion=atoi(buff);
			  GetDlgItemText(hwnd,IDC_ACCURACYNOTES,buff,64); if(buff[0]!=0) concfg.accuracynotes=atoi(buff);
              GetDlgItemText(hwnd,IDC_ACCURACYTEMPO,buff,64); if(buff[0]!=0) concfg.accuracytempo=atoi(buff);
              GetDlgItemText(hwnd,IDC_ARMANOTEREADING,buff,64); if(buff[0]!=0) concfg.armaduranotereading=atoi(buff);
			  GetDlgItemText(hwnd,IDC_ARMANOTEINTERVAL,buff,64); if(buff[0]!=0) concfg.armaduranoteinterval=atoi(buff);
			  GetDlgItemText(hwnd,IDC_NOTEREADINGMEMO,buff,64); if(buff[0]!=0) concfg.notereadingmemo=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MAXINTERSOL_NOTEREAD,buff,64); if(buff[0]!=0) concfg.maxintervalog_noteread=atoi(buff);
              GetDlgItemText(hwnd,IDC_MININTERSOL_NOTEREAD,buff,64); if(buff[0]!=0) concfg.minintervalog_noteread=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MAXINTERFA_NOTEREAD,buff,64);  if(buff[0]!=0) concfg.maxintervalof_noteread=atoi(buff);
			  GetDlgItemText(hwnd,IDC_MININTERFA_NOTEREAD,buff,64);  if(buff[0]!=0) concfg.minintervalof_noteread=atoi(buff);			
			  if(concfg.maxcompascreen>9) concfg.maxcompascreen=9;              
			  HWND hCtl = GetDlgItem(hwnd, IDC_NOTERESE);
			  if(hCtl!=NULL)
                 concfg.notereadlearning=SendMessage(hCtl, CB_GETCURSEL, 0, 0);

              hCtl = GetDlgItem(hwnd, IDC_RANDMODE);
			  if(hCtl!=NULL)
                 concfg.randommodegenerationmusic=SendMessage(hCtl, CB_GETCURSEL, 0, 0);

              hCtl = GetDlgItem(hwnd, IDC_OCULTANOTA);
			  if(hCtl!=NULL)
                 concfg.ocultanota=SendMessage(hCtl, CB_GETCURSEL, 0, 0);

			  HWND hCtly = GetDlgItem(hwnd, IDC_SINGOCTAVAY);		
			  HWND hCtln = GetDlgItem(hwnd, IDC_SINGOCTAVAN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.singoctava=1;
			        else concfg.singoctava=0;
			  }

			  hCtly = GetDlgItem(hwnd, IDC_EAROCTAVAY);		
			  hCtln = GetDlgItem(hwnd, IDC_EAROCTAVAN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.earoctava=1;
			     else concfg.earoctava=0;
			  }

			  hCtly = GetDlgItem(hwnd, IDC_STOPONERRORY);		
			  hCtln = GetDlgItem(hwnd, IDC_STOPONERRORN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.stoponerror=1;
			     else concfg.stoponerror=0;
			  }

			  hCtly = GetDlgItem(hwnd, IDC_FINGUERY);		
			  hCtln = GetDlgItem(hwnd, IDC_FINGUERN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.fingering=1;
			     else concfg.fingering=0;
			  }

			  hCtly = GetDlgItem(hwnd, IDC_SOLORITMOY);		
			  hCtln = GetDlgItem(hwnd, IDC_SOLORITMON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.soloritmo=1;
			     else concfg.soloritmo=0;
			  }

			  hCtly = GetDlgItem(hwnd, IDC_RITMOY);		
			  hCtln = GetDlgItem(hwnd, IDC_RITMON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.ritmo=1;
			     else concfg.ritmo=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_SILENCIOY);		
			  hCtln = GetDlgItem(hwnd, IDC_SILENCION);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.silencios=1;
			     else concfg.silencios=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_RITMOASISY);		
			  hCtln = GetDlgItem(hwnd, IDC_RITMOASISN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.followme=1;
			     else concfg.followme=0;
			  }
		      hCtly = GetDlgItem(hwnd, IDC_REPMALRITMOY);		
			  hCtln = GetDlgItem(hwnd, IDC_REPMALRITMON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.repsierrritmo=1;
			     else concfg.repsierrritmo=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_SIGUEMEY);		
			  hCtln = GetDlgItem(hwnd, IDC_SIGUEMEN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.sigueme=1;
			     else concfg.sigueme=0;
			  }
		      hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSRITMOY);		
			  hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSRITMON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.randommusicqosritmo=1;
			     else concfg.randommusicqosritmo=0;
			  }
		      hCtly = GetDlgItem(hwnd, IDC_NOTEREADQOSRITMOY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADQOSRITMON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadqosritmo=1;
			     else concfg.notereadqosritmo=0;
			  }
		      hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSLEGATOY);		
			  hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSLEGATON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.randommusicqoslegato=1;
			     else concfg.randommusicqoslegato=0;
			  }
		      hCtly = GetDlgItem(hwnd, IDC_NOTEREADQOSLEGATOY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADQOSLEGATON);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadqoslegato=1;
			     else concfg.notereadqoslegato=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSCOMPASY);		
			  hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSCOMPASN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.randommusicqoscompas=1;
			     else concfg.randommusicqoscompas=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGACCIY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGACCIN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadingacci=1;
			     else concfg.notereadingacci=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGCHORDY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGCHORDN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadingchord=1;
		 	     else concfg.notereadingchord=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGDEEPY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGDEEPN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadingdeep=1;
			     else concfg.notereadingdeep=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGLEADY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGLEADN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadinglead=1;
			     else concfg.notereadinglead=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGFIGURY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGFIGURN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadingfiguras=1;
			     else concfg.notereadingfiguras=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGFINGERY);		
			  hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGFINGERN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.notereadingfingering=1;
			     else concfg.notereadingfingering=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICACCIY);		
			  hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICACCIN);		
			  if(hCtly!=NULL && hCtln!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.randommusicacci=1;
			     else concfg.randommusicacci=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_OIDOABSOLUTO);		
			  if(hCtly!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.oidoabrel=1;
			     else concfg.oidoabrel=0;
			  }
			  hCtly = GetDlgItem(hwnd, IDC_SOLFEOHABLADO);		
			  if(hCtly!=NULL){
			     if(SendMessage(hCtly,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.tiposolfeo=1;
			     else concfg.tiposolfeo=0;			
			  }
  			  if(concfg.randommusicqosritmo==0 && concfg.randommusicqoslegato==0 &&
                 concfg.randommusicqoscompas==0) 
				 concfg.randommusicqoscompas=1;
			  saveconfiguracion();        
			  EndDialog(hwnd, TRUE);
			  _this->m_dlgvisible = FALSE;
			  return TRUE;
		   }

		   if(idButton==IDC_NOTEREADINGMEMO){
			  int temp;
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGLEADY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGLEADN);		
              GetDlgItemText(hwnd,IDC_NOTEREADINGMEMO,buff,64); 
		      temp=atoi(buff);			      
		      if(temp>0){
		         if(concfg.notereadinglead==1){ 
			        SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                    SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
				 }else{
			        SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                    SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
				 }
			  }else{
			   SendMessage(hCtly,BM_SETCHECK ,BST_INDETERMINATE,0);
               SendMessage(hCtln,BM_SETCHECK ,BST_INDETERMINATE,0);
			  }
		   } 
		   
		   if(idButton==IDC_SINGOCTAVAY || idButton==IDC_SINGOCTAVAN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_SINGOCTAVAY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_SINGOCTAVAN);		
		      if(idButton==IDC_SINGOCTAVAY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_EAROCTAVAY || idButton==IDC_EAROCTAVAN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_EAROCTAVAY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_EAROCTAVAN);		
		      if(idButton==IDC_EAROCTAVAY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_STOPONERRORY || idButton==IDC_STOPONERRORN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_STOPONERRORY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_STOPONERRORN);		
		      if(idButton==IDC_STOPONERRORY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_NOTEREADINGACCIY || idButton==IDC_NOTEREADINGACCIN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGACCIY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGACCIN);		
		      if(idButton==IDC_NOTEREADINGACCIY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_NOTEREADINGCHORDY || idButton==IDC_NOTEREADINGCHORDN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGCHORDY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGCHORDN);		
		      if(idButton==IDC_NOTEREADINGCHORDY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_NOTEREADINGDEEPY || idButton==IDC_NOTEREADINGDEEPN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGDEEPY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGDEEPN);		
		      if(idButton==IDC_NOTEREADINGDEEPY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }               

		   if(idButton==IDC_NOTEREADINGLEADY || idButton==IDC_NOTEREADINGLEADN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGLEADY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGLEADN);		
		      if(idButton==IDC_NOTEREADINGLEADY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_NOTEREADINGFIGURY || idButton==IDC_NOTEREADINGFIGURN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGFIGURY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGFIGURN);		
		      if(idButton==IDC_NOTEREADINGFIGURY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
		         SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_NOTEREADINGFINGERY || idButton==IDC_NOTEREADINGFINGERN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADINGFINGERY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADINGFINGERN);		
		      if(idButton==IDC_NOTEREADINGFINGERY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_RANDOMMUSICACCIY || idButton==IDC_RANDOMMUSICACCIN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICACCIY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICACCIN);		
		      if(idButton==IDC_RANDOMMUSICACCIY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_FINGUERY || idButton==IDC_FINGUERN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_FINGUERY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_FINGUERN);		
		      if(idButton==IDC_FINGUERY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_SOLORITMOY || idButton==IDC_SOLORITMON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_SOLORITMOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_SOLORITMON);		
		      if(idButton==IDC_SOLORITMOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_RITMOY || idButton==IDC_RITMON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_RITMOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_RITMON);		
		      if(idButton==IDC_RITMOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_SILENCIOY || idButton==IDC_SILENCION){
		      HWND hCtly = GetDlgItem(hwnd, IDC_SILENCIOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_SILENCION);		
		      if(idButton==IDC_SILENCIOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_RITMOASISY || idButton==IDC_RITMOASISN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_RITMOASISY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_RITMOASISN);		
		      if(idButton==IDC_RITMOASISY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
		         SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_REPMALRITMOY || idButton==IDC_REPMALRITMON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_REPMALRITMOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_REPMALRITMON);		
		      if(idButton==IDC_REPMALRITMOY){ 
		         SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_SIGUEMEY || idButton==IDC_SIGUEMEN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_SIGUEMEY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_SIGUEMEN);		
		      if(idButton==IDC_SIGUEMEY){ 
		         SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_RANDOMMUSICQOSRITMOY || idButton==IDC_RANDOMMUSICQOSRITMON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSRITMOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSRITMON);		
		      if(idButton==IDC_RANDOMMUSICQOSRITMOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_NOTEREADQOSRITMOY || idButton==IDC_NOTEREADQOSRITMON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADQOSRITMOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADQOSRITMON);		
		      if(idButton==IDC_NOTEREADQOSRITMOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_RANDOMMUSICQOSLEGATOY || idButton==IDC_RANDOMMUSICQOSLEGATON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSLEGATOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSLEGATON);		
		      if(idButton==IDC_RANDOMMUSICQOSLEGATOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_NOTEREADQOSLEGATOY || idButton==IDC_NOTEREADQOSLEGATON){
		      HWND hCtly = GetDlgItem(hwnd, IDC_NOTEREADQOSLEGATOY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_NOTEREADQOSLEGATON);		
		      if(idButton==IDC_NOTEREADQOSLEGATOY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }
		   if(idButton==IDC_RANDOMMUSICQOSCOMPASY || idButton==IDC_RANDOMMUSICQOSCOMPASN){
		      HWND hCtly = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSCOMPASY);		
		      HWND hCtln = GetDlgItem(hwnd, IDC_RANDOMMUSICQOSCOMPASN);		
		      if(idButton==IDC_RANDOMMUSICQOSCOMPASY){ 
			     SendMessage(hCtly,BM_SETCHECK ,BST_CHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_UNCHECKED,0);
			  }else{
			     SendMessage(hCtly,BM_SETCHECK ,BST_UNCHECKED,0);
                 SendMessage(hCtln,BM_SETCHECK ,BST_CHECKED,0);
			  }
		   }

		   if(idButton==IDC_RANDMODE){
		      HWND hCtl = GetDlgItem(hwnd, IDC_RANDMODE);
              int val=SendMessage(hCtl,CB_GETCURSEL, 0, 0);
			  hCtl = GetDlgItem(hwnd,IDC_OPENFILE);
			  if(val==4 || val==5){ 
				 ShowWindow(hCtl,SW_SHOW);
			     EnableWindow(hCtl,1);
			  }else{ 
				 EnableWindow(hCtl,0);
			     ShowWindow(hCtl,SW_HIDE);
			  }
		   }
		   if(idButton==IDC_OPENFILE){
		     char *apopenfile;
			 HWND hCtl = GetDlgItem(hwnd, IDC_RANDMODE);
			 int val=SendMessage(hCtl,CB_GETCURSEL, 0, 0);
			 if(val==4){ 
			    apopenfile=openfile();
                if(apopenfile==NULL){ 
				   SetCurrentDirectory(currentdir);
				   SetForegroundWindow(hwnd);				  
				   break;                     
				}
                strcpy(concfg.midfile,apopenfile);
			    miditoabc(apopenfile);		
				strcpy(titlebar,"MIDI::");
				strcat(titlebar,apopenfile);
				SetCurrentDirectory(currentdir);
				SetForegroundWindow(hwnd);				  
			 }
			 if(val==5){
			    apopenfile=openfile();
                if(apopenfile==NULL){ 
				   SetCurrentDirectory(currentdir);
				   SetForegroundWindow(hwnd);				  
				   break;                     
				}
                strcpy(concfg.xmlfile,apopenfile);
				strcpy(titlebar,"MUSICXML::");
				strcat(titlebar,apopenfile);
				SetCurrentDirectory(currentdir);
				SetForegroundWindow(hwnd);				  
			 }
		   }
        break;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		_this->m_dlgvisible = FALSE;
		return TRUE;
	}
	return 0;
}

extern HWND hwnd;

char *openfile(void)
{
   OPENFILENAME ofn;       // common dialog box structure
   static char szFile[260];       // buffer for file name
 
   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;  // handle to owner window
   ofn.lpstrFile = szFile;
   ofn.lpstrTitle = TEXT("PianoTrain Open File");
   ofn.lpstrFile[0] = '\0'; 
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = "*.mid\0*.*\0";
   ofn.nFilterIndex = 0; // initial filter to select
   ofn.lpstrFileTitle = NULL; // initial file to select
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
   if(GetOpenFileName(&ofn) == TRUE){ 
      return ofn.lpstrFile;
   }
   return NULL;
}


//******************************************************

pianogoto::pianogoto()
{
	m_dlgvisible = FALSE;
}

pianogoto::~pianogoto()
{
}

// Initialisation
BOOL
pianogoto::Init()
{
	return TRUE;
}

// Dialog box handling functions
BOOL
pianogoto::Show(BOOL show)
{
	int error;

	if (show)
	{
		if (!m_dlgvisible)
		{ 
			error=DialogBoxParam(hInst,
				MAKEINTRESOURCE(DIAL_GOTO), 
				NULL,
				(DLGPROC) DialogProc,
				(LONG) this);
		}
		else
		{
			// The dialog is already displayed, just raise it to foreground.
			SetForegroundWindow(m_hDialog);
		}
	}
	return error;
}

BOOL CALLBACK
pianogoto::DialogProc(HWND hwnd,
					 UINT uMsg,
					 WPARAM wParam,
					 LPARAM lParam )
{
    char buff[256];

	pianogoto *_this= (pianogoto *) GetWindowLong(hwnd, GWL_USERDATA);
	switch (uMsg){
	   case WM_INITDIALOG:
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (pianogoto *) lParam;
			SetDlgItemText(hwnd,IDC_GOTOCOMPAS,_itoa(_this->initcompas,buff,10));
			_this->m_hDialog = hwnd;
			_this->m_dlgvisible = TRUE;
			return TRUE;
	   case WM_COMMAND:
		   switch (LOWORD(wParam)){
		      case IDCANCEL:
			     EndDialog(hwnd, FALSE);
			     _this->m_dlgvisible = FALSE;
			     return FALSE;
		      case IDOK:
                 GetDlgItemText(hwnd,IDC_GOTOCOMPAS,buff,64);
			     _this->initcompas=atoi(buff);
			     EndDialog(hwnd, TRUE);
			     _this->m_dlgvisible = FALSE;
			     return TRUE;
		   }
		   break;

	   case WM_DESTROY:
	      EndDialog(hwnd, FALSE);
		  _this->m_dlgvisible = FALSE;
		  return TRUE;
	}
	return 0;
}

pianoplay::pianoplay()
{
	m_dlgvisible = FALSE;
}

pianoplay::~pianoplay()
{
}

// Initialisation
BOOL
pianoplay::Init()
{
	return TRUE;
}

// Dialog box handling functions
BOOL
pianoplay::Show(BOOL show)
{
	int error;

	if (show)
	{
		if (!m_dlgvisible)
		{ 
			error=DialogBoxParam(hInst,
				MAKEINTRESOURCE(DIAL_PLAY), 
				NULL,
				(DLGPROC) DialogProc,
				(LONG) this);
		}
		else
		{
			// The dialog is already displayed, just raise it to foreground.
			SetForegroundWindow(m_hDialog);
		}
	}
	return error;
}

BOOL CALLBACK
pianoplay::DialogProc(HWND hwnd,
					 UINT uMsg,
					 WPARAM wParam,
					 LPARAM lParam )
{
	HWND hCtlo,hCtlf;

	pianoplay *_this= (pianoplay *) GetWindowLong(hwnd, GWL_USERDATA);
	switch (uMsg){
	   case WM_INITDIALOG:
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (pianoplay *) lParam;
			
			hCtlo = GetDlgItem(hwnd, IDC_PLAYONLYACT);		
			hCtlf = GetDlgItem(hwnd, IDC_PLAYFROMACT);		
			if(concfg.playmode==0){
				SendMessage(hCtlo,BM_SETCHECK ,BST_CHECKED,0);
                SendMessage(hCtlf,BM_SETCHECK ,BST_UNCHECKED,0);
			}else{
				SendMessage(hCtlo,BM_SETCHECK ,BST_UNCHECKED,0);
                SendMessage(hCtlf,BM_SETCHECK ,BST_CHECKED,0);
			}			
			_this->m_hDialog = hwnd;
			_this->m_dlgvisible = TRUE;
			return TRUE;
	   case WM_COMMAND:
		   switch (LOWORD(wParam)){
		      case IDCANCEL:
			     EndDialog(hwnd, FALSE);
			     _this->m_dlgvisible = FALSE;
			     return FALSE;
		      case IDOK:
			     hCtlo= GetDlgItem(hwnd, IDC_PLAYONLYACT);		
			     hCtlf= GetDlgItem(hwnd, IDC_PLAYFROMACT);
			     if(SendMessage(hCtlo,BM_GETCHECK ,0,0)==BST_CHECKED) concfg.playmode=0;
			     else concfg.playmode=1;
			     EndDialog(hwnd, TRUE);
			     _this->m_dlgvisible = FALSE;
			     return TRUE;
		   }
       case BN_CLICKED:
		   {
		      int idButton = (int) LOWORD(wParam);    // identifier of button 
	          if(idButton==IDC_PLAYONLYACT || idButton==IDC_PLAYFROMACT){
		         hCtlo = GetDlgItem(hwnd, IDC_PLAYONLYACT);		
			     hCtlf = GetDlgItem(hwnd, IDC_PLAYFROMACT);		
			     if(idButton==IDC_PLAYONLYACT){ 
			        SendMessage(hCtlo,BM_SETCHECK ,BST_CHECKED,0);
                    SendMessage(hCtlf,BM_SETCHECK ,BST_UNCHECKED,0);
				 }else{
			        SendMessage(hCtlo,BM_SETCHECK ,BST_UNCHECKED,0);
                    SendMessage(hCtlf,BM_SETCHECK ,BST_CHECKED,0);
				 }
			  }
		   }
		   break;
	   case WM_DESTROY:
	      EndDialog(hwnd, FALSE);
		  _this->m_dlgvisible = FALSE;
		  return TRUE;
	}
	return 0;
}


