#include "Fpch.h"
#include "D3D11IndexBuffer.h"
#include "D3D11Device.h"

namespace d3d11
{
	D3D11IndexBuffer::D3D11IndexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, const Uint32* indices, Uint32 amount )
		: m_contextPtr( contextPtr )
		, m_indicesAmount( amount )
	{
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory( &indexBufferDesc, sizeof( indexBufferDesc ) );

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof( Uint32 ) * amount;

		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;

		device.GetDevice()->CreateBuffer( &indexBufferDesc, &iinitData, &m_buffer );
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
		m_buffer->Release();
	}

	void D3D11IndexBuffer::Set( Uint32 offset ) const
	{
		m_contextPtr->GetDeviceContext()->IASetIndexBuffer( m_buffer, DXGI_FORMAT_R32_UINT, offset );
	}

	Uint32 D3D11IndexBuffer::GetIndicesAmount() const
	{
		return m_indicesAmount;
	}

}
