#include "Triangle.h"
#include <math.h>

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

BarycentricCoord Triangle::CalculateBarycentricCoordinatesAlt( const Vec3 & PixPos ) const
{
	auto &p = PixPos;

	auto bp = b - p;
	auto cp = c - p;
	auto ap = a - p;
	auto cb = c - b;
	auto ac = a - c;
	auto ba = b - a;

	auto cpResultBACB = ba.CrossProduct( cb );
	float area = sqrtf( cpResultBACB.DotProduct( cpResultBACB ) );

	auto cpResultCBP = bp.CrossProduct( cb );
	auto cpResultACP = cp.CrossProduct( ac );
	auto cpResultBAP = ap.CrossProduct( ba );

	auto sqAreaCBP = sqrtf( cpResultCBP.DotProduct( cpResultCBP ) );
	auto sqAreaACP = sqrtf( cpResultACP.DotProduct( cpResultACP ) );
	auto sqAreaBAP = sqrtf( cpResultBAP.DotProduct( cpResultBAP ) );

	auto lambdaA = static_cast<float>( copysign( sqAreaCBP, cpResultCBP.z ) / area );
	auto lambdaB = static_cast<float>( copysign( sqAreaACP, cpResultACP.z ) / area );
	auto lambdaC = static_cast<float>( copysign( sqAreaBAP, cpResultBAP.z ) / area );

	return{ lambdaA, lambdaB, lambdaC };
}
