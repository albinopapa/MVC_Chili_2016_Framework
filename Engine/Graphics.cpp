/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
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
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>
#include <random>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );
	
	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL	featureLevelsRequested = D3D_FEATURE_LEVEL_9_1;
	UINT				numLevelsRequested = 1;
	D3D_FEATURE_LEVEL	featureLevelsSupported;
	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		&featureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		&featureLevelsSupported,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	unsigned bufferSize =
		sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight;
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( bufferSize, 16u ) );
	pZBuffer = reinterpret_cast<float*>(
		_aligned_malloc( bufferSize, 16u ) );
}

Vec4 Graphics::BilinearSample( const Vec2 & UV, unsigned * pImage, int ImageWidth, int ImageHeight )
{
	float colorToFloat = 1.f / 255.f;

	float ftx = UV.x * static_cast<float>( ImageWidth  );
	float fty = UV.y * static_cast<float>( ImageHeight );

	int tx = static_cast<int>( floorf( ftx ) );
	int ty = static_cast<int>( floorf( fty ) );

	int fracU = static_cast<int>( ( ftx - floorf( ftx ) ) * 255.f );
	int fracV = static_cast<int>( ( fty - floorf( fty ) ) * 255.f );

	int invFracU = 255 - fracU;
	int invFracV = 255 - fracV;

	int ul = ( ( invFracU * invFracV ) >> 8 ) + 1;
	int ur = ( ( fracU * invFracV ) >> 8 ) + 1;
	int ll = ( ( invFracU * fracV ) >> 8 ) + 1;
	int lr = ( ( fracU * fracV ) >> 8 ) + 1;

	int rowOffset0 = ty * ImageWidth;
	int rowOffset1 = std::min( ty + 1, ImageHeight ) * ImageHeight;
	int colOffset = std::min( tx + 1, ImageWidth );

	int texIdx0 = tx + rowOffset0;
	int texIdx1 = colOffset + rowOffset0;
	int texIdx2 = tx + rowOffset1;
	int texIdx3 = colOffset + rowOffset1;

	int sr0 = Color( pImage[ texIdx0 ] ).GetR() * ul;
	int sr1 = Color( pImage[ texIdx1 ] ).GetR() * ur;
	int sr2 = Color( pImage[ texIdx2 ] ).GetR() * ll;
	int sr3 = Color( pImage[ texIdx3 ] ).GetR() * lr;

	int sg0 = Color( pImage[ texIdx0 ] ).GetG() * ul;
	int sg1 = Color( pImage[ texIdx1 ] ).GetG() * ur;
	int sg2 = Color( pImage[ texIdx2 ] ).GetG() * ll;
	int sg3 = Color( pImage[ texIdx3 ] ).GetG() * lr;

	int sb0 = Color( pImage[ texIdx0 ] ).GetB() * ul;
	int sb1 = Color( pImage[ texIdx1 ] ).GetB() * ur;
	int sb2 = Color( pImage[ texIdx2 ] ).GetB() * ll;
	int sb3 = Color( pImage[ texIdx3 ] ).GetB() * lr;

	int r = ( ( sr0 + sr1 + sr2 + sr3 ) >> 8 );
	int g = ( ( sg0 + sg1 + sg2 + sg3 ) >> 8 );
	int b = ( ( sb0 + sb1 + sb2 + sb3 ) >> 8 );

	return{
		static_cast<float>( r ) * colorToFloat,
		static_cast<float>( g ) * colorToFloat,
		static_cast<float>( b ) * colorToFloat
	};
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	if( pZBuffer )
	{
		_aligned_free( pZBuffer );
		pZBuffer = nullptr;
	}

	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

VertexBufferTypeAllInOne Graphics::TransformVertex( const VertexBufferTypeAllInOne &Vertex, 
	const DirectX::XMMATRIX & Trans )
{
	static const XMVECTOR mScrnOffset = DirectX::XMVectorSet(
		static_cast<float>( ScreenWidth  >> 1),
		static_cast<float>( ScreenHeight >> 1 ),
		0.f, 0.f
	);
	static const XMVECTOR mScrnScale = DirectX::XMVectorSet(
		static_cast<float>( ScreenWidth >> 1 ),
		static_cast<float>( -ScreenHeight >> 1 ),
		0.f, 0.f
	);

	VertexBufferTypeAllInOne result;
	auto mPosition = XMLoadFloat3( &Vertex.position );
	mPosition = DirectX::XMVector3TransformCoord( mPosition, Trans );
	mPosition = _mm_mul_ps( mPosition, mScrnScale );
	mPosition = _mm_add_ps( mPosition, mScrnOffset );

	XMStoreFloat3( &result.position, mPosition );
	result.uv = Vertex.uv;
	result.normal = Vertex.normal;
	result.color = Vertex.color;

	return result;
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer and z-buffer
	int screenArea = Graphics::ScreenHeight * Graphics::ScreenWidth;
	int bufferSize = sizeof( Color ) * screenArea;
	memset( pSysBuffer, 0u, bufferSize );
	for( int i = 0; i < screenArea; ++i )
	{
		pZBuffer[ i ] = 1.f;
	}
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

void Graphics::PutPixel( int X, int Y, float Z, Color C )
{
	int idx = X + ( Y * ScreenWidth );
	pZBuffer[ idx ] = Z;
	pSysBuffer[ idx ] = C;
}

float Graphics::GetZ( int X, int Y ) const
{
	return pZBuffer[ X + ( Y * ScreenWidth ) ];
}

void Graphics::DrawTriangle( const Triangle & T, const DirectX::XMMATRIX & Trans )
{
	Vec3 a( T.a );
	Vec3 b( T.b );
	Vec3 c( T.c );

	XMVECTOR mA = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &a ) );
	XMVECTOR mB = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &b ) );
	XMVECTOR mC = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &c ) );

	// Transform the vertices of the triangle
	mA = XMVector3TransformCoord( mA, Trans );
	mB = XMVector3TransformCoord( mB, Trans );
	mC = XMVector3TransformCoord( mC, Trans );

	// Store the results
	XMStoreFloat3( reinterpret_cast<XMFLOAT3 *>( &a ), mA );
	XMStoreFloat3( reinterpret_cast<XMFLOAT3 *>( &b ), mB );
	XMStoreFloat3( reinterpret_cast<XMFLOAT3 *>( &c ), mC );
	
	// Clip depth
	if( a.z < 0.f || a.z > 1.f || b.z < 0.f || b.z > 1.f || c.z < 0.f || c.z > 1.f )
	{
		return;
	}

	// Setup screen positioning
	Vec2 scrnSize( 
		static_cast<float>( ScreenWidth ) * .5f,
		static_cast<float>( ScreenHeight ) * .5f 
	);

	// scale and translate to screen space
	Vec3 sA( ( Vec2( a.x * scrnSize.x, a.y * -scrnSize.y ) ) + scrnSize );
	Vec3 sB( ( Vec2( b.x * scrnSize.x, b.y * -scrnSize.y ) ) + scrnSize );
	Vec3 sC( ( Vec2( c.x * scrnSize.x, c.y * -scrnSize.y ) ) + scrnSize );

	// Calculate bouding box and clip to screen bounding box
	const int xStart = std::max( std::min( sA.x, std::min( sB.x, sC.x ) ), 0.f );
	const int yStart = std::max( std::min( sA.y, std::min( sB.y, sC.y ) ), 0.f );
	const int xEnd = std::min( std::max( sA.x, std::max( sB.x, sC.x ) ), static_cast<float>( ScreenWidth ) );
	const int yEnd = std::min( std::max( sA.y, std::max( sB.y, sC.y ) ), static_cast<float>( ScreenHeight ) );

	// Precalculate edges
	auto ba( sB - sA );
	auto ca( sC - sA );
	
	// Precalculate inverse area
	float d00 = ba.DotProduct( ba );
	float d01 = ba.DotProduct( ca );
	float d11 = ca.DotProduct( ca );
	float invDenom = 1.0 / ( d00 * d11 - d01 * d01 );

	BarycentricCoord baryCoordinates{};
	for( int y = yStart; y < yEnd; ++y )
	{
		for( int x = xStart; x < xEnd; ++x )
		{
			Vec3 p( x, y, 1.f );

			// Calculate vector PA
			auto pa( p - sA );

			// Calculate vector
			float d20 = pa.DotProduct( ba );
			float d21 = pa.DotProduct( ca );

			// A
			baryCoordinates.x = ( d11 * d20 - d01 * d21 ) * invDenom;
			// B
			baryCoordinates.y = ( d00 * d21 - d01 * d20 ) * invDenom;
			// C
			baryCoordinates.z = 1.0f - baryCoordinates.x - baryCoordinates.y;

			// If barycentric, fill pixel
			if( baryCoordinates.x >= 0.f && baryCoordinates.y >= 0.f && baryCoordinates.z >= 0.f )
			{				
				float z = 
					( a.z * baryCoordinates.x ) + 
					( b.z * baryCoordinates.y ) + 
					( c.z * baryCoordinates.z );				
				
				unsigned char rScale = static_cast<unsigned char>( baryCoordinates.x * 255.f );
				unsigned char gScale = static_cast<unsigned char>( baryCoordinates.y * 255.f );
				unsigned char bScale = static_cast<unsigned char>( baryCoordinates.z * 255.f );

				unsigned char blue = ( T.color.GetB() * bScale ) >> 8u;
				unsigned char green = ( T.color.GetG() * gScale ) >> 8u;
				unsigned char red = ( T.color.GetR() * rScale ) >> 8u;

				int index = x + ( y * Graphics::ScreenWidth );
				if( pZBuffer[ index ] > z )
				{
					pZBuffer[ index ] = z;
					PutPixel( x, y, { red, green, blue } );
				}
			}
		}
	}
}

void Graphics::DrawTriangle( const Triangle & T, IWICBitmap * const pImage, 
	const DirectX::XMMATRIX & Trans, InfiniteLight *const pInfLight,
	AmbientLight *pAmbLight )
{
	Vec3 a( T.a );
	Vec3 b( T.b );
	Vec3 c( T.c );

	// Transform to projection 
	{
		XMVECTOR mA = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &a ) );
		XMVECTOR mB = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &b ) );
		XMVECTOR mC = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &c ) );

		// Transform the vertices of the triangle
		mA = XMVector3TransformCoord( mA, Trans );
		mB = XMVector3TransformCoord( mB, Trans );
		mC = XMVector3TransformCoord( mC, Trans );

		// Store the results
		XMStoreFloat3( reinterpret_cast<XMFLOAT3 *>( &a ), mA );
		XMStoreFloat3( reinterpret_cast<XMFLOAT3 *>( &b ), mB );
		XMStoreFloat3( reinterpret_cast<XMFLOAT3 *>( &c ), mC );
	}

	// Clip depth
	if( a.z < 0.f || a.z > 1.f || b.z < 0.f || b.z > 1.f || c.z < 0.f || c.z > 1.f )
	{
		//auto cpResult = ( b - a ).CrossProduct( c - a );
		return;
	}

	// Setup screen positioning
	Vec2 scrnSize(
		static_cast<float>( ScreenWidth ) * .5f,
		static_cast<float>( ScreenHeight ) * .5f
	);

	// scale and translate to screen space
	Vec3 sA( ( Vec2( a.x * scrnSize.x, a.y * -scrnSize.y ) ) + scrnSize );
	Vec3 sB( ( Vec2( b.x * scrnSize.x, b.y * -scrnSize.y ) ) + scrnSize );
	Vec3 sC( ( Vec2( c.x * scrnSize.x, c.y * -scrnSize.y ) ) + scrnSize );
	
	// Calculate bouding box and clip to screen bounding box
	/*float fxStart = std::min( sA.x, std::min( sB.x, sC.x ) );
	float fyStart = std::min( sA.y, std::min( sB.y, sC.y ) );
	float fxEnd   = std::max( sA.x, std::max( sB.x, sC.x ) );
	float fyEnd   = std::max( sA.y, std::max( sB.y, sC.y ) );

	int xStart = static_cast<int>( floorf( fxStart ) );
	int yStart = static_cast<int>( floorf( fyStart ) );
	int xEnd = static_cast<int>( ceilf( fxEnd ) );
	int yEnd = static_cast<int>( ceilf( fyEnd ) );

	xStart = std::max( xStart, 0 );
	yStart = std::max( yStart, 0 );
	xEnd = std::min( xEnd, static_cast<int>( Graphics::ScreenWidth  - 1) );
	yEnd = std::min( yEnd, static_cast<int>( Graphics::ScreenHeight - 1 ));*/

	// Precalculate edges
	auto ba( sB - sA );
	auto ca( sC - sA );

	// Calculate perpendicular vector to triangle
	auto perpV = ba.CrossProduct( ca );
	// Calculate normalized perpV
	perpV = perpV.Normalize();
	if( perpV.z < 0.f )
	{
		return;
	}

	// Precalculate inverse area
	float d00 = ba.DotProduct( ba );
	float d01 = ba.DotProduct( ca );
	float d11 = ca.DotProduct( ca );
	float invDenom = 1.0 / ( d00 * d11 - d01 * d01 );

	unsigned width = 0;
	unsigned height = 0;
	pImage->GetSize( &width, &height );
	ComPtr<IWICBitmapLock> pLock;
	unsigned *pTexture = nullptr;
	float  texWidth = static_cast<float>(  width -1 );
	float texHeight = static_cast<float>( height -1 );
	float invColorMax = 1.f / 255.f;
	
	auto ambient = pAmbLight->GetColor();

	// Get texture data from IWICBitmap
	{
		unsigned buffersize = width * height * sizeof( unsigned );
		unsigned char *pUCharTexture = nullptr;

		WICRect wr{};
		wr.Width = static_cast<int>( width );
		wr.Height = static_cast<int>( height );
		pImage->Lock( &wr, WICBitmapLockRead, &pLock );
		pLock->GetDataPointer( &buffersize, &pUCharTexture );

		pTexture = reinterpret_cast<unsigned*>( pUCharTexture );
	}	

	auto RasterTriangle = [ & ]( const Vec3 &P, int X, int Y )
	{
		// Calculate vector PA
		auto pa( P - sA );

		// Calculate vector
		float d20 = pa.DotProduct( ba );
		float d21 = pa.DotProduct( ca );

		BarycentricCoord baryCoords{};
		// C
		baryCoords.x = ( d11 * d20 - d01 * d21 ) * invDenom;
		// A
		baryCoords.y = ( d00 * d21 - d01 * d20 ) * invDenom;
		// B
		baryCoords.z = 1.0f - baryCoords.x - baryCoords.y;

		// If barycentric, fill pixel
		if( baryCoords.x >= 0.f && baryCoords.y >= 0.f && baryCoords.z >= 0.f )
		{
			float z =
				( a.z * baryCoords.z ) +
				( b.z * baryCoords.x ) +
				( c.z * baryCoords.y );

			int index = X + ( Y * Graphics::ScreenWidth );
			if( pZBuffer[ index ] > z )
			{
				pZBuffer[ index ] = z;

				Vec2 ta, tb, tc;
				ta.x = T.ta.x < 0.f ? 1.f + T.ta.x : T.ta.x > 1.f ? 1.f - T.ta.x : T.ta.x;
				ta.y = T.ta.y < 0.f ? 1.f + T.ta.y : T.ta.y > 1.f ? 1.f - T.ta.y : T.ta.y;
				tb.x = T.tb.x < 0.f ? 1.f + T.tb.x : T.tb.x > 1.f ? 1.f - T.tb.x : T.tb.x;
				tb.y = T.tb.y < 0.f ? 1.f + T.tb.y : T.tb.y > 1.f ? 1.f - T.tb.y : T.tb.y;
				tc.x = T.tc.x < 0.f ? 1.f + T.tc.x : T.tc.x > 1.f ? 1.f - T.tc.x : T.tc.x;
				tc.y = T.tc.y < 0.f ? 1.f + T.tc.y : T.tc.y > 1.f ? 1.f - T.tc.y : T.tc.y;

				Vec2 texcoords =
					( ta * baryCoords.z ) +
					( tb * baryCoords.x ) +
					( tc * baryCoords.y );

				Vec4 fTexColor(
					BilinearSample(
						texcoords, pTexture,
						static_cast<int>( width - 1 ),
						static_cast<int>( height - 1 )
					)
				);

				Vec4 ambientColor(
					fTexColor.x * ambient.x,
					fTexColor.y * ambient.y,
					fTexColor.z * ambient.z
				);

				Vec3 normal =
					( T.na * baryCoords.z ) +
					( T.nb * baryCoords.x ) +
					( T.nc * baryCoords.y );

				auto lightDir = -pInfLight->GetDirection();
				float intensity = lightDir.DotProduct( normal );
				auto lightColor = ( pInfLight->GetColor() * intensity );

				Vec4 diffuseColor(
					lightColor.x * fTexColor.x,
					lightColor.y * fTexColor.y,
					lightColor.z * fTexColor.z
				);

				Vec4 fFinalColor = Saturate( diffuseColor + ambientColor );

				Color finalColor =
				{
					static_cast<unsigned char>( fFinalColor.x * 255.f ),
					static_cast<unsigned char>( fFinalColor.y * 255.f ),
					static_cast<unsigned char>( fFinalColor.z * 255.f )
				};

				pSysBuffer[ index ] = finalColor;
			}
		}
	};

	Vec2 top( sA.x, sA.y ),
		middle( sB.x, sB.y ),
		bottom( sC.x, sC.y );
	if( top.y > middle.y )
	{
		std::swap( top, middle );
	}
	if( middle.y > bottom.y )
	{
		std::swap( middle, bottom );
	}
	if( top.y > middle.y )
	{
		std::swap( top, middle );
	}

	float slope0 = ( ( middle - top ) / ( ( middle.y == top.y ) ? 1.f : ( middle.y - top.y ) ) ).x;
	float slope1 = ( ( bottom - top ) / ( ( bottom.y == top.y ) ? 1.f : ( bottom.y - top.y ) ) ).x;
	float slope2 = ( ( bottom - middle ) / ( ( bottom.y == middle.y ) ? 1.f : ( bottom.y - middle.y ) ) ).x;

	int yStart0 = static_cast<int>( std::max( 0.f, ceilf( top.y ) ) );
	int yEnd0 = static_cast<int>( std::min<float>( Graphics::ScreenHeight - 1, ceilf( middle.y ) ) );

	for( int y = yStart0; y <= yEnd0; ++y )
	{		
		int xStart = static_cast<int>( 
			std::max( 
				top.x + ( ( y - yStart0 ) * slope0 ), 0.f ) );
		int xEnd = static_cast<int>(
			std::min<float>(
				static_cast<float>( Graphics::ScreenWidth - 1 ),
				top.x + ( static_cast<float>( ( y - yStart0 ) ) * slope1 )
				)
			);

		if( xStart > xEnd )
		{
			std::swap( xStart, xEnd );
		}
		for( int x = xStart; x <= xEnd; ++x )
		{
			Vec3 p( 
				static_cast<float>(x) + .5f, 
				static_cast<float>(y) + .5f, 1.f );

			RasterTriangle( p, x, y );
		}
	}

	int yStart1 = static_cast<int>( std::max( 0.f, ceilf( middle.y ) ) );
	int yEnd1 = static_cast<int>( std::min<float>( Graphics::ScreenHeight - 1, ceilf( bottom.y ) ) );

	for( int y = yStart1; y <= yEnd1; ++y )
	{
		int xStart = static_cast<int>( std::max<float>( 0.f, middle.x + ( ( y - yStart1 ) * slope2 ) ) );
		int xEnd = static_cast<int>( std::min<float>( Graphics::ScreenWidth - 1, top.x + ( ( y - yStart1 )* slope1 ) ) );
		if( xStart > xEnd )
		{
			std::swap( xStart, xEnd );
		}
		for( int x = xStart; x <= xEnd; ++x )
		{
			Vec3 p(
				static_cast<float>( x ) + .5f,
				static_cast<float>( y ) + .5f, 1.f );

			RasterTriangle( p, x, y );
		}
	}
}

void Graphics::DrawMesh( unsigned VertexCount, const SoaVertexBuffer &pVertices,
	IWICBitmap *const pImage, const DirectX::XMMATRIX &Trans,
	InfiniteLight *const pInfLight, AmbientLight *pAmbLight )
{
	
}

void Graphics::Rasterize( unsigned VertexCount, 
	const VertexBufferTypeAllInOne * pVertices, IWICBitmap * const pImage, 
	const DirectX::XMMATRIX& Trans )
{
	const unsigned triCount = VertexCount / 3;
	XMFLOAT4X4 matTrans{};
	XMStoreFloat4x4( &matTrans, Trans );

	// Setup screen positioning
	Vec2 scrnSize(
		static_cast<float>( ScreenWidth ) * .5f,
		static_cast<float>( ScreenHeight ) * .5f
	);

	float w =
		matTrans._41 +
		matTrans._42 +
		matTrans._43 +
		matTrans._44;

	for( int y = 0; y < ScreenHeight; ++y )
	{
		for( int x = 0; x < ScreenWidth; ++x )
		{
			for( int i = 0; i < triCount; ++i )
			{
				const int vOffset = i * 3;
				const int j = vOffset + 0;
				const int k = vOffset + 1;
				const int l = vOffset + 2;

				// Transform vertices to projection space
				Vec3 a, b, c;				
				{	
					a.x =
						( matTrans._11 * pVertices[ j ].position.x ) +
						( matTrans._12 * pVertices[ j ].position.x ) +
						( matTrans._13 * pVertices[ j ].position.x ) +
						( matTrans._14 * pVertices[ j ].position.x );
					a.y =
						( matTrans._21 * pVertices[ j ].position.y ) +
						( matTrans._22 * pVertices[ j ].position.y ) +
						( matTrans._23 * pVertices[ j ].position.y ) +
						( matTrans._24 * pVertices[ j ].position.y );
					a.z =
						( matTrans._31 * pVertices[ j ].position.z ) +
						( matTrans._32 * pVertices[ j ].position.z ) +
						( matTrans._33 * pVertices[ j ].position.z ) +
						( matTrans._34 * pVertices[ j ].position.z );

					b.x =
						matTrans._11 * pVertices[ k ].position.x +
						matTrans._12 * pVertices[ k ].position.x +
						matTrans._13 * pVertices[ k ].position.x +
						matTrans._14 * pVertices[ k ].position.x;
					b.y =
						matTrans._21 * pVertices[ k ].position.y +
						matTrans._22 * pVertices[ k ].position.y +
						matTrans._23 * pVertices[ k ].position.y +
						matTrans._24 * pVertices[ k ].position.y;
					b.z =
						matTrans._31 * pVertices[ k ].position.z +
						matTrans._32 * pVertices[ k ].position.z +
						matTrans._33 * pVertices[ k ].position.z +
						matTrans._34 * pVertices[ k ].position.z;
					
					c.x =
						matTrans._11 * pVertices[ l ].position.x +
						matTrans._12 * pVertices[ l ].position.x +
						matTrans._13 * pVertices[ l ].position.x +
						matTrans._14 * pVertices[ l ].position.x;
					c.y =
						matTrans._21 * pVertices[ l ].position.y +
						matTrans._22 * pVertices[ l ].position.y +
						matTrans._23 * pVertices[ l ].position.y +
						matTrans._24 * pVertices[ l ].position.y;
					c.z =
						matTrans._31 * pVertices[ l ].position.z +
						matTrans._32 * pVertices[ l ].position.z +
						matTrans._33 * pVertices[ l ].position.z +
						matTrans._34 * pVertices[ l ].position.z;
					
					a = a / w;
					b = b / w;
					c = c / w;
				}

				// Discard triangle if doesn't pass depth test
				{
					if( a.z < 0.f || a.z > 1.f ||
						b.z < 0.f || b.z > 1.f ||
						c.z < 0.f || c.z > 1.f )
					{
						return;
					}
				}

				// scale and translate to screen space
				Vec3 sA, sB, sC;
				{
					sA = { ( Vec2( a.x * scrnSize.x, a.y * -scrnSize.y ) ) + scrnSize };
					sB = { ( Vec2( b.x * scrnSize.x, b.y * -scrnSize.y ) ) + scrnSize };
					sC = { ( Vec2( c.x * scrnSize.x, c.y * -scrnSize.y ) ) + scrnSize };
				}
			}
		}
	}
}

//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}