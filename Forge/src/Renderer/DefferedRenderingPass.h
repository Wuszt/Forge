#pragma once
#include "IRenderingPass.h"
#include "ConstantBuffer.h"

namespace renderer
{
	class IRenderer;
	class IRenderTargetView;
	class ITexture;
	class FullScreenRenderingPass;
	class ICamera;
	class IBlendState;

	class DefferedRenderingPass : public IMeshesRenderingPass
	{
	public:
		DefferedRenderingPass( IRenderer& renderer, std::function< const forge::ICamera&() > activeCameraGetter );

		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData& lightingData ) override;
		virtual void ClearTargetTexture() override;
		virtual void SetTargetTexture( ITexture& targetTexture ) override;
		virtual void OnTargetTextureResized( const Vector2& size ) override;

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
		std::function< const forge::ICamera&( ) > m_activeCameraGetter;
		std::vector< LightData > m_lightsData;

		std::unique_ptr< IConstantBuffer > m_cbDefferedRendering;
		std::unique_ptr< IConstantBuffer > m_CBdefferedLighting;

		std::unique_ptr< IBlendState > m_blendState;
	};
}

