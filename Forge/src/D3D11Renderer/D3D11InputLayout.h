#pragma once
#include "../Renderer/IInputLayout.h"


struct ID3D11InputLayout;
enum D3D_PRIMITIVE_TOPOLOGY;

namespace d3d11
{
	class D3D11Device;
	class D3D11VertexShader;
	class D3D11RenderContext;

	class D3D11InputLayout : public renderer::IInputLayout
	{
	public:
		D3D11InputLayout( D3D11RenderContext* contextPtr, const D3D11Device& device, const D3D11VertexShader& vertexShader, const D3D11VertexBuffer& vertexBuffer );
		~D3D11InputLayout();

		ID3D11InputLayout* const& GetLayout() const
		{
			return m_layout;
		}

		D3D_PRIMITIVE_TOPOLOGY GetTopology() const;

		virtual void Set() override;

	private:
		ID3D11InputLayout* m_layout = nullptr;
		D3D11RenderContext* m_contextPtr = nullptr;
	};
}

