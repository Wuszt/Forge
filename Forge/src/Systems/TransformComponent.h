#pragma once

namespace forge
{
	struct TransformFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( TransformFragment, ecs::Fragment );
		REGISTER_ECS_FRAGMENT();

	public:
		Transform m_transform;
		Vector4 m_scale = Vector4::ONES();

		Matrix ToMatrix() const
		{
			Matrix scaleMatrix;
			scaleMatrix.SetScale( m_scale );
			return scaleMatrix * m_transform.ToMatrix();
		}
	};

	struct TransformModifiedThisFrame : public ecs::Tag
	{
		REGISTER_ECS_TAG();
	};

	class TransformComponent : public DataComponent< TransformFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( TransformComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;

		virtual void OnAttach( EngineInstance& engineInstance );

		TransformFragment& GetDirtyData()
		{
			GetOwner().GetEngineInstance().GetECSManager().AddTagToEntity< TransformModifiedThisFrame >( GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
			return GetData_Internal();
		}
	};
}

