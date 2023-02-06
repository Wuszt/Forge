#include "Fpch.h"

#ifdef FORGE_DEBUGGING
#include "DebugSystem.h"
#include "../Core/FPSCounter.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#include "TransformComponent.h"
#include "RenderingComponent.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/IRenderer.h"
#include "ECSDebug.h"
#endif

IMPLEMENT_TYPE( systems::DebugSystem);

systems::DebugSystem::DebugSystem() = default;
systems::DebugSystem::~DebugSystem() = default;

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

	m_ecsDebug = std::make_unique< ecs::ECSDebug >( GetEngineInstance() );
#endif

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [this]() { Update(); } );
}

void systems::DebugSystem::DrawSphere( const Vector3& position, Float radius, const Vector4& color, Float lifetime )
{
	GetEngineInstance().GetObjectsManager().RequestCreatingObject< forge::Object >( [ = ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ = ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\sphere.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( position );
			transformComponent->GetDirtyData().m_scale = { radius, radius, radius };
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->SetRenderingPass( renderer::RenderingPass::Overlay );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
		} );

		m_debugObjects.emplace_back( DebugObject{ obj->GetObjectID(), forge::Time::GetTime() + lifetime } );
	} );
}

void systems::DebugSystem::DrawCube( const Vector3& position, const Vector3& extension, const Vector4& color, Float lifetime )
{
	GetEngineInstance().GetObjectsManager().RequestCreatingObject< forge::Object >( [ = ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ = ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( position );
			transformComponent->GetDirtyData().m_scale = extension;
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->SetRenderingPass( renderer::RenderingPass::Overlay );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
		} );

		m_debugObjects.emplace_back( DebugObject{ obj->GetObjectID(), forge::Time::GetTime() + lifetime } );
	} );
}

void systems::DebugSystem::DrawLine( const Vector3& start, const Vector3& end, Float thickness, const Vector4& color, Float lifetime )
{
	GetEngineInstance().GetObjectsManager().RequestCreatingObject< forge::Object >( [ = ]( forge::Object* obj )
		{
			obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ = ]()
				{
					auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
	                auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( "Models\\cylinder.obj" );

					transformComponent->GetDirtyData().m_transform.SetPosition( start + ( end - start ) * 0.5f );
					transformComponent->GetDirtyData().m_scale = { thickness, thickness, ( end - start ).Mag() };
					transformComponent->GetDirtyData().m_transform.SetOrientation( Quaternion::GetRotationBetweenVectors( Vector3::EZ(), end - start ) );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->SetRenderingPass( renderer::RenderingPass::Overlay );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
				} );

	        m_debugObjects.emplace_back( DebugObject{ obj->GetObjectID(), forge::Time::GetTime() + lifetime } );
		} );
}

void systems::DebugSystem::DrawCone( const Vector3& top, const Vector3& base, Float angle, const Vector4& color, Float lifetime )
{
	GetEngineInstance().GetObjectsManager().RequestCreatingObject< forge::Object >( [ = ]( forge::Object* obj )
		{
			obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ = ]()
				{
					auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
	                auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( "Models\\cone.obj" );

					Vector3 direction = base - top;
					const Float length = direction.Normalize();

					Matrix transform( Quaternion( Vector3{ DEG2RAD * 90.0f, 0.0f, 0.0f } ) );
					transform = transform * Matrix( Vector3{ 0.0f, length * 0.5f, 0.0f } );
					transform = transform * Quaternion::CreateFromDirection( direction );

					transformComponent->GetDirtyData().m_transform.SetOrientation( transform.GetRotation() );
					transformComponent->GetDirtyData().m_transform.SetPosition( transform.GetTranslation() );

					const Float size = length * Math::Tg( angle * 0.5f ) * 2.0f;
					transformComponent->GetDirtyData().m_scale = Vector3{ size, size, length };

					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->SetRenderingPass( renderer::RenderingPass::Overlay );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
				} );

	        m_debugObjects.emplace_back( DebugObject{ obj->GetObjectID(), forge::Time::GetTime() + lifetime } );
		} );
}

void systems::DebugSystem::Update()
{
	Float currentTime = forge::Time::GetTime();
	for( auto it = m_debugObjects.begin(); it != m_debugObjects.end(); )
	{
		if( currentTime > it->m_timestamp )
		{
			GetEngineInstance().GetObjectsManager().RequestDestructingObject( it->m_objectId );
			it = forge::utils::RemoveReorder( m_debugObjects, it );
		}
		else
		{
			++it;
		}
	}
}

#endif