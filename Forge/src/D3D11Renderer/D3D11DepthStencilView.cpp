#include "Fpch.h"
#include "D3D11DepthStencilView.h"
#include "D3D11Device.h"

d3d11::D3D11DepthStencilView::D3D11DepthStencilView( const D3D11Device& device, const D3D11RenderContext& context, ID3D11Texture2D& texture, Uint32 textureIndex )
	: m_device( device )
	, m_context( context )
{
	D3D11_TEXTURE2D_DESC texDesc;
	texture.GetDesc( &texDesc );

	Bool isCube = ( texDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE ) != 0u;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = isCube ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;

	if( isCube )
	{
		dsvDesc.Texture2DArray.ArraySize = 1;
		dsvDesc.Texture2DArray.FirstArraySlice = textureIndex;
		dsvDesc.Texture2DArray.MipSlice = 0;
	}
	else
	{
		dsvDesc.Texture2D.MipSlice = 0;
	}

	FORGE_ASSURE( m_device.GetDevice()->CreateDepthStencilView( &texture, &dsvDesc, &m_view ) == S_OK );
}

d3d11::D3D11DepthStencilView::~D3D11DepthStencilView()
{
	m_view->Release();
}

void d3d11::D3D11DepthStencilView::Clear()
{
	m_context.GetDeviceContext()->ClearDepthStencilView( m_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}
