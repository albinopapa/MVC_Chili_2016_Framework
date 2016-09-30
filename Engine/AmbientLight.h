#pragma once

#include "Vec4.h"

class AmbientLight
{
public:
	AmbientLight();
	AmbientLight( float R, float G, float B );
	~AmbientLight();

	void SetColor( float R, float G, float B );
	Vec4 GetColor()const;
private:
	Vec4 m_color;
};

