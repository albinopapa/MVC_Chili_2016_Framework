#pragma once

#include "Mat4.h"
#include "Graphics.h"
#include "Mempool.h"

struct VertexBufferTypeAllInOne;

struct Transforms
{
	Mat4 world, view, proj;
};

struct LightBuffer
{
	Vec4 ambient;
	Vec4 diffuse;
	Vec3 direction;
};

struct Tex2D
{
	unsigned width, height;
	Color *pPixels;
};

class Rasterizer
{
public:
	Rasterizer();
	~Rasterizer();
	
	void CreateVertexBuffer( const VertexBufferTypeAllInOne *pBuffer, unsigned VertexCount, SoaVertexBuffer *pSVB );
	void CreateTex2D( IWICBitmap *const pImage, Tex2D *pTexture );

	void SetLightBuffer( const LightBuffer &Light );
	void SetTransforms( const Transforms &Trans );
	void SetTexture( const Tex2D &Image );
	void SetVertexBuffer( const SoaVertexBuffer &VertexBuffer );
	void Rasterize( Graphics &Gfx );
private:

	Vec4 BilinearSample( const Vec2 &TexCoords );
	bool IsBackfacing( const Vec3 &Side0, const Vec3 &Side1 );
private:
	Transforms			m_trans;
	LightBuffer			m_light;
	SoaVertexBuffer		m_vertexBuffers;
	Tex2D			    m_tex2d;
	Mempool				m_mempool;
};

