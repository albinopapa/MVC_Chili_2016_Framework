#include "AmbientLight.h"



AmbientLight::AmbientLight()
{}

AmbientLight::AmbientLight( float R, float G, float B )
	:
	m_color( R, G, B, 1.f )
{}


AmbientLight::~AmbientLight()
{}

void AmbientLight::SetColor( float R, float G, float B )
{
	m_color = { R, G, B, 1.f };
}

Vec4 AmbientLight::GetColor() const
{
	return m_color;
}
