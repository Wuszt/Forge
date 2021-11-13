#include "Fpch.h"
#include "D3D11VertexBuffer.h"
#include "D3D11Device.h"

//const D3D11_INPUT_ELEMENT_DESC D3D11Vertex::c_layout[] =
//{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//};
//
//const D3D11_INPUT_ELEMENT_DESC* const& D3D11Vertex::GetLayout()
//{
//	std::vector< D3D11_INPUT_ELEMENT_DESC > layout;
//
//	return c_layout;
//}
//
//Uint32 D3D11Vertex::GetElementsAmount()
//{
//	return ARRAYSIZE( c_layout );
//}

namespace d3d11
{
	const char* InputTypeToString( renderer::InputType type )
	{
		if( type == renderer::InputType::Position )
		{
			return "POSITION";
		}
		else if( type == renderer::InputType::Color )
		{
			return "COLOR";
		}
		else
		{
			FORGE_FATAL( "Not known input type" );
			return "";
		}
	}

	DXGI_FORMAT InputFormatToD3D11Format( renderer::InputFormat format )
	{
		if( format == renderer::InputFormat::R32G32B32 )
		{
			return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if( format == renderer::InputFormat::R32G32B32A32 )
		{
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		else
		{
			FORGE_FATAL( "Not known format" );
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	D3D11_INPUT_CLASSIFICATION InputClassificationToD3D11Classification( renderer::InputClassification classification )
	{
		if( classification == renderer::InputClassification::PerVertex )
		{
			return D3D11_INPUT_PER_VERTEX_DATA;
		}
		else
		{
			FORGE_FATAL( "Not known classification" );
			return D3D11_INPUT_PER_VERTEX_DATA;
		}
	}

	void ConstructLayout( const std::vector< renderer::InputElementDescription >& inputElements, std::vector< D3D11_INPUT_ELEMENT_DESC >& outLayout )
	{
		Uint32 semanticIndices[ static_cast<Uint32>( renderer::InputType::Count ) ] = { 0 };

		Uint32 currentOffset = 0u;
		for( auto it = inputElements.begin(); it != inputElements.end(); ++it )
		{
			outLayout.push_back( { InputTypeToString( it->m_inputType ), semanticIndices[ static_cast<Uint32>( it->m_inputType ) ]++, InputFormatToD3D11Format( it->m_inputFormat ), 0, currentOffset, InputClassificationToD3D11Classification( it->m_classification ), 0 } );
			currentOffset += it->m_size;
		}
	}

	D3D11VertexBuffer::D3D11VertexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, const renderer::Vertices& vertices )
		: m_contextPtr( contextPtr )
		, m_stride( vertices.GetVertexByteWidth() )
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = vertices.GetVerticesByteWidth();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
		vertexBufferData.pSysMem = vertices.GetData();
		FORGE_ASSURE( device.GetDevice()->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &m_buffer ) == S_OK );

		ConstructLayout( vertices.GetInputElements(), m_layout );
	}

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{
		m_buffer->Release();
	}

	void D3D11VertexBuffer::Set() const
	{
		Uint32 stride = GetStride();
		Uint32 offset = 0u;

		m_contextPtr->GetDeviceContext()->IASetVertexBuffers( 0, 1, &GetBuffer(), &stride, &offset );
	}

	const D3D11_INPUT_ELEMENT_DESC* D3D11VertexBuffer::GetLayout() const
	{
		return m_layout.data();
	}

	Uint32 D3D11VertexBuffer::GetElementsAmount() const
	{
		return static_cast<Uint32>( m_layout.size() );
	}
}