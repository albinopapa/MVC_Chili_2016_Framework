#include "Triangle.h"

BarycentricCoord Triangle::CalculateBarycentricCoordinates( const Vec2 &PixPos )const
{
	float x1_Sub_X3 = ( a.x - c.x );
	float y1_Sub_Y3 = ( a.y - c.y );
	float y2_Sub_Y3 = ( b.y - c.y );
	float x3_Sub_X2 = ( c.x - b.x );
	float y3_Sub_Y1 = ( c.y - a.y );

	float x0_Sub_X3 = ( PixPos.x - c.x );
	float y0_Sub_Y3 = ( PixPos.y - c.y );

	float denominator = x1_Sub_X3 * y2_Sub_Y3 + x3_Sub_X2 * y3_Sub_Y1;

	float lambda0 = ( ( y2_Sub_Y3 * x0_Sub_X3 ) + ( x3_Sub_X2 * y0_Sub_Y3 ) ) / denominator;
	float lambda1 = ( ( y3_Sub_Y1 * x0_Sub_X3 ) + ( x1_Sub_X3 * y0_Sub_Y3 ) ) / denominator;
	float lambda2 = ( 1.f - lambda1 - lambda0 );
	return{ lambda0, lambda1, lambda2 };

}
