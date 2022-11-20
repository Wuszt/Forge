#pragma once
#include "../Renderer/IRenderContext.h"

struct ID3D11DeviceContext;

namespace d3d11
{
	class D3D11RenderTargetView;
	class D3D11VertexShader;
	class D3D11PixelShader;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	class D3D11InputLayout;

	class D3D11RenderContext : public renderer::IRenderContext
	{
	public:
		D3D11RenderContext( ID3D11DeviceContext* deviceContext );
		~D3D11RenderContext();

		ID3D11DeviceContext* GetDeviceContext() const
		{
			return m_deviceContext;
		}

		virtual void Draw( Uint32 indexCount, Uint32 offset ) override;

	private:
		ID3D11DeviceContext* m_deviceContext = nullptr;
	};
}
