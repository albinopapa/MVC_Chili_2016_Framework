/*******************************************************************************
*  MAIN ARCHITECTURE:                                                          *
*  1) The WinMain function required by Win32 g_pSystem is contained in Main.cpp*
*  2) The guts of the standard Windows instantiation code (MSG proc, etc),     *
*  are contained in separate "MainWindow.h/.cpp" files.                        *
*  3) The Game logic is contained in Game.h/cpp                                *
*  4) DirectX graphics functionality is contained in Graphics.h/cpp            *
*  5) Main.cpp includes both MainWindow.h and Game.h;                          *
*  Game takes a MainWindow instance in its constructor; this MainWindow        *
*  object will contain the handle to the main window and everything else       *
*  the game will need to initialize DirectX Device and access the main window  *
*  through the graphics class.                                                 *
*                                                                              *
*  This way, the GAME LOGIC, DX GRAPHICS LOGIC, and Win32 WINDOWS LOGIC,       *
*  are all encapsulated.                                                       *
*                                                                              *
********************************************************************************/
#pragma once


///////////////////////////
// Custom class includes //
///////////////////////////

#include "Input.h"
#include "Graphics.h"
#include "Game.h"

class System
{
public:
	System();
	System( const System& ) = delete; // never creates the copy constructor
	~System();

	bool Initialize();
	void Run();

	// forward declaration
	LRESULT CALLBACK MessageHandler( HWND, UINT, WPARAM, LPARAM );

private:
	void Quit();
	bool Frame();
	void InitializeWindows( int&, int& );

private:
	std::wstring m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	std::shared_ptr<Input> m_Input;
	std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<Game> m_pGame;
	bool done;
};

///////////////////////////////////////////////////////////////////////
// The WndProc function and ApplicationHandle pointer are also included 
// in this class file so we can re-direct the windows g_pSystem messaging 
// into our MessageHandler function inside the g_pSystem class.
///////////////////////////////////////////////////////////////////////

// FUNCTION PROTOTYPES //
static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

// GLOBALS //
static System* ApplicationHandle = 0;
