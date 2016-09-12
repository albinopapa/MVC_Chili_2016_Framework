#pragma once

class Vec2
{
public:
	Vec2();
	Vec2( float X, float Y );
	~Vec2();
	
	Vec2 operator+( const Vec2 &V )const;
	Vec2 &operator+=( const Vec2 &V );
	Vec2 operator-( const Vec2 &V )const;
	Vec2 operator*( const float S )const;
	Vec2 &operator*=( const float S );
	float DotProduct( const Vec2 &V )const;
	float Length()const;
	Vec2 Normalize()const;
	// Not correctly defined
	Vec2 CrossProduct( const Vec2 &V )const;

public:
	float x, y;
};

