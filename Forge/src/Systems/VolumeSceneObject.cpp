#include "Fpch.h"
#include "VolumeSceneObject.h"
#include "RenderingComponent.h"
#include "../Renderer/Material.h"
#include "DebugSystem.h"
#include "TransformComponent.h"

RTTI_IMPLEMENT_TYPE( forge::VolumeSceneObject, RTTI_ADD_METADATA( "Spawnable" ) );

void forge::VolumeSceneObject::OnInit( ObjectInitData& initData )
{
	Super::OnInit( initData );

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, 
		[ this ]()
		{
			auto& debugSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >();
			auto* transformComponent = GetComponent< forge::TransformComponent >();
			const Transform& transform = transformComponent->GetWorldTransform();

			auto dirToPos = [&]( const Vector4& dir )
				{
					return transform.GetPosition() +  transform.GetOrientation() * ( transformComponent->GetWorldScale() * dir * 0.5f );
				};

			systems::DebugSystem::DebugDrawParams drawParams;
			drawParams.m_color = LinearColor( 0.75f, 0.75f, 0.0f, 1.0f );

			const float thickness = 0.05f;

			debugSystem.DrawLine( dirToPos( Vector4( 1.0f, 1.0f, 1.0f ) ), dirToPos( Vector4( 1.0f, 1.0f, -1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( 1.0f, 1.0f, -1.0f ) ), dirToPos( Vector4( 1.0f, -1.0f, -1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( 1.0f, -1.0f, -1.0f ) ), dirToPos( Vector4( 1.0f, -1.0f, 1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( 1.0f, -1.0f, 1.0f ) ), dirToPos( Vector4( 1.0f, 1.0f, 1.0f ) ), thickness, drawParams );

			debugSystem.DrawLine( dirToPos( Vector4( -1.0f, 1.0f, 1.0f ) ), dirToPos( Vector4( -1.0f, 1.0f, -1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( -1.0f, 1.0f, -1.0f ) ), dirToPos( Vector4( -1.0f, -1.0f, -1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( -1.0f, -1.0f, -1.0f ) ), dirToPos( Vector4( -1.0f, -1.0f, 1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( -1.0f, -1.0f, 1.0f ) ), dirToPos( Vector4( -1.0f, 1.0f, 1.0f ) ), thickness, drawParams );

			debugSystem.DrawLine( dirToPos( Vector4( 1.0f, 1.0f, 1.0f ) ), dirToPos( Vector4( -1.0f, 1.0f, 1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( -1.0f, -1.0f, 1.0f ) ), dirToPos( Vector4( 1.0f, -1.0f, 1.0f ) ), thickness, drawParams );

			debugSystem.DrawLine( dirToPos( Vector4( 1.0f, 1.0f, -1.0f ) ), dirToPos( Vector4( -1.0f, 1.0f, -1.0f ) ), thickness, drawParams );
			debugSystem.DrawLine( dirToPos( Vector4( -1.0f, -1.0f, -1.0f ) ), dirToPos( Vector4( 1.0f, -1.0f, -1.0f ) ), thickness, drawParams );
		} );	
}
