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

		//desc.Usage = D3D11_USAGE_DYNAMIC;
		//desc.ByteWidth = dataSize;
		//desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		desc.Usage = D3D11_USAGE_DEFAULT;;
		desc.ByteWidth = dataSize;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		m_dataSize = dataSize;
		m_device.GetDevice()->CreateBuffer( &desc, nullptr, &m_buff );
	}

	void D3D11ConstantBufferImpl::UpdateBuffer( void* data )
	{
		//D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		//FORGE_ASSURE( m_context.GetDeviceContext()->Map( m_buff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer ) == S_OK );
		//memcpy( mappedBuffer.pData, data, m_dataSize );
		//m_context.GetDeviceContext()->Unmap( m_buff, 0 );
		m_context.GetDeviceContext()->UpdateSubresource( m_buff, 0, nullptr, data, 0, 0 );
	}

}
