#pragma once
#include "../Renderer/IVertexShader.h"

struct ID3D10Blob;
struct ID3D11VertexShader;

namespace d3d11
{
	class D3D11Device;

	class D3D11VertexShader : public renderer::IVertexShader
	{
	public:
		D3D11VertexShader( D3D11Device& device, D3D11RenderContext& context, const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines );
		~D3D11VertexShader();

		ID3D11VertexShader* GetShader() const
		{
			return m_vertexShader;
		}

		ID3D10Blob* GetBuffer() const
		{
			return m_buffer;
		}

		virtual void Set() const override;

	private:
		ID3D10Blob* m_buffer = nullptr;
		ID3D11VertexShader* m_vertexShader = nullptr;
		D3D11RenderContext& m_context;
	};
}

