#pragma once

#include "Utilities.h"
#include "PrimitiveFactory.h"
#include "Graphics.h"
#include "Mat4.h"

class Model_Textured
{
public:
	Model_Textured() = default;
	~Model_Textured() = default;

	bool Initialize( const PrimitiveFactory &PrimMaker );

	void Update( float RotAngle );
	void SetScale( const DirectX::XMFLOAT3 &Scal );
	Mat4 GetWorldMatrix() const;
	const VertexBufferTypeAllInOne *GetVertexBuffer()const;
	unsigned GetVertexCount()const;
	void Draw( const DirectX::XMMATRIX &View, 
		const DirectX::XMMATRIX &Proj, IWICBitmap *pImage,
		AmbientLight *const pAmbLight, InfiniteLight *pInfLight,
		Graphics &Gfx );
private:
	std::vector<VertexBufferTypeAllInOne> m_Vertices;
	DirectX::XMMATRIX m_world;
	int m_vertexCount, m_indexCount;
};