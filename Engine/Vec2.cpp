#include "Vec2.h"

#include <math.h>

Vec2::Vec2()
	:
	x( 0.f ),
	y( 0.f )
{}

Vec2::Vec2( float X, float Y )
	:
	x(X),
	y(Y)
{}


Vec2::~Vec2()
{}

Vec2 Vec2::operator+( const Vec2 & V ) const
{
	return Vec2(x + V.x, y + V.y);
}

Vec2 & Vec2::operator+=( const Vec2 & V )
{
	*this = *this + V;
	return *this;
}

Vec2 Vec2::operator-( const Vec2 & V ) const
{
	return Vec2( x - V.x, y - V.y );
}

Vec2 Vec2::operator*( const float S ) const
{
	return Vec2( x * S, y * S );
}

Vec2 & Vec2::operator*=( const float S )
{
	*this = *this * S;
	return *this;
}

float Vec2::DotProduct( const Vec2 & V ) const
{
	return ( ( x * V.x ) + ( y * V.y ) );
}

float Vec2::Length() const
{
	return sqrtf( DotProduct( *this ) );
}

Vec2 Vec2::Normalize() const
{
	float invLength = 1.f / Length();
	return *this * invLength;
}

Vec2 Vec2::CrossProduct( const Vec2 & V ) const
{
	return
	{
		( y * V.x ) - ( x * V.y ),
		0.f
	};
}
