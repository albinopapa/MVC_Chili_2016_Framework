//////////////////////////////////////////////////////////
// The input object will store the state of each key 
// in a keyboard array. When queried it will tell the 
// calling functions if a certain key is pressed.
//////////////////////////////////////////////////////////
#pragma once

#include "Includes.h"

// TODO: Make input more versatile, needs actual event handling
// Input needs have three events, one for button press, one for button
// release and one for when no events have been received.
class Input
{
public:
	Input();
	~Input();

	void Initialize( HWND WinHandle );

	// Relative data is only sent when a mouse event happens, 
	// FlushRelativeData resets the relative data back to 0.
	// Gets called once a frame before the message loop is 
	// processed.
	void FlushRelativeData();

	// Handles mouse events
	void OnMouseInput( const RAWMOUSE &RawMouseInput );

	// GetX gets mouse X position on screen
	int GetX() const;
	// GetY gets mouse Y position on screen
	int GetY()const;

	// Gets relative X position, not screen coordinates
	int GetRelativeX()const;
	
	// Gets relative Y position, not screen coordinates
	int GetRelativeY()const;

	void KeyDown( unsigned int );
	void KeyUp( unsigned int );

	bool IsKeyDown( unsigned int )const;

private:
	bool m_keys[ 256 ];

	bool m_leftDown, m_rightDown;
	int m_x, m_y;
	int m_relX, m_relY;
	RECT m_clamp;
};
