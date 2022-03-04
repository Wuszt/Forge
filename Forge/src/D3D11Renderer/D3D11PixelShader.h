#pragma once
#include "../Renderer/IPixelShader.h"

struct ID3D10Blob;
struct ID3D11PixelShader;

namespace d3d11
{
	class D3D11Device;

	class D3D11PixelShader : public renderer::IPixelShader
	{
	public:
		D3D11PixelShader( const D3D11Device& device, D3D11RenderContext& context, const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines );
		~D3D11PixelShader();

		FORGE_INLINE ID3D11PixelShader* GetShader() const
		{
			return m_pixelShader;
		}

		virtual void Set() const override;

		FORGE_INLINE ID3D10Blob* GetBuffer() const
		{
			return m_buffer;
		}

	private:
		ID3D10Blob* m_buffer = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		D3D11RenderContext& m_context;
	};
}
