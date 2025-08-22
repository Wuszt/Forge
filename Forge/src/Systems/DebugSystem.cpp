#include "Fpch.h"

#ifdef FORGE_DEBUGGING
#include "DebugSystem.h"
#include "../Core/FPSCounter.h"
#include "TransformComponent.h"
#include "RenderingComponent.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/Renderer.h"
#include "ECSDebug.h"
#include "../Renderer/IVertexBuffer.h"
#include "../GameEngine/RenderingManager.h"
#include "../Renderer/Model.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
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

void systems::DebugSystem::DrawSphere( const Vector3& position, Float radius, const DebugDrawParams& debugDrawParams )
{
	auto initFunc = [ debugDrawParams, position, radius ]( forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\sphere.obj" ) );

		if ( debugDrawParams.m_wireFrame )
		{
			renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
		}

		transformComponent->SetWorldPosition( position );
		transformComponent->SetWorldScale( { radius, radius, radius } );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( debugDrawParams.m_overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", debugDrawParams.m_color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), debugDrawParams.m_lifetime } );
}
void systems::DebugSystem::DrawCube( const Transform& transform, const Vector3& extension, const DebugDrawParams& debugDrawParams )
{
	auto initFunc = [ debugDrawParams, transform, extension ]( forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\cube.obj" ) );

		if ( debugDrawParams.m_wireFrame )
		{
			renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
		}

		transformComponent->SetWorldTransform( transform );
		transformComponent->SetWorldScale( extension );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( debugDrawParams.m_overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", debugDrawParams.m_color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), debugDrawParams.m_lifetime } );
}

void systems::DebugSystem::DrawLine( const Vector3& start, const Vector3& end, Float thickness, const DebugDrawParams& debugDrawParams )
{
	auto initFunc = [ debugDrawParams, start, end, thickness ]( forge::Object& obj )
	{
			auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\cylinder.obj" ) );

		if ( debugDrawParams.m_wireFrame )
		{
			renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
		}

		transformComponent->SetWorldPosition( start + ( end - start ) * 0.5f );
		transformComponent->SetWorldScale( { thickness, thickness, ( end - start ).Mag() } );
		transformComponent->SetWorldOrientation( Quaternion::GetRotationBetweenVectors( Vector3::EZ(), end - start ) );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( debugDrawParams.m_overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", debugDrawParams.m_color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), debugDrawParams.m_lifetime } );
}

void systems::DebugSystem::DrawCone( const Vector3& top, const Vector3& base, Float angle, const DebugDrawParams& debugDrawParams )
{
	auto initFunc = [ debugDrawParams, top, base, angle ]( forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\cone.obj" ) );

		if ( debugDrawParams.m_wireFrame )
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
		renderingComponent->SetDrawAsOverlayEnabled( debugDrawParams.m_overlay );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", debugDrawParams.m_color );
		renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
	};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), debugDrawParams.m_lifetime } );
}

void systems::DebugSystem::DrawShape( const Transform& transform, forge::ArraySpan< const Vector3 > vertices, const DebugDrawParams& debugDrawParams )
{
	std::vector< renderer::InputPosition > positions;
	positions.reserve( vertices.GetSize() );
	for ( const Vector3& vert : vertices )
	{
		positions.push_back( vert );
	}

	auto initFunc = [ debugDrawParams, transform, positions = std::move( positions ), this ]( forge::Object& obj )
		{
			auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

			auto& renderer = GetEngineInstance().GetRenderingManager().GetRenderer();

			renderer::Shape shape;
			shape.m_indices.resize( positions.size() );
			std::iota( shape.m_indices.begin(), shape.m_indices.end(), 0 );
			renderer::Model model( renderer, renderer::Vertices( forge::ArraySpan< const renderer::InputPosition >( positions ) ), {shape});

			renderingComponent->GetDirtyData()->m_renderable.SetModel( std::move( model ) );
			renderingComponent->GetDirtyData()->m_renderable.SetDefaultMaterial( renderer );

			if ( debugDrawParams.m_wireFrame )
			{
				renderingComponent->GetDirtyData()->m_renderable.SetFillMode( renderer::FillMode::WireFrame );
			}

			transformComponent->SetWorldTransform( transform );

			renderingComponent->SetInteractingWithLight( false );
			renderingComponent->SetDrawAsOverlayEnabled( debugDrawParams.m_overlay );
			renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", debugDrawParams.m_color );
			renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
		};

	m_objectsCreationRequests.emplace_back( ObjectCreationRequest{ std::move( initFunc ), debugDrawParams.m_lifetime } );
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