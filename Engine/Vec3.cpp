#include "Vec3.h"
#include "Vec2.h"
#include <math.h>

Vec3::Vec3()
{}

Vec3::Vec3( const Vec2 & V, float Z )
	:
	x( V.x ),
	y( V.y ),
	z( Z )
{}

Vec3::Vec3( float X, float Y, float Z )
	:
	x(X),
	y(Y),
	z(Z)
{}


Vec3::~Vec3()
{}

Vec3 & Vec3::operator=( const DirectX::XMFLOAT3 & V )
{
	x = V.x;
	y = V.y;
	z = V.z;

	return *this;
}

Vec3 Vec3::operator+( const Vec3 & V ) const
{
	return Vec3( x + V.x, y + V.y, z + V.z );
}

Vec3 Vec3::operator-() const
{
	return Vec3( -x, -y, -z );
}

Vec3 Vec3::operator-( const Vec3 & V ) const
{
	return Vec3( x - V.x, y - V.y, z - V.z );
}

Vec3 Vec3::operator*( const float S ) const
{
	return Vec3( x * S, y * S, z * S );
}

Vec3 Vec3::operator/( const float S ) const
{
	float invScalar = 1.f / S;
	return *this * invScalar;
}

float Vec3::DotProduct( const Vec3 & V )const
{
	return ( ( x * V.x ) + ( y * V.y ) + ( z * V.z ) );
}

Vec3 Vec3::CrossProduct( const Vec3 & V ) const
{
	return{
		( y * V.z ) - ( z * V.y ),
		( z * V.x ) - ( x * V.z ),
		( x * V.y ) - ( y * V.x )
	};
}

Vec3 Vec3::Normalize()
{
	float recLen = 1.f / sqrtf( DotProduct( *this ) );
	return *this * recLen;
}
