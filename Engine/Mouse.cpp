/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Mouse.cpp																			  *
 *	Copyright 2014 PlanetChili <http://www.planetchili.net>								  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "Mouse.h"


std::pair<int,int> Mouse::GetPos() const
{
	return { x,y };
}
bool Mouse::LeftIsPressed() const
{
	return leftIsPressed;
}
bool Mouse::RightIsPressed() const
{
	return rightIsPressed;
}
bool Mouse::IsInWindow() const
{
	return isInWindow;
}
Mouse::Event Mouse::ReadMouse()
{
	if( buffer.size() > 0u )
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event( Mouse::Event::Invalid,0,0 );
	}
}
bool Mouse::BufferIsEmpty() const
{
	return buffer.empty( );
}
void Mouse::ClearBuffer()
{
	std::swap( buffer,std::queue<Event>() );
}
void Mouse::OnMouseLeave()
{
	isInWindow = false;
}
void Mouse::OnMouseEnter()
{
	isInWindow = true;
}
void Mouse::OnMouseMove( int newx,int newy )
{
	x = newx;
	y = newy;

	buffer.push( Mouse::Event( Mouse::Event::Move,x,y ) );
	TrimBuffer();
}
void Mouse::OnLeftPressed( int x,int y )
{
	leftIsPressed = true;

	buffer.push( Mouse::Event( Mouse::Event::LPress,x,y ) );
	TrimBuffer();
}
void Mouse::OnLeftReleased( int x,int y )
{
	leftIsPressed = false;

	buffer.push( Mouse::Event( Mouse::Event::LRelease,x,y ) );
	TrimBuffer();
}
void Mouse::OnRightPressed( int x,int y )
{
	rightIsPressed = true;

	buffer.push( Mouse::Event( Mouse::Event::RPress,x,y ) );
	TrimBuffer();
}
void Mouse::OnRightReleased( int x,int y )
{
	rightIsPressed = false;

	buffer.push( Mouse::Event( Mouse::Event::RRelease,x,y ) );
	TrimBuffer();
}
void Mouse::OnWheelUp( int x,int y )
{
	buffer.push( Mouse::Event( Mouse::Event::WheelUp,x,y ) );
	TrimBuffer();
}
void Mouse::OnWheelDown( int x,int y )
{
	buffer.push( Mouse::Event( Mouse::Event::WheelDown,x,y ) );
	TrimBuffer();
}
void Mouse::TrimBuffer()
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}