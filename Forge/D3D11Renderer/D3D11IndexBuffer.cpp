#include "Fpch.h"
#include "D3D11IndexBuffer.h"
#include "D3D11Device.h"

D3D11IndexBuffer::D3D11IndexBuffer( D3D11Context* contextPtr, const D3D11Device& device, const Uint32* indices, Uint32 amount )
	: m_contextPtr( contextPtr )
{
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof( indexBufferDesc ) );

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( Uint32 ) * amount;
	device.GetDevice()->CreateBuffer( &indexBufferDesc, &iinitData, &m_buffer );
}

D3D11IndexBuffer::~D3D11IndexBuffer()
{
	m_buffer->Release();
}

void D3D11IndexBuffer::Set( Uint32 offset )
{
	m_contextPtr->GetDeviceContext()->IASetIndexBuffer( m_buffer, DXGI_FORMAT_R32_UINT, offset );
}
