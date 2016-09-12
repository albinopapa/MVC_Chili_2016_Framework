#include "Vec3.h"
#include "Vec2.h"


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

float Vec3::DotProduct( const Vec3 & V )const
{
	return ( ( x * V.x ) + ( y * V.y ) + ( z * V.z ) );
}
