#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"
#include "../Renderer/IRenderer.h"
#include "CamerasSystem.h"
#include "CameraComponent.h"
#include "../Renderer/ConstantBuffer.h"
#include "../Renderer/Renderable.h"

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	
	m_beforeDrawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) ) );
	m_drawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) ) );
	m_presentToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) ) );
	m_cameraCB = m_renderer->CreateStaticConstantBuffer< renderer::cbCamera >();
	m_rawRenderablesPackage = m_renderer->CreateRawRenderablesPackage( {} );
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
	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::RenderingSystem >();

	auto* activeCamera = m_camerasSystem->GetActiveCamera();

	if( !activeCamera )
	{
		return;
	}

	m_cameraCB->GetData().VP = activeCamera->GetCamera().GetViewProjectionMatrix();
	m_cameraCB->UpdateBuffer();
	m_cameraCB->SetVS( renderer::VSConstantBufferType::Camera );

	Bool anyArchetypeDirty = false;
	for( auto& archetype : archetypes )
	{
		anyArchetypeDirty |= archetype->IsDirty();
	}

	if( anyArchetypeDirty )
	{
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

	m_renderer->Draw( *m_rawRenderablesPackage );
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}
