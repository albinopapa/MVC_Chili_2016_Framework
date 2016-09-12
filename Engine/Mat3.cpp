#include "Mat3.h"

#include <math.h>

Mat3::Mat3()
{}

Mat3::Mat3( const Vec3 & R0, const Vec3 & R1, const Vec3 & R2 )
{
	r[ 0 ] = R0;
	r[ 1 ] = R1;
	r[ 2 ] = R2;
}


Mat3::~Mat3()
{}

Mat3 Mat3::operator*( const Mat3 & M )const
{
	Mat3 m( M.Transpose() );
	Mat3 t;

	for( int j = 0; j < 3; ++j )
	{
		t.r[ j ].x = m.r[ 0 ].DotProduct( r[ j ] );
		t.r[ j ].y = m.r[ 1 ].DotProduct( r[ j ] );
		t.r[ j ].z = m.r[ 2 ].DotProduct( r[ j ] );
	}

	return t;
}

Vec2 Mat3::operator*( const Vec2 & V ) const
{
	Vec3 v( V );
	Mat3 m( Transpose() );	
	
	return{
		v.DotProduct( m.r[ 0 ] ),
		v.DotProduct( m.r[ 1 ] )
	};
}

Mat3 Mat3::Identity()
{
	return{
		{ 1.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 1.f }
	};
}

Mat3 Mat3::Transpose()const
{
	return{
		{ r[ 0 ].x, r[ 1 ].x, r[ 2 ].x },
		{ r[ 0 ].y, r[ 1 ].y, r[ 2 ].y },
		{ r[ 0 ].z, r[ 1 ].z, r[ 2 ].z }
	};
}

Mat3 Mat3::Translation( const Vec2 & V )
{	
	return{
		{ 1.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f },
		{ V }
	};
}

Mat3 Mat3::Scale( const Vec2 & V )
{
	return{
		{ V.x, 0.f, 0.f },
		{ 0.f, V.y, 0.f },
		{ 0.f, 0.f, 1.f }
	};
}

Mat3 Mat3::Rotation( const float Theta )
{
	float cosine = cosf( Theta );
	float sine = sinf( Theta );
		
	return{
		{ cosine, -sine, 0.f },
		{ sine, cosine, 0.f },
		{ 0.f, 0.f, 1.f }
	};
}
