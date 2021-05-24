#pragma once
class D3D11PixelShader
{
public:
	D3D11PixelShader( const D3D11Device& device, const std::string& path );
	~D3D11PixelShader();

	FORGE_INLINE ID3D11PixelShader* const& GetShader() const
	{
		return m_pixelShader;
	}

private:
	ID3D10Blob* m_buffer = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
};

