#pragma once

#include "Vec2.h"
#include "Vec3.h"

class Mat3
{
public:
	Mat3();
	Mat3( const Vec3 &R0, const Vec3 &R1, const Vec3 &R2 );	
	~Mat3();
	
	Mat3 operator*( const Mat3 &M )const;
	Vec2 operator*( const Vec2 &V )const;

	Mat3 Transpose()const;

	static Mat3 Identity();
	static Mat3 Translation( const Vec2 &V );
	static Mat3 Scale( const Vec2 &V );
	static Mat3 Rotation( const float Theta );

public:
	Vec3 r[ 3 ];
};

