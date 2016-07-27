#include "PrimitiveMaker.h"

using namespace DirectX;

PrimitiveMaker::PrimitiveMaker()
{}

PrimitiveMaker::~PrimitiveMaker()
{}

void PrimitiveMaker::CreateTriangle( const XMFLOAT3 & Center, 
	const XMFLOAT2 & Size, const XMFLOAT3 &Orientation )
{
	auto extentHalf = DirectX::XMFLOAT2(Size.x * 0.5f, Size.y * 0.5f );
	vertices = 
	{
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z},
		{Center.x, Center.y + extentHalf.y, Center.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z}
	};

	normals = 
	{
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f}
	};

	uvs = 
	{
		{0.f, 1.f},
		{0.5f, 0.f},
		{1.f, 1.f}
	};

	indices.resize( vertices.size() );
	for( int i = 0; i < vertices.size(); ++i )
	{
		indices[ i ] = i;
	}

	// Load the orienation into SIMD register and create the rotation matrix
	auto rotationVector = XMLoadFloat3( &Orientation );
	auto rotationMatrix = XMMatrixRotationRollPitchYawFromVector( rotationVector );

	// Rotate each vertex
	for( auto &vertex : vertices )
	{
		auto xmVector = XMLoadFloat3( &vertex );
		auto rotatedVector = XMVector3TransformCoord( xmVector, rotationMatrix );
		XMStoreFloat3( &vertex, rotatedVector );
	}

	// Load the reverse orientation into SIMD register and create the reverse rotation matrix
	// for the normals.  This makes sure that lights don't follow the direction of the 
	// plane, but instead go in the opposite direction
	auto reverseRotationMatrix = XMMatrixRotationRollPitchYawFromVector( -rotationVector );
	for( auto &normal : normals )
	{
		auto xmVector = XMLoadFloat3( &normal );
		auto rotatedVector = XMVector3TransformCoord( xmVector, reverseRotationMatrix );
		XMStoreFloat3( &normal, rotatedVector );
	}

}

void PrimitiveMaker::CreatePlane( const XMFLOAT3 & Center, 
	const XMFLOAT2 & Size, const XMFLOAT3 & Orientation )
{
	// There needs to be as many normals and uvs as there are vertices, 
	// there can be more indices

	auto extentHalf = DirectX::XMFLOAT2(Size.x * 0.5f, Size.y * 0.5f );
	// Create vertex list
	vertices = 
	{
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z}
	};
	
	// Create normals list
	normals =
	{
		{0.f,0.f,-1.f},
		{0.f,0.f,-1.f}, 
		{0.f,0.f,-1.f},
		{0.f,0.f,-1.f},
		{0.f,0.f,-1.f},
		{0.f,0.f,-1.f}
	};

	// Create uv coordinate list
	uvs = 
	{ 
		{0.f, 0.f}, 
		{1.f, 0.f}, 
		{0.f, 1.f}, 
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f} 
	};

	// This is for a D3D11_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, for a 
	// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP you would only need 0,1,2,3
	indices.resize( vertices.size() );
	for( int i = 0; i < vertices.size(); ++i )
	{
		indices[ i ] = i;
	}

	// Load the orienation into SIMD register and create the rotation matrix
	auto rotationVector = XMLoadFloat3( &Orientation );
	auto rotationMatrix = XMMatrixRotationRollPitchYawFromVector( rotationVector );

	// Rotate each vertex
	for( auto &vertex : vertices )
	{
		auto xmVector = XMLoadFloat3( &vertex );
		auto rotatedVector = XMVector3TransformCoord( xmVector, rotationMatrix );
		XMStoreFloat3( &vertex, rotatedVector );		
	}

	// Load the reverse orientation into SIMD register and create the reverse rotation matrix
	// for the normals.  This makes sure that lights don't follow the direction of the 
	// plane, but instead go in the opposite direction
	auto reverseRotationMatrix = XMMatrixRotationRollPitchYawFromVector( -rotationVector );
	for( auto &normal : normals )
	{
		auto xmVector = XMLoadFloat3( &normal );
		auto rotatedVector = XMVector3TransformCoord( xmVector, reverseRotationMatrix );
		XMStoreFloat3( &normal, rotatedVector );
	}

}

void PrimitiveMaker::CreateCube( 
    const XMFLOAT3 & Center, 
    const XMFLOAT3 & Size,
    const XMFLOAT3 & Orientation )
{
	auto extentHalf = DirectX::XMFLOAT3(Size.x * 0.5f, Size.y * 0.5f, Size.z * 0.5f );
	vertices =
	{
		// Left
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},

		// Right
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},

		// Bottom
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},

		// Top
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},

		// Front
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z - extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z - extentHalf.z},

		// Back
		{Center.x + extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},
		{Center.x + extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y + extentHalf.y, Center.z + extentHalf.z},
		{Center.x - extentHalf.x, Center.y - extentHalf.y, Center.z + extentHalf.z}
	};

	normals = 
	{
		// Left
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},
		{-1.f, 0.f, 0.f},

		// Right
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},

		// Bottom
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, -1.f, 0.f},

		// Top
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},

		// Front
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, -1.f},

		// Back
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f},
		{0.f, 0.f, 1.f}
	};

	uvs = 
	{
		// Left
		{0.f, 0.f},
		{1.f, 0.f},
		{0.f, 1.f},
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// Right
		{0.f, 0.f},
		{1.f, 0.f},
		{0.f, 1.f},
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// Bottom
		{0.f, 0.f},
		{1.f, 0.f},
		{0.f, 1.f},
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// Top
		{0.f, 0.f},
		{1.f, 0.f},
		{0.f, 1.f},
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// Front
		{0.f, 0.f},
		{1.f, 0.f},
		{0.f, 1.f},
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// Back
		{0.f, 0.f},
		{1.f, 0.f},
		{0.f, 1.f},
		{0.f, 1.f},
		{1.f, 0.f},
		{1.f, 1.f}
	};

	indices.resize( vertices.size() );
	for( int i = 0; i < vertices.size(); ++i )
	{
		indices[ i ] = i;
	}

	// Load the orienation into SIMD register and create the rotation matrix
	auto rotationVector = XMLoadFloat3( &Orientation );
	auto rotationMatrix = XMMatrixRotationRollPitchYawFromVector( rotationVector );

	// Rotate each vertex
	for( auto &vertex : vertices )
	{
		auto xmVector = XMLoadFloat3( &vertex );
		auto rotatedVector = XMVector3TransformCoord( xmVector, rotationMatrix );
		XMStoreFloat3( &vertex, rotatedVector );
	}

	// Load the reverse orientation into SIMD register and create the reverse rotation matrix
	// for the normals.  This makes sure that lights don't follow the direction of the 
	// plane, but instead go in the opposite direction
	auto reverseRotationMatrix = XMMatrixRotationRollPitchYawFromVector( -rotationVector );
	for( auto &normal : normals )
	{
		auto xmVector = XMLoadFloat3( &normal );
		auto rotatedVector = XMVector3TransformCoord( xmVector, reverseRotationMatrix );
		XMStoreFloat3( &normal, rotatedVector );
	}


}

void PrimitiveMaker::CreateColor( float R, float G, float B, float A )
{
	color = XMFLOAT4( R, G, B, A );
}

std::vector<XMFLOAT3> PrimitiveMaker::GetVertices() const
{
	return vertices;
}

std::vector<XMFLOAT3> PrimitiveMaker::GetNormals() const
{
	return normals;
}

std::vector<XMFLOAT2> PrimitiveMaker::GetUVs() const
{
	return uvs;
}

std::vector<DWORD> PrimitiveMaker::GetIndices() const
{
	return indices;
}

XMFLOAT4 PrimitiveMaker::GetColor() const
{
	return color;
}
