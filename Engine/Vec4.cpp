#include "Vec4.h"
#include <math.h>
#include <algorithm>

Vec4::Vec4()
{}

Vec4::Vec4( const Vec3 & V, float W )
	:
	x( V.x ),
	y( V.y ),
	z( V.z ),
	w( W )
{}

Vec4::Vec4( const DirectX::XMFLOAT3 & V, float W )
	:
	x( V.x ),
	y( V.y ),
	z( V.z ),
	w( W )
{}

Vec4::Vec4( float X, float Y, float Z, float W )
	:
	x( X ),
	y( Y ),
	z( Z ),
	w( W )
{}


Vec4::~Vec4()
{}

Vec4 Vec4::operator+( const Vec4 & V ) const
{
	return Vec4( x + V.x, y + V.y, z + V.z, w + V.w );
}

Vec4 Vec4::operator-( const Vec4 & V ) const
{
	return Vec4( x - V.x, y - V.y, z - V.z, w - V.w );
}

Vec4 Vec4::operator*( const float S ) const
{
	return Vec4( x * S, y * S, z * S, w * S );
}

Vec4 Vec4::operator/( const float S ) const
{
	float recScalar = 1.f / S;
	return *this * recScalar;
}

Vec4 & Vec4::operator+=( const Vec4 & V )
{
	x += V.x;
	y += V.y;
	z += V.z;
	w += V.w;

	return *this;
}

float Vec4::DotProduct( const Vec4 & V ) const
{
	return (
		( x * V.x ) +
		( y * V.y ) +
		( z * V.z ) +
		( w * V.w )
		);
}

Vec4 Vec4::CrossProduct( const Vec4 & V ) const
{
	return{
		( y * V.z ) - ( z * V.y ),
		( z * V.w ) - ( w * V.z ),
		( w * V.x ) - ( x * V.w ),
		( x * V.y ) - ( y * V.x )
	};
}

Vec4 Vec4::Normalize()
{
	float recLen = 1.f / sqrtf( DotProduct( *this ) );
	return *this * recLen;
}

Vec4 Saturate( const Vec4 & V )
{	
	return{
		std::min( 1.f, std::max( 0.f, V.x ) ),
		std::min( 1.f, std::max( 0.f, V.y ) ),
		std::min( 1.f, std::max( 0.f, V.z ) ),
		std::min( 1.f, std::max( 0.f, V.w ) )
	};
}
