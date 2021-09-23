#pragma once

namespace forge
{
	struct TransformComponentData
	{
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
	public:
		using DataComponent::DataComponent;
	};
}

