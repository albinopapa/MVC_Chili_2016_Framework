/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Game.cpp																			  *
 *	Copyright 2014 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "MainWindow.h"
#include "Game.h"
#include <chrono>
#include <fstream>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	//while( !wnd.mouse.BufferIsEmpty() )
	//{
	//	if( wnd.mouse.LeftIsPressed() )
	//	{
	//		const Mouse::Event e = wnd.mouse.ReadMouse();
	//		if( e.GetType() == Mouse::Event::Move || e.GetType() == Mouse::Event::LPress )
	//		{
	//			const auto pt = e.GetPos();
	//			m_x = pt.first + 1;
	//			m_y = pt.second + 1;
	//		}
	//	}
	//	else
	//	{
	//		wnd.mouse.ClearBuffer();
	//	}
	//}
	if( !wnd.IsMinimized() )
	{
		m_x++;
	}
	if( wnd.kbd.KeyIsPressed( VK_ESCAPE ) )
	{
		wnd.Kill();
	}
}

void Game::ComposeFrame()
{
	using namespace std::chrono;
	std::wofstream log( L"alog.txt" );

	const auto start = high_resolution_clock::now();
	for( int y = m_y; y < m_y + 50; y++ )
	{
		for( int x = m_x; x < m_x + 50; x++ )
		{
			gfx.PutPixel( x,y,Colors::Red );
		}
	}
	const auto end = high_resolution_clock::now();
	log << L"Operation took "
		<< duration_cast<microseconds>( end - start ).count() 
		<< L" microseconds." << std::endl;
}
