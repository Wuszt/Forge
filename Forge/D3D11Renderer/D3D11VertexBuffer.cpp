#include "Fpch.h"
#include "D3D11VertexBuffer.h"
#include "D3D11Device.h"

Vertex::Vertex( Vector3 pos, Vector4 clr )
	: m_pos( pos )
	, m_clr ( clr )
{}

Vertex::Vertex()
{}

Vertex::~Vertex()
{}

const D3D11_INPUT_ELEMENT_DESC Vertex::c_layout[] =
{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D11_INPUT_ELEMENT_DESC* const& Vertex::GetLayout()
{
	return c_layout;
}

Uint32 Vertex::GetElementsAmount()
{
	return ARRAYSIZE( c_layout );
}

D3D11VertexBuffer::D3D11VertexBuffer( D3D11Context* contextPtr, const D3D11Device& device, const ElementType* vertices, Uint32 amount )
	: m_contextPtr( contextPtr )
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( ElementType ) * amount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
	vertexBufferData.pSysMem = vertices;
	auto hr = device.GetDevice()->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &m_buffer );

	FORGE_ASSERT( hr == S_OK ); 
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{
	m_buffer->Release();
}

void D3D11VertexBuffer::Set()
{
	auto stride = GetStride();
	Uint32 offset = 0u;

	m_contextPtr->GetDeviceContext()->IASetVertexBuffers( 0, 1, &GetBuffer(), &stride, &offset );
}