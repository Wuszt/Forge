#pragma once
#include "../GameEngine/IComponent.h"
#include "../ECS/Fragment.h"
#include "../ECS/FragmentViews.h"

namespace forge
{
	struct TransformFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( TransformFragment, ecs::Fragment );

		Transform m_transform = Transform::IDENTITY();
		Vector3 m_scale = Vector3::ONES();

		Matrix ToMatrix() const
		{
			Matrix scaleMatrix;
			scaleMatrix.SetScale( m_scale );
			return scaleMatrix * m_transform.ToMatrix();
		}
	};

	struct TransformWasModifiedThisFrame : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( TransformWasModifiedThisFrame, ecs::Tag );
	};

	struct ChildRequiringRecalculatingLocalTransform : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( ChildRequiringRecalculatingLocalTransform, ecs::Tag );
	};

	struct TransformParentFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( TransformParentFragment, ecs::Fragment );

		std::vector< ecs::EntityID > m_children;
	};

	struct TransformChildFragment : public TransformFragment
	{
		RTTI_DECLARE_STRUCT( TransformChildFragment, TransformFragment );

		TransformChildFragment() = default;
		TransformChildFragment( ecs::EntityID parentId )
			: m_parentId( parentId )
		{}

		ecs::EntityID m_parentId;
	};

	struct ChildRequiringRecalculatingWorldTransform : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( ChildRequiringRecalculatingWorldTransform, ecs::Tag );
	};

	class TransformComponent : public DataComponent< TransformFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( TransformComponent, forge::IDataComponent );
	public:
		using DataComponent::DataComponent;

		void SetWorldTransform( const Transform& transform )
		{
			GetMutableData()->m_transform = transform;
		}

		std::vector< forge::ObjectID > GetChildren() const;

		forge::ObjectID GetParent() const;

		const Transform& GetWorldTransform() const
		{
			return GetData()->m_transform;
		}

		void SetRelativeTransform( const Transform& transform )
		{
			auto entityId = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );
			if ( ecs::MutableFragmentView< forge::TransformChildFragment > childFragmentView = GetOwner().GetEngineInstance().GetECSManager().GetMutableFragmentView< forge::TransformChildFragment >( entityId ) )
			{
				childFragmentView->m_transform = transform;
			}

			GetMutableData()->m_transform = transform;
		}

		const Transform& GetRelativeTransform() const
		{
			auto entityId = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );
			if ( ecs::FragmentView< forge::TransformChildFragment > childFragmentView = GetOwner().GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformChildFragment >( entityId ) )
			{
				return childFragmentView->m_transform;
			}

			return GetData()->m_transform;
		}

		void SetWorldPosition( const Vector3& position )
		{
			GetMutableData()->m_transform.SetPosition( position );
		}

		const Vector3& GetWorldPosition() const
		{
			return GetWorldTransform().GetPosition3();
		}

		void SetRelativePosition( const Vector3& position )
		{
			GetMutableRelativeTransform().SetPosition( position );
		}

		const Vector3& GetRelativePosition() const
		{
			return GetRelativeTransform().GetPosition3();
		}

		void SetWorldOrientation( const Quaternion& orientation )
		{
			GetMutableData()->m_transform.SetOrientation( orientation );
		}

		const Quaternion& GetWorldOrientation() const
		{
			return GetWorldTransform().GetOrientation();
		}

		void SetRelativeOrientation( const Quaternion& orientation )
		{
			return GetMutableRelativeTransform().SetOrientation( orientation );
		}

		const Quaternion& GetRelativeOrientation() const
		{
			return GetRelativeTransform().GetOrientation();
		}

		void SetWorldScale( const Vector3& scale )
		{
			GetMutableData()->m_scale = scale;
		}

		const Vector3& GetWorldScale() const
		{
			return GetData()->m_scale;
		}

		void SetRelativeScale( const Vector3& scale )
		{
			auto entityId = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );
			if ( ecs::MutableFragmentView< forge::TransformChildFragment > childFragmentView = GetOwner().GetEngineInstance().GetECSManager().GetMutableFragmentView< forge::TransformChildFragment >( entityId ) )
			{
				childFragmentView->m_scale = scale;
			}
			else
			{
				SetWorldScale( scale );
			}
		}

		const Vector3& GetRelativeScale() const
		{
			auto entityId = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );
			if ( ecs::FragmentView< forge::TransformChildFragment > childFragmentView = GetOwner().GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformChildFragment >( entityId ) )
			{
				return childFragmentView->m_scale;
			}

			return GetData()->m_scale;
		}

		void SetParent( TransformComponent& parent, bool keepWorldTransform );
		void DetachFromParent();

		template< class TFunc >
		CallbackToken RegisterOnTransformParentChanged( TFunc&& func )
		{
			return m_onTransformParentChanged.AddListener( std::move( func ) );
		}

	private:
		Transform& GetMutableRelativeTransform()
		{
			auto entityId = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );
			if ( ecs::MutableFragmentView< forge::TransformChildFragment > childFragmentView = GetOwner().GetEngineInstance().GetECSManager().GetMutableFragmentView< forge::TransformChildFragment >( entityId ) )
			{
				return childFragmentView->m_transform;
			}

			return GetMutableData()->m_transform;
		}

		Callback<> m_onTransformParentChanged;
	};
}

