/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "Mat3.h"
#include <random>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	m_pTriangles( new Triangle[ m_triangleCount ] )
{
	const float sqSize = 15.f;
	const float spacing = sqSize * 2.f;
	float xStart = sqSize;
	float yStart = sqSize;
	

	for( int y = 0; y < m_triCountHeight; ++y )
	{
		const float fY = static_cast<float>( y );
		int count = 0;
		const int rowOffset = ( y * m_triCountWidth );
		const float yPos = yStart + (spacing * fY);

		for( int x = 0; x < m_triCountWidth; x += 2, ++count )
		{
			float fCount = static_cast<float>( count );

			const float step = spacing * fCount;
			const float xPos = xStart + step;

			const float left =	 xPos - sqSize;
			const float top =	 yPos - sqSize;
			const float right =	 xPos + sqSize;
			const float bottom = yPos + sqSize;

			Vec2 a = { right, top };
			Vec2 b = { left, bottom };
			Vec2 c = { left, top };

			const int idx = x + rowOffset;
			m_pTriangles[ idx ].a = a;
			m_pTriangles[ idx ].b = b;
			m_pTriangles[ idx ].c = c;
			m_pTriangles[ idx ].color = Colors::Blue;
		}

		count = 0;
		for( int x = 1; x < m_triCountWidth; x += 2, ++count )
		{
			float fCount = static_cast<float>( count );

			const float step = spacing * fCount;
			const float xPos = xStart + step;

			const float left =	 xPos - sqSize;
			const float top =	 yPos - sqSize;
			const float right =  xPos + sqSize;
			const float bottom = yPos + sqSize;

			Vec2 a = { left, bottom };
			Vec2 b = { right, top };
			Vec2 c = { right, bottom };

			const int idx = x + rowOffset;
			m_pTriangles[ idx ].a = a;
			m_pTriangles[ idx ].b = b;
			m_pTriangles[ idx ].c = c;
			m_pTriangles[ idx ].color = Colors::Blue;
		}
	}
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
}

void Game::ComposeFrame()
{
	for( int i = 0; i < m_triangleCount; ++i )
	{
		auto &triangle = m_pTriangles[ i ];
		gfx.DrawTriangle( triangle, triangle.color );
	}
}
