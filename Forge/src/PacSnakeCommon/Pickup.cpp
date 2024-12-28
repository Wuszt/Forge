#include "Fpch.h"
#include "Pickup.h"
#include "../Systems/RenderingComponent.h"
#include "../Systems/TransformComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Pickup );

void pacsnake::Pickup::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	FORGE_ASSURE( AddComponent< forge::RenderingComponent >() );
	GetComponent< forge::RenderingComponent >()->LoadMeshAndMaterial( forge::Path( "PacSnake\\Apple\\Apple.fbx" ) );
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, [this]()
		{
			auto* transformComp = GetComponent< forge::TransformComponent >();
			const Quaternion currOrientation =  transformComp->GetWorldOrientation();
			transformComp->SetWorldOrientation( Quaternion( currOrientation.ToEulerAngles() + Vector3( 0.0f, 0.0f, forge::Time::GetDeltaTime() ) ) );
		
			Vector3 currPos = transformComp->GetWorldPosition();
			currPos.Z = ( Math::Sin( forge::Time::GetTime() ) + 3.0f ) * 0.2f;
			transformComp->SetWorldPosition( currPos );
			transformComp->SetWorldScale( { 0.5f, 0.5f, 0.5f } );
		});
}
