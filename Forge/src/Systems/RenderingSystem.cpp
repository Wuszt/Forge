#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"
#include "../Renderer/IRenderer.h"
#include "CamerasSystem.h"
#include "CameraComponent.h"
#include "../Renderer/ConstantBuffer.h"
#include "../Renderer/Renderable.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#endif

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	
	m_beforeDrawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) ) );
	m_drawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) ) );
	m_presentToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) ) );
	m_cameraCB = m_renderer->CreateStaticConstantBuffer< renderer::cbCamera >();
	m_rawRenderablesPackage = m_renderer->CreateRawRenderablesPackage( {} );

#ifdef FORGE_DEBUGGING
	m_clearingCacheToken = std::make_unique< forge::CallbackToken >( m_renderer->GetShadersManager()->RegisterCacheClearingListener( [ this ]()
	{
		const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::RenderingSystem >();
		for( systems::Archetype* archetype : archetypes )
		{
			forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();
			for( Uint32 i = 0; i < renderableComponents.GetDataSize(); ++i )
			{
				auto& materials = renderableComponents[ i ].m_renderable->GetMaterials();
				for( auto& material : materials )
				{
					material.SetVertexShader( material.GetVertexShaderPath() );
					material.SetPixelShader( material.GetPixelShaderPath() );
				}
			}
		}
	} ) );
#endif

#ifdef FORGE_IMGUI_ENABLED
	m_overlayDebugToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ this ]()
	{
		if( ImGui::Button( "Reload shaders" ) )
		{
			GetEngineInstance().GetRenderer().GetShadersManager()->ClearCache();
		}
	} ) );
#endif
}

void systems::RenderingSystem::OnBeforeDraw()
{
	m_renderer->BeginScene();
	m_renderer->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
	if( m_renderer->GetDepthStencilBuffer() )
	{
		m_renderer->GetDepthStencilBuffer()->Clear();
	}
}

void systems::RenderingSystem::OnDraw()
{
	PC_SCOPE_FUNC();

	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::RenderingSystem >();

	auto* activeCamera = m_camerasSystem->GetActiveCamera();

	if( !activeCamera )
	{
		return;
	}

	m_cameraCB->GetData().VP = activeCamera->GetCamera().GetViewProjectionMatrix();
	m_cameraCB->GetData().CameraPosition = activeCamera->GetCamera().GetPosition();
	m_cameraCB->UpdateBuffer();
	m_cameraCB->SetVS( renderer::VSConstantBufferType::Camera );
	m_cameraCB->SetPS( renderer::PSConstantBufferType::Camera );

	m_rawRenderablesPackage = nullptr;
	for( systems::Archetype* archetype : archetypes )
	{
		const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();

		std::vector< const renderer::Renderable* > renderables;

		for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
		{
			renderables.emplace_back( renderableComponents[ i ].m_renderable );
		}

		m_rawRenderablesPackage = m_renderer->CreateRawRenderablesPackage( renderables );
	}

	for( systems::Archetype* archetype : archetypes )
	{
		const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();

		for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
		{
			auto& cb = renderableComponents[ i ].m_renderable->GetCBMesh();
			auto prev = cb.GetData().W;
			cb.GetData().W = transformComponents[ i ].ToMatrix();

			// It's stupid, but UpdateBuffer takes so much time that it makes sense for now
			// I should probably separate static and dynamic objects in the future or/and keep an information which entity changed
			// it's transform somehow
			if( prev != cb.GetData().W )
			{
				cb.UpdateBuffer();
			}
		}
	}

	if( m_rawRenderablesPackage )
	{
		m_renderer->Draw( *m_rawRenderablesPackage );
	}
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}

void systems::RenderingSystem::SetSamplers( const std::vector< renderer::SamplerStateFilterType >& filterTypes )
{
	std::vector< renderer::ISamplerState* > samplerStates;
	for( auto filterType : filterTypes )
	{
		auto it = m_samplerStates.find( filterType );
		if( it == m_samplerStates.end() )
		{
			it = m_samplerStates.emplace( filterType, m_renderer->CreateSamplerState( filterType ) ).first;
		}

		samplerStates.push_back( it->second.get() );
	}

	m_renderer->SetSamplerStates( samplerStates );
}
