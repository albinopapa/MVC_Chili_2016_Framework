/***********************************************************************************
File: PrimitiveFactory.cpp
Function: Create basic 3d primitives using vector math. Can be combined with
texture functionality for more detailed graphics.

***************************************************************************************/
#include "PrimitiveFactory.h"
#include "Utilities.h"

using namespace DirectX;

void PrimitiveFactory::CreateTriangle( const ModelSpecs_L &Specs )
{
	auto CreateVertexList = [ &Specs ]()
	{
		auto extentHalf = Specs.size * 0.5f;

		return std::vector<XMFLOAT3>
		{
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z },
			{ Specs.center.x, Specs.center.y + extentHalf.y, Specs.center.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z }
		};
	};
	auto CreateNormalsList = []()
	{
		return std::vector<DirectX::XMFLOAT3>
		{
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f }
		};
	};
	auto CreateUVList = []()
	{
		return std::vector<DirectX::XMFLOAT2>
		{
			{ 0.f, 1.f },
			{ 0.5f, 0.f },
			{ 1.f, 1.f }
		};
	};

	PrimitiveFactory::ClearAllBuffers();

	PrimitiveFactory::vertices = CreateVertexList();
	PrimitiveFactory::normals = CreateNormalsList();
	PrimitiveFactory::uvs = CreateUVList();

	Common( Specs );
}

void PrimitiveFactory::CreatePlane( const ModelSpecs_L &Specs )
{
	auto CreateVertexList = [ &Specs ]()
	{
		// Extent half is size of the object in each direction
		auto extentHalf = Specs.size * .5f;

		return std::vector<DirectX::XMFLOAT3>
		{
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z }
		};

	};
	auto CreateNormalsList = []()
	{
		return std::vector<DirectX::XMFLOAT3>
		{
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f }
		};
	};
	auto CreateUVList = []()
	{
		return std::vector<DirectX::XMFLOAT2>
		{
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f }
		};
	};

	PrimitiveFactory::ClearAllBuffers();

	PrimitiveFactory::vertices = CreateVertexList();
	PrimitiveFactory::normals = CreateNormalsList();
	PrimitiveFactory::uvs = CreateUVList();

	Common( Specs );
}

void PrimitiveFactory::CreatePlaneNM( const ModelSpecs_L & Specs )
{
	CreatePlane( Specs );

	auto vertCount = vertices.size();
	tangents.resize( vertCount );
	binormals.resize( vertCount );
	normals.resize( vertCount );

	for( int i = 0; i < vertCount; i += 3 )
	{
		auto edge10 = vertices[ i + 1 ] - vertices[ i ];
		auto edge20 = vertices[ i + 2 ] - vertices[ i ];

		auto tEdge10 = uvs[ i + 1 ] - uvs[ i ];
		auto tEdge20 = uvs[ i + 2 ] - uvs[ i ];

		tangents[ i ] = CalculateTangent( edge10, tEdge10 );
		binormals[ i ] = CalculateBiNormal( tangents[ i ], normals[ i ] );

		tangents[ i + 2 ] = tangents[ i + 1 ] = tangents[ i ];
		binormals[ i + 2 ] = binormals[ i + 1 ] = binormals[ i ];
		normals[ i + 2 ] = normals[ i + 1 ] = normals[ i ];
	}
}

void PrimitiveFactory::CreateCube( const ModelSpecs_L &Specs )
{
	auto CreateVertexList = [ &Specs ]()->std::vector<DirectX::XMFLOAT3>
	{
		auto extentHalf = Specs.size * .5f;

		return std::vector<DirectX::XMFLOAT3>
		{
			// Left
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },

				// Right
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },

				// Bottom
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },

				// Top
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },

				// Front
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z - extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z - extentHalf.z },

				// Back
			{ Specs.center.x + extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x + extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y + extentHalf.y, Specs.center.z + extentHalf.z },
			{ Specs.center.x - extentHalf.x, Specs.center.y - extentHalf.y, Specs.center.z + extentHalf.z }
		};
	};
	auto CreateNormalsList = []()->std::vector<DirectX::XMFLOAT3>
	{
		return std::vector<DirectX::XMFLOAT3>
		{
			// Left
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },
			{ -1.f, 0.f, 0.f },

				// Right
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },

				// Bottom
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },
			{ 0.f, -1.f, 0.f },

				// Top
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 1.f, 0.f },

				// Front
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },
			{ 0.f, 0.f, -1.f },

				// Back
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f },
			{ 0.f, 0.f, 1.f }
		};
	};
	auto CreateUVList = []()
	{
		return std::vector<XMFLOAT2>
		{
			// Left
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f },
				// Right
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f },
				// Bottom
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f },
				// Top
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f },
				// Front
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f },
				// Back
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 0.f, 1.f },
			{ 0.f, 1.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f }
		};
	};

	PrimitiveFactory::ClearAllBuffers();

	PrimitiveFactory::vertices = CreateVertexList();
	PrimitiveFactory::normals = CreateNormalsList();
	PrimitiveFactory::uvs = CreateUVList();

	Common( Specs );
}

void PrimitiveFactory::CreateCubeNM( const ModelSpecs_L & Specs )
{
	CreateCube( Specs );

	int vertCount = vertices.size();
	tangents.resize( vertCount );
	binormals.resize( vertCount );
	normals.resize( vertCount );

	// Create the tangent matrix for each face (set of three vertex positions)
	for( int i = 0; i < vertCount; i += 3 )
	{
		auto edge10 = vertices[ i + 1 ] - vertices[ i + 0 ];
		auto edge20 = vertices[ i + 2 ] - vertices[ i + 0 ];

		auto tEdge10 = uvs[ i + 1 ] - uvs[ i + 0 ];
		auto tEdge20 = uvs[ i + 2 ] - uvs[ i + 0 ];

		tangents[ i ] = CalculateTangent( edge10, tEdge10 );
		normals[ i ] = CalculateNormal( edge10, edge20 );
		binormals[ i ] = CalculateBiNormal( tangents[ i ], normals[ i ] );

		tangents[ i + 2 ] = tangents[ i + 1 ] = tangents[ i ];
		binormals[ i + 2 ] = binormals[ i + 1 ] = binormals[ i ];
		normals[ i + 2 ] = normals[ i + 1 ] = normals[ i ];
	}
}

void PrimitiveFactory::CreateMeshNM( const wstring & Filename )
{
	///////////////////////////
	// LOAD MODEL FROM FILE  //
	///////////////////////////
	int vertexCount, indexCount;

	auto VertsFromF = [ &Filename, &vertexCount, &indexCount ]()->bool
	{
		ifstream file;
		char input;
		int i;

		// Open the model file.
		file.open( Filename );

		// If it could not open the file then exit.
		if( file.fail() )
		{
			return false;
		}

		// Read up to the value of vertex count.
		file.get( input );
		while( input != ':' )
		{
			file.get( input );
		}

		// Read in the vertex count.
		file >> vertexCount;

		// Set the number of indices to be the same as the vertex count.
		indexCount = vertexCount;

		// Read up to the beginning of the data.
		file.get( input );
		while( input != ':' )
		{
			file.get( input );
		}
		file.get( input );
		file.get( input );

		// load straight into static members
		vertices.resize( vertexCount );
		uvs.resize( vertexCount );
		normals.resize( vertexCount );

		for( i = 0; i < vertexCount; i++ )
		{
			file >> vertices[ i ].x >> vertices[ i ].y >> vertices[ i ].z;
			file >> uvs[ i ].x >> uvs[ i ].y;
			file >> normals[ i ].x >> normals[ i ].y >> normals[ i ].z;
		}

		// Close the model file.
		file.close();

		return true;
	};

	auto VertsFromB = [ &Filename, &vertexCount, &indexCount ]()->bool
	{
		ifstream file( Filename, std::ios::binary ); // open file in binary mode
		if( !file.is_open() )
		{
			return false;
		}

		vertexCount = 0; // read first 32 bits into vertexCount
		file.read( reinterpret_cast<char*>( &vertexCount ), sizeof( int ) );

		// Pack into member variables of PrimFactory
		vertices.resize( vertexCount );
		uvs.resize( vertexCount );
		normals.resize( vertexCount );
		indices.resize( vertexCount );

		auto expectedSize = vertexCount * sizeof( VertexBufferTypePosUvNormal );
		auto altSize = vertexCount * sizeof( VertexBufferTypeAllInOne );
		auto curFilePosition = file.tellg();
		file.seekg( 0, std::ios::end );
		auto fileSize = file.tellg().seekpos();
		file.seekg( curFilePosition, std::ios::beg );

		fileSize -= curFilePosition;
		if( fileSize == altSize )
		{
			vector<VertexBufferTypeAllInOne> vertList( vertexCount );
			file.read( reinterpret_cast<char*>( vertList.data() ), sizeof( VertexBufferTypeAllInOne )*vertexCount );

			for( int i = 0; i < vertexCount; i++ )
			{
				vertices[ i ] = vertList[ i ].position;
				uvs[ i ] = vertList[ i ].uv;
				normals[ i ] = vertList[ i ].normal;
				indices[ i ] = i; // TODO: not sure about this
			}
		}
		else
		{
			vector<VertexBufferTypePosUvNormal> vertList( vertexCount );
			file.read( reinterpret_cast<char*>( vertList.data() ), sizeof( VertexBufferTypePosUvNormal )*vertexCount );

			for( int i = 0; i < vertexCount; i++ )
			{
				vertices[ i ] = vertList[ i ].position;
				uvs[ i ] = vertList[ i ].uv;
				normals[ i ] = vertList[ i ].normal;
				indices[ i ] = i; // TODO: not sure about this
			}
		}
		
		file.close();
		return true;
	};

	PrimitiveFactory::ClearAllBuffers();

	// TODO: Check GameView lines 60~82 for exact data it needs to load a model.
	// store file data into ModelType pModel list of verts,uvs,norms
	//bool result = VertsFromF();
	bool result = VertsFromB();

	///////////////////////////////////////  
	// LOAD MODEL DATA into vertex array //
	///////////////////////////////////////
	indexCount = vertexCount;
	indices.resize( indexCount );

	tangents.resize( vertexCount );
	binormals.resize( vertexCount );

	for( int i = 0; i < vertexCount; i += 3 )
	{
		auto edge10 = vertices[ i + 1 ] - vertices[ i ];
		auto edge20 = vertices[ i + 2 ] - vertices[ i ];
		auto tEdge10 = uvs[ i + 1 ] - uvs[ i ];
		auto tEdge20 = uvs[ i + 2 ] - uvs[ i ];

		// Calculate the denominator of the tangent/binormal equation.
		float den = 1.0f / ( 
			( tEdge10.x * tEdge20.y ) - ( tEdge10.y * tEdge20.x )
			);

		// Calculate the cross products and multiply by the coefficient to get 
		// the tangent and binormal.
		XMFLOAT3 tangent{}, binormal{};
		tangent.x = ( tEdge20.y * edge10.x - tEdge10.y * edge20.x ) * den;
		tangent.y = ( tEdge20.y * edge10.y - tEdge10.y * edge20.y ) * den;
		tangent.z = ( tEdge20.y * edge10.z - tEdge10.y * edge20.z ) * den;

		binormal.x = ( tEdge10.x * edge20.x - tEdge20.x * edge10.x ) * den;
		binormal.y = ( tEdge10.x * edge20.y - tEdge20.x * edge10.y ) * den;
		binormal.z = ( tEdge10.x * edge20.z - tEdge20.x * edge10.z ) * den;

		tangent = Normalize( tangent );
		binormal = Normalize( binormal );

		tangents[ i + 2 ] = tangents[ i + 1 ] = tangents[ i ] = tangent;
		binormals[ i + 2 ] = binormals[ i + 1 ] = binormals[ i ] = binormal;
	}

}

// Custom file format - Each line contains position vector(x, y, z), texture coordinates(tu, tv), and normal vector(nx, ny, nz). 
void PrimitiveFactory::CreateMesh(
	const wstring& fileName )
{
	////////////////////////////////  
	// LOAD MODEL FROM FILE
	////////////////////////////////
	ID3D11Buffer *pVertexBuffer, *pIndexBuffer;
	int vertexCount, indexCount;
	ModelType* pModel;

	auto VertsFromF = [ &fileName, &vertexCount, &indexCount, &pModel ]()->bool
	{
		ifstream fin;
		char input;
		int i;

		// Open the model file.
		fin.open( fileName );

		// If it could not open the file then exit.
		if( fin.fail() )
		{
			return false;
		}

		// Read up to the value of vertex count.
		fin.get( input );
		while( input != ':' )
		{
			fin.get( input );
		}

		// Read in the vertex count.
		fin >> vertexCount;

		// Set the number of indices to be the same as the vertex count.
		indexCount = vertexCount;

		// Create the model using the vertex count that was read in.
		pModel = new ModelType[ vertexCount ];
		if( !pModel )
		{
			return false;
		}

		// Read up to the beginning of the data.
		fin.get( input );
		while( input != ':' )
		{
			fin.get( input );
		}
		fin.get( input );
		fin.get( input );

		// load straight into static members
		vertices.resize( vertexCount );
		uvs.resize( vertexCount );
		normals.resize( vertexCount );

		for( i = 0; i < vertexCount; i++ )
		{
			fin >> vertices[ i ].x >> vertices[ i ].y >> vertices[ i ].z;
			fin >> uvs[ i ].x >> uvs[ i ].y;
			fin >> normals[ i ].x >> normals[ i ].y >> normals[ i ].z;
		}

		// Close the model file.
		fin.close();

		return true;
	};

	auto VertsFromB = [ &fileName, &vertexCount, &indexCount, &pModel ]()->bool
	{
		ifstream file( fileName, std::ios::binary ); // open file in binary mode

		if( file.fail() )
		{
			return false;
		}

		vertexCount = 0; // read first 32 bits into vertexCount
		file.read( reinterpret_cast<char*>( &vertexCount ), sizeof( int ) );

		// pack into vector of structs
		// TODO: change OBJ to BINARY LOADER
		vector<VertexBufferTypePosUvNormal> vertList( vertexCount );
		file.read( reinterpret_cast<char*>( vertList.data() ), sizeof( VertexBufferTypeAllInOne )*vertexCount );


		// Pack into member variables of PrimFactory
		vertices.resize( vertexCount );
		uvs.resize( vertexCount );
		normals.resize( vertexCount );
		indices.resize( vertexCount );

		for( int i = 0; i < vertexCount; i++ )
		{
			vertices[ i ] = vertList[ i ].position;
			uvs[ i ] = vertList[ i ].uv;
			normals[ i ] = vertList[ i ].normal;
			indices[ i ] = i; // TODO: not sure about this
		}

		file.close();
		return true;
	};

	PrimitiveFactory::ClearAllBuffers();

	// TODO: Check GameView lines 60~82 for exact data it needs to load a model.
	// store file data into ModelType pModel list of verts,uvs,norms
	//bool result = VertsFromF();
	bool result = VertsFromB();

	///////////////////////////////////////  
	// LOAD MODEL DATA into vertex array //
	///////////////////////////////////////
	indexCount = vertexCount;
	indices.resize( indexCount );

	for( int i = 0; i < indexCount; i++ )
	{
		indices[ i ] = i;
	}
}

void PrimitiveFactory::CreateColor( float R, float G, float B, float A )
{
	color = DirectX::XMFLOAT4( R, G, B, A );
}

std::vector<DirectX::XMFLOAT3> PrimitiveFactory::GetVertices()
{
	return PrimitiveFactory::vertices;
}

std::vector<DirectX::XMFLOAT3> PrimitiveFactory::GetNormals()
{
	return PrimitiveFactory::normals;
}

std::vector<DirectX::XMFLOAT2> PrimitiveFactory::GetUVs()
{
	return PrimitiveFactory::uvs;
}

std::vector<DirectX::XMFLOAT3> PrimitiveFactory::GetTangent()
{
	return tangents;
}

std::vector<DirectX::XMFLOAT3> PrimitiveFactory::GetBiTangent()
{
	return binormals;
}

std::vector<DWORD> PrimitiveFactory::GetIndices()
{
	return PrimitiveFactory::indices;
}

DirectX::XMFLOAT4 PrimitiveFactory::GetColor()
{
	return PrimitiveFactory::color;
}

XMFLOAT3 PrimitiveFactory::CalculateTangent(
	const XMFLOAT3 &Edge10,
	const XMFLOAT2& tEdge10 )
{
	return Normalize( Edge10 / tEdge10.x );
}

XMFLOAT3 PrimitiveFactory::CalculateNormal(
	const XMFLOAT3 &Edge10,
	const XMFLOAT3 &Edge20 )
{
	return Normalize( CrossProduct( Edge10, Edge20 ) );
}

XMFLOAT3 PrimitiveFactory::CalculateBiNormal( const XMFLOAT3 & Tangent, const XMFLOAT3 & Normal )
{
	return CrossProduct( Tangent, Normal );
}

void PrimitiveFactory::Common( const ModelSpecs_L & Specs )
{
	PrimitiveFactory::indices.resize( vertices.size() );
	for( int i = 0; i < vertices.size(); ++i )
	{
		indices[ i ] = i;
	}

	// Load the orienation into SIMD register and create the rotation matrix
	auto rotationVector = ConvertToRadians( XMLoadFloat3( &Specs.orientation ) );
	auto rotationMatrix = XMMatrixRotationRollPitchYawFromVector( rotationVector );

	// Rotate each vertex
	for( int i = 0; i < vertices.size(); ++i )
	{
		auto &vertex = vertices[ i ];
		auto &normal = normals[ i ];

		auto xmPosition = XMLoadFloat3( &vertex );
		auto xmNormal = XMLoadFloat3( &normal );

		auto rotatedPosition = XMVector3Transform( xmPosition, rotationMatrix );
		auto rotatedNormal = XMVector3Transform( xmNormal, rotationMatrix );

		XMStoreFloat3( &vertex, rotatedPosition );
		XMStoreFloat3( &normal, rotatedNormal );
	}
}

void PrimitiveFactory::ClearAllBuffers()
{
	PrimitiveFactory::vertices.clear();
	PrimitiveFactory::normals.clear();
	PrimitiveFactory::uvs.clear();
	PrimitiveFactory::indices.clear();
	PrimitiveFactory::tangents.clear();
	PrimitiveFactory::binormals.clear();
}

std::vector<DirectX::XMFLOAT3> PrimitiveFactory::vertices;
std::vector<DirectX::XMFLOAT3> PrimitiveFactory::normals;
std::vector<DirectX::XMFLOAT2> PrimitiveFactory::uvs;
std::vector<DirectX::XMFLOAT3> PrimitiveFactory::tangents;
std::vector<DirectX::XMFLOAT3> PrimitiveFactory::binormals;

std::vector<DWORD> PrimitiveFactory::indices;
DirectX::XMFLOAT4 PrimitiveFactory::color;