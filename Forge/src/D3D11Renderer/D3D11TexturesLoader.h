#pragma once
#include "../Renderer/ITexturesLoader.h"

namespace d3d11
{
	class D3D11Device;

	class D3D11TexturesLoader : public renderer::ITexturesLoader
	{
	public:
		D3D11TexturesLoader( const d3d11::D3D11Device& device );
		virtual std::shared_ptr< renderer::ITexture > LoadTexture( const std::string& path ) override;

	private:
		const d3d11::D3D11Device& m_device;
	};
}

