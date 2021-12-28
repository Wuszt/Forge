#pragma once
#include "../Renderer/ITexture.h"

struct ID3D11Texture2D;
struct D3D11_TEXTURE2D_DESC;

namespace d3d11
{
	class D3D11Texture : public renderer::ITexture
	{
	public:
		D3D11Texture( ID3D11Texture2D* texture, ID3D11ShaderResourceView* resourceView );
		D3D11Texture( const D3D11Device& device, const D3D11_TEXTURE2D_DESC& desc, DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN );
		~D3D11Texture();

		FORGE_INLINE ID3D11Texture2D* GetTexture() const
		{
			return m_texture;
		}

		FORGE_INLINE ID3D11ShaderResourceView* GetShaderResourceView() const
		{
			return m_srv;
		}

	private:
		ID3D11Texture2D* m_texture = nullptr;
		ID3D11ShaderResourceView* m_srv = nullptr;
	};
}
