#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class Renderer;
	class ITexture;
	class IDepthStencilBuffer;

	class SkyboxRenderingPass : public IRenderingPass
	{
	public:
		SkyboxRenderingPass( forge::AssetsManager& assetsManager, renderer::Renderer& renderer, std::shared_ptr< const renderer::ITexture > texture );

		void Draw( const renderer::ICamera& camera );

		void SetDepthStencilBuffer( IDepthStencilBuffer* depthStencilBuffer )
		{
			m_depthStencilBuffer = depthStencilBuffer;
		}

		IDepthStencilBuffer* GetDepthStencilBuffer() const
		{
			return m_depthStencilBuffer;
		}

	private:
		Renderable m_renderable;
		std::unique_ptr< renderer::StaticConstantBuffer< CBCamera > > m_cameraCB;
		IDepthStencilBuffer* m_depthStencilBuffer = nullptr;
	};
}
