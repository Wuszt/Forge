#include "Fpch.h"
#include "D3D11Texture.h"
#include "D3D11Device.h"
#include <d3d11.h>

namespace d3d11
{
	D3D11Texture::D3D11Texture( ID3D11Texture2D* texture, ID3D11ShaderResourceView* resourceView )
		: m_texture( texture )
		, m_srv( resourceView )
	{}

	D3D11Texture::D3D11Texture( const D3D11Device& device, const D3D11_TEXTURE2D_DESC& desc, DXGI_FORMAT srvFormat )
	{
		FORGE_ASSURE( device.GetDevice()->CreateTexture2D( &desc, nullptr, &m_texture ) == S_OK );

		if( srvFormat != DXGI_FORMAT_UNKNOWN )
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = srvFormat;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;

			FORGE_ASSURE( device.GetDevice()->CreateShaderResourceView( m_texture, &srvDesc, &m_srv ) == S_OK );
		}
	}

	D3D11Texture::~D3D11Texture()
	{
		m_texture->Release();

		if( m_srv )
		{
			m_srv->Release();
		}
	}
}