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
		D3D11VertexShader( D3D11RenderContext* contextPtr, const D3D11Device& device, const std::string& path );
		~D3D11VertexShader();

		FORGE_INLINE ID3D11VertexShader* const& GetShader() const
		{
			return m_vertexShader;
		}

		FORGE_INLINE ID3D10Blob* const& GetBuffer() const
		{
			return m_buffer;
		}

		virtual void Set() override;

	private:
		ID3D10Blob* m_buffer = nullptr;
		ID3D11VertexShader* m_vertexShader = nullptr;
		D3D11RenderContext* m_contextPtr = nullptr;
	};
}
