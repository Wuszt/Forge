#include "Fpch.h"
#include "D3D11DepthStencilBuffer.h"
#include "D3D11Device.h"

Bool InitializeInternal( const D3D11Device& device, Uint32 width, Uint32 height, std::shared_ptr< D3D11Texture >& outTexture, ID3D11DepthStencilView** outView )
{
	D3D11_TEXTURE2D_DESC desc;

	ZeroMemory( &desc, sizeof( desc ) );

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	outTexture = std::make_shared< D3D11Texture >( device, desc );

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	return device.GetDevice()->CreateDepthStencilView( outTexture->GetTexture(), &dsvDesc, outView ) == S_OK;
}

D3D11DepthStencilBuffer::D3D11DepthStencilBuffer( const D3D11Device& device, const D3D11RenderContext& context, Uint32 width, Uint32 height )
	: m_device( device )
	, m_context( context )
{
	FORGE_ASSURE( InitializeInternal( device, width, height, m_texture, &m_view ) );
}

D3D11DepthStencilBuffer::~D3D11DepthStencilBuffer()
{
	m_view->Release();
}

void D3D11DepthStencilBuffer::Clear()
{
	m_context.GetDeviceContext()->ClearDepthStencilView( GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}
