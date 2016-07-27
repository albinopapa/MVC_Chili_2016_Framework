#include "Loader_Image.h"
/************************************************************************************
*  File name		: ImageLoader.cpp												*
*  Creation Date	: 7/10/2016														*
*  Purpose			: Used to load/create images from files or memory				*
*  Description		: ImageLoader is a wrapper for the Windows Imaging Component	*
*					  API for loading images and creating compatible buffers with	*
*					  Direct2D														*
*  Requirements		:																*
*************************************************************************************/

using namespace Microsoft::WRL;

Loader_Image::Loader_Image()
{}

Loader_Image::~Loader_Image()
{
}

WicBitmapResult Loader_Image::CreateBitmap( const std::wstring & Filename, const Wic &crWic )
{
	// Get the WIC factory from the Wic object
	auto pFactory = crWic.GetFactory();

	std::wstring extension( Filename.substr( Filename.size() - 3, 3 ) );
	int cmpResult = extension.compare( L"tga" );

	UINT width = 0, height = 0;

	if( cmpResult == 0 )
	{
		return loadTarga( Filename, crWic );
	}
	else
	{
		WicBitmapResult imgResult;

		// Create the decoder from the file
		ComPtr<IWICBitmapDecoder> pDecoder;
		imgResult.first = pFactory->CreateDecoderFromFilename( Filename.c_str(), nullptr,
			GENERIC_READ, WICDecodeMetadataCacheOnDemand, pDecoder.GetAddressOf() );
		if( FAILED( imgResult.first ) )
		{
			return imgResult;
		}
		
		// Decode the image, which for bitmaps is the 0th frame
		ComPtr<IWICBitmapFrameDecode> pFrame;
		imgResult.first = pDecoder->GetFrame( 0, pFrame.GetAddressOf() );
		if( FAILED( imgResult.first ) )
		{
			return imgResult;
		}

		// Create the format converter, used to convert to different color
		// palettes and such.
		ComPtr<IWICFormatConverter> pConverter;
		imgResult.first = pFactory->CreateFormatConverter( pConverter.GetAddressOf() );
		if( FAILED( imgResult.first ) )
		{
			return imgResult;
		}

		// Initialize the converter using the frame grabbed.
		// This is just to make sure that the colors are in BGRA order
		imgResult.first = pConverter->Initialize( pFrame.Get(), GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, nullptr, 1.f, WICBitmapPaletteTypeCustom );
		if( FAILED( imgResult.first ) )
		{
			return imgResult;
		}

		// Create the bitmap from the converter interface
		imgResult.first = pFactory->CreateBitmapFromSource( pConverter.Get(), WICBitmapCacheOnDemand,
			&imgResult.second );
		if( FAILED( imgResult.first ) )
		{
			return imgResult;
		}

		return imgResult;
	}
}

WicBitmapResult  Loader_Image::CreateBitmap( const UINT Width, const UINT Height, const Wic &crWic )
{
	// Get the WIC factory from the Wic object
	auto pFactory = crWic.GetFactory();

	// Create temp bitmap
	IWICBitmap *pBitmap;

	// Create bitmap from memory using the temp bitmap
	HRESULT hr = pFactory->CreateBitmap(
		Width, Height,								// Width and height of bitmap
		GUID_WICPixelFormat32bppPBGRA,				// BGRA pixel format
		WICBitmapCacheOnDemand,						// Cache on demand, not sure what this does
		&pBitmap									// Pass address of the bitmap interface
	);

	return{hr, pBitmap};							// if creation fails, the pointer is nullptr
}

WicBitmapResult Loader_Image::loadTarga( const std::wstring &Filename, const Wic &rcWic )
{
	// Found out that this procedure doesn't support compressed or palette based images
	// The tga file must be an RGB uncompressed image

	WicBitmapResult bitmapResult;

	// Open the targa file for reading in binary.
	std::ifstream file( Filename, std::ios::binary );
	bitmapResult.first = file.is_open() ? S_OK : STG_E_FILENOTFOUND;
	if( FAILED( bitmapResult.first ) )
	{
		return std::move(bitmapResult);
	}

	// Read in the file header.
	TargaHeader targaFileHeader{};
	file.read( reinterpret_cast<char*>(&targaFileHeader), sizeof( TargaHeader ) );

	// Check that width and height are greater than 0 and that it is 32 bit and not 24 bit.
	bitmapResult.first =
		( targaFileHeader.width > 0 ) && 
		( targaFileHeader.height > 0 ) && 
		( targaFileHeader.bpp == 32 ) ? S_OK : STG_E_INVALIDHEADER;
	if( FAILED( bitmapResult.first ) )
	{
		file.close();
		return std::move( bitmapResult );
	}

	// Create an empty WIC bitmap to copy the image data into
	bitmapResult = CreateBitmap( targaFileHeader.width, targaFileHeader.height, rcWic );
	if( FAILED( bitmapResult.first ) )
	{
		file.close();
		return std::move( bitmapResult );
	}

	// Calculate the size of the 32 bit image data.	
	UINT imageSize = targaFileHeader.width * targaFileHeader.height * ( targaFileHeader.bpp / 8 );

	// Acquire the bitmap lock
	comptr<IWICBitmapLock> pLock;
	WICRect rect{0, 0, targaFileHeader.width, targaFileHeader.height};
	bitmapResult.first = bitmapResult.second->Lock( &rect, WICBitmapLockWrite, pLock.GetAddressOf() );
	if( FAILED( bitmapResult.first ) )
	{
		file.close();
		return std::move( bitmapResult );
	}

	// Read in the targa image data.
	char *pData = nullptr;
	bitmapResult.first = pLock->GetDataPointer( &imageSize, reinterpret_cast<BYTE**>( &pData ) );
	if( FAILED( bitmapResult.first ) )
	{
		file.close();
		return std::move( bitmapResult );
	}

	file.read( pData, imageSize );
	bitmapResult.first = pData != nullptr ? S_OK : STG_E_READFAULT;
	if( FAILED( bitmapResult.first ) )
	{
		file.close();
		return std::move( bitmapResult );
	}
	
	file.close();

	return bitmapResult;
}
