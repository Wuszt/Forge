#pragma once
#include "IRenderingPass.h"
#include "ConstantBuffer.h"

namespace renderer
{
	class Renderer;
	class IRenderTargetView;
	class ITexture;
	class FullScreenRenderingPass;
	class ICamera;
	class IBlendState;

	class DeferredRenderingPass : public IMeshesRenderingPass
	{
	public:
		DeferredRenderingPass( Renderer& renderer );

		virtual void OnDraw( const renderer::ICamera& camera, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData ) override;
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

		static forge::ArraySpan< const ShaderDefine > GetRequiredShaderDefines();

	private:
		std::unique_ptr< ITexture > m_normalsTexture;
		std::unique_ptr< ITexture > m_diffuseTexture;
		std::unique_ptr< FullScreenRenderingPass > m_lightingPass;

		std::unique_ptr< IConstantBuffer > m_cbDeferredRendering;
		std::unique_ptr< IConstantBuffer > m_cbPointLight;
		std::unique_ptr< IConstantBuffer > m_cbSpotLight;
		std::unique_ptr< IConstantBuffer > m_cbDirectionalLight;

		std::unique_ptr< IBlendState > m_blendState;
	};
}

