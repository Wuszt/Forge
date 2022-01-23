#pragma once
#include "../Renderer/ITexture.h"
#include "D3D11ShaderResourceView.h"

struct ID3D11Texture2D;
struct D3D11_TEXTURE2D_DESC;

namespace d3d11
{
	class D3D11ShaderResourceView;

	class D3D11Texture : public renderer::ITexture
	{
	public:
		D3D11Texture( const D3D11Device& device, ID3D11Texture2D* texture, std::unique_ptr< D3D11ShaderResourceView > resourceView );
		D3D11Texture( const D3D11Device& device, ID3D11Texture2D* texture, DXGI_FORMAT srvFormat );
		D3D11Texture( const D3D11Device& device, const D3D11_TEXTURE2D_DESC& desc, DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN );
		D3D11Texture( const D3D11Device& device, Uint32 width, Uint32 height, Flags flags, Format format, Format srvFormat = ITexture::Format::Unknown );
		~D3D11Texture();

		virtual void Resize( Uint32 width, Uint32 height );

		FORGE_INLINE ID3D11Texture2D* GetTexture() const
		{
			return m_texture;
		}

		virtual FORGE_INLINE D3D11ShaderResourceView* GetShaderResourceView() const override
		{
			return m_srv.get();
		}

	private:
		const D3D11Device& m_device;
		ID3D11Texture2D* m_texture = nullptr;
		std::unique_ptr< D3D11ShaderResourceView > m_srv;
	};
}
