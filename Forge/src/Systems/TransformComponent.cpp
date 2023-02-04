#include "Fpch.h"
#include "TransformComponent.h"

IMPLEMENT_TYPE( forge::TransformFragment );
IMPLEMENT_TYPE( forge::TransformComponent );
IMPLEMENT_TYPE( forge::TransformModifiedThisFrame );

void forge::TransformComponent::OnAttach( EngineInstance& engineInstance )
{
	DataComponent< TransformFragment >::OnAttach( engineInstance );
	GetOwner().GetEngineInstance().GetECSManager().AddTagToEntity< TransformModifiedThisFrame >( GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
}
