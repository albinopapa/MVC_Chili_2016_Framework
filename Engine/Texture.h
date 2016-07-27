#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
//////////////
// INCLUDES //
//////////////
#include "Includes.h"
#include "Utilities.h"
#include "Loader_Image.h"

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
class Graphics;

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class Texture
{
public:
	Texture();
	Texture( const Texture& );
	~Texture();

	Texture &operator=( const Texture &Tex );

	// This procedure loads a texture from file
	bool Initialize( const Graphics &Gfx, const std::wstring &Filename);

	// This prcedure creates a blank texture based on dimensions
	bool Initialize( const Graphics &Gfx, UINT TextureWidth, UINT TextureHeight);

	// This procedure creates a texture from a ID3D11Texture2D bitmap
	bool Initialize( const Graphics &Gfx, ID3D11Texture2D *pBitmap);
	
	// If needed, would also make a procudure that creates a texture from memory
	// I would consider using an std::vector<BYTE> because vector keeps track of it's size, 
	// so wouldn't need a BYTE pointer and a separate UINT ImageSize parameter.
	//bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::vector<BYTE> &PixelData );

	// Returns the shader resource view of the texture
	ID3D11Texture2D *GetTexture()const;
	ID3D11ShaderResourceView* GetTextureView()const;

private:
	bool createTextureAndResourceView(const Graphics &Gfx, UINT TextureWidth, UINT TextureHeight, std::unique_ptr<BYTE[]> &pImageData);
	bool createTextureAndResourceView( const Graphics &Gfx, ID3D11Texture2D *pBitmap);
	bool loadImageData( const std::wstring &Filename, UINT &TextureWidth, UINT &TextureHeight, 
		std::unique_ptr<BYTE[]> &pImageData, const Wic &crWic );
	bool createTextureFromWICImage( IWICBitmap *pBitmap, std::unique_ptr<BYTE[]> &pImageData );
private:
	comptr<ID3D11Texture2D> m_pTexture;
	comptr<ID3D11ShaderResourceView> m_pTextureView;
};

