#include "Fpch.h"
#include "SceneGrid.h"
#include "../GameEngine/ObjectsManager.h"
#include "../GameEngine/Object.h"
#include "../Systems/TransformComponent.h"
#include "../Systems/RenderingComponent.h"
#include "../Renderer/Material.h"

const Vector4 c_gridColor = Vector4( 1.0f, 1.0f, 1.0f, 0.5f );

editor::SceneGrid::SceneGrid( forge::EngineInstance& engineInstance )
{
	auto drawFunc = [ this ]( Int32 xStart, Int32 xEnd, Int32 yStart, Int32 yEnd, forge::Object& obj )
	{
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( "Models\\cylinder.obj" );

		const Vector3 start = { static_cast< Float >( xStart ), static_cast< Float >( yStart ), 0.0f };
		const Vector3 end = { static_cast< Float >( xEnd ), static_cast< Float >( yEnd ), 0.0f };
		const Float thickness = 0.01f;

		transformComponent->GetDirtyTransform().SetPosition( start + ( end - start ) * 0.5f );
		transformComponent->GetDirtyScale() = { thickness, thickness, ( end - start ).Mag() };
		transformComponent->GetDirtyTransform().SetOrientation( Quaternion::GetRotationBetweenVectors( Vector3::EZ(), end - start ) );

		renderingComponent->SetInteractingWithLight( false );
		renderingComponent->SetDrawAsOverlayEnabled( false );
		auto& material = *renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ];
		auto* materialBuffer = material.GetConstantBuffer();
		materialBuffer->SetData( "diffuseColor", c_gridColor );
		materialBuffer->UpdateBuffer();
		material.SetRenderingPass( renderer::RenderingPass::Transparent );
	};

	for ( Int32 x = -100; x < 100; ++x )
	{
		engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ this, x, drawFunc ]( forge::Object* obj )
			{
				m_linesTokens.emplace_back( *obj );

				obj->AddComponents< forge::TransformComponent, forge::RenderingComponent >();
				drawFunc( x, x, -100, 100, *obj );
			} );
	}

	for ( Int32 y = -100; y < 100; ++y )
	{
		engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ this, y, drawFunc ]( forge::Object* obj )
			{
				m_linesTokens.emplace_back( *obj );

		obj->AddComponents< forge::TransformComponent, forge::RenderingComponent >();
		drawFunc( -100, 100, y, y, *obj );
			} );
	}
}