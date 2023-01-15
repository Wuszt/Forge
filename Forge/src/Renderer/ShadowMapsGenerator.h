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
	
		void GenerateShadowMaps( ecs::ECSManager& ecsManager, const ecs::Query& query, renderer::RenderingPass renderingPass, LightingData& lightingData );

	private:
		IRenderer& m_renderer;
	};
}

