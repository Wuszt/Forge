#include "Fpch.h"
#include "TransformComponent.h"

RTTI_IMPLEMENT_TYPE( forge::TransformFragment,
	RTTI_REGISTER_PROPERTY( m_transform );
	RTTI_REGISTER_PROPERTY( m_scale );
	);
RTTI_IMPLEMENT_TYPE( forge::ChildRequiringRecalculatingLocalTransform );
RTTI_IMPLEMENT_TYPE( forge::TransformWasModifiedThisFrame );
RTTI_IMPLEMENT_TYPE( forge::TransformParentFragment );
RTTI_IMPLEMENT_TYPE( forge::TransformChildFragment );
RTTI_IMPLEMENT_TYPE( forge::ChildRequiringRecalculatingWorldTransform );
RTTI_IMPLEMENT_TYPE( forge::TransformComponent );

std::vector< forge::ObjectID > forge::TransformComponent::GetChildren() const
{
	std::vector< forge::ObjectID > result;

	auto& objectsManager = GetOwner().GetEngineInstance().GetObjectsManager();
	ecs::EntityID entityID = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
	auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
	if ( auto fragmentView = ecsManager.GetFragmentView< forge::TransformParentFragment >( entityID ) )
	{
		result.reserve( fragmentView->m_children.size() );
		for ( auto child : fragmentView->m_children )
		{
			result.emplace_back( ecsManager.GetFragmentView< forge::ObjectFragment >( child )->m_objectID );
		}
	}

	return result;
}

forge::ObjectID forge::TransformComponent::GetParent() const
{
	auto& objectsManager = GetOwner().GetEngineInstance().GetObjectsManager();
	ecs::EntityID entityID = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
	auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
	if ( auto fragmentView = ecsManager.GetFragmentView< forge::TransformChildFragment >( entityID ) )
	{
		auto parentObjectID = ecsManager.GetFragmentView< forge::ObjectFragment >( fragmentView->m_parentId )->m_objectID;
		return parentObjectID;
	}

	return forge::ObjectID();
}

static void DetachFrom( ecs::ECSManager& ecsManager, ecs::EntityID myId, forge::TransformChildFragment& childFragment )
{
	auto parentTransform = ecsManager.GetMutableFragmentView< forge::TransformParentFragment >( childFragment.m_parentId );
	forge::utils::RemoveValueReorder( parentTransform->m_children, myId );
	if ( parentTransform->m_children.empty() )
	{
		ecsManager.RemoveFragmentFromEntity< forge::TransformParentFragment >( childFragment.m_parentId );
	}
}

void forge::TransformComponent::SetParent( TransformComponent& parent, bool keepWorldTransform )
{
	auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
	auto& objectManager = GetOwner().GetEngineInstance().GetObjectsManager();

	ecs::EntityID myId = objectManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
	ecs::EntityID parentId = objectManager.GetOrCreateEntityId( parent.GetOwner().GetObjectID() );
	
	auto parentTransform = *ecsManager.GetFragmentView< forge::TransformFragment >( parentId );
	auto myTransform = *ecsManager.GetFragmentView< forge::TransformFragment >( myId );

	{
		auto childFragment = ecsManager.GetMutableFragmentView< forge::TransformChildFragment >( myId );
		{
			if ( childFragment )
			{
				DetachFrom( ecsManager, myId, *childFragment );
			}
			else
			{
				ecsManager.AddFragmentDataToEntity< forge::TransformChildFragment >( myId, { parentId } );
				childFragment = ecsManager.GetMutableFragmentView< forge::TransformChildFragment >( myId );
			}
			childFragment->m_parentId = parentId;
		}

		{
			if ( keepWorldTransform )
			{
				const Matrix localMatrix = myTransform.ToMatrix() * parentTransform.ToMatrix().OrthonormInverted();

				Vector3 localPos;
				Quaternion localRot;

				Vector3 local = localMatrix.TransformVector( Vector3::EY() );

				localMatrix.Decompose( childFragment->m_scale, localRot, localPos );
				ecsManager.GetMutableFragmentView< forge::TransformChildFragment >( myId )->m_transform = Transform( localPos, localRot );
			}
		}
	}

	{
		auto transformParent = ecsManager.GetMutableFragmentView< forge::TransformParentFragment >( parentId );
		if ( !transformParent )
		{
			ecsManager.AddFragmentToEntity< forge::TransformParentFragment >( parentId );
			transformParent = ecsManager.GetMutableFragmentView< forge::TransformParentFragment >( parentId );
		}

		transformParent->m_children.emplace_back( myId );
	}

	m_onTransformParentChanged.Invoke();
}

void forge::TransformComponent::DetachFromParent()
{
	auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
	auto& objectManager = GetOwner().GetEngineInstance().GetObjectsManager();

	ecs::EntityID myId = objectManager.GetOrCreateEntityId( GetOwner().GetObjectID() );

	auto childFragment = ecsManager.GetMutableFragmentView< forge::TransformChildFragment >( myId );
	{
		if ( childFragment )
		{
			DetachFrom( ecsManager, myId, *childFragment );
			m_onTransformParentChanged.Invoke();
		}
	}
}