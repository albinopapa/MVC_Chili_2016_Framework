#include "Wic.h"

bool Wic::Initialize()
{
	// Create an instance of the imaging factory
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS( m_pFactory.GetAddressOf() )
	);
	RETURN_IF_FAILED( hr );

	return true;
}

IWICImagingFactory * Wic::GetFactory() const
{
	return m_pFactory.Get();
}