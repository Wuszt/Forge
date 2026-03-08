#include "Fpch.h"
#include "NavmeshVolume.h"
#include "NavmeshSystem.h"

RTTI_IMPLEMENT_TYPE( forge::ai::NavmeshVolume, RTTI_ADD_METADATA( "Spawnable" ) );
RTTI_IMPLEMENT_TYPE( forge::ai::NavmeshVolume::NavmeshVolumeTag );

void forge::ai::NavmeshVolume::OnInit( ObjectInitData& initData )
{
	Super::OnInit( initData );
	auto& ecsManager = GetEngineInstance().GetECSManager();
	auto& objectsManager = GetEngineInstance().GetObjectsManager();
	ecsManager.AddTagToEntity< NavmeshVolumeTag >( objectsManager.GetOrCreateEntityId( GetObjectID() ) );
}
