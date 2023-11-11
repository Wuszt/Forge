#pragma once

namespace renderer
{
	class Renderer;
	struct LightingData;
	class RawRenderablesPacks;
	class ShaderDefine;

	class ShadowMapsGenerator
	{
	public:
		ShadowMapsGenerator( Renderer& renderer );
	
		void GenerateShadowMaps( const ecs::Query& query, renderer::RenderingPass renderingPass, LightingData& lightingData );

	private:
		Renderer& m_renderer;
	};
}

