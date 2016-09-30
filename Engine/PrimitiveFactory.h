/***********************************************************************************
File: PrimitiveFactory.cpp
Function: Create basic 3d primitives using vector math. Can be combined with 
texture functionality for more detailed graphics.

***************************************************************************************/#pragma once

#include "Includes.h"
#include "Utilities.h"
#include <fstream>
#include <string>

using namespace std;

// Contains position, texture, and normal vectors 
// corresponding to mesh file format
	struct ModelType
	{
		float x, y, z; // vertex coordinates
		float tu, tv; // UV coordinates
		float nx, ny, nz; // normal vector
	};

 //   struct VertexType
	//{
	//	XMFLOAT3 position;
	//	XMFLOAT2 texture;
	//	XMFLOAT3 normal;
	//};

class PrimitiveFactory
{
    //// prevent making instances of this class
    //PrimitiveFactory() = delete;

public:
	// TODO: Should we keep as is, or use ModelSpecs_L struct instead to
	// TODO: to initialize these primitives?
    // Take center, size, and orientation of primitive to be created
	static void CreateTriangle( const ModelSpecs_L &Specs );
	static void CreatePlane( const ModelSpecs_L &Specs );
	static void CreatePlaneNM( const ModelSpecs_L &Specs );
	static void CreateCube( const ModelSpecs_L &Specs );
	static void CreateCubeNM( const ModelSpecs_L &Specs );
	static void CreateMeshNM( const wstring&Filename );
	static void PrimitiveFactory::CreateColor( float R, float G, float B, float A );
    static void PrimitiveFactory::CreateMesh(
        //const ModelSpecs_L & Specs, 
        //const ModelType & type, 
        const wstring& fileName);

    // ACCESSOR FUNCTIONS
	static std::vector<DirectX::XMFLOAT3> GetVertices();
	static std::vector<DirectX::XMFLOAT3> GetNormals();
	static std::vector<DirectX::XMFLOAT2> GetUVs();
	static std::vector<DirectX::XMFLOAT3> GetTangent();
	static std::vector<DirectX::XMFLOAT3> GetBiTangent();
	static std::vector<DWORD> GetIndices();
    static DirectX::XMFLOAT4 GetColor();

private:
	static XMFLOAT3 CalculateTangent( 
		const XMFLOAT3 &Edge10, 
		const XMFLOAT2& tEdge10 );
	static XMFLOAT3 CalculateNormal(
		const XMFLOAT3 &Edge10,
		const XMFLOAT3 &Edge20 );
	static XMFLOAT3 CalculateBiNormal(
		const XMFLOAT3 &Tangent,
		const XMFLOAT3 &Normal );
	static void Common( const ModelSpecs_L &Specs );
	static void ClearAllBuffers();
private:
	 static std::vector<DirectX::XMFLOAT3> vertices;
	 static std::vector<DirectX::XMFLOAT3> normals;
	 static std::vector<DirectX::XMFLOAT2> uvs; 
	 // Tangent runs parallel to surface in the U coord of texture 
	 // coordinates UV
	 static std::vector<DirectX::XMFLOAT3> tangents;	  
	 // Suppose to be called binormal, runs parallel to surface in the V
	 // coord of the texture coordinates UV
	 static std::vector<DirectX::XMFLOAT3> binormals;

	 static std::vector<DWORD> indices;
   	 static DirectX::XMFLOAT4 color;
};

