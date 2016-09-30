#include "Mat4.h"



Mat4::Mat4()
{}

Mat4::Mat4( const Vec4 & R0, const Vec4 & R1, const Vec4 & R2, const Vec4 & R3 )
{
	r[ 0 ] = R0;
	r[ 1 ] = R1;
	r[ 2 ] = R2;
	r[ 3 ] = R3;
}


Mat4::~Mat4()
{}

Mat4 Mat4::operator*( const Mat4 & M ) const
{
	Mat4 t;

	// Row 0
	t.r[ 0 ].x = 
		( r[ 0 ].x * M.r[ 0 ].x ) +
		( r[ 0 ].y * M.r[ 1 ].x ) +
		( r[ 0 ].z * M.r[ 2 ].x ) +
		( r[ 0 ].w * M.r[ 3 ].x );

	t.r[ 0 ].y = 
		( r[ 1 ].x * M.r[ 0 ].x ) +
		( r[ 1 ].y * M.r[ 1 ].x ) +
		( r[ 1 ].z * M.r[ 2 ].x ) +
		( r[ 1 ].w * M.r[ 3 ].x );

	t.r[ 0 ].z = 
		( r[ 2 ].x * M.r[ 0 ].x ) +
		( r[ 2 ].y * M.r[ 1 ].x ) +
		( r[ 2 ].z * M.r[ 2 ].x ) +
		( r[ 2 ].w * M.r[ 3 ].x );

	t.r[ 0 ].w = 
		( r[ 3 ].x * M.r[ 0 ].x ) +
		( r[ 3 ].y * M.r[ 1 ].x ) +
		( r[ 3 ].z * M.r[ 2 ].x ) +
		( r[ 3 ].w * M.r[ 3 ].x );

	// Row 1
	t.r[ 1 ].x =
		( r[ 0 ].x * M.r[ 0 ].y ) +
		( r[ 0 ].y * M.r[ 1 ].y ) +
		( r[ 0 ].z * M.r[ 2 ].y ) +
		( r[ 0 ].w * M.r[ 3 ].y );

	t.r[ 1 ].y = 
		( r[ 1 ].x  * M.r[ 0 ].y) + 
		( r[ 1 ].y  * M.r[ 1 ].y) + 
		( r[ 1 ].z  * M.r[ 2 ].y) + 
		( r[ 1 ].w  * M.r[ 3 ].y);

	t.r[ 1 ].z = 
		( r[ 2 ].x * M.r[ 0 ].y ) + 
		( r[ 2 ].y * M.r[ 1 ].y ) + 
		( r[ 2 ].z * M.r[ 2 ].y ) + 
		( r[ 2 ].w * M.r[ 3 ].y );

	t.r[ 1 ].w = 
		( r[ 3 ].x * M.r[ 0 ].y ) + 
		( r[ 3 ].y * M.r[ 1 ].y ) + 
		( r[ 3 ].z * M.r[ 2 ].y ) + 
		( r[ 3 ].w * M.r[ 3 ].y );

	// Row 2
	t.r[ 2 ].x = 
		( r[ 0 ].x  * M.r[ 0 ].z ) + 
		( r[ 0 ].y  * M.r[ 1 ].z ) + 
		( r[ 0 ].z  * M.r[ 2 ].z ) + 
		( r[ 0 ].w  * M.r[ 3 ].z );

	t.r[ 2 ].y = 
		( r[ 1 ].x * M.r[ 0 ].z ) + 
		( r[ 1 ].y * M.r[ 1 ].z ) + 
		( r[ 1 ].z * M.r[ 2 ].z ) + 
		( r[ 1 ].w * M.r[ 3 ].z );

	t.r[ 2 ].z = 
		( r[ 2 ].x * M.r[ 0 ].z ) + 
		( r[ 2 ].y * M.r[ 1 ].z ) + 
		( r[ 2 ].z * M.r[ 2 ].z ) + 
		( r[ 2 ].w * M.r[ 3 ].z );

	t.r[ 2 ].w = 
		( r[ 3 ].x * M.r[ 0 ].z ) + 
		( r[ 3 ].y * M.r[ 1 ].z ) + 
		( r[ 3 ].z * M.r[ 2 ].z ) + 
		( r[ 3 ].w * M.r[ 3 ].z );

	// Row 3
	t.r[ 3 ].x = 
		( r[ 0 ].x * M.r[ 0 ].w ) + 
		( r[ 0 ].y * M.r[ 1 ].w ) + 
		( r[ 0 ].z * M.r[ 2 ].w ) + 
		( r[ 0 ].w * M.r[ 3 ].w );

	t.r[ 3 ].y = 
		( r[ 1 ].x * M.r[ 0 ].w ) + 
		( r[ 1 ].y * M.r[ 1 ].w ) + 
		( r[ 1 ].z * M.r[ 2 ].w ) + 
		( r[ 1 ].w * M.r[ 3 ].w );

	t.r[ 3 ].z = 
		( r[ 2 ].x * M.r[ 0 ].w ) + 
		( r[ 2 ].y * M.r[ 1 ].w ) + 
		( r[ 2 ].z * M.r[ 2 ].w ) + 
		( r[ 2 ].w * M.r[ 3 ].w );

	t.r[ 3 ].w = 
		( r[ 3 ].x * M.r[ 0 ].w ) + 
		( r[ 3 ].y * M.r[ 1 ].w ) + 
		( r[ 3 ].z * M.r[ 2 ].w ) + 
		( r[ 3 ].w * M.r[ 3 ].w );

	return t;
}

Vec3 Mat4::operator*( const Vec3 & V ) const
{
	return{
		( r[ 0 ].x * V.x ) + ( r[ 0 ].y * V.x ) + ( r[ 0 ].z * V.x ) + r[ 0 ].w,
		( r[ 1 ].x * V.y ) + ( r[ 1 ].y * V.y ) + ( r[ 1 ].z * V.y ) + r[ 1 ].w,
		( r[ 2 ].x * V.z ) + ( r[ 2 ].y * V.z ) + ( r[ 2 ].z * V.z ) + r[ 2 ].w
	};
}

Mat4 Mat4::Transpose() const
{
	return Mat4(
	{ r[ 0 ].x, r[ 1 ].x, r[ 2 ].x, r[ 3 ].x },
	{ r[ 0 ].y, r[ 1 ].y, r[ 2 ].y, r[ 3 ].y },
	{ r[ 0 ].z, r[ 1 ].z, r[ 2 ].z, r[ 3 ].z },
	{ r[ 0 ].w, r[ 1 ].w, r[ 2 ].w, r[ 3 ].w }
	);
}

Mat4 Mat4::Identity()
{
	return Mat4(
	{ 1.f, 0.f, 0.f, 0.f },
	{ 0.f, 1.f, 0.f, 0.f },
	{ 0.f, 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f, 1.f }
	);
}

Mat4 Mat4::Translation( const Vec3 & V )
{
	auto temp = Mat4::Identity();
	temp.r[ 3 ] = V;
	return temp;
}

Mat4 Mat4::Scale( const Vec3 & V )
{
	auto temp = Mat4::Identity();
	temp.r[ 0 ].x = V.x;
	temp.r[ 1 ].y = V.y;
	temp.r[ 2 ].z = V.z;
	return temp;
}

Mat4 Mat4::RotateX( const float Theta )
{
	float sine = sinf( Theta );
	float cosine = cosf( Theta );
	
	return Mat4(
	{ 1.f,	  0.f,	  0.f, 0.f },
	{ 0.f, cosine,   sine, 0.f },
	{ 0.f,	-sine, cosine, 0.f },
	{ 0.f,    0.f,    0.f, 1.f }
	);
}

Mat4 Mat4::RotateY( const float Theta )
{
	float sine = sinf( Theta );
	float cosine = cosf( Theta );

	return Mat4(
	{ cosine, 0.f,  -sine, 0.f },
	{    0.f, 1.f,	  0.f, 0.f },
	{   sine, 0.f, cosine, 0.f },
	{	 0.f, 0.f,	  0.f, 1.f }
	);
}

Mat4 Mat4::RotateZ( const float Theta )
{
	float sine = sinf( Theta );
	float cosine = cosf( Theta );

	return Mat4(
	{ cosine,   sine, 0.f, 0.f },
	{  -sine, cosine, 0.f, 0.f },
	{    0.f,	 0.f, 1.f, 0.f },
	{    0.f,	 0.f, 0.f, 1.f }
	);
}

Mat4 Mat4::Rotate( const Vec3 & V, const float Theta )
{
	float alpha = atan2f( V.x, V.y );
	float beta = atan2f( V.y, V.z );

	Mat4 res = Mat4::RotateX( -alpha );
	res = res * Mat4::RotateZ( -beta );
	res = res * Mat4::RotateY( Theta );
	res = res * Mat4::RotateZ( beta );
	res = res * Mat4::RotateX( alpha );
	return res;
}
