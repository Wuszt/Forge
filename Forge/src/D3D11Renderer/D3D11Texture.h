#pragma once
struct ID3D11Texture2D;
struct D3D11_TEXTURE2D_DESC;

namespace d3d11
{
	class D3D11Texture
	{
	public:
		D3D11Texture( ID3D11Texture2D* texture );
		D3D11Texture( const D3D11Device& device, const D3D11_TEXTURE2D_DESC& desc );
		~D3D11Texture();

		FORGE_INLINE ID3D11Texture2D* const& GetTexture() const
		{
			return m_texture;
		}

	private:
		ID3D11Texture2D* m_texture = nullptr;
	};
}
