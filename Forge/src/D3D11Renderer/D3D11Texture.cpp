#include "Fpch.h"
#include "D3D11Texture.h"
#include "D3D11Device.h"
#include <d3d11.h>

namespace d3d11
{
	DXGI_FORMAT FormatToD3D11Format( renderer::ITexture::Format format )
	{
		switch( format )
		{
		case renderer::ITexture::Format::Unknown:
			return DXGI_FORMAT_UNKNOWN;

		case renderer::ITexture::Format::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case renderer::ITexture::Format::R24G8_TYPELESS:
			return DXGI_FORMAT_R24G8_TYPELESS;

		default:
			FORGE_FATAL( "Unknown format" );
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	Uint32 FlagsToD3D11Flags( renderer::ITexture::Flags flags )
	{
		Uint32 result = 0u;

		for( Uint32 i = 0; ( 1 << i ) <= static_cast< Uint32 >( renderer::ITexture::Flags::Last ); ++i )
		{
			if( (( 1 << i ) & static_cast<Uint32>( flags )) != 0u )
			{
				switch( static_cast<renderer::ITexture::Flags>( 1 << i ) )
				{
				case renderer::ITexture::Flags::BIND_RENDER_TARGET:
					result |= D3D11_BIND_RENDER_TARGET;
					break;
				case renderer::ITexture::Flags::BIND_SHADER_RESOURCE:
					result |= D3D11_BIND_SHADER_RESOURCE;
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	D3D11Texture::D3D11Texture( const D3D11Device& device, const D3D11RenderContext& context, const D3D11_TEXTURE2D_DESC& desc, DXGI_FORMAT srvFormat )
		: m_device( device )
		, m_context( context )
	{
		FORGE_ASSURE( device.GetDevice()->CreateTexture2D( &desc, nullptr, &m_texture ) == S_OK );

		CreateViewsIfRequired( desc.BindFlags, srvFormat );
	}

	D3D11Texture::D3D11Texture( const D3D11Device& device, const D3D11RenderContext& context, ID3D11Texture2D& texture, DXGI_FORMAT srvFormat )
		: m_texture( &texture )
		, m_device( device )
		, m_context( context )
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		texture.GetDesc( &textureDesc );
		CreateViewsIfRequired( textureDesc.BindFlags, srvFormat );
	}

	D3D11Texture::D3D11Texture( const D3D11Device& device, const D3D11RenderContext& context, Uint32 width, Uint32 height, Flags flags, Format format, Format srvFormat )
		: m_device( device )
		, m_context( context )
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( D3D11_TEXTURE2D_DESC ) );
		desc.Width = width;
		desc.Height = height;
		desc.Format = FormatToD3D11Format( format );
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.BindFlags = FlagsToD3D11Flags( flags );
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		FORGE_ASSURE( device.GetDevice()->CreateTexture2D( &desc, nullptr, &m_texture ) == S_OK );

		CreateViewsIfRequired( FlagsToD3D11Flags( flags ), FormatToD3D11Format( format ) );
	}

	D3D11Texture::~D3D11Texture()
	{
		ReleaseResources();
	}

	void D3D11Texture::Resize( const Vector2& size )
	{
		D3D11_TEXTURE2D_DESC texDesc;

		{
			m_texture->GetDesc( &texDesc );
			m_texture->Release();
		}

		texDesc.Width = static_cast< Uint32 >( size.X );
		texDesc.Height = static_cast< Uint32 >( size.Y );

		FORGE_ASSURE( m_device.GetDevice()->CreateTexture2D( &texDesc, nullptr, &m_texture ) == S_OK );

		if( m_rtv )
		{
			m_rtv = std::make_unique< D3D11RenderTargetView >( m_device, m_context, *m_texture );
		}

		if( m_srv )
		{
			m_srv = std::make_unique< D3D11ShaderResourceView >( m_device, *m_texture, m_srv->GetDesc().Format );
		}

		InvokeResizeCallback( size );
	}

	Vector2 D3D11Texture::GetTextureSize() const
	{
		D3D11_TEXTURE2D_DESC texDesc;
		m_texture->GetDesc( &texDesc );

		return { static_cast< Float >( texDesc.Width ), static_cast< Float >( texDesc.Height ) };
	}

	void D3D11Texture::CreateViewsIfRequired( Uint32 flags, Uint32 srvFormat )
	{
		if( ( flags & D3D11_BIND_RENDER_TARGET ) != 0u )
		{
			m_rtv = std::make_unique< D3D11RenderTargetView >( m_device, m_context, *m_texture );
		}

		if( ( flags & D3D11_BIND_SHADER_RESOURCE ) != 0u )
		{
			m_srv = std::make_unique< D3D11ShaderResourceView >( m_device, *m_texture, static_cast< DXGI_FORMAT >( srvFormat ) );
		}
	}

}