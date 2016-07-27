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
#include "Includes.h"

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

Graphics::Graphics( HWNDKey& key, Model &TheModel)
	:
	m_client(TheModel)
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
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::FillFlatTopTriangle(const XMFLOAT3 & P0, const XMFLOAT3 & P1, const XMFLOAT3 & P2, Color C)
{
	// Calculate slopes from bottom point ( P2 ) to each top points ( P0 and P1 )
	float m0 = (P2.x - P0.x) / (P2.y - P0.y);
	float m1 = (P2.x - P1.x) / (P2.y - P1.y);

	int yStart = static_cast<int>(std::max(0.f, ceilf(P0.y)));
	int yEnd = static_cast<int>(std::min(static_cast<float>(ScreenHeight - 1), ceilf(P2.y) - 1.f));

	for (int y = yStart; y <= yEnd; ++y)
	{
		float px0 = m0 * (y - P0.y) + P0.x;
		float px1 = m1 * (y - P1.y) + P1.x;
		
		int xStart = static_cast<int>(std::max(0.f, ceilf(px0)));
		int xEnd = static_cast<int>(std::min(static_cast<float>(ScreenWidth - 1), ceilf(px1) - 1.f));
		
		for (int x = xStart; x <= xEnd; ++x)
		{
			PutPixel(x, y, C);
		}
	}
}

void Graphics::FillFlatBottomTriangle(const XMFLOAT3 & P0, const XMFLOAT3 & P1, const XMFLOAT3 & P2, Color C)
{
	// Calculate slopes from top point ( P0 ) to each bottom points ( P1 and P2 )
	float m0 = (P1.x - P0.x) / (P1.y - P0.y);
	float m1 = (P2.x - P0.x) / (P2.y - P0.y);

	int yStart = static_cast<int>(std::max(0.f, ceilf(P0.y)));
	int yEnd = static_cast<int>(std::min(static_cast<float>(ScreenHeight - 1), ceilf(P2.y) - 1.f));

	for (int y = yStart; y <= yEnd; ++y)
	{
		float px0 = m0 * (y - P0.y) + P0.x;
		float px1 = m1 * (y - P0.y) + P0.x;

		int xStart = static_cast<int>(std::max(0.f, ceilf(px0)));
		int xEnd = static_cast<int>(std::min(static_cast<float>(ScreenWidth - 1), ceilf(px1) - 1.f));

		for (int x = xStart; x <= xEnd; ++x)
		{
			PutPixel(x, y, C);
		}
	}
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
	if( FAILED( hr = pSwapChain->Present( 0u,0u ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

void Graphics::FilledTriangle(const XMFLOAT3 & P0, const XMFLOAT3 & P1, const XMFLOAT3 & P2, Color C)
{
	// Order the points from top to bottom
	XMFLOAT3 p0(P0), p1(P1), p2(P2);
	if (p0.y > p1.y)
	{
		std::swap(p0, p1);
	}
	if (p2.y < p1.y)
	{
		std::swap(p1, p2);
	}
	if (p0.y > p1.y)
	{
		std::swap(p0, p1);
	}

	// Test for flat top triangle
	if (p0.y == p1.y)
	{
		if (p0.x > p1.x)
		{
			std::swap(p0, p1);
		}
		FillFlatTopTriangle(p0, p1, p2, C);
	}
	else if (p1.y == p2.y)
	{
		if (p1.x > p2.x)
		{
			std::swap(p1, p2);
		}
		FillFlatBottomTriangle(p0, p1, p2, C);
	}
	else
	{
		// Calculate intermediate vertex point on major segment
		const XMFLOAT3 p3 = { ((p2.x - p0.x) / (p2.y - p0.y)) *
			(p1.y - p0.y) + p0.x, p1.y, p0.z };

		// If major right
		if (p1.x < p3.x)
		{
			FillFlatBottomTriangle(p0, p1, p3, C);
			FillFlatTopTriangle(p1, p3, p2, C);
		}
		else
		{
			FillFlatBottomTriangle(p0, p3, p1, C);
			FillFlatTopTriangle(p3, p1, p2, C);
		}
	}
}

void Graphics::FilledRect(int X, int Y, int Width, int Height, Color C)
{
	for (int y = Y; y < Y + Height; ++y)
	{
		for (int x = X; x < X + Width; ++x)
		{
			this->PutPixel(x, y, C);
		}
	}
}

void Graphics::ComposeFrame()
{
	// Get camera to move to view space
	auto &cam = m_client.GetCamera();
	auto viewMatrix = cam.GetViewMatrix();
	auto orthoMatrix = cam.GetOrthoMatrix();

	// Create transform for screen space
	auto screenResHalf = XMFLOAT4(ScreenWidth * 0.5f, ScreenHeight * 0.5f, 0.f, 0.f);
	auto xmScreenResHalf = XMLoadFloat4(&screenResHalf);

	auto screenOffset = XMMatrixIdentity();
	screenOffset.r[0] = screenOffset.r[0] * xmScreenResHalf;
	screenOffset.r[1] = screenOffset.r[1] * xmScreenResHalf;
	screenOffset.r[3] = screenOffset.r[3] + xmScreenResHalf;


	// BeginFrame clears the drawing buffer
	BeginFrame();

	//////////////////////////// Drawing code goes here /////////////////////////////
	
	// Get position of each object and transform their positions to screen space
	auto worldMatrix = m_client.GetPlayer().GetWorldMatrix();

	auto wvp = (worldMatrix * viewMatrix * orthoMatrix) * screenOffset;
	
	auto pos = m_client.GetPlayer().GetPosition();
	auto playerVertexList = m_client.GetPlayer().GetMesh()->VertexList();
	auto playerFloatColor = playerVertexList[0].color;
	auto playerIntColor = Color(
		static_cast<unsigned char>(playerFloatColor.x * 255.f),
		static_cast<unsigned char>(playerFloatColor.y * 255.f),
		static_cast<unsigned char>(playerFloatColor.z * 255.f)
	);

	for (int i = 0; i < playerVertexList.size(); i += 3)
	{
		auto xmVerts0 = XMLoadFloat3(&playerVertexList[i + 0].position);
		auto xmVerts1 = XMLoadFloat3(&playerVertexList[i + 1].position);
		auto xmVerts2 = XMLoadFloat3(&playerVertexList[i + 2].position);

		xmVerts0 = XMVector3TransformCoord(xmVerts0, wvp);
		xmVerts1 = XMVector3TransformCoord(xmVerts1, wvp);
		xmVerts2 = XMVector3TransformCoord(xmVerts2, wvp);

		XMFLOAT3 p0, p1, p2;
		XMStoreFloat3(&p0, xmVerts0);
		XMStoreFloat3(&p1, xmVerts1);
		XMStoreFloat3(&p2, xmVerts2);

		FilledTriangle(p0, p1, p2, playerIntColor);
	}



	////////////////////////////// End of drawing code //////////////////////////////

	// EndFrame renders the drawing buffer to the back buffer
	EndFrame();
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