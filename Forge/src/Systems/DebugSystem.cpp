#include "Fpch.h"

#ifdef FORGE_DEBUGGING
#include "DebugSystem.h"
#include "../Core/FPSCounter.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../../External/imgui/imgui.h"
#include "TransformComponent.h"
#include "RenderingComponent.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#endif

void systems::DebugSystem::OnInitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	m_fpsCounterDrawingToken = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( []()
	{
		Float fps = forge::FPSCounter::GetAverageFPS( 1u );
		Vector4 color{ 1.0f, 0.0f, 0.0f, 1.0f };

		if( fps > 30.0f )
		{
			if( fps > 6.0f )
			{
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				color = { 1.0f, 1.0f, 0.0f, 1.0f };
			}
		}

		Uint32 bufferSize = 0u;
		Uint32 currentOffset = 0u;
		Float* buff = forge::FPSCounter::GetFPSCounter().GetBuffer( currentOffset, bufferSize );

		ImGui::PlotHistogram( "", buff, bufferSize, currentOffset, NULL, 0.0f, 120.0f, ImVec2( 100.0f, 40.0f ) );
		ImGui::SameLine();

		ImGui::SetWindowFontScale( 1.5f );
		ImGui::PushStyleColor( ImGuiCol_Text, { color.X, color.Y, color.Z, color.W } );
		ImGui::Text( "FPS: %.2f", fps );
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale( 1.0f );
	} );
#endif

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [this]() { Update(); } );
}

void systems::DebugSystem::DrawSphere( const Vector3& position, Float radius, const Vector4& color, Float lifetime )
{
	GetEngineInstance().GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ = ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ = ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "sphere.obj" );

			transformComponent->GetData().m_transform.SetPosition( position );
			transformComponent->GetData().m_scale = { radius, radius, radius };
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->SetData( "diffuseColor", color );
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->UpdateBuffer();
		} );

		m_debugEntities.emplace_back( DebugEntity{ obj->GetEntityID(), forge::Time::GetTime() + lifetime } );
	} );
}

void systems::DebugSystem::DrawCube( const Vector3& position, const Vector3& extension, const Vector4& color, Float lifetime )
{
	GetEngineInstance().GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ = ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ = ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "cube.obj" );

			transformComponent->GetData().m_transform.SetPosition( position );
			transformComponent->GetData().m_scale = extension * 2.0f;
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->SetData( "diffuseColor", color );
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->UpdateBuffer();
		} );

		m_debugEntities.emplace_back( DebugEntity{ obj->GetEntityID(), forge::Time::GetTime() + lifetime } );
	} );
}

void systems::DebugSystem::Update()
{
	Float currentTime = forge::Time::GetTime();
	for( auto it = m_debugEntities.begin(); it != m_debugEntities.end(); )
	{
		if( currentTime > it->m_timestamp )
		{
			GetEngineInstance().GetEntitiesManager().RequestDestructingEntity( it->m_entityId );
			it = forge::utils::RemoveReorder( m_debugEntities, it );
		}
		else
		{
			++it;
		}
	}
}

#endif