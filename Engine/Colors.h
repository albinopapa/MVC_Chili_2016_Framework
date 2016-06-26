#pragma once

class Color
{
public:
	union
	{
		unsigned int c;
		struct
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char x;
		};
	};
public:
	constexpr Color() : c() {}
	constexpr Color( const Color& col )
		:
		c( col.c )
	{}
	constexpr Color( unsigned int col )
		:
		c( col )
	{}
	constexpr Color( unsigned char x,unsigned char r,unsigned char g,unsigned char b )
		:
		x( x ),
		r( r ),
		g( g ),
		b( b )
	{}
	constexpr Color( unsigned char r,unsigned char g,unsigned char b )
		:
		r( r ),
		g( g ),
		b( b )
	{}
	constexpr Color( Color col,unsigned char x )
		:
		x( x ),
		r( col.r ),
		g( col.g ),
		b( col.b )
	{
	}
	Color& operator =( Color color )
	{
		c = color.c;
		return *this;
	}
};

namespace Colors
{
	static constexpr Color MakeRGB( unsigned char r,unsigned char g,unsigned char b )
	{
		return (r << 16) | (g << 8) | b;
	}
	static constexpr Color White = MakeRGB( 255u,255u,255u );
	static constexpr Color Black = MakeRGB( 0u,0u,0u );
	static constexpr Color Gray = MakeRGB( 0x80u,0x80u,0x80u );
	static constexpr Color LightGray = MakeRGB( 0xD3u,0xD3u,0xD3u );
	static constexpr Color Red = MakeRGB( 255u,0u,0u );
	static constexpr Color Green = MakeRGB( 0u,255u,0u );
	static constexpr Color Blue = MakeRGB( 0u,0u,255u );
	static constexpr Color Yellow = MakeRGB( 255u,255u,0u );
	static constexpr Color Cyan = MakeRGB( 0u,255u,255u );
	static constexpr Color Magenta = MakeRGB( 255u,0u,255u );
}