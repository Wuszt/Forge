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
#include "../Renderer/Renderer.h"
#include "ECSDebug.h"
#endif

RTTI_IMPLEMENT_TYPE( systems::DebugSystem );

systems::DebugSystem::DebugSystem() = default;
systems::DebugSystem::DebugSystem( DebugSystem&& ) = default;
systems::DebugSystem::~DebugSystem() = default;

void systems::DebugSystem::OnPostInit()
{
#ifdef FORGE_IMGUI_ENABLED
	m_fpsCounterDrawingToken = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( []()
		{
			Float fps = forge::FPSCounter::GetAverageFPS( 1u );
			Vector4 color{ 1.0f, 0.0f, 0.0f, 1.0f };

			const Float minFps = 30.0f;
			const Float criticalFps = 6.0f;

			if ( fps > minFps )
			{
				if ( fps > criticalFps )
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

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, [ this ]() { Update(); } );
}

void systems::DebugSystem::DrawSphere( const Vector3& position, Float radius, const LinearColor& color, Bool wireFrame, Bool overlay, Float lifetime )
{
	auto initFunc = [ = ]( forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\sphere.obj" ) );

		if ( wireFrame )
		{
			renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
		}

		transformComponent->SetWorldPosition( position );
		transformComponent->SetWorldScale( { radius, radius, radius } );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), lifetime } );
}

void systems::DebugSystem::DrawCube( const Vector3& position, const Vector3& extension, const LinearColor& color, Bool wireFrame, Bool overlay, Float lifetime )
{
	auto initFunc = [ = ]( forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\cube.obj" ) );

		if ( wireFrame )
		{
			renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
		}

		transformComponent->SetWorldPosition( position );
		transformComponent->SetWorldScale( extension );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), lifetime } );
}

void systems::DebugSystem::DrawLine( const Vector3& start, const Vector3& end, Float thickness, const LinearColor& color, Bool overlay, Float lifetime )
{
	auto initFunc = [ = ]( forge::Object& obj )
	{
			auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\cylinder.obj" ) );

		transformComponent->SetWorldPosition( start + ( end - start ) * 0.5f );
		transformComponent->SetWorldScale( { thickness, thickness, ( end - start ).Mag() } );
		transformComponent->SetWorldOrientation( Quaternion::GetRotationBetweenVectors( Vector3::EZ(), end - start ) );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), lifetime } );
}

void systems::DebugSystem::DrawCone( const Vector3& top, const Vector3& base, Float angle, const LinearColor& color, Bool wireFrame, Bool overlay, Float lifetime )
{
	auto initFunc = [ = ]( forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\cone.obj" ) );

		if ( wireFrame )
		{
			renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
		}

		Vector3 direction = base - top;
		const Float length = direction.Normalize();

		Matrix transform( Quaternion( Vector3{ DEG2RAD * 90.0f, 0.0f, 0.0f } ) );
		transform = transform * Matrix( Vector3{ 0.0f, length * 0.5f, 0.0f } );
		transform = transform * Quaternion::CreateFromDirection( direction );
		transform = transform * Matrix( top );

		transformComponent->SetWorldOrientation( transform.GetRotation() );
		transformComponent->SetWorldPosition( transform.GetTranslation() );

		const Float size = length * Math::Tg( angle );
		transformComponent->SetWorldScale( { size, size, length } );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), lifetime } );
}

void systems::DebugSystem::Update()
{
	Float currentTime = forge::Time::GetTime();
	Int32 requestsLastIndex = static_cast< Int32 >( m_objectsCreationRequests.size() ) - 1;
	for ( auto it = m_debugObjects.begin(); it != m_debugObjects.end(); )
	{
		if ( currentTime > it->m_timestamp )
		{
			if ( requestsLastIndex >= 0 )
			{
				m_objectsCreationRequests[ requestsLastIndex ].m_initFunc( *GetEngineInstance().GetObjectsManager().GetObject( it->m_objectId ) );
				it->m_timestamp = forge::Time::GetTime() + m_objectsCreationRequests[ requestsLastIndex ].m_timestamp;
				--requestsLastIndex;
			}
			else
			{
				GetEngineInstance().GetObjectsManager().RequestDestructingObject( it->m_objectId );
				it = forge::utils::RemoveReorder( m_debugObjects, it );
				continue;
			}
		}

		++it;
	}

	m_objectsCreationRequests.resize( requestsLastIndex + 1 );

	for ( auto&& request : std::move( m_objectsCreationRequests ) )
	{
		GetEngineInstance().GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ request = std::move( request ), this ]( forge::Object& obj, forge::ObjectInitData& )
		{
			obj.AddComponents< forge::TransformComponent, forge::RenderingComponent >();
			request.m_initFunc( obj );
			m_debugObjects.emplace_back( DebugObject{ obj.GetObjectID(), forge::Time::GetTime() + request.m_timestamp } );
		} } );
	}

	m_objectsCreationRequests.clear();
}

#endif