#pragma once
#include "../Renderer/IRenderContext.h"

class D3D11RenderTargetView;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11InputLayout;
struct ID3D11DeviceContext;

class D3D11RenderContext : public IRenderContext
{
public:
	D3D11RenderContext( ID3D11DeviceContext* deviceContext );
	~D3D11RenderContext();

	FORGE_INLINE ID3D11DeviceContext* const& GetDeviceContext() const
	{
		return m_deviceContext;
	}

	virtual void Draw( Uint32 indexCount, Uint32 offset ) override;

private:
	ID3D11DeviceContext* m_deviceContext = nullptr;
};

