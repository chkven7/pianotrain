#include <windows.h>


//class vncAbout;

#if (!defined(_ABOUT))
#define _ABOUT

// The vncAbout class itself
class pianoAbout
{
public:
	// Constructor/destructor
	pianoAbout();
	~pianoAbout();

	// Initialisation
	BOOL Init();

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// General
	void Show(BOOL show);

	// Implementation
	BOOL m_dlgvisible;
	HWND m_hDialog;
};

#endif // _WINVNC_VNCABOUT
