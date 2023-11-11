#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IBlendState;
	class ShaderDefine;

	class ForwardRenderingPass : public IMeshesRenderingPass 
	{
	public:
		ForwardRenderingPass( Renderer& renderer );
		~ForwardRenderingPass();

		virtual void OnDraw( const renderer::ICamera& camera, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData ) override;

		static forge::ArraySpan< const ShaderDefine > GetRequiredShaderDefines();

	private:
		std::unique_ptr< IConstantBuffer > m_cbForwardRendering;
		std::unique_ptr< IConstantBuffer > m_cbPointLight;
		std::unique_ptr< IConstantBuffer > m_cbSpotLight;
		std::unique_ptr< IConstantBuffer > m_cbDirectionalLight;
		std::unique_ptr< IBlendState > m_blendState;
	};
}

