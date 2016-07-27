#pragma once
#pragma comment(lib, "windowscodecs.lib")

#include "Includes.h"
#include "Utilities.h"
class Wic
{
public:
	Wic();
	~Wic();

	bool Initialzie();
	IWICImagingFactory *GetFactory()const;
private:
	comptr<IWICImagingFactory> m_pFactory;
};

