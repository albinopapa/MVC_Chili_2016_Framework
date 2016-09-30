#pragma once
#include <DirectXMath.h>
class Vec2;

class Vec3
{
public:
	Vec3();
	Vec3( const Vec2 &V, float Z = 1.f );
	Vec3( float X, float Y, float Z );
	~Vec3();

	Vec3 &operator=( const DirectX::XMFLOAT3 &V );

	Vec3 operator+( const Vec3 &V )const;
	Vec3 operator-()const;
	Vec3 operator-( const Vec3 &V )const;
	Vec3 operator*( const float S )const;
	Vec3 operator/( const float S )const;

	float DotProduct( const Vec3 &V )const;
	Vec3 CrossProduct( const Vec3 &V )const;
	Vec3 Normalize();
public:
	float x, y, z;
};

