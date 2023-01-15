#include "Fpch.h"
#include "TransformComponent.h"

IMPLEMENT_TYPE( forge::TransformFragment );
IMPLEMENT_TYPE( forge::TransformComponent );

void forge::TransformComponent::OnAttach( EngineInstance& engineInstance )
{
	DataComponent< TransformFragment >::OnAttach( engineInstance );
	GetOwner().GetEngineInstance().GetECSManager().AddTagToEntity< TransformModifiedThisFrame >( GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
}
