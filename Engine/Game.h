/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

/*User includes*/
#include <memory>
#include "Triangle.h"
#include "Vec3.h"
#include "Camera.h"
#include "ImageLoader.h"
#include "Model_Textured.h"
#include "Timer.h"
#include "InfiniteLight.h"
#include "Rasterizer.h"


class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */

	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	Wic wic;
	int m_triangleCount, m_groundTriCount, m_aikoTriCount;

	//std::unique_ptr<Triangle[]> m_pTriangles;
	//std::unique_ptr<Triangle[]> m_ground;
	//std::unique_ptr<Triangle[]> m_aiko;	
	Model_Textured m_bear;
	Model_Textured m_city;
	Model_Textured m_aiko;
	Model_Textured m_ground;

	AmbientLight m_ambLight;
	InfiniteLight m_infLight;

	Camera m_cam;
	comptr<IWICBitmap> m_pTexture;
	comptr<IWICBitmap> m_cityTex;
	comptr<IWICBitmap> m_grassTex;

	Rasterizer m_raster;
	LightBuffer m_lights;

	Tex2D			m_groundTex;
	SoaVertexBuffer m_groundBuffer;

	Tex2D			m_aikoTex;
	SoaVertexBuffer m_aikoBuffer;

	Timer t;
	/********************************/
};