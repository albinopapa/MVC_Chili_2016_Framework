#pragma once
#include "AmbientLight.h"
#include "Vec3.h"
class InfiniteLight:
	public AmbientLight
{
public:
	InfiniteLight();
	~InfiniteLight();

	void SetDirection( float X, float Y, float Z );
	Vec3 GetDirection()const;

private:
	Vec3 m_direction;
};

