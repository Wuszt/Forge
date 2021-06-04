#include "Fpch.h"
#include "D3D11Texture.h"

D3D11Texture::D3D11Texture( ID3D11Texture2D* texture )
	: m_texture( texture )
{}

D3D11Texture::~D3D11Texture()
{
	m_texture->Release();
}
