#pragma once

class D3D11RenderTargetView;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11InputLayout;
struct ID3D11DeviceContext;

class D3D11Context
{
public:
	D3D11Context( ID3D11DeviceContext* deviceContext );
	~D3D11Context();

	FORGE_INLINE ID3D11DeviceContext* const& GetDeviceContext() const
	{
		return m_deviceContext;
	}

	void Draw( Uint32 indexCount, Uint32 offset );

private:
	ID3D11DeviceContext* m_deviceContext = nullptr;
};

