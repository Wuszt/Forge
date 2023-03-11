#include "Fpch.h"
#include "TransformComponent.h"

IMPLEMENT_TYPE( forge::TransformFragment );
IMPLEMENT_TYPE( forge::TransformComponent );
IMPLEMENT_TYPE( forge::TransformModifiedThisFrame );

void forge::TransformComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	DataComponent< TransformFragment >::OnAttached( engineInstance, commandsQueue );
	GetOwner().GetEngineInstance().GetECSManager().AddTagToEntity< TransformModifiedThisFrame >( GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
}
