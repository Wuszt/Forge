#include "Fpch.h"
#include "D3D11ShaderResourceView.h"
#include "D3D11Device.h"

d3d11::D3D11ShaderResourceView::D3D11ShaderResourceView( const D3D11Device& device, ID3D11Texture2D& texture, DXGI_FORMAT srvFormat )
{
	FORGE_ASSERT( srvFormat != DXGI_FORMAT_UNKNOWN );

	D3D11_TEXTURE2D_DESC textureDesc;
	texture.GetDesc( &textureDesc );

	Bool isCube = ( textureDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE ) != 0u;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = srvFormat;
	srvDesc.ViewDimension = isCube ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;

	if( isCube )
	{
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = -1;
	}
	else
	{
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
	}

	FORGE_ASSURE( device.GetDevice()->CreateShaderResourceView( &texture, &srvDesc, &m_srv ) == S_OK );
}

d3d11::D3D11ShaderResourceView::D3D11ShaderResourceView( ID3D11ShaderResourceView* srv )
	: m_srv( srv )
{}

d3d11::D3D11ShaderResourceView::~D3D11ShaderResourceView()
{
	m_srv->Release();
}

D3D11_SHADER_RESOURCE_VIEW_DESC d3d11::D3D11ShaderResourceView::GetDesc() const
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	m_srv->GetDesc( &srvDesc );

	return srvDesc;
}
