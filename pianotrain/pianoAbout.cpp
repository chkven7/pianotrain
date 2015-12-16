#include <windows.h>
#include "resource.h"
#include "pianoAbout.h"

#define VERSION "1.55"
// Constructor/destructor

extern HINSTANCE hInst;

pianoAbout::pianoAbout()
{
	m_dlgvisible = FALSE;
}

pianoAbout::~pianoAbout()
{
}

// Initialisation
BOOL
pianoAbout::Init()
{
	return TRUE;
}

// Dialog box handling functions
void
pianoAbout::Show(BOOL show)
{
	int error;

	if (show)
	{
		if (!m_dlgvisible)
		{ 
			error=DialogBoxParam(hInst,
				MAKEINTRESOURCE(IDD_ABOUT), 
				NULL,
				(DLGPROC) DialogProc,
				(LONG) this);
			error=GetLastError();
		}
		else
		{
			// The dialog is already displayed, just raise it to foreground.
			SetForegroundWindow(m_hDialog);
		}
	}
}

BOOL CALLBACK
pianoAbout::DialogProc(HWND hwnd,
					 UINT uMsg,
					 WPARAM wParam,
					 LPARAM lParam )
{
	// We use the dialog-box's USERDATA to store a _this pointer
	// This is set only once WM_INITDIALOG has been recieved, though!
	pianoAbout *_this = (pianoAbout *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (uMsg)
	{

	case WM_INITDIALOG:
		{
			// Retrieve the Dialog box parameter and use it as a pointer
			// to the calling vncProperties object
			char buff[64]="PianoTrain version ";
			strcat(buff,VERSION);
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (pianoAbout *) lParam;

			// Set information about build time
			// SetDlgItemText(hwnd, IDC_BUILDTIME, g_buildTime);

			// Show the dialog
			SetDlgItemText(hwnd,IDC_STATIC_VERSION,buff);
			SetForegroundWindow(hwnd);

			_this->m_hDialog = hwnd;
			_this->m_dlgvisible = TRUE;

			return TRUE;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
		case IDOK:
			// Close the dialog
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
