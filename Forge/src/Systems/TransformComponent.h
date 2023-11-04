#pragma once
#include "../GameEngine/IComponent.h"
#include "../ECS/Fragment.h"

namespace forge
{
	struct TransformFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( TransformFragment, ecs::Fragment );

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
		RTTI_DECLARE_STRUCT( PreviousFrameTransformFragment, ecs::Fragment );

		Transform m_previousTransform;
	};

	struct PreviousFrameScaleFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( PreviousFrameScaleFragment, ecs::Fragment );

		Vector3 m_previousScale;
	};

	class TransformComponent : public DataComponent< TransformFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( TransformComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;

		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue );

		const Transform& GetTransform() const
		{
			return GetData().m_transform;
		}

		Vector3 GetScale() const
		{
			return GetData().m_scale;
		}

		const Vector3* GetPrevFrameScale() const;

		Transform& GetDirtyTransform();

		Vector3& GetDirtyScale();
	};
}

