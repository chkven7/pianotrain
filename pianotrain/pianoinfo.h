#include <windows.h>


//class vncAbout;

#if (!defined(_PIANOINFO))
#define _PIANOINFO

// The vncAbout class itself
class pianoinfo
{
public:
	// Constructor/destructor
    char buff[256];
	char buffkey;
	
	pianoinfo();
	~pianoinfo();
	pianoinfo(char *);

	// Initialisation
	BOOL Init();

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// General
	BOOL Show(BOOL show);

	// Implementation
	BOOL m_dlgvisible;
	HWND m_hDialog;
};

#endif 
