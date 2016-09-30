#include "InfiniteLight.h"



InfiniteLight::InfiniteLight()
{}


InfiniteLight::~InfiniteLight()
{}

void InfiniteLight::SetDirection( float X, float Y, float Z )
{
	m_direction = Vec3( X, Y, Z );
}

Vec3 InfiniteLight::GetDirection() const
{
	return m_direction;
}
