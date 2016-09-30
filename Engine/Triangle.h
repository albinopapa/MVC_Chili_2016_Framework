#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Colors.h"


struct BarycentricCoord
{
	float x, y, z;
};


class Triangle
{
public:
	Triangle() = default;
	~Triangle() = default;

	BarycentricCoord CalculateBarycentricCoordinates( const Vec2 &PixPos )const;
	BarycentricCoord CalculateBarycentricCoordinatesAlt( const Vec3 &PixPos )const;
public:
	Vec3 a, b, c;
	Vec2 ta, tb, tc;
	Vec3 na, nb, nc;
	Color color;
};

