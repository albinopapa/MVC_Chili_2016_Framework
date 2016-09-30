#pragma once

#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

using namespace DirectX;

// Permit me to be a little lazy :)
// This is just an alias to shorten the amount of typing
template<class T>
using comptr = Microsoft::WRL::ComPtr<T>;

// Helper macros to decrease the amount of typing and space
#define RETURN_IF_FALSE(Result)\
if( !( Result ) ) \
{ \
	ShowCursor(TRUE);\
	return false; \
}\
else int a = 0
#define RETURN_MESSAGE_IF_FALSE(Result, Message)\
if(!(Result))\
{\
	ShowCursor(TRUE);\
	MessageBox(nullptr, Message, L"Error!", MB_OK);\
	return false;\
}\
else int a = 0
#define RETURN_IF_FAILED(Result) RETURN_IF_FALSE( SUCCEEDED( ( Result ) ) )

////////////////////////////////////////////////////
// Vector Math Operator Overloads
///////////////////////////////////////////////////
// Add more when/if needed
XMFLOAT2 operator+( const XMFLOAT2 &V1, const XMFLOAT2 &V2 );
XMFLOAT2 operator-( const XMFLOAT2 &V );
XMFLOAT2 operator-( const XMFLOAT2 &V1, const XMFLOAT2 &V2 );

// Keep operator overloads out of namespace so we can access them
// without having to declare: using namespace
XMFLOAT3 operator+( const XMFLOAT3 &V1, const XMFLOAT3& V2 );
XMFLOAT3 operator-( const XMFLOAT3 &V1, const XMFLOAT3& V2 );
XMFLOAT3 operator*( const XMFLOAT3 &V, const float S );
XMFLOAT3 operator/( const XMFLOAT3 &V, const float S );
XMFLOAT3 &operator+= ( XMFLOAT3& V1, const XMFLOAT3& V2 );
XMFLOAT3 &operator-= ( XMFLOAT3& V1, const XMFLOAT3& V2 );
XMFLOAT3 &operator*= ( XMFLOAT3 &V, const float S );
XMFLOAT3 &operator/= ( XMFLOAT3 &V, const float S );

// PI is equal to 180 degrees
constexpr float g_PI = 3.141592654f;

// TwoPI is equal to 360 degrees
constexpr float g_TwoPI = 2.f * g_PI;

// PIDivFour is equal to 45 degrees
constexpr float g_PIDivFour = g_PI / 4.f;

// PIDivTwo is equal to 90 degrees
constexpr float g_PIDivTwo = g_PI / 2.f;

// Radian is 0.01745329238f degrees, multiply angle in degrees by radian 
// to convert degrees to radians for rotation functions
constexpr float g_radian = g_PI / 180.f;

// Degree is 1 radian, 180 / PI, multiply angle in radians by degree to 
// convert radians to degrees
constexpr float g_degree = 180.f / g_PI;

// Calculates the cross-product of two float3 vectors
XMFLOAT3 CrossProduct( const XMFLOAT3 &V1, const XMFLOAT3 &V2 );

// Calculates the dot-product of two float3 vectors
float DotProduct( const XMFLOAT3 &V1, const XMFLOAT3 &V2 );

// Calculates the magnitude of a float3 vector
float Magnitude( const XMFLOAT3 &V );

// Calcualtes the length between two float3 point vectors
float Length( const XMFLOAT3 &V1, const XMFLOAT3 &V2 );

// Normalizes a float3 vector
XMFLOAT3 Normalize( const XMFLOAT3 &V );

XMVECTOR ConvertToRadians( const XMVECTOR& angleInDegrees );

XMFLOAT3 ConvertToRadians( const XMFLOAT3& angleInDegrees );

/////////////////////////////
// CLAMP FUNCTION
// (for clamping angles)
/////////////////////////////
// NOTE: The intent may have been for angles, but this can be 
// used for clamping in any context, it's a pretty generic function
template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

////////////////////////////////////////////////////////////////
// STRUCTS CONTAINING MODEL INFO for gameplay and rendering
// TODO: make it global; see which is faster
////////////////////////////////////////////////////////////////
struct ModelSpecs_W
{
	XMFLOAT3 position, orientation, scale;
};

struct ModelSpecs_L
{
	ModelSpecs_L(
		const XMFLOAT3 &Center = { 0.f, 0.f, 0.f },
		const XMFLOAT3 &Orientation = { 0.f,0.f,0.f },
		const XMFLOAT3 &Size = { 5.f, 5.f, 5.f } );

	XMFLOAT3 center;
	XMFLOAT3 orientation;
   	XMFLOAT3 size;
};

// Common vertex buffer types and corresponding input element descriptions
constexpr DWORD appendAlignment = D3D11_APPEND_ALIGNED_ELEMENT;
constexpr DXGI_FORMAT float2Format = DXGI_FORMAT_R32G32_FLOAT;
constexpr DXGI_FORMAT float3Format = DXGI_FORMAT_R32G32B32_FLOAT;
constexpr DXGI_FORMAT float4Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
constexpr D3D11_INPUT_CLASSIFICATION vertexDataType = D3D11_INPUT_PER_VERTEX_DATA;

struct VertexBufferTypePosUvNormal
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
};

struct SoaVertexBuffer
{
	unsigned vertCount = 0;
	float *px, *py, *pz;
	float *tu, *tv;
	float *nx, *ny, *nz;

	// Used for normal mapping, not currently implemented.
	float *tx, *ty, *tz;
	float *bx, *by, *bz;
};

struct VertexBufferTypeAllInOne
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;

	// Input layout descriptions for position, texcoord, normals and color
	static std::vector<D3D11_INPUT_ELEMENT_DESC> CreateLayoutDescriptions();

};

struct VertexBufferTypeAllInOneNMap
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMFLOAT3 normal;

	// Input layout descriptions for position, texcoord, normals and color
	static std::vector<D3D11_INPUT_ELEMENT_DESC> CreateLayoutDescriptions();
};

// Use these light types so the shader can determine which light calculations
// need to be done and what values it can use.
enum eLightType
{
	INFINITE_LIGHT, POINT_LIGHT, SPOT_LIGHT
};
// Use this one type for ALL different lights
struct LightBufferType
{
	XMFLOAT4 lightColor;
   	XMFLOAT3 lightPosition;
    float coneAngle;  
    XMFLOAT3 lightDirection;	
	eLightType type;

    static D3D11_BUFFER_DESC CreateLightDescription( unsigned int MaxLightCount );
};

// This must match the number of lights in the shader (hlsl) array or
// be less than it.  Can/Will be used to make sure we don't try copying
// more light data than shader is meant to process.
#define MAX_SHADER_LIGHTS 10
struct SceneBufferType
{
	XMFLOAT4 ambientColor;
	int lightCount;
	XMFLOAT3 padding;
	LightBufferType lights[ MAX_SHADER_LIGHTS ];

	static D3D11_BUFFER_DESC CreateLightDescription();
};

D3D11_SAMPLER_DESC CreateSamplerDescription();
