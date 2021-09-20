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

	class TransformSystem : public systems::ECSSystem< TransformComponentData >
	{
		using ECSSystem::ECSSystem;
	};
}

