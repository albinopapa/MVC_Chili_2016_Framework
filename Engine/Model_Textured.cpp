#include "Model_Textured.h"
#include "Timer.h"
#include "Mat4.h"

using namespace DirectX;

bool Model_Textured::Initialize( const PrimitiveFactory &PrimMaker )
{
	m_world = XMMatrixIdentity();

	// Create the vertex array.
	auto verts = PrimMaker.GetVertices();

	// Set the number of verticex indices in the vertex array.
	m_vertexCount = verts.size();
	m_indexCount = verts.size();
	m_Vertices.resize( m_vertexCount );

	// Get the color
	auto uvs = PrimMaker.GetUVs();
    // Get normals
    auto normals = PrimMaker.GetNormals();

	// Load the vertex buffer array with data.
	UINT idx = 0;
	for( auto &v : verts )
	{
		m_Vertices[ idx ] = {verts[ idx ], uvs[ idx ], normals[idx]};
		++idx;
	}

	// Load the index array with data.
	auto indices = PrimMaker.GetIndices();

	// Set the number of indices in the index array.
	m_indexCount = indices.size();

	return true;
}

void Model_Textured::Update( float RotAngle )
{
	Vec3 axis{ Vec3{ 0.f, 1.f, 0.f }.Normalize() };
	auto maxis = XMLoadFloat3( reinterpret_cast<XMFLOAT3*>( &axis ) );
	
	m_world = DirectX::XMMatrixRotationAxis( maxis, RotAngle ) * m_world;
}

void Model_Textured::SetScale( const DirectX::XMFLOAT3 & Scal )
{
	m_world.r[ 0 ].m128_f32[ 0 ] = Scal.x;
	m_world.r[ 1 ].m128_f32[ 1 ] = Scal.y;
	m_world.r[ 2 ].m128_f32[ 2 ] = Scal.z;
}

Mat4 Model_Textured::GetWorldMatrix() const
{
	Mat4 mat{};
	XMStoreFloat4x4( reinterpret_cast<XMFLOAT4X4*>( &mat ), m_world );
	return mat;
}

const VertexBufferTypeAllInOne * Model_Textured::GetVertexBuffer() const
{
	return m_Vertices.data();
}

unsigned Model_Textured::GetVertexCount() const
{
	return m_vertexCount;
}

void Model_Textured::Draw( const DirectX::XMMATRIX & View, 
	const DirectX::XMMATRIX & Proj, IWICBitmap *pImage,
	AmbientLight *const pAmbLight, InfiniteLight *pInfLight,
	Graphics & Gfx )
{
	auto trans = m_world * View * Proj;

	const int triCount = m_vertexCount / 3;
	Triangle t;
	for( int i = 0; i < triCount; ++i )
	{
		const int j = i * 3 + 0;
		const int k = i * 3 + 1;
		const int l = i * 3 + 2;
		t.a = m_Vertices[ j ].position;
		t.ta = m_Vertices[ j ].uv;
		t.na = m_Vertices[ j ].normal;

		t.b = m_Vertices[ k ].position;
		t.tb = m_Vertices[ k ].uv;
		t.nb = m_Vertices[ k ].normal;

		t.c = m_Vertices[ l ].position;
		t.tc = m_Vertices[ l ].uv;
		t.nc = m_Vertices[ l ].normal;

		Gfx.DrawTriangle( t, pImage, trans, pInfLight, pAmbLight );
	}
}
