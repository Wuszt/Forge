#pragma once

namespace renderer
{
	class ShadowsRenderingPass : public IMeshesRenderingPass
	{
	public:
		using IMeshesRenderingPass::IMeshesRenderingPass;
		virtual void OnDraw( const renderer::ICamera& camera, ecs::ECSManager& ecsManager, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData ) override;

		static forge::ArraySpan< const ShaderDefine > GetRequiredShaderDefines();

	private:
		static const renderer::ShaderDefine c_shadowPassDefine;
	};
}
