#pragma once

namespace renderer
{
	class ShadowsRenderingPass : public IMeshesRenderingPass
	{
	public:
		using IMeshesRenderingPass::IMeshesRenderingPass;
		virtual void OnDraw( const renderer::ICamera& camera, const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData );

		static forge::ArraySpan< const ShaderDefine > GetRequiredShaderDefines();

	private:
		static const renderer::ShaderDefine c_shadowPassDefine;
	};
}
