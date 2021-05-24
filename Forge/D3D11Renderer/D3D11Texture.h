#pragma once
class D3D11Texture
{
public:
	D3D11Texture( ID3D11Texture2D* texture );
	~D3D11Texture();

	FORGE_INLINE ID3D11Texture2D* const& GetTexture() const
	{
		return m_texture;
	}

private:
	ID3D11Texture2D* m_texture = nullptr;
};

