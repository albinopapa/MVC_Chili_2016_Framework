#pragma once

#include "Vec2.h"
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
public:
	Vec2 a, b, c;
	Color color;
};

