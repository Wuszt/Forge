#include "Fpch.h"
#include "D3D11Texture.h"
#include "D3D11Device.h"

D3D11Texture::D3D11Texture( ID3D11Texture2D* texture )
	: m_texture( texture )
{}

D3D11Texture::D3D11Texture( const D3D11Device& device, const D3D11_TEXTURE2D_DESC& desc )
{
	HRESULT hr = device.GetDevice()->CreateTexture2D( &desc, nullptr, &m_texture );
	FORGE_ASSERT( hr == S_OK );
}

D3D11Texture::~D3D11Texture()
{
	m_texture->Release();
}
