#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IRenderer;
	class IRenderTargetView;
	class ITexture;
	class FullScreenRenderingPass;

	class DefferedRenderingPass : public IMeshesRenderingPass
	{
	public:
		DefferedRenderingPass( IRenderer& renderer );

		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables ) override;
		virtual void ClearRenderTargetView() override;
		virtual void SetRenderTargetView( IRenderTargetView* renderTargetView ) override;

		ITexture* GetNormalsTexture() const
		{
			return m_normalsTexture.get();
		}

		ITexture* GetDiffuseTexture() const
		{
			return m_diffuseTexture.get();
		}

	private:
		std::unique_ptr< ITexture > m_normalsTexture;
		std::unique_ptr< ITexture > m_diffuseTexture;
		std::unique_ptr< FullScreenRenderingPass > m_lightingPass;
	};
}

