#include "Fpch.h"
#include "D3D11ConstantBufferImpl.h"

D3D11ConstantBufferImpl::D3D11ConstantBufferImpl( D3D11RenderContext* contextPtr, D3D11Device* device, void* data, Uint32 dataSize )
	: m_context( contextPtr )
	, m_data( data )
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	device->GetDevice()->CreateBuffer( &desc, nullptr, &m_buff );
}

D3D11ConstantBufferImpl::~D3D11ConstantBufferImpl()
{
	m_buff->Release();
}

void D3D11ConstantBufferImpl::SetVS( Uint32 index )
{
	m_context->GetDeviceContext()->UpdateSubresource( m_buff, 0, nullptr, m_data, 0, 0 );
	m_context->GetDeviceContext()->VSSetConstantBuffers( index, 1, &m_buff );
}

void D3D11ConstantBufferImpl::SetPS( Uint32 index )
{
	m_context->GetDeviceContext()->UpdateSubresource( m_buff, 0, nullptr, m_data, 0, 0 );
	m_context->GetDeviceContext()->PSSetConstantBuffers( index, 1, &m_buff );
}
