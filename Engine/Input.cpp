#include "Input.h"

//////////////////////////////////////////////////////////
// The input object will store the state of each key 
// in a keyboard array. When queried it will tell the 
// calling functions if a certain key is pressed.
//////////////////////////////////////////////////////////

Input::Input()
{
}

Input::~Input()
{
	// Raw input test
	// Unregister mouse as raw input device
	RAWINPUTDEVICE rawDevice{};
	rawDevice.usUsagePage = 1;
	rawDevice.usUsage = 2;
	rawDevice.dwFlags = RIDEV_REMOVE;
	
	// Setting hwndTarget to null unregisters the device
	rawDevice.hwndTarget = nullptr;			
											
	RegisterRawInputDevices( &rawDevice, 1, sizeof( RAWINPUTDEVICE ) );
}

void Input::Initialize( HWND WinHandle )
{
	// Raw input test
	// Register the mouse for raw input
	RAWINPUTDEVICE rawDevice{};
	rawDevice.usUsagePage = 1;
	rawDevice.usUsage = 2;
	rawDevice.dwFlags = RIDEV_NOLEGACY;
	rawDevice.hwndTarget = WinHandle;
	RegisterRawInputDevices( &rawDevice, 1, sizeof( RAWINPUTDEVICE ) );

	ZeroMemory( m_keys, 256 );

	
	GetWindowRect( WinHandle, &m_clamp );
	m_x = ( m_clamp.right - m_clamp.left ) / 2;
	m_y = ( m_clamp.bottom - m_clamp.top ) / 2;
	m_relX = 0;
	m_relY = 0;
	SetCursorPos( m_x, m_y );
}

void Input::FlushRelativeData()
{
	m_relX = 0;
	m_relY = 0;
}

void Input::OnMouseInput( const RAWMOUSE & RawMouseInput )
{
	// Store any relative mouse movements from last frame
	m_relX = RawMouseInput.lLastX;
	m_relY = RawMouseInput.lLastY;

	m_leftDown = RawMouseInput.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN;
	m_rightDown = ( ( RawMouseInput.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN ) >> 2 );

	// Clip the cursor to window boundaries
	ClipCursor( &m_clamp );

	// Get the mouse position
	POINT currentMousePos{};
	GetCursorPos( &currentMousePos );

	// Update Input's mouse position
	m_x = currentMousePos.x;
	m_y = currentMousePos.y;

	// Register the new position with Windows
	SetCursorPos( currentMousePos.x, currentMousePos.y );
}

int Input::GetX() const
{
	return m_x;
}

int Input::GetY() const
{
	return m_y;
}

int Input::GetRelativeX() const
{
	return m_relX;
}

int Input::GetRelativeY() const
{
	return m_relY;
}

void Input::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void Input::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}

// CODE_CHANGE: Made function const
bool Input::IsKeyDown(unsigned int key)const
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}