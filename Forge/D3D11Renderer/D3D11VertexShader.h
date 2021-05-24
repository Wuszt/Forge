#pragma once

class D3D11Device;

class D3D11VertexShader
{
public:
	D3D11VertexShader( const D3D11Device& device, const std::string& path );
	~D3D11VertexShader();

	FORGE_INLINE ID3D11VertexShader* const& GetShader() const
	{
		return m_vertexShader;
	}

private:
	ID3D10Blob* m_buffer = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
};

