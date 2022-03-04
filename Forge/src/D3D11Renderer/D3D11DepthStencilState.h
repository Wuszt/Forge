#pragma once
#include "../Renderer/IDepthStencilState.h"

struct ID3D11DepthStencilState;

namespace d3d11
{
	class D3D11DepthStencilState : public renderer::IDepthStencilState
	{
	public:
		D3D11DepthStencilState( d3d11::D3D11Device& device, d3d11::D3D11RenderContext& context, renderer::DepthStencilComparisonFunc comparisonFunc );
		~D3D11DepthStencilState();

		virtual void Set() override;
		virtual void Clear() override;

	private:
		ID3D11DepthStencilState* m_depthStencilState = nullptr;
		D3D11RenderContext& m_context;
	};
}

