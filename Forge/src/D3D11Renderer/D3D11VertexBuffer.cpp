#include "Fpch.h"
#include "D3D11VertexBuffer.h"
#include "D3D11Device.h"

namespace d3d11
{
	const char* InputTypeToSemanticName( renderer::InputType type )
	{
		switch( type )
		{
		case renderer::InputType::Position:
			return "POSITION";

		case renderer::InputType::Color:
			return "COLOR";

		case renderer::InputType::TexCoord:
			return "TEXCOORD";

		case renderer::InputType::Normal:
			return "NORMAL";

		case renderer::InputType::BlendWeights:
			return "BLENDWEIGHTS";

		case renderer::InputType::BlendIndices:
			return "BLENDINDICES";

		default:
			FORGE_FATAL( "Not known input type" );
		}

		return nullptr;
	}

	DXGI_FORMAT InputFormatToD3D11Format( renderer::InputFormat format )
	{
		switch( format )
		{
		case renderer::InputFormat::R32_UINT:
			return DXGI_FORMAT_R32_UINT;

		case renderer::InputFormat::R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

		case renderer::InputFormat::R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;

		case renderer::InputFormat::R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;

		case renderer::InputFormat::R32G32B32_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;

		case renderer::InputFormat::R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;

		case renderer::InputFormat::R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;

		default:
			FORGE_FATAL( "Not known format" );
		}

		return DXGI_FORMAT_UNKNOWN;
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

	void ConstructLayout( const forge::ArraySpan< const renderer::InputElementDescription >& inputElements, std::vector< D3D11_INPUT_ELEMENT_DESC >& outLayout )
	{
		Uint32 semanticIndices[ static_cast< Uint32 >( renderer::InputType::Count  ) ] = { 0 };

		Uint32 currentOffset = 0u;
		for( const auto& it : inputElements )
		{
			outLayout.emplace_back( D3D11_INPUT_ELEMENT_DESC{ InputTypeToSemanticName( it.m_inputType ), semanticIndices[ static_cast< Uint32 >( it.m_inputType ) ]++, InputFormatToD3D11Format( it.m_inputFormat ), 0, currentOffset, InputClassificationToD3D11Classification( it.m_classification ), 0 } );
			currentOffset += it.m_size;
		}
	}

	D3D11VertexBuffer::D3D11VertexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, const renderer::Vertices& vertices )
		: m_contextPtr( contextPtr )
		, m_stride( vertices.GetVertexSize() )
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = vertices.GetBufferSize();
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

		m_contextPtr->GetDeviceContext()->IASetVertexBuffers( 0, 1, &m_buffer, &stride, &offset );
	}

	const D3D11_INPUT_ELEMENT_DESC* D3D11VertexBuffer::GetLayout() const
	{
		return m_layout.data();
	}

	Uint32 D3D11VertexBuffer::GetElementsAmount() const
	{
		return static_cast<Uint32>( m_layout.size() );
	}

	std::vector< renderer::IVertexBuffer::SemanticDesc > D3D11VertexBuffer::GetSemanticDescriptions() const
	{
		std::vector< renderer::IVertexBuffer::SemanticDesc > result;

		for( const auto& entry : m_layout )
		{
			auto it = std::find_if( result.begin(), result.end(), [ &entry ]( const renderer::IVertexBuffer::SemanticDesc& desc )
			{
				return std::strcmp( desc.m_name.c_str(), entry.SemanticName ) == 0;
			} );

			if( it == result.end() )
			{
				result.emplace_back( renderer::IVertexBuffer::SemanticDesc{ entry.SemanticName, entry.SemanticIndex + 1u } );
			}
			else
			{
				it->m_amount = Math::Max( it->m_amount, entry.SemanticIndex + 1u );
			}
		}

		return result;
	}
}