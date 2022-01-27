#pragma once
#include "../Renderer/IRenderTargetView.h"

struct Vector4;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;

namespace d3d11
{
	class D3D11RenderContext;
	class D3D11Device;
	class D3D11Texture;

	class D3D11RenderTargetView : public renderer::IRenderTargetView
	{
	public:
		D3D11RenderTargetView( const D3D11Device& device, const D3D11RenderContext& context, ID3D11Texture2D& texture );
		~D3D11RenderTargetView();

		FORGE_INLINE ID3D11RenderTargetView* GetRenderTargetView() const
		{
			return m_renderTargetView;
		}

		virtual void Clear( const Vector4& bgColor = { 0.0f, 0.0f, 0.0f, 0.0f } ) override;

	private:
		ID3D11RenderTargetView* m_renderTargetView = nullptr;
		const D3D11RenderContext& m_context;
	};
}
