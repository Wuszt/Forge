#include "Fpch.h"
#include "D3D11DepthStencilBuffer.h"
#include "D3D11Device.h"
#include "D3D11DepthStencilView.h"

namespace d3d11
{
	D3D11DepthStencilBuffer::D3D11DepthStencilBuffer( const D3D11Device& device, const D3D11RenderContext& context, Uint32 width, Uint32 height, Bool cubeTexture )
		: m_device( device )
		, m_context( context )
	{
		CreateTexture( width, height, cubeTexture );
		CreateDepthStencilViews();
	}

	std::shared_ptr< renderer::ITexture > D3D11DepthStencilBuffer::GetTexture() const
	{
		return m_texture;
	}

	renderer::IDepthStencilView& D3D11DepthStencilBuffer::GetView( Uint32 index ) const
	{
		return *m_dsvs[ index ];
	}

	void D3D11DepthStencilBuffer::CreateDepthStencilViews()
	{
		m_dsvs.clear();

		switch( m_texture->GetType() )
		{
		case renderer::ITexture::Type::Texture2D:
			m_dsvs.emplace_back( std::make_shared< D3D11DepthStencilView >( m_device, m_context, *m_texture->GetTexture() ) );
			break;

		case renderer::ITexture::Type::TextureCube:
			for( Uint32 i = 0u; i < 6u; ++i )
			{
				m_dsvs.emplace_back( std::make_shared< D3D11DepthStencilView >( m_device, m_context, *m_texture->GetTexture(), i ) );
			}
			break;

		default:
			FORGE_FATAL( "Unknown type" );
		}		
	}

	void D3D11DepthStencilBuffer::CreateTexture( Uint32 width, Uint32 height, Bool cubeTexture )
	{
		renderer::ITexture::Type textureType = cubeTexture ? renderer::ITexture::Type::TextureCube : renderer::ITexture::Type::Texture2D;

		m_texture = std::make_shared< D3D11Texture >( m_device, m_context, width, height, renderer::ITexture::Flags::BIND_DEPTH_STENCIL | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
			renderer::ITexture::Format::R24G8_TYPELESS, textureType, renderer::ITexture::Format::R24_UNORM_X8_TYPELESS );
	}

	void D3D11DepthStencilBuffer::Resize( Uint32 width, Uint32 height )
	{
		CreateTexture( width, height, m_texture->GetType() == renderer::ITexture::Type::TextureCube );
		CreateDepthStencilViews();
	}
}