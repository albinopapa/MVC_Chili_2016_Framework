////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Texture.h"
#include "Graphics.h"

Texture::Texture()
{
}

Texture::Texture( const Texture& other )
{}

Texture::~Texture()
{}

Texture & Texture::operator=( const Texture & Tex )
{
	m_pTexture = Tex.m_pTexture;
	m_pTextureView = Tex.m_pTextureView;
	
	return *this;
}

bool Texture::Initialize( const Graphics &Gfx, const std::wstring &Filename )
{
	/*UINT height = 0, width = 0;
	std::unique_ptr<BYTE[]> pImageData;
	auto &wic = Gfx.GetWIC();

	bool result = loadImageData( Filename, width, height, pImageData, wic);
	RETURN_IF_FALSE( result );

	auto pDevice = Gfx.GetDirect3D().GetDevice();
	auto pContext = Gfx.GetDirect3D().GetDeviceContext();
	result = createTextureAndResourceView( Gfx, width, height, pImageData );
	RETURN_IF_FALSE( result );
*/
	return true;
}

bool Texture::Initialize( const Graphics &Gfx, UINT TextureWidth, UINT TextureHeight )
{
	/*auto pDevice = Gfx.GetDirect3D().GetDevice();
	auto pContext = Gfx.GetDirect3D().GetDeviceContext();
	auto &wic = Gfx.GetWIC();

	bool result = false;
	UINT width = TextureWidth, height = TextureHeight;
		
	auto imgResult = Loader_Image::CreateBitmap( width, height, wic);
	result = SUCCEEDED( imgResult.first );
	RETURN_IF_FALSE( result );

	std::unique_ptr<BYTE[]> pImageData;
	result = createTextureFromWICImage( imgResult.second, pImageData );
	RETURN_IF_FALSE( result );

	result = createTextureAndResourceView( Gfx, width, height, pImageData );
	RETURN_IF_FALSE( result );*/

	return true;
}

bool Texture::Initialize( const Graphics & Gfx, ID3D11Texture2D *pBitmap)
{
	bool result = createTextureAndResourceView(Gfx, pBitmap);
	RETURN_IF_FALSE( result );

	return true;
}

ID3D11Texture2D* Texture::GetTexture()const
{
	return m_pTexture.Get();
}

ID3D11ShaderResourceView * Texture::GetTextureView() const
{
	return m_pTextureView.Get();
}

bool Texture::createTextureAndResourceView(const Graphics &Gfx, UINT TextureWidth, UINT TextureHeight, std::unique_ptr<BYTE[]> &pImageData )
{
	//// Setup the description of the texture.
	//D3D11_TEXTURE2D_DESC textureDesc{};
	//textureDesc.Width = TextureWidth;
	//textureDesc.Height = TextureHeight;
	//textureDesc.MipLevels = 0;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.SampleDesc.Quality = 0;
	//textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	//textureDesc.CPUAccessFlags = 0;
	//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//auto pDevice = Gfx.GetDirect3D().GetDevice();
	//// Create the empty texture.
	//HRESULT hResult = pDevice->CreateTexture2D( &textureDesc, nullptr, m_pTexture.GetAddressOf() );
	//RETURN_IF_FAILED( hResult );

	//auto pContext = Gfx.GetDirect3D().GetDeviceContext();
	//pContext->UpdateSubresource( m_pTexture.Get(), 0, nullptr, pImageData.get(), TextureWidth * 4, 0 );

	//// Setup the shader resource view description.
	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = textureDesc.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MostDetailedMip = 0;
	//srvDesc.Texture2D.MipLevels = -1;

	//// Create the shader resource view for the texture.
	//hResult = pDevice->CreateShaderResourceView( m_pTexture.Get(), &srvDesc,
	//	m_pTextureView.GetAddressOf() );
	//RETURN_IF_FAILED( hResult );

	//// Generate mipmaps for this texture.
	//pContext->GenerateMips( m_pTextureView.Get() );

	return true;
}

bool Texture::createTextureAndResourceView( const Graphics &Gfx, ID3D11Texture2D *pBitmap)
{
	//// Setup the shader resource view description.
	//D3D11_TEXTURE2D_DESC td{};
	//m_pTexture.Attach(pBitmap);
	//m_pTexture->GetDesc(&td);

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Texture2D.MipLevels = td.MipLevels;
	//srvDesc.Format = td.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	//// Create the shader resource view for the texture.
	//auto pDevice = Gfx.GetDirect3D().GetDevice();
	//HRESULT hResult = pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc,
	//	m_pTextureView.GetAddressOf() );
	//RETURN_IF_FAILED( hResult );

	return true;
}

bool Texture::loadImageData( const std::wstring &Filename, UINT &TextureWidth, 
	UINT &TextureHeight, std::unique_ptr<BYTE[]> &pImageData, const Wic &crWic )
{	
	auto imgResult = Loader_Image::CreateBitmap( Filename, crWic );
	bool result = SUCCEEDED( imgResult.first );
	RETURN_MESSAGE_IF_FALSE( result, L"Not a supported file type, or file not found." );

	imgResult.second->GetSize( &TextureWidth, &TextureHeight );
	result = createTextureFromWICImage( imgResult.second, pImageData );
	RETURN_MESSAGE_IF_FALSE( result, L"Failed to create WIC bitmap." );

	return true;
}

bool Texture::createTextureFromWICImage( IWICBitmap * pBitmap, std::unique_ptr<BYTE[]> &pImageData )
{
	// Get the size of the image to create a source rectangle that encompasses 
	// the entire image
	UINT width = 0, height = 0;
	HRESULT hr = pBitmap->GetSize( &width, &height );
	RETURN_IF_FAILED( hr );
	WICRect srcRect{0, 0, static_cast<long>( width ), static_cast<long>( height )};

	// Create a lock to get the data pointer to the pixel array
	comptr<IWICBitmapLock> pLock;
	hr = pBitmap->Lock( &srcRect, WICBitmapLockRead, pLock.GetAddressOf() );
	RETURN_IF_FAILED( hr );

	// Calculate the image size
	UINT stride = 0;
	pLock->GetStride( &stride );	
	UINT imageSize = stride * height;

	// Get pointer to the pixel data
	BYTE *pPixels = nullptr;
	hr = pLock->GetDataPointer( &imageSize, &pPixels );
	RETURN_MESSAGE_IF_FALSE(
		SUCCEEDED( hr ),
		L"Failed to acquire the bitmap pointer."
	);

	// Allocate memory for the image data member
	pImageData.reset( new BYTE[ imageSize ] );
	RETURN_MESSAGE_IF_FALSE(
		pImageData != nullptr,
		L"Failed to allocate memory for m_pImageData"
	);
	
	// Copy the image data from the WIC bitmap to the image data member
	auto pDest = pImageData.get();
	for (int y = 0; y < height; ++y)
	{
		UINT srcIdx = y * stride;
		CopyMemory(&pDest[srcIdx], &pPixels[srcIdx], stride);
	}
	

	return true;
}

