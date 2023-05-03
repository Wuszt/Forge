#pragma once

namespace forge
{
	struct TransformFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( TransformFragment, ecs::Fragment );

		Transform m_transform;
		Vector4 m_scale = Vector4::ONES();

		Matrix ToMatrix() const
		{
			Matrix scaleMatrix;
			scaleMatrix.SetScale( m_scale );
			return scaleMatrix * m_transform.ToMatrix();
		}
	};

	struct PreviousFrameTransformFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( PreviousFrameTransformFragment, ecs::Fragment );

		Transform m_previousTransform;
	};

	struct PreviousFrameScaleFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( PreviousFrameScaleFragment, ecs::Fragment );

		Vector3 m_previousScale;
	};

	class TransformComponent : public DataComponent< TransformFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( TransformComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;

		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue );

		Transform GetTransform() const
		{
			return GetData().m_transform;
		}

		Vector3 GetScale() const
		{
			return GetData().m_scale;
		}

		const Vector3* GetPrevFrameScale() const;

		Transform& GetDirtyTransform()
		{
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
			auto entityID = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );

			if ( ecsManager.GetFragment< PreviousFrameTransformFragment >( entityID ) == nullptr )
			{
				ecsManager.AddFragmentToEntity< PreviousFrameTransformFragment >( entityID );
				ecsManager.GetFragment< PreviousFrameTransformFragment >( entityID )->m_previousTransform = GetData().m_transform;
			}

			return GetMutableData().m_transform;
		}

		Vector3& GetDirtyScale()
		{
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
			auto entityID = GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );

			if ( ecsManager.GetFragment< PreviousFrameScaleFragment >( entityID ) == nullptr )
			{
				ecsManager.AddFragmentToEntity< PreviousFrameScaleFragment >( entityID );
				ecsManager.GetFragment< PreviousFrameScaleFragment >( entityID )->m_previousScale = GetData().m_scale;
			}

			return GetMutableData().m_scale.AsVector3();
		}
	};
}

