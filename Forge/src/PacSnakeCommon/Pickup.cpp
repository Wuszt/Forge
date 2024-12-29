#include "Fpch.h"
#include "Pickup.h"
#include "../Systems/RenderingComponent.h"
#include "../Systems/TransformComponent.h"
#include "GridSystem.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Pickup );

void pacsnake::Pickup::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	FORGE_ASSURE( AddComponent< forge::RenderingComponent >() );
	GetComponent< forge::RenderingComponent >()->LoadMeshAndMaterial( forge::Path( "PacSnake\\Apple\\Apple.fbx" ) );
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	m_id = gridSystem.GetGrid().AddPawn( {}, {} );
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, [this]()
		{
			auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
			pacsnake::GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_id );

			auto* transformComp = GetComponent< forge::TransformComponent >();
			const Quaternion currOrientation =  transformComp->GetWorldOrientation();
			transformComp->SetWorldOrientation( Quaternion( currOrientation.ToEulerAngles() + Vector3( 0.0f, 0.0f, forge::Time::GetDeltaTime() ) ) );
		
			const Vector3 currPos(pawn->m_pos, ( Math::Sin( forge::Time::GetTime() ) + 3.0f ) * 0.2f );
			transformComp->SetWorldPosition( currPos );
			transformComp->SetWorldScale( { 1.5f, 1.5f, 1.5f } );
		});
}
