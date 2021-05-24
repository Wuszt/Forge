#pragma once
class D3D11Context
{
public:
	D3D11Context( ID3D11DeviceContext* deviceContext );
	~D3D11Context();

	void SetRenderTarget( const D3D11RenderTargetView& renderTargetView );
	void SetVertexShader( const D3D11VertexShader& vertexShader );
	void SetPixelShader( const D3D11PixelShader& pixelShader );

	FORGE_INLINE ID3D11DeviceContext* const& GetDeviceContext() const
	{
		return m_deviceContext;
	}

private:
	ID3D11DeviceContext* m_deviceContext = nullptr;
};

