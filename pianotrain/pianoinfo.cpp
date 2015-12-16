#include <windows.h>
#include <string.h>
#include "resource.h"
#include "pianoinfo.h"

#define BACKGROUND RGB(244,238,217)   // fondo de la ventana

HBRUSH hWhiteBrushdlg;

// Constructor/destructor

extern HINSTANCE hInst;
extern HWND hwnd;
extern HFONT hSmallFontItalic;

pianoinfo::~pianoinfo()
{
}

pianoinfo::pianoinfo()
{
	m_dlgvisible = FALSE;
}

pianoinfo::pianoinfo(char *s)
{
	m_dlgvisible = FALSE;
	strcpy(buff,s);
}


// Initialisation
BOOL
pianoinfo::Init()
{
	buffkey=0;
	return TRUE;
}

// Dialog box handling functions
BOOL
pianoinfo::Show(BOOL show)
{

	if (show)
	{
		if (!m_dlgvisible)
		{ 
			CreateDialogParam(hInst, MAKEINTRESOURCE(DIAL_INFORM),hwnd,(DLGPROC) DialogProc,(LONG) this);
		}
		else
		{
			// The dialog is already displayed, just raise it to foreground.
			SetForegroundWindow(m_hDialog);
		}
	}
	return 1;
}

BOOL CALLBACK
pianoinfo::DialogProc(HWND hwnd,
					 UINT uMsg,
					 WPARAM wParam,
					 LPARAM lParam )
{
    pianoinfo *_this = (pianoinfo *) GetWindowLong(hwnd, GWL_USERDATA);
	switch (uMsg){
		case WM_PAINT:
			HDC hdc;
			HGDIOBJ oldf;

			hdc=GetDC(hwnd);			
            SetBkColor(hdc, BACKGROUND);
			oldf=SelectObject(hdc,hSmallFontItalic);
			TextOut(hdc,20,40,_this->buff,strlen(_this->buff));
			SelectObject(hdc,oldf);
			ReleaseDC(hwnd,hdc);
            return 0;
		case WM_CTLCOLORDLG:
			{
			   HDC hdcDlg;

               hdcDlg = (HDC) wParam;
			   hWhiteBrushdlg =CreateSolidBrush(BACKGROUND); 
			   return (long)hWhiteBrushdlg;
			}

		case WM_INITDIALOG:
		{
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (pianoinfo *) lParam;
			SetForegroundWindow(hwnd);
			_this->m_hDialog = hwnd;
			_this->m_dlgvisible = TRUE;
			return TRUE;
		}
	   case WM_COMMAND:
		   switch (LOWORD(wParam)){
		      case IDCANCEL:
			     EndDialog(hwnd, FALSE);
			     _this->m_dlgvisible = FALSE;
			     return FALSE;
		   }
	   case WM_KEYUP:
		  _this->buffkey = HIWORD(lParam) & 0x0FF;
		  return 0;		   
	   case WM_DESTROY:
		   DeleteObject(hWhiteBrushdlg);
	       EndDialog(hwnd, FALSE);
	       _this->m_dlgvisible = FALSE;
	       return TRUE;		   
	}
	return 0;
}
