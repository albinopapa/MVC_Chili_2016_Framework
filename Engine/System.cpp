/*******************************************************************************
*  MAIN ARCHITECTURE:                                                          *
*  1) The WinMain function required by Win32 g_pSystem is contained in Main.cpp   *
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
#include "System.h"

System::System()
	:
	m_hInstance( GetModuleHandle( NULL ) ),
	m_applicationName( L"DX11_Framework" ),
	done(false)
{    
}

System::~System() 
{
	// Show the mouse cursor.
	ShowCursor( true );

	// Return screen settings back to normal and release the 
	// window and the handles associated with it.
	// Fix the display settings if leaving full screen mode.
	if( g_fullScreen )
	{
		ChangeDisplaySettings( NULL, 0 );
	}

	// Remove the window.
	DestroyWindow( m_hwnd );
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass( m_applicationName.c_str(), m_hInstance );
	m_hInstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

}

bool System::Initialize()
{
    // initialize screenwidth / height to 0; will be later modified w variables
    int screenWidth = 0;
	int screenHeight = 0;

    // Initialize the windows api.
	InitializeWindows( screenWidth, screenHeight );

    // Create input object for keyboard input.
	m_Input.reset( new Input );
	bool result = m_Input != nullptr;
	RETURN_IF_FALSE( result );

    // Initialize the input object.
    m_Input->Initialize(m_hwnd);

    // Create graphics object to handle all rendering.
	m_Graphics.reset( new Graphics );
	result = m_Graphics != nullptr;
	RETURN_IF_FALSE( result );

	// Initialize the graphics object.
	result = m_Graphics->Initialize(
		screenWidth, screenHeight,					// Screen dimensions
		m_hwnd,										// Window handle
		0.f, 0.f, 0.f, 0.f							// Background color (R,G,B,A)
	);
	RETURN_IF_FALSE( result );

	// Create game object to handle assets and game logic
	m_pGame.reset( new Game(m_Input));
	result = m_pGame != nullptr;
	RETURN_IF_FALSE( result );

	// Initialize the game object
	result = m_pGame->Initialize( 
        m_Graphics.get(), screenWidth, screenHeight);
	RETURN_IF_FALSE( result );
	return true;
}

//////////////////////////////////////
// MAIN APPLICATION LOOP            //
// Continues processing until quit. //
//////////////////////////////////////

void System::Run() 
{
	// Initialize the message structure.
	MSG msg{};

	// Loop until there is a quit message from the window or the user.
	while( !done )
	{
		// Input relative members should be reset before processing
		// the message queue
		m_Input->FlushRelativeData();

		// Handle the windows messages.
		while( PeekMessage( &msg, nullptr, NULL, NULL, PM_REMOVE ) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if( !Frame() )
		{
			Quit();
		}
	}
}

void System::Quit()
{
	DestroyWindow( m_hwnd );
	done = true;
}

//////////////////////////////////
// THIS is where  the frame     //
// processing is done:          //
//////////////////////////////////

bool System::Frame()
{
	// Check if user pressed esc and wants to exit.
	RETURN_IF_FALSE( !m_Input->IsKeyDown( VK_ESCAPE ) );

	// Do the frame processing for the game object.
	RETURN_IF_FALSE( m_pGame->Frame() );

	return true;
}

/////////////////////////////////////////////////////////
// This message handler function FUNNELS the Windows
// messages into the application.
/////////////////////////////////////////////////////////

LRESULT CALLBACK System::MessageHandler(
    HWND hwnd, 
    UINT umsg, 
    WPARAM wparam, 
    LPARAM lparam)
{
	switch(umsg)
	{
		// Check if the window is being closed.
		case WM_CLOSE:
		{
			DestroyWindow( m_hwnd );
			return 0;
		}

		// Check if the window is being destroyed.
		case WM_DESTROY:
		{			
			PostQuitMessage( 0 );
			return 0;
		}

		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown( static_cast<unsigned int>( wparam ) );
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp( static_cast<unsigned int>( wparam ) );
			return 0;
		}
		case WM_INPUT:
		{
			bool discardData = GET_RAWINPUT_CODE_WPARAM( wparam ) & RIM_INPUTSINK;

			if( !discardData )
			{
				RAWINPUT rInput{};
				UINT dataSize = sizeof( RAWINPUT );

				auto result = GetRawInputData(
					reinterpret_cast<HRAWINPUT>( lparam ),
					RID_INPUT,
					&rInput,
					&dataSize,
					sizeof( RAWINPUTHEADER ) );
				m_Input->OnMouseInput( rInput.data.mouse );
			}
			else
			{
				// If data was sent while window not in foreground
				// we'll just let windows handle it
				return DefWindowProc( hwnd, umsg, wparam, lparam );
			}
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//////////////////////////////////////////////////////////////
// Here, we build the actual window that we will render to. //
//////////////////////////////////////////////////////////////

void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
    // Pass this object to external pointer so it will persist outside of scope.
    ApplicationHandle = this;

    // Setup window class with default settings
	WNDCLASSEX wcex{};
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc; // pass global g_result of function from header
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wcex.hIconSm       = wcex.hIcon;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_applicationName.c_str();
    wcex.cbSize = sizeof(WNDCLASSEX);

    // register the window class
    RegisterClassEx(&wcex);

    // Get client's desktop resolution
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0, posY = 0;
	// Setup screen settings depending on whether it is running in full screen or windowed.
	if( g_fullScreen )
    {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings{};
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (DWORD)screenWidth;
        dmScreenSettings.dmPelsHeight = (DWORD)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    }
    else
    {
        // If windowed then set it to 800x600 resolution.
        screenWidth = 800;
        screenHeight = 600;

        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName.c_str(), m_applicationName.c_str(),
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // Hide the mouse cursor.
    ShowCursor(false);
}

///////////////////////////////////////////////////////////////
// This is where Windows sends it messages to. This function 
// is tied directly into the MessageHandler function.
///////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	return ApplicationHandle->MessageHandler( hwnd, umessage, wparam, lparam );
}