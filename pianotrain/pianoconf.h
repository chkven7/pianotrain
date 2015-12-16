#include <windows.h>


//class vncAbout;

#if (!defined(_PIANOCONF))
#define _PIANOCONF

// The vncAbout class itself
class pianoconf
{
public:
	// Constructor/destructor
	pianoconf(unsigned int);
	~pianoconf();

	// Initialisation
	BOOL Init();

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// General
	BOOL Show(BOOL show);

	// Implementation
	BOOL m_dlgvisible;
	HWND m_hDialog;
	unsigned int dialogo;
};

class pianogoto
{
public:
	// Constructor/destructor
	pianogoto();
	~pianogoto();

	// Initialisation
	BOOL Init();

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// General
	BOOL Show(BOOL show);

	// Implementation
	int initcompas;
	BOOL m_dlgvisible;
	HWND m_hDialog;
};

class pianoplay
{
public:
	// Constructor/destructor
	pianoplay();
	~pianoplay();

	// Initialisation
	BOOL Init();

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// General
	BOOL Show(BOOL show);

	// Implementation
	// 0 solo to compas actual
	// 1 toca desde compas actual hasta el final

	BOOL m_dlgvisible;
	HWND m_hDialog;
};

#endif 
