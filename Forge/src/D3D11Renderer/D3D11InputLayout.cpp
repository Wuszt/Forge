#include "Fpch.h"
#include "D3D11InputLayout.h"
#include "D3D11Device.h"
#include "D3D11VertexShader.h"

namespace d3d11
{
	D3D11InputLayout::D3D11InputLayout( D3D11RenderContext* contextPtr, const D3D11Device& device, const D3D11VertexShader& vertexShader, const D3D11VertexBuffer& vertexBuffer )
		: m_contextPtr( contextPtr )
	{
		FORGE_ASSURE( device.GetDevice()->CreateInputLayout( vertexBuffer.GetLayout(), vertexBuffer.GetElementsAmount(),
			vertexShader.GetBuffer()->GetBufferPointer(), vertexShader.GetBuffer()->GetBufferSize(), &m_layout ) == S_OK );
	}

	D3D11InputLayout::~D3D11InputLayout()
	{
		m_layout->Release();
	}

	D3D_PRIMITIVE_TOPOLOGY D3D11InputLayout::GetTopology() const
	{
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	void D3D11InputLayout::Set()
	{
		m_contextPtr->GetDeviceContext()->IASetInputLayout( m_layout );
		m_contextPtr->GetDeviceContext()->IASetPrimitiveTopology( GetTopology() );
	}
}