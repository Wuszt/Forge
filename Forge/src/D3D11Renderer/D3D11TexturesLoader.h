#pragma once
#include "../Renderer/ITexturesLoader.h"

namespace d3d11
{
	class D3D11Device;
	class D3D11RenderContext;

	class D3D11TexturesLoader : public renderer::ITexturesLoader
	{
	public:
		D3D11TexturesLoader( const forge::DepotsContainer& depotsContainer, const D3D11Device& device, const D3D11RenderContext& context );
		virtual std::shared_ptr< renderer::ITexture > LoadTexture( const std::string& path ) override;

	private:
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
	};
}

