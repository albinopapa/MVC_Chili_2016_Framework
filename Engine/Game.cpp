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
#include "PrimitiveFactory.h"
#include <sstream>


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	m_ambLight( .1f, .1f, .1f )
{
	// Load Models
	{
		PrimitiveFactory prim;

		{
			prim.CreateColor( 1.f, 1.f, 1.f, 1.f );
			prim.CreateMesh( L"Meshes/cyborg_aiko.BinaryMesh" );
			m_aiko.Initialize( prim );
			m_aiko.SetScale( { 4.f, 4.f, 4.f } );

			m_raster.CreateVertexBuffer( m_aiko.GetVertexBuffer(), m_aiko.GetVertexCount(), &m_aikoBuffer );
		}
		{
			prim.CreateMesh( L"Meshes/city.BinaryMesh" );			
			m_city.Initialize( prim );
		}
		{
			ModelSpecs_L lSpecs;
			lSpecs.center = { 0.f, 0.f, 0.f };
			lSpecs.orientation = { 90.f, 0.f, 0.f };
			lSpecs.size = { 10.f, 10.f, .1f };
			prim.CreatePlane( lSpecs );
			m_ground.Initialize( prim );
			m_raster.CreateVertexBuffer( m_ground.GetVertexBuffer(), m_ground.GetVertexCount(), &m_groundBuffer );
		}
	}

	// Initialize camera
	{
		m_cam.Initialize( 
		{ 0.f, 7.f, 0.f }, 
		{ 90.f, 0.f, 0.f }, 
		{ Graphics::ScreenWidth, Graphics::ScreenHeight }, 
		{ 1.f, 1000.f } );
	}

	wnd.mouse.FlushRelativeData();

	// Load texture
	{
		wic.Initialize();

		auto textureResult = ImageLoader::CreateBitmap( L"Images/diffAiko.png", wic );
		assert( SUCCEEDED( textureResult.first ) );
		m_pTexture = std::move( textureResult.second );
		m_raster.CreateTex2D( m_pTexture.Get(), &m_aikoTex );

		textureResult = ImageLoader::CreateBitmap( L"Images/city.png", wic );
		assert( SUCCEEDED( textureResult.first ) );
		m_cityTex = std::move( textureResult.second );

		textureResult = ImageLoader::CreateBitmap( L"Images/grass.png", wic );
		assert( SUCCEEDED( textureResult.first ) );
		m_grassTex = std::move( textureResult.second );
		m_raster.CreateTex2D( m_grassTex.Get(), &m_groundTex );
	}

	// Set lights
	{
		m_infLight.SetColor( 1.f, 1.f, 1.f );
		m_infLight.SetDirection( 0.f, -1.f, 0.f );
	}
}

void Game::Go()
{
	UpdateModel();

	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();

}

void Game::UpdateModel()
{
	m_cam.Update( wnd.mouse, wnd.kbd );

	m_lights.ambient = m_ambLight.GetColor();
	m_lights.diffuse = m_infLight.GetColor();
	m_lights.direction = m_infLight.GetDirection();

	//m_aiko.Update( .17f );

	// After all objects are done using raw mouse data flush relative data.
	wnd.mouse.FlushRelativeData();
}

void Game::ComposeFrame()
{
	t.Start();
	Transforms trans{};
	trans.view = m_cam.GetViewMatrix();
	trans.proj = m_cam.GetProjectionMatrix();
	
	m_raster.SetLightBuffer( m_lights );

	// Render aiko model
	{
		/*m_raster.SetTexture( m_aikoTex );
		trans.world = m_aiko.GetWorldMatrix();
		m_raster.SetTransforms( trans );
		m_raster.SetVertexBuffer( m_aikoBuffer );
		m_raster.Rasterize( gfx );*/
		auto view = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &trans.view ) );
		auto proj = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &trans.proj ) );
		m_aiko.Draw( view, proj, m_pTexture.Get(), &m_ambLight, &m_infLight, gfx );
	}

	// Render city
	{
		//m_raster.SetTexture( m_cityTex.Get() );
		//m_city.Draw( view, proj, m_cityTex.Get(), &m_ambLight, &m_infLight, gfx );
	}
	
	// Render ground plane
	{
		/*m_raster.SetTexture( m_groundTex );
		trans.world = m_ground.GetWorldMatrix();
		m_raster.SetTransforms( trans );
		m_raster.SetVertexBuffer( m_groundBuffer );
		m_raster.Rasterize( gfx );*/
		auto view = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &trans.view ) );
		auto proj = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &trans.proj ) );
		m_ground.Draw( view, proj, m_grassTex.Get(), &m_ambLight, &m_infLight, gfx );
	}
	t.Stop();
	auto milli = t.GetMilli();

	std::wstringstream ws;
	ws << L"Frames per second: " << ( 1000.f / milli );
	wnd.SetText( ws.str() );

}
