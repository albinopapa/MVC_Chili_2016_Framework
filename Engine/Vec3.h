#pragma once

class Vec2;

class Vec3
{
public:
	Vec3();
	Vec3( const Vec2 &V, float Z = 1.f );
	Vec3( float X, float Y, float Z );
	~Vec3();

	float DotProduct( const Vec3 &V )const;
public:
	float x, y, z;
};

