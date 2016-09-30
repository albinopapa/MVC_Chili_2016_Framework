#pragma once
#pragma comment(lib, "windowscodecs.lib")

#include "Includes.h"
#include "Utilities.h"
class Wic
{
public:
	Wic()  = default;
	~Wic() = default;

	bool Initialize();
	IWICImagingFactory *GetFactory()const;
private:
	comptr<IWICImagingFactory> m_pFactory;
};