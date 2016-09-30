#pragma once
#include "Vec3.h"
#include <DirectXMath.h>

class Vec4
{
public:
	Vec4();
	Vec4( const Vec3& V, float W = 1.f );
	Vec4( const DirectX::XMFLOAT3 &V, float W = 1.f );
	Vec4( float X, float Y, float Z, float W = 1.f );
	~Vec4();

	Vec4 operator+( const Vec4 &V )const;
	Vec4 operator-( const Vec4 &V )const;
	Vec4 operator*( const float S )const;
	Vec4 operator/( const float S )const;

	Vec4 &operator+=( const Vec4 &V );

	float DotProduct( const Vec4 &V )const;
	Vec4 CrossProduct( const Vec4 &V )const;
	Vec4 Normalize();


	float x, y, z, w;
};

Vec4 Saturate( const Vec4 &V );