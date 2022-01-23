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


	D3D11Texture::D3D11Texture( const D3D11Device& device, ID3D11Texture2D* texture, std::unique_ptr< D3D11ShaderResourceView > resourceView )
		: m_texture( texture )
		, m_srv( std::move( resourceView ) )
		, m_device( device )
	{}

	ID3D11ShaderResourceView* CreateShaderResourceView( const D3D11Device& device, ID3D11Texture2D* texture, DXGI_FORMAT srvFormat )
	{
		ID3D11ShaderResourceView* srv = nullptr;
		if( srvFormat != DXGI_FORMAT_UNKNOWN )
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = srvFormat;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;

			FORGE_ASSURE( device.GetDevice()->CreateShaderResourceView( texture, &srvDesc, &srv ) == S_OK );
		}

		return srv;
	}

	D3D11Texture::D3D11Texture( const D3D11Device& device, const D3D11_TEXTURE2D_DESC& desc, DXGI_FORMAT srvFormat )
		: m_device( device )
	{
		FORGE_ASSURE( device.GetDevice()->CreateTexture2D( &desc, nullptr, &m_texture ) == S_OK );

		m_srv = std::make_unique< D3D11ShaderResourceView >( device, m_texture, srvFormat );
	}

	D3D11Texture::D3D11Texture( const D3D11Device& device, ID3D11Texture2D* texture, DXGI_FORMAT srvFormat )
		: m_texture( texture )
		, m_device( device )
	{
		m_srv = std::make_unique< D3D11ShaderResourceView >( device, m_texture, srvFormat );
	}

	D3D11Texture::D3D11Texture( const D3D11Device& device, Uint32 width, Uint32 height, Flags flags, Format format, Format srvFormat )
		: m_device( device )
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

		m_srv = std::make_unique< D3D11ShaderResourceView >( device, m_texture, FormatToD3D11Format( srvFormat ) );
	}

	D3D11Texture::~D3D11Texture()
	{
		m_texture->Release();
	}

	void D3D11Texture::Resize( Uint32 width, Uint32 height )
	{
		D3D11_TEXTURE2D_DESC texDesc;

		{
			m_texture->GetDesc( &texDesc );
			m_texture->Release();
		}

		texDesc.Width = width;
		texDesc.Height = height;

		FORGE_ASSURE( m_device.GetDevice()->CreateTexture2D( &texDesc, nullptr, &m_texture ) == S_OK );

		if( m_srv )
		{
			m_srv = std::make_unique< D3D11ShaderResourceView >( m_device, m_texture, m_srv->GetDesc().Format );
		}
	}

}