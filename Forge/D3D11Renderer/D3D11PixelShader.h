#pragma once
class D3D11Device;
struct ID3D10Blob;
struct ID3D11PixelShader;

class D3D11PixelShader
{
public:
	D3D11PixelShader( D3D11Context* contextPtr, const D3D11Device& device, const std::string& path );
	~D3D11PixelShader();

	FORGE_INLINE ID3D11PixelShader* const& GetShader() const
	{
		return m_pixelShader;
	}

	void Set();

private:
	ID3D10Blob* m_buffer = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	D3D11Context* m_contextPtr = nullptr;
};

