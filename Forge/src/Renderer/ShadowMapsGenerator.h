#pragma once

namespace renderer
{
	class IRenderer;
	class ShadowMapRenderingPass;
	struct LightingData;
	class RawRenderablesPacks;
	struct ShaderDefine;

	class ShadowMapsGenerator
	{
	public:
		ShadowMapsGenerator( IRenderer& renderer );
	
		void GenerateShadowMaps( const renderer::RawRenderablesPacks& rawRenderablesPacks, LightingData& lightingData );

	private:
		IRenderer& m_renderer;
	};
}

