#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IBlendState;
	struct ShaderDefine;

	class ForwardRenderingPass : public IMeshesRenderingPass 
	{
	public:
		ForwardRenderingPass( IRenderer& renderer );
		~ForwardRenderingPass();

		virtual void Draw( const renderer::ICamera& camera, const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData ) override;

		static forge::ArraySpan< const ShaderDefine > GetRequiredShaderDefines();

	private:
		std::unique_ptr< IConstantBuffer > m_cbForwardRendering;
		std::unique_ptr< IConstantBuffer > m_cbPointLight;
		std::unique_ptr< IConstantBuffer > m_cbSpotLight;
		std::unique_ptr< IConstantBuffer > m_cbDirectionalLight;
		std::unique_ptr< IBlendState > m_blendState;
	};
}

