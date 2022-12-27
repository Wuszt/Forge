#pragma once

namespace renderer
{
	class IRenderer;
	struct LightingData;
	class RawRenderablesPacks;
	class ShaderDefine;

	class ShadowMapsGenerator
	{
	public:
		ShadowMapsGenerator( IRenderer& renderer );
	
		void GenerateShadowMaps( const renderer::RawRenderablesPacks& rawRenderablesPacks, LightingData& lightingData );

	private:
		IRenderer& m_renderer;
	};
}

