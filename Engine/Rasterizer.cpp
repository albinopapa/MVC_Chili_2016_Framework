#include "Rasterizer.h"
#include "Utilities.h"

#undef min
#undef max

using Microsoft::WRL::ComPtr;

Rasterizer::Rasterizer()
{
}


Rasterizer::~Rasterizer()
{}

void Rasterizer::CreateVertexBuffer( 
	const VertexBufferTypeAllInOne * pBuffer, unsigned VertexCount, SoaVertexBuffer *pSVB )
{
	pSVB->vertCount = VertexCount;
	pSVB->px = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );
	pSVB->py = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );
	pSVB->pz = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );

	pSVB->tu = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );
	pSVB->tv = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );

	pSVB->nx = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );
	pSVB->ny = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );
	pSVB->nz = m_mempool.AllocateBlockAligned<float>( sizeof( float ) * VertexCount );
		
	for( unsigned i = 0; i < VertexCount; ++i )
	{
		pSVB->px[ i ] = pBuffer[ i ].position.x;
		pSVB->py[ i ] = pBuffer[ i ].position.y;
		pSVB->pz[ i ] = pBuffer[ i ].position.z;

		pSVB->tu[ i ] = pBuffer[ i ].uv.x;
		pSVB->tv[ i ] = pBuffer[ i ].uv.y;

		pSVB->nx[ i ] = pBuffer[ i ].normal.x;
		pSVB->ny[ i ] = pBuffer[ i ].normal.y;
		pSVB->nz[ i ] = pBuffer[ i ].normal.z;
	}
}

void Rasterizer::CreateTex2D( IWICBitmap * const pImage, Tex2D * pTexture )
{
	pImage->GetSize( &pTexture->width, &pTexture->height );

	WICRect wr{};
	wr.Width  = static_cast<int>( pTexture->width );
	wr.Height = static_cast<int>( pTexture->height );
	unsigned pitch = pTexture->width * sizeof( Color );
	unsigned bufferSize = pitch * pTexture->height;

	pTexture->pPixels = 
		reinterpret_cast<Color*>( m_mempool.AllocateTexture( pTexture->width, pTexture->height ) );
	BYTE *pPixels = reinterpret_cast<BYTE*>( pTexture->pPixels );

	pImage->CopyPixels( &wr, pitch, bufferSize, pPixels );
}

void Rasterizer::SetLightBuffer( const LightBuffer & Light )
{
	m_light.ambient = Light.ambient;
	m_light.diffuse = Light.diffuse;
	m_light.direction = Light.direction;
}

void Rasterizer::SetTransforms( const Transforms & Trans )
{
	m_trans = Trans;
}

void Rasterizer::SetTexture( const Tex2D &Image )
{
	m_tex2d.width = Image.width;
	m_tex2d.height = Image.height;
	m_tex2d.pPixels = Image.pPixels;
}

void Rasterizer::SetVertexBuffer( const SoaVertexBuffer & VertexBuffer )
{
	m_vertexBuffers.vertCount = VertexBuffer.vertCount;
	m_vertexBuffers.px = VertexBuffer.px;
	m_vertexBuffers.py = VertexBuffer.py;
	m_vertexBuffers.pz = VertexBuffer.pz;

	m_vertexBuffers.tu = VertexBuffer.tu;
	m_vertexBuffers.tv = VertexBuffer.tv;
	
	m_vertexBuffers.nx = VertexBuffer.nx;
	m_vertexBuffers.ny = VertexBuffer.ny;
	m_vertexBuffers.nz = VertexBuffer.nz;

	m_vertexBuffers.tx = VertexBuffer.tx;
	m_vertexBuffers.ty = VertexBuffer.ty;
	m_vertexBuffers.tz = VertexBuffer.tz;
}

void Rasterizer::Rasterize( Graphics & Gfx )
{
	m_mempool.ResetWorkSpace();
	unsigned triCount = m_vertexBuffers.vertCount / 3;

	Vec2 scrnSize(
		static_cast<float>( Graphics::ScreenWidth ) * .5f,
		static_cast<float>( Graphics::ScreenHeight ) * .5f
	);

	// Setup screen positioning	
	auto mNegOne = _mm_set_ps1( -1.f );
	auto mScrnSizeX = _mm_set_ps1( static_cast<float>( Graphics::ScreenWidth ) * .5f );
	auto mScrnSizeY = _mm_set_ps1( static_cast<float>( Graphics::ScreenHeight ) * .5f );
	auto mScrnScale = mScrnSizeY * mNegOne;

	auto xmWorld = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &m_trans.world ) );
	auto xmView = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &m_trans.view ) );
	auto xmProj = XMLoadFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &m_trans.proj ) );
	auto xmTrans = xmWorld * xmView * xmProj;

	SoaVertexBuffer workspace{};
	workspace.vertCount = m_vertexBuffers.vertCount;
	workspace.px = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );
	workspace.py = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );
	workspace.pz = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );
	float *pw = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );

	workspace.tu = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );
	workspace.tv = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );

	workspace.nx = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );
	workspace.ny = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );
	workspace.nz = m_mempool.AllocateFromWorkspace( m_vertexBuffers.vertCount );

	auto xmOne = XMVectorReplicate( 1.f );
	for( int idx = 0; idx < m_vertexBuffers.vertCount; idx += 3)
	{
		unsigned i = idx + 0;
		auto xmVax = _mm_loadu_ps( &m_vertexBuffers.px[ i ] );
		auto xmVay = _mm_loadu_ps( &m_vertexBuffers.py[ i ] );
		auto xmVaz = _mm_loadu_ps( &m_vertexBuffers.pz[ i ] );
		auto xmVaw = xmOne;

		_MM_TRANSPOSE4_PS( xmVax, xmVay, xmVaz, xmVaw );
		// Transform the vertices of the triangle
		xmVax = XMVector4Transform( xmVax, xmTrans );
		xmVay = XMVector4Transform( xmVay, xmTrans );
		xmVaz = XMVector4Transform( xmVaz, xmTrans );

		xmVax = xmVax / XMVectorSplatW( xmVax );
		xmVay = xmVay / XMVectorSplatW( xmVay );
		xmVaz = xmVaz / XMVectorSplatW( xmVaz );

		_MM_TRANSPOSE4_PS( xmVax, xmVay, xmVaz, xmVaw );
		_mm_storeu_ps( &workspace.px[ i ], xmVax );
		_mm_storeu_ps( &workspace.py[ i ], xmVay );
		_mm_storeu_ps( &workspace.pz[ i ], xmVaz );
	}

	for( unsigned triIdx = 0; triIdx < triCount; ++triIdx )
	{
		unsigned triOffset = triIdx * 3;
		unsigned i = triOffset + 0;
		unsigned j = triOffset + 1;
		unsigned k = triOffset + 2;

		// TODO: Cut triangles off at screen boundaries, not cull
		// Clip depth
		if( workspace.pz[ i ] < 0.f || workspace.pz[ i ] > 1.f ||
			workspace.pz[ j ] < 0.f || workspace.pz[ j ] > 1.f ||
			workspace.pz[ k ] < 0.f || workspace.pz[ k ] > 1.f )
		{
			continue;
		}
		
		// scale and translate to screen space
		Vec3 sA( ( Vec2( workspace.px[ i ] * scrnSize.x, workspace.py[ i ] * -scrnSize.y ) ) + scrnSize, 0.f );
		Vec3 sB( ( Vec2( workspace.px[ j ] * scrnSize.x, workspace.py[ j ] * -scrnSize.y ) ) + scrnSize, 0.f );
		Vec3 sC( ( Vec2( workspace.px[ k ] * scrnSize.x, workspace.py[ k ] * -scrnSize.y ) ) + scrnSize, 0.f );

		auto msa = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &sA ) );
		auto msb = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &sB ) );
		auto msc = XMLoadFloat3( reinterpret_cast<XMFLOAT3 *>( &sC ) );

		// Calculate bouding box and clip to screen bounding box
		/*float fxStart = std::min( sA.x, std::min( sB.x, sC.x ) );
		float fyStart = std::min( sA.y, std::min( sB.y, sC.y ) );
		float fxEnd = std::max( sA.x, std::max( sB.x, sC.x ) );
		float fyEnd = std::max( sA.y, std::max( sB.y, sC.y ) );

		int xStart = static_cast<int>( floorf( fxStart ) );
		int yStart = static_cast<int>( floorf( fyStart ) );
		int xEnd = static_cast<int>( ceilf( fxEnd ) );
		int yEnd = static_cast<int>( ceilf( fyEnd ) );

		xStart = std::max( xStart, 0 );
		yStart = std::max( yStart, 0 );
		xEnd = std::min( xEnd, static_cast<int>( Graphics::ScreenWidth - 1 ) );
		yEnd = std::min( yEnd, static_cast<int>( Graphics::ScreenHeight - 1 ) );*/

		// Precalculate edges
		auto mba = msb - msa;
		auto mca = msc - msa;
		
		auto ba( sB - sA );
		auto ca( sC - sA );

		// Calculate perpendicular vector to triangle
		XMVECTOR yzx0, zxy0, yzx1, zxy1;
		yzx0 = _mm_shuffle_ps( mba, mba, _MM_SHUFFLE( 3, 0, 2, 1 ) );
		zxy0 = _mm_shuffle_ps( mba, mba, _MM_SHUFFLE( 3, 1, 0, 2 ) );
		yzx1 = _mm_shuffle_ps( mca, mca, _MM_SHUFFLE( 3, 0, 2, 1 ) );
		zxy1 = _mm_shuffle_ps( mca, mca, _MM_SHUFFLE( 3, 1, 0, 2 ) );
		auto mPerpV = ( yzx0 * zxy1 ) - ( zxy0 * yzx1 );
		if( XMVectorGetZ( mPerpV ) < 0.f )
		{
			continue;
		}
		//auto perpV = ba.CrossProduct( ca );
		// Calculate normalized perpV
		/*perpV = perpV.Normalize();
		if( perpV.z < 0.f )
		{
			continue;
		}*/

		// Precalculate inverse area
		auto temp = ( mba * mba );
		auto md00 = ( _mm_shuffle_ps( temp, temp, _MM_SHUFFLE( 0, 1, 0, 1 ) ) + temp );
		temp = mba * mca;
		auto md01 = ( _mm_shuffle_ps( temp, temp, _MM_SHUFFLE( 0, 1, 0, 1 ) ) + temp );
		temp = mca * mca;
		auto md11 = ( _mm_shuffle_ps( temp, temp, _MM_SHUFFLE( 0, 1, 0, 1 ) ) + temp );;
		auto mInvDenom = ( md00 * md11 ) - ( md01 * md01 );
		mInvDenom = _mm_rcp_ps( mInvDenom );

		float d00 = ba.DotProduct( ba );
		float d01 = ba.DotProduct( ca );
		float d11 = ca.DotProduct( ca );		
		float invDenom = 1.0 / ( d00 * d11 - d01 * d01 );

		auto DrawTriangle = [ & ](const Vec3 &P, int X, int Y)
		{
			// Calculate vector PA
			//auto pa( p - sA );
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
					( workspace.pz[ i ] * baryCoords.z ) +
					( workspace.pz[ j ] * baryCoords.x ) +
					( workspace.pz[ k ] * baryCoords.y );

				//int index = x + ( y * Graphics::ScreenWidth );
				int index = X + ( Y * Graphics::ScreenWidth );
				//if( Gfx.GetZ( x, y ) > z )
				if( Gfx.GetZ( X, Y ) > z )
				{
					Vec2 ta, tb, tc;
					ta.x = m_vertexBuffers.tu[ i ] < 0.f ? 1.f + m_vertexBuffers.tu[ i ] : m_vertexBuffers.tu[ i ] > 1.f ? 1.f - m_vertexBuffers.tu[ i ] : m_vertexBuffers.tu[ i ];
					ta.y = m_vertexBuffers.tv[ i ] < 0.f ? 1.f + m_vertexBuffers.tv[ i ] : m_vertexBuffers.tv[ i ] > 1.f ? 1.f - m_vertexBuffers.tv[ i ] : m_vertexBuffers.tv[ i ];
					tb.x = m_vertexBuffers.tu[ j ] < 0.f ? 1.f + m_vertexBuffers.tu[ j ] : m_vertexBuffers.tu[ j ] > 1.f ? 1.f - m_vertexBuffers.tu[ j ] : m_vertexBuffers.tu[ j ];
					tb.y = m_vertexBuffers.tv[ j ] < 0.f ? 1.f + m_vertexBuffers.tv[ j ] : m_vertexBuffers.tv[ j ] > 1.f ? 1.f - m_vertexBuffers.tv[ j ] : m_vertexBuffers.tv[ j ];
					tc.x = m_vertexBuffers.tu[ k ] < 0.f ? 1.f + m_vertexBuffers.tu[ k ] : m_vertexBuffers.tu[ k ] > 1.f ? 1.f - m_vertexBuffers.tu[ k ] : m_vertexBuffers.tu[ k ];
					tc.y = m_vertexBuffers.tv[ k ] < 0.f ? 1.f + m_vertexBuffers.tv[ k ] : m_vertexBuffers.tv[ k ] > 1.f ? 1.f - m_vertexBuffers.tv[ k ] : m_vertexBuffers.tv[ k ];

					Vec2 texcoords =
						( ta * baryCoords.z ) +
						( tb * baryCoords.x ) +
						( tc * baryCoords.y );

					Vec4 fTexColor( BilinearSample( texcoords ) );

					Vec4 ambientColor(
						fTexColor.x * m_light.ambient.x,
						fTexColor.y * m_light.ambient.y,
						fTexColor.z * m_light.ambient.z
					);

					Vec3 normal = {
						( m_vertexBuffers.nx[ i ] * baryCoords.z ) +
						( m_vertexBuffers.ny[ i ] * baryCoords.z ) +
						( m_vertexBuffers.nz[ i ] * baryCoords.z ),
						( m_vertexBuffers.nx[ j ] * baryCoords.x ) +
						( m_vertexBuffers.ny[ j ] * baryCoords.x ) +
						( m_vertexBuffers.nz[ j ] * baryCoords.x ),
						( m_vertexBuffers.nx[ k ] * baryCoords.y ) +
						( m_vertexBuffers.ny[ k ] * baryCoords.y ) +
						( m_vertexBuffers.nz[ k ] * baryCoords.y )
					};

					auto lightDir = -m_light.direction;
					float intensity = lightDir.DotProduct( normal );
					auto lightColor = ( m_light.diffuse * intensity );

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
					//Gfx.PutPixel( x, y, z, finalColor );
					Gfx.PutPixel( X, Y, z, finalColor );
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
			int xStart = static_cast<int>( std::max( top.x + ( ( y - yStart0 ) * slope0 ), 0.f ) );
			int xEnd = static_cast<int>( std::min<float>( Graphics::ScreenWidth - 1, top.x + ( ( y - yStart0 )* slope1 ) ) );
			if( xStart > xEnd )
			{
				std::swap( xStart, xEnd );
			}
			for( int x = xStart; x <= xEnd; ++x )
			{
				Vec3 p(
					static_cast<float>( x ) + .5f,
					static_cast<float>( y ) + .5f, 0.f );
				DrawTriangle( p, x, y );
		//		// Calculate vector PA
		//		auto pa( p - sA );

		//		// Calculate vector
		//		float d20 = pa.DotProduct( ba );
		//		float d21 = pa.DotProduct( ca );

		//		BarycentricCoord baryCoords{};
		//		// C
		//		baryCoords.x = ( d11 * d20 - d01 * d21 ) * invDenom;
		//		// A
		//		baryCoords.y = ( d00 * d21 - d01 * d20 ) * invDenom;
		//		// B
		//		baryCoords.z = 1.0f - baryCoords.x - baryCoords.y;

		//		// If barycentric, fill pixel
		//		if( baryCoords.x >= 0.f && baryCoords.y >= 0.f && baryCoords.z >= 0.f )
		//		{
		//			float z =
		//				( workspace.pz[ i ] * baryCoords.z ) +
		//				( workspace.pz[ j ] * baryCoords.x ) +
		//				( workspace.pz[ k ] * baryCoords.y );

		//			int index = x + ( y * Graphics::ScreenWidth );
		//			if( Gfx.GetZ( x, y ) > z )
		//			{
		//				Vec2 ta, tb, tc;
		//				ta.x = m_vertexBuffers.tu[ i ] < 0.f ? 1.f + m_vertexBuffers.tu[ i ] : m_vertexBuffers.tu[ i ] > 1.f ? 1.f - m_vertexBuffers.tu[ i ] : m_vertexBuffers.tu[ i ];
		//				ta.y = m_vertexBuffers.tv[ i ] < 0.f ? 1.f + m_vertexBuffers.tv[ i ] : m_vertexBuffers.tv[ i ] > 1.f ? 1.f - m_vertexBuffers.tv[ i ] : m_vertexBuffers.tv[ i ];
		//				tb.x = m_vertexBuffers.tu[ j ] < 0.f ? 1.f + m_vertexBuffers.tu[ j ] : m_vertexBuffers.tu[ j ] > 1.f ? 1.f - m_vertexBuffers.tu[ j ] : m_vertexBuffers.tu[ j ];
		//				tb.y = m_vertexBuffers.tv[ j ] < 0.f ? 1.f + m_vertexBuffers.tv[ j ] : m_vertexBuffers.tv[ j ] > 1.f ? 1.f - m_vertexBuffers.tv[ j ] : m_vertexBuffers.tv[ j ];
		//				tc.x = m_vertexBuffers.tu[ k ] < 0.f ? 1.f + m_vertexBuffers.tu[ k ] : m_vertexBuffers.tu[ k ] > 1.f ? 1.f - m_vertexBuffers.tu[ k ] : m_vertexBuffers.tu[ k ];
		//				tc.y = m_vertexBuffers.tv[ k ] < 0.f ? 1.f + m_vertexBuffers.tv[ k ] : m_vertexBuffers.tv[ k ] > 1.f ? 1.f - m_vertexBuffers.tv[ k ] : m_vertexBuffers.tv[ k ];

		//				Vec2 texcoords =
		//					( ta * baryCoords.z ) +
		//					( tb * baryCoords.x ) +
		//					( tc * baryCoords.y );

		//				Vec4 fTexColor( BilinearSample( texcoords ) );

		//				Vec4 ambientColor(
		//					fTexColor.x * m_light.ambient.x,
		//					fTexColor.y * m_light.ambient.y,
		//					fTexColor.z * m_light.ambient.z
		//				);

		//				Vec3 normal = {
		//					( m_vertexBuffers.nx[ i ] * baryCoords.z ) +
		//					( m_vertexBuffers.ny[ i ] * baryCoords.z ) +
		//					( m_vertexBuffers.nz[ i ] * baryCoords.z ),
		//					( m_vertexBuffers.nx[ j ] * baryCoords.x ) +
		//					( m_vertexBuffers.ny[ j ] * baryCoords.x ) +
		//					( m_vertexBuffers.nz[ j ] * baryCoords.x ),
		//					( m_vertexBuffers.nx[ k ] * baryCoords.y ) +
		//					( m_vertexBuffers.ny[ k ] * baryCoords.y ) +
		//					( m_vertexBuffers.nz[ k ] * baryCoords.y )
		//				};

		//				auto lightDir = -m_light.direction;
		//				float intensity = lightDir.DotProduct( normal );
		//				auto lightColor = ( m_light.diffuse * intensity );

		//				Vec4 diffuseColor(
		//					lightColor.x * fTexColor.x,
		//					lightColor.y * fTexColor.y,
		//					lightColor.z * fTexColor.z
		//				);

		//				Vec4 fFinalColor = Saturate( diffuseColor + ambientColor );

		//				Color finalColor =
		//				{
		//					static_cast<unsigned char>( fFinalColor.x * 255.f ),
		//					static_cast<unsigned char>( fFinalColor.y * 255.f ),
		//					static_cast<unsigned char>( fFinalColor.z * 255.f )
		//				};
		//				Gfx.PutPixel( x, y, z, finalColor );
		//			}
		//		}
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
					static_cast<float>( y ) + .5f, 0.f );
				DrawTriangle( p, x, y );
				
				//// Calculate vector PA
				//auto pa( p - sA );

				//// Calculate vector
				//float d20 = pa.DotProduct( ba );
				//float d21 = pa.DotProduct( ca );

				//BarycentricCoord baryCoords{};
				//// C
				//baryCoords.x = ( d11 * d20 - d01 * d21 ) * invDenom;
				//// A
				//baryCoords.y = ( d00 * d21 - d01 * d20 ) * invDenom;
				//// B
				//baryCoords.z = 1.0f - baryCoords.x - baryCoords.y;

				//// If barycentric, fill pixel
				//if( baryCoords.x >= 0.f && baryCoords.y >= 0.f && baryCoords.z >= 0.f )
				//{
				//	float z =
				//		( workspace.pz[ i ] * baryCoords.z ) +
				//		( workspace.pz[ j ] * baryCoords.x ) +
				//		( workspace.pz[ k ] * baryCoords.y );

				//	int index = x + ( y * Graphics::ScreenWidth );
				//	if( Gfx.GetZ( x, y ) > z )
				//	{
				//		Vec2 ta, tb, tc;
				//		ta.x = m_vertexBuffers.tu[ i ] < 0.f ? 1.f + m_vertexBuffers.tu[ i ] : m_vertexBuffers.tu[ i ] > 1.f ? 1.f - m_vertexBuffers.tu[ i ] : m_vertexBuffers.tu[ i ];
				//		ta.y = m_vertexBuffers.tv[ i ] < 0.f ? 1.f + m_vertexBuffers.tv[ i ] : m_vertexBuffers.tv[ i ] > 1.f ? 1.f - m_vertexBuffers.tv[ i ] : m_vertexBuffers.tv[ i ];
				//		tb.x = m_vertexBuffers.tu[ j ] < 0.f ? 1.f + m_vertexBuffers.tu[ j ] : m_vertexBuffers.tu[ j ] > 1.f ? 1.f - m_vertexBuffers.tu[ j ] : m_vertexBuffers.tu[ j ];
				//		tb.y = m_vertexBuffers.tv[ j ] < 0.f ? 1.f + m_vertexBuffers.tv[ j ] : m_vertexBuffers.tv[ j ] > 1.f ? 1.f - m_vertexBuffers.tv[ j ] : m_vertexBuffers.tv[ j ];
				//		tc.x = m_vertexBuffers.tu[ k ] < 0.f ? 1.f + m_vertexBuffers.tu[ k ] : m_vertexBuffers.tu[ k ] > 1.f ? 1.f - m_vertexBuffers.tu[ k ] : m_vertexBuffers.tu[ k ];
				//		tc.y = m_vertexBuffers.tv[ k ] < 0.f ? 1.f + m_vertexBuffers.tv[ k ] : m_vertexBuffers.tv[ k ] > 1.f ? 1.f - m_vertexBuffers.tv[ k ] : m_vertexBuffers.tv[ k ];

				//		Vec2 texcoords =
				//			( ta * baryCoords.z ) +
				//			( tb * baryCoords.x ) +
				//			( tc * baryCoords.y );

				//		Vec4 fTexColor( BilinearSample( texcoords ) );

				//		Vec4 ambientColor(
				//			fTexColor.x * m_light.ambient.x,
				//			fTexColor.y * m_light.ambient.y,
				//			fTexColor.z * m_light.ambient.z
				//		);

				//		Vec3 normal = {
				//			( m_vertexBuffers.nx[ i ] * baryCoords.z ) +
				//			( m_vertexBuffers.ny[ i ] * baryCoords.z ) +
				//			( m_vertexBuffers.nz[ i ] * baryCoords.z ),
				//			( m_vertexBuffers.nx[ j ] * baryCoords.x ) +
				//			( m_vertexBuffers.ny[ j ] * baryCoords.x ) +
				//			( m_vertexBuffers.nz[ j ] * baryCoords.x ),
				//			( m_vertexBuffers.nx[ k ] * baryCoords.y ) +
				//			( m_vertexBuffers.ny[ k ] * baryCoords.y ) +
				//			( m_vertexBuffers.nz[ k ] * baryCoords.y )
				//		};

				//		auto lightDir = -m_light.direction;
				//		float intensity = lightDir.DotProduct( normal );
				//		auto lightColor = ( m_light.diffuse * intensity );

				//		Vec4 diffuseColor(
				//			lightColor.x * fTexColor.x,
				//			lightColor.y * fTexColor.y,
				//			lightColor.z * fTexColor.z
				//		);

				//		Vec4 fFinalColor = Saturate( diffuseColor + ambientColor );

				//		Color finalColor =
				//		{
				//			static_cast<unsigned char>( fFinalColor.x * 255.f ),
				//			static_cast<unsigned char>( fFinalColor.y * 255.f ),
				//			static_cast<unsigned char>( fFinalColor.z * 255.f )
				//		};
				//		Gfx.PutPixel( x, y, z, finalColor );
				//	}
				//}
			}
		}
	}
}

Vec4 Rasterizer::BilinearSample( const Vec2 & TexCoords )
{	
	int width = static_cast<int>(  m_tex2d.width - 1 );
	int height = static_cast<int>( m_tex2d.height - 1 );

	float colorToFloat = 1.f / 255.f;

	float ftx = TexCoords.x * static_cast<float>( width );
	float fty = TexCoords.y * static_cast<float>( height );

	int tx = static_cast<int>( floorf( ftx ) );
	int ty = static_cast<int>( floorf( fty ) );

	int fracU = static_cast<int>( ( ftx - floorf( ftx ) ) * 255.f );
	int fracV = static_cast<int>( ( fty - floorf( fty ) ) * 255.f );

	int invFracU = 255 - fracU;
	int invFracV = 255 - fracV;

	int ul = ( invFracU * invFracV ) >> 8;
	int ur = ( fracU * invFracV ) >> 8;
	int ll = ( invFracU * fracV ) >> 8;
	int lr = ( fracU * fracV ) >> 8;

	int rowOffset0 = ty * width;
	int rowOffset1 = std::min( ty + 1, height ) * height;
	int colOffset = std::min( tx + 1, width );

	int texIdx0 = tx + rowOffset0;
	int texIdx1 = colOffset + rowOffset0;
	int texIdx2 = tx + rowOffset1;
	int texIdx3 = colOffset + rowOffset1;
	
	int sr0 = m_tex2d.pPixels[ texIdx0 ].GetR() * ul;
	int sg0 = m_tex2d.pPixels[ texIdx0 ].GetG() * ul;
	int sb0 = m_tex2d.pPixels[ texIdx0 ].GetB() * ul;

	int sr1 = m_tex2d.pPixels[ texIdx1 ].GetR() * ur;
	int sg1 = m_tex2d.pPixels[ texIdx1 ].GetG() * ur;
	int sb1 = m_tex2d.pPixels[ texIdx1 ].GetB() * ur;

	int sr2 = m_tex2d.pPixels[ texIdx2 ].GetR() * ll;
	int sg2 = m_tex2d.pPixels[ texIdx2 ].GetG() * ll;
	int sb2 = m_tex2d.pPixels[ texIdx2 ].GetB() * ll;

	int sr3 = m_tex2d.pPixels[ texIdx3 ].GetR() * lr;
	int sg3 = m_tex2d.pPixels[ texIdx3 ].GetG() * lr;
	int sb3 = m_tex2d.pPixels[ texIdx3 ].GetB() * lr;

	return{
		static_cast<float>( ( sr0 + sr1 + sr2 + sr3 ) >> 8 ) * colorToFloat,
		static_cast<float>( ( sg0 + sg1 + sg2 + sg3 ) >> 8 ) * colorToFloat,
		static_cast<float>( ( sb0 + sb1 + sb2 + sb3 ) >> 8 ) * colorToFloat
	};
}

bool Rasterizer::IsBackfacing( const Vec3 &Side0, const Vec3 &Side1 )
{
	// Calculate perpendicular vector to triangle
	auto perpV = Side0.CrossProduct( Side1 );
	
	// Calculate normalized perpV
	perpV = perpV.Normalize();
	if( perpV.z < 0.f )
	{
		return true;
	}
	
	return false;
}
