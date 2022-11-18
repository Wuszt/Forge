#include "Fpch.h"
#include "ShadowMapsGenerator.h"
#include "LightData.h"
#include "ForwardRenderingPass.h"
#include "CustomCamera.h"
#include "OrthographicCamera.h"

renderer::ShadowMapsGenerator::ShadowMapsGenerator( IRenderer& renderer )
	: m_renderer( renderer )
{}

void renderer::ShadowMapsGenerator::GenerateShadowMaps( const renderer::RawRenderablesPacks& rawRenderablesPacks, LightingData& lightingData )
{
	PC_SCOPE_FUNC();

	if( lightingData.m_pointLights.IsEmpty() && lightingData.m_spotLights.IsEmpty() && lightingData.m_directionalLights.IsEmpty() )
	{
		return;
	}

	m_renderer.SetDepthBias( 160.0f, 2.0f, 0.0f );

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

	for( auto& lightData : lightingData.m_directionalLights )
	{
		if( lightData.m_shadowMap )
		{
			lightData.m_shadowMap->GetView().Clear();

			renderer::ForwardRenderingPass shadowsRenderingPass( m_renderer );
			shadowsRenderingPass.SetDepthStencilBuffer( lightData.m_shadowMap.get() );
			shadowsRenderingPass.ClearTargetTexture();

			auto camera = renderer::OrthographicCamera( 5000.0f, 1.0f, 0.1f, 6000.0f );
			camera.SetPosition( -lightData.m_shaderData.Direction * 3500.0f );
			camera.SetOrientation( Quaternion::CreateFromDirection( lightData.m_shaderData.Direction ) );
			lightData.m_shaderData.VP = camera.GetViewProjectionMatrix();
			shadowsRenderingPass.Draw( camera, rawRenderablesPacks.GetRendenderablesPack( RenderingPass::Opaque ), nullptr );
		}
	}

	m_renderer.SetDepthBias( 0.0f, 0.0f, 0.0f );
}