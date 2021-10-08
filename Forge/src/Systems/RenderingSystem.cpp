#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"
#include "../Renderer/IRenderer.h"
#include "CamerasSystem.h"
#include "CameraComponent.h"
#include "../Renderer/ConstantBuffer.h"

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	
	m_beforeDrawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) ) );
	m_drawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) ) );
	m_presentToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) ) );

	m_buffer = m_renderer->CreateStaticConstantBuffer< cbMesh >();
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
	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypeOfSystem< systems::RenderingSystem >();

	Matrix vp = m_camerasSystem->GetActiveCamera()->GetCamera().GetViewProjectionMatrix();

	for( systems::Archetype* archetype : archetypes )
	{
		const forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::RenderingComponentData >& renderables = archetype->GetData< forge::RenderingComponentData >();

		for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
		{
			m_buffer->GetData().W = transforms[ i ].ToMatrix();
			m_buffer->GetData().VP = vp;
			m_buffer->UpdateBuffer();
			m_buffer->SetVS( renderer::VSConstantBufferType::Mesh );

			m_renderer->Draw( *renderables[ i ].m_renderable );
		}
	}
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}
