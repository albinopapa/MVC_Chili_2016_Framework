#pragma once

#include "Vec4.h"

class Mat4
{
public:
	Mat4();
	Mat4( const Vec4 &R0, const Vec4 &R1, const Vec4 &R2, const Vec4 &R3 );
	~Mat4();

	Mat4 operator*( const Mat4 &M )const;
	Vec3 operator*( const Vec3 &V )const;

	Mat4 Transpose()const;

	static Mat4 Identity();
	static Mat4 Translation( const Vec3 &V );
	static Mat4 Scale( const Vec3 &V );
	static Mat4 RotateX( const float Theta );
	static Mat4 RotateY( const float Theta );
	static Mat4 RotateZ( const float Theta );
	static Mat4 Rotate( const Vec3 &V, const float Theta );


	Vec4 r[ 4 ];
};

