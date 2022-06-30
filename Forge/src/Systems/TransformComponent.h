#pragma once
#include "IComponent.h"

namespace forge
{
	struct TransformComponentData
	{
		DECLARE_STRUCT( TransformComponentData );

		Transform m_transform;
		Vector4 m_scale = Vector4::ONES();

		Matrix ToMatrix() const
		{
			Matrix scaleMatrix;
			scaleMatrix.SetScale( m_scale );
			return scaleMatrix * m_transform.ToMatrix();
		}
	};

	class TransformComponent : public DataComponent< TransformComponentData >
	{
		DECLARE_TYPE( TransformComponent, forge, IComponent );
	public:
		using DataComponent::DataComponent;
	};
}

