#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class Renderer;
	class ITexture;

	class SkyboxRenderingPass : public IRenderingPass
	{
	public:
		SkyboxRenderingPass( forge::AssetsManager& assetsManager, renderer::Renderer& renderer, std::shared_ptr< const renderer::ITexture > texture );

		void Draw( const renderer::ICamera& camera );

	private:
		Renderable m_renderable;
		std::unique_ptr< renderer::StaticConstantBuffer< CBCamera > > m_cameraCB;
	};
}
