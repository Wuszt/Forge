#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IRenderer;
	class ITexture;

	class SkyboxRenderingPass : public IRenderingPass
	{
	public:
		SkyboxRenderingPass( forge::AssetsManager& assetsManager, renderer::IRenderer& renderer, const std::string& skymapPath );

		void Draw( const renderer::ICamera& camera );

	private:
		std::shared_ptr< ITexture > m_skymapTexture;
		Renderable m_renderable;
		std::unique_ptr< renderer::StaticConstantBuffer< CBCamera > > m_cameraCB;
	};
}
