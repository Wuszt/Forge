#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IRenderer;
	class IRenderTargetView;
	class FullScreenRenderingPass;

	class DefferedRenderingPass : public IMeshesRenderingPass
	{
	public:
		DefferedRenderingPass( IRenderer& renderer );

		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables ) override;
		virtual void ClearRenderTargetView() override;
		virtual void SetRenderTargetView( IRenderTargetView* renderTargetView ) override;

		IRenderTargetView* GetNormalsTargetView() const
		{
			return m_normalsTargetView.get();
		}

		IRenderTargetView* GetDiffuseTargetView() const
		{
			return m_diffuseTargetView.get();
		}

	private:
		std::unique_ptr< IRenderTargetView > m_normalsTargetView;
		std::unique_ptr< IRenderTargetView > m_diffuseTargetView;
		std::unique_ptr< FullScreenRenderingPass > m_lightingPass;
	};
}

