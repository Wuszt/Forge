#include "Fpch.h"
#include "TransformComponent.h"

RTTI_IMPLEMENT_TYPE( forge::TransformFragment );
RTTI_IMPLEMENT_TYPE( forge::PreviousFrameTransformFragment );
RTTI_IMPLEMENT_TYPE( forge::PreviousFrameScaleFragment );
RTTI_IMPLEMENT_TYPE( forge::TransformComponent );

void forge::TransformComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	DataComponent< TransformFragment >::OnAttached( engineInstance, commandsQueue );
	GetDirtyTransform() = Transform::IDENTITY();
	GetDirtyScale() = Vector3::ONES();
}

const Vector3* forge::TransformComponent::GetPrevFrameScale() const
{
	auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
	auto entityID = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );

	if ( const PreviousFrameScaleFragment* prevFrameScaleFragment = ecsManager.GetFragment< PreviousFrameScaleFragment >( entityID ) )
	{
		return &prevFrameScaleFragment->m_previousScale;
	}

	return nullptr;
}
