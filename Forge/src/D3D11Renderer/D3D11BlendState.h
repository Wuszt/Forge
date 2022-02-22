#pragma once
#include "../Renderer/IBlendState.h"

struct ID3D11BlendState;

namespace d3d11
{
	class D3D11BlendState : public renderer::IBlendState
	{
	public:
		D3D11BlendState( d3d11::D3D11Device& device, d3d11::D3D11RenderContext& context, const renderer::BlendOperationDesc& rgbOperation, const renderer::BlendOperationDesc& alphaOperation );
		~D3D11BlendState();

		virtual void Set() override;
		virtual void Clear() override;

	private:
		ID3D11BlendState* m_blendState;
		D3D11RenderContext& m_context;
	};
}

