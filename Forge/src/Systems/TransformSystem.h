#pragma once

namespace forge
{
	struct TransformComponentData
	{
		Transform m_transform;
	};

	class TransformSystem : public systems::ECSSystem< TransformComponentData >
	{
		using ECSSystem::ECSSystem;
	};
}

