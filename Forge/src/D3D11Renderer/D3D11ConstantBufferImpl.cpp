#include "Fpch.h"
#include "D3D11ConstantBufferImpl.h"

namespace d3d11
{
	D3D11ConstantBufferImpl::D3D11ConstantBufferImpl( D3D11RenderContext& context, D3D11Device& device )
		: m_context( context )
		, m_device( device )
	{}

	D3D11ConstantBufferImpl::~D3D11ConstantBufferImpl()
	{
		m_buff->Release();
	}

	void D3D11ConstantBufferImpl::SetVS( Uint32 index )
	{
		m_context.GetDeviceContext()->VSSetConstantBuffers( index, 1, &m_buff );
	}

	void D3D11ConstantBufferImpl::SetPS( Uint32 index )
	{
		
		m_context.GetDeviceContext()->PSSetConstantBuffers( index, 1, &m_buff );
	}

	void D3D11ConstantBufferImpl::CreateBuffer( Uint32 dataSize )
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = dataSize;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		m_device.GetDevice()->CreateBuffer( &desc, nullptr, &m_buff );
	}

	void D3D11ConstantBufferImpl::UpdateBuffer( void* data )
	{
		m_context.GetDeviceContext()->UpdateSubresource( m_buff, 0, nullptr, data, 0, 0 );
	}

}
