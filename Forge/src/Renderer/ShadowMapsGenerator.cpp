#include "Fpch.h"
#include "ShadowMapsGenerator.h"
#include "LightData.h"
#include "ForwardRenderingPass.h"
#include "CustomCamera.h"

renderer::ShadowMapsGenerator::ShadowMapsGenerator( IRenderer& renderer )
	: m_renderer( renderer )
{}

void renderer::ShadowMapsGenerator::GenerateShadowMaps( const renderer::RawRenderablesPacks& rawRenderablesPacks, LightingData& lightingData )
{
	for( auto& lightData : lightingData.m_pointLights )
	{
		if( lightData.m_shadowMap )
		{

			renderer::ForwardRenderingPass shadowsRenderingPass( m_renderer );
			shadowsRenderingPass.ClearTargetTexture();

			renderer::PerspectiveCamera camera( 1.0f, FORGE_PI_HALF, 1.0f, 1000.0f );
			
			constexpr Uint32 facesAmount = 6u;
			Vector3 directions[ facesAmount ] =
			{
				{ 1.0f, 0.0f, 0.0f },
				{ -1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 0.0f, 0.0f, -1.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, -1.0f, 0.0f }
			};
 
			camera.SetPosition( lightData.m_shaderData.Position );

			for( Uint32 i = 0u; i < facesAmount; ++i )
			{
				lightData.m_shadowMap->GetView( i ).Clear();
				shadowsRenderingPass.SetDepthStencilBuffer( lightData.m_shadowMap.get(), i );
				camera.SetOrientation( Quaternion::CreateFromDirection( directions[i] ) );
				shadowsRenderingPass.Draw( camera, rawRenderablesPacks.GetRendenderablesPack( RenderingPass::Opaque ), nullptr );
			}
		}
	}

	for( auto& lightData : lightingData.m_spotLights )
	{
		if( lightData.m_shadowMap )
		{
			lightData.m_shadowMap->GetView().Clear();

			renderer::ForwardRenderingPass shadowsRenderingPass( m_renderer );
			shadowsRenderingPass.SetDepthStencilBuffer( lightData.m_shadowMap.get() );
			shadowsRenderingPass.ClearTargetTexture();

			auto camera = renderer::CustomCamera( lightData.m_shaderData.VP, 0.0f, 0.0f );
			shadowsRenderingPass.Draw( camera, rawRenderablesPacks.GetRendenderablesPack( RenderingPass::Opaque ), nullptr );
		}
	}
}